#!/bin/sh
export DISPLAY=:3.0
export HARDWARE_DESC=adafruit-hat
make xserver-screen

# 120 FPS (8333 microseconds)
sudo ./xserver-screen --update-interval 8333 --led-rows=64 --led-cols=128 --led-chain=2 --led-slowdown-gpio=4 --led-pixel-mapper="V-mapper" --led-brightness 80 