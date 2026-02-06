# Filename: motor1Test.py

from DRV8825 import DRV8825
import time

motor1 = DRV8825(dir_pin=13, step_pin=19, enable_pin=12)

motor1.move(800, direction=True)    # quarter turn
time.sleep(1)
motor1.move(800, direction=False)
time.sleep(1)

# Since I am current set for 1/16 microsteps per pulse 3220 steps are
# required for one revolution

motor1.move (3200, True)
time.sleep(1)
motor1.move (3200, False)
time.sleep(1)


