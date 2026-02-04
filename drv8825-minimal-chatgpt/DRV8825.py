# Filename: DRV8825_minimal.py
# Correct for Waveshare Stepper Motor HAT

import RPi.GPIO as GPIO
import time

class DRV8825:
    def __init__(self, dir_pin, step_pin, enable_pin):
        self.dir_pin = dir_pin
        self.step_pin = step_pin
        self.enable_pin = enable_pin

        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)

        GPIO.setup(self.dir_pin, GPIO.OUT)
        GPIO.setup(self.step_pin, GPIO.OUT)
        GPIO.setup(self.enable_pin, GPIO.OUT)

        self.disable()

    def enable(self):
        GPIO.output(self.enable_pin, GPIO.HIGH)   # ✔ ACTIVE HIGH (Waveshare)

    def disable(self):
        GPIO.output(self.enable_pin, GPIO.LOW)

    def move(self, steps, direction, step_delay=0.002):
        GPIO.output(self.dir_pin, GPIO.HIGH if direction else GPIO.LOW)
        self.enable()

        for _ in range(steps):
            GPIO.output(self.step_pin, GPIO.HIGH)
            time.sleep(step_delay)
            GPIO.output(self.step_pin, GPIO.LOW)
            time.sleep(step_delay)

        self.disable()
