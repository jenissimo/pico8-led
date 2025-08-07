Displaying PICO-8 on RGB LED display with Raspberry Pi GPIO
==================================================

This little program runs PICO-8 in virtual X-server (Xvfb), makes screenshot every 10000 microseconds (100 FPS) by default and display it on LED matrix

**📍 Project description and discussion:**
https://www.lexaloffle.com/bbs/?tid=45501

Instructions
--------

Clone repo recursively with submodules
```
git clone --recurse-submodules https://github.com/jenissimo/pico8-led.git
```

Put your copy of *PICO-8* for Raspberry PI into `pico-8/` directory

Install dependencies:
```
sudo apt-get install xvfb libx11-dev
```

Adjust settings of your LED matrix in `run_led.sh` using `rpi-rgb-led-matrix` documentation: https://github.com/hzeller/rpi-rgb-led-matrix/blob/master/README.md

Running
--------
Run PICO-8 in Splore mode:
```
./run_splore.sh
```
--------
Run PICO-8 game:
```
./run_game.sh your_pico_game.p8
```

Update Frequency
--------
You can adjust the screenshot update frequency using the `--update-interval` parameter:

- Default: 100 FPS (10000 microseconds)
- 120 FPS: `--update-interval 8333`
- 60 FPS: `--update-interval 16667`
- 30 FPS: `--update-interval 33333`

Example for 120 FPS:
```
sudo ./xserver-screen --update-interval 8333 --led-rows=64 --led-cols=128 --led-chain=2 --led-slowdown-gpio=4 --led-pixel-mapper="V-mapper" --led-brightness 80
```

Configure gamepad
--------
For configuring gamepad you can use following documentation: https://pico-8.fandom.com/wiki/Controllers

Pre-compiled `controllermap` is included in repo

Adding autostart
--------
To automatically start pico8-led on boot, create a systemd service:

```bash
sudo nano /etc/systemd/system/pico8-led.service
```

Add the following content:

```ini
[Unit]
Description=Pico-8 LED Matrix Display
After=network.target display-manager.service graphical.target

[Service]
# Ensure the program can access the X server
Environment="DISPLAY=:0"
# Uncomment the line below if you encounter issues with X server access (e.g., "cannot open display")
# Environment="XAUTHORITY=/home/pi/.Xauthority"

# Set the working directory where your scripts and executable are located
WorkingDirectory=/home/pi/pico8-led

# --- CHOOSE ONE OF THE ExecStart OPTIONS BELOW ---

# OPTION A: Run PICO-8 in Splore mode
ExecStart=/home/pi/pico8-led/run_splore.sh

# OPTION B: Run a specific PICO-8 game (uncomment and replace 'your_pico_game.p8')
# ExecStart=/home/pi/pico8-led/run_game.sh your_pico_game.p8

# OPTION C: Run xserver-screen directly with custom parameters (e.g., for 120 FPS)
# ExecStart=/home/pi/pico8-led/xserver-screen --update-interval 8333 --led-rows=64 --led-cols=128 --led-chain=2 --led-slowdown-gpio=4 --led-pixel-mapper="V-mapper" --led-brightness 80

# --- END OF ExecStart OPTIONS ---

StandardOutput=inherit # Inherit stdout to see logs with 'journalctl -f -u pico8-led.service'
StandardError=inherit  # Inherit stderr for error messages
Restart=always         # Automatically restart if the program crashes
User=pi                # Run the service as the 'pi' user (adjust if different)

[Install]
WantedBy=graphical.target # Start after the graphical environment is ready
```

Save the file (Ctrl+X, Y, Enter).

Reload systemd daemon, enable, and start the service:

```bash
sudo systemctl daemon-reload
sudo systemctl enable pico8-led.service
sudo systemctl start pico8-led.service
```

Recommended way to shut it down
--------
pico8-led code is designed to perform proper cleanup (clearing the matrix, freeing resources) when it receives SIGINT or SIGTERM signals.

So you have several reliable options to stop the application and ensure the matrix is cleared:

- Press CTRL+C in the terminal where pico8-led is running (if launched manually). This sends a SIGINT signal.
- Execute `sudo systemctl stop pico8-led.service` (if running as a systemd service). This sends a SIGTERM signal.
- Perform a system shutdown or reboot (e.g., `sudo shutdown now` or `sudo reboot`). The Linux kernel will send a SIGTERM signal to all running processes, including pico8-led.

Known issues
--------
**Mouse and keyboard input support**

One challenge that hasn't been fully solved yet is mouse and keyboard input support. `xvfb` in headless mode doesn't seem to "see" mouse and keyboard events. 

A potential solution might be using an HDMI dummy plug to simulate a connected monitor, but this approach hasn't been tested yet. This could be an area for future exploration!
