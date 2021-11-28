Displaying PICO-8 on RGB LED display with Raspberry Pi GPIO
==================================================

This little program runs PICO-8 in virtual X-server (Xvfb), makes screenshot every 10000 microseconds and display it on LED matrix

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
Configure gamepad
--------
For configuring gamepad you can use following documentation: https://pico-8.fandom.com/wiki/Controllers

Pre-compiled `controllermap` is included in repo
