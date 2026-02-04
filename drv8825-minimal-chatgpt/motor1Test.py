# Filename: motor1Test.py

from DRV8825 import DRV8825
import time

motor1 = DRV8825(dir_pin=13, step_pin=19, enable_pin=12)

motor1.move(800, direction=True)    # quarter turn
time.sleep(1)
motor1.move(800, direction=False)
