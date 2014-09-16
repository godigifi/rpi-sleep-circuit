import RPi.GPIO as GPIO
import time
import os
from power import shutdown
"""
Sample code modified from
http://www.raspberrypi.org/forums/viewtopic.php?p=276780#p276780
"""
GPIO.setmode(GPIO.BCM)
GPIO.setup(24, GPIO.IN)
prev_input = 0
while True:
    input = GPIO.input(24)
    if ((not prev_input) and input):
        shutdown(25) # Shutdown power after 25 seconds
    prev_input = input
    time.sleep(0.05)