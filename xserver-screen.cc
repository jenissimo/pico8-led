#include "led-matrix.h"

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define UPDATE_INTERVAL 10000
#define SCREEENSHOT_X 128
#define SCREEENSHOT_Y 128
#define SCREEENSHOT_WIDTH 128
#define SCREEENSHOT_HEIGHT 128

using rgb_matrix::Canvas;
using rgb_matrix::FrameCanvas;
using rgb_matrix::RGBMatrix;

struct ColorComponentModifier
{
    unsigned long shift;
    unsigned long bits;
};

// Make sure we can exit gracefully when Ctrl-C is pressed.
volatile bool interrupt_received = false;
static void InterruptHandler(int signo)
{
    interrupt_received = true;
}

ColorComponentModifier GetColorComponentModifier(unsigned long mask)
{
    ColorComponentModifier color_component_modifier;

    color_component_modifier.shift = 0;
    color_component_modifier.bits = 0;

    while (!(mask & 1))
    {
        color_component_modifier.shift++;
        mask >>= 1;
    }
    while (mask & 1)
    {
        color_component_modifier.bits++;
        mask >>= 1;
    }
    if (color_component_modifier.bits > 8)
    {
        color_component_modifier.shift += color_component_modifier.bits - 8;
        color_component_modifier.bits = 8;
    }

    return color_component_modifier;
}

int ShowScreen(Display *display, size_t x, size_t y, size_t width, size_t height, RGBMatrix *matrix)
{
    FrameCanvas *offscreen_canvas = matrix->CreateFrameCanvas();
    XColor color;
    int screen = XDefaultScreen(display);
    XWindowAttributes attribs;
    Window window = XRootWindow(display, screen);
    XImage *img;
    ColorComponentModifier r_modifier, g_modifier, b_modifier;

    XGetWindowAttributes(display, window, &attribs);

    // based on original code from http://www.roard.com/docs/cookbook/cbsu19.html
    r_modifier = GetColorComponentModifier(attribs.visual->red_mask);
    g_modifier = GetColorComponentModifier(attribs.visual->green_mask);
    b_modifier = GetColorComponentModifier(attribs.visual->blue_mask);

    while (!interrupt_received)
    {
        if (interrupt_received)
            break;

        img = XGetImage(display, window, x, y, width, height, AllPlanes, XYPixmap);
        unsigned char color_channel[3];

        for (int xPixel = 0; xPixel < img->width; xPixel++)
        {
            for (int yPixel = 0; yPixel < img->height; yPixel++)
            {
                color.pixel = XGetPixel(img, xPixel, yPixel);
                color_channel[0] = ((color.pixel >> b_modifier.shift) & ((1 << b_modifier.bits) - 1)) << (8 - b_modifier.bits);
                color_channel[1] = ((color.pixel >> g_modifier.shift) & ((1 << g_modifier.bits) - 1)) << (8 - g_modifier.bits);
                color_channel[2] = ((color.pixel >> r_modifier.shift) & ((1 << r_modifier.bits) - 1)) << (8 - r_modifier.bits);

                offscreen_canvas->SetPixel(xPixel, yPixel, color_channel[2], color_channel[1], color_channel[0]);
            }
        }

        offscreen_canvas = matrix->SwapOnVSync(offscreen_canvas);
        XFree(img);
        usleep(UPDATE_INTERVAL);
    }

    return 0;
}

int usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [led-matrix-options]\n",
            progname);
    rgb_matrix::PrintMatrixFlags(stderr);
    return 1;
}

int main(int argc, char *argv[])
{
    // Initialize the RGB matrix with
    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;
    if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv,
                                           &matrix_options, &runtime_opt))
    {
        return usage(argv[0]);
    }

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL)
        return 1;

    Display *display;
    char *dpy_name = std::getenv("DISPLAY");

    if (!dpy_name)
    {
        fprintf(stderr, "No DISPLAY set\n");
        return 1;
    }

    fprintf(stdout, "DISPLAY is %s:\n", dpy_name);

    display = XOpenDisplay(dpy_name);
    if (display == NULL)
    {
        fprintf(stderr, "Display %s cannot be found, exiting", dpy_name);
        return 1;
    }

    // Put screnshot from display on RGB Matrix
    ShowScreen(display, SCREEENSHOT_X, SCREEENSHOT_Y, SCREEENSHOT_WIDTH, SCREEENSHOT_HEIGHT, matrix);

    XCloseDisplay(display);
    matrix->Clear();
    delete matrix;

    return 0;
}