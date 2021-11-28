#!/bin/sh
export DISPLAY=:3.0
export HARDWARE_DESC=adafruit-hat
make xserver-screen
sudo ./xserver-screen --led-rows=64 --led-cols=128 --led-chain=2 --led-slowdown-gpio=4 --led-pixel-mapper="V-mapper" --led-brightness 80