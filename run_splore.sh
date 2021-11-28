#!/bin/sh
export DISPLAY=:3.0
Xvfb $DISPLAY -screen 0 800x600x24 &
killall pico8
./pico-8/pico8 -windowed 1 -window_x 128 -window_y 128 -width 128 -height 128 -frameless 1 -splore &
./run_led.sh