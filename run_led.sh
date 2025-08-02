#!/bin/sh
export DISPLAY=:3.0
export HARDWARE_DESC=adafruit-hat
make xserver-screen

# Default: 100 FPS (10000 microseconds)
# For 120 FPS use: --update-interval 8333
# For 60 FPS use: --update-interval 16667
sudo ./xserver-screen --led-rows=64 --led-cols=128 --led-chain=2 --led-slowdown-gpio=4 --led-pixel-mapper="V-mapper" --led-brightness 80
