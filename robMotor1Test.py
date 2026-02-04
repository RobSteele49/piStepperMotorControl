# Rob modified for testing motor 1
# Filename: robMotor1Test.py

import RPi.GPIO as GPIO
import time
from DRV8825 import DRV8825

try:
	Motor1 = DRV8825(dir_pin=13, step_pin=19, enable_pin=12, mode_pins=(16, 17, 20))
        
	"""
	# 1.8 degree: nema23, nema14
	# software Control :
	# 'fullstep': A cycle = 200 steps
	# 'halfstep': A cycle = 200 * 2 steps
	# '1/4step': A cycle = 200 * 4 steps
	# '1/8step': A cycle = 200 * 8 steps
	# '1/16step': A cycle = 200 * 16 steps
	# '1/32step': A cycle = 200 * 32 steps
	"""

	Motor1.SetMicroStep('software','fullstep')
	Motor1.TurnStep(Dir='forward', steps=800, stepdelay = 0.002)
	time.sleep(0.5)
	Motor1.TurnStep(Dir='backward', steps=800, stepdelay = 0.002)
	Motor1.Stop()
	time.sleep(0.5)
	"""
	# 28BJY-48:
	# software Control :
	# 'fullstep': A cycle = 2048 steps
	# 'halfstep': A cycle = 2048 * 2 steps
	# '1/4step': A cycle = 2048 * 4 steps
	# '1/8step': A cycle = 2048 * 8 steps
	# '1/16step': A cycle = 2048 * 16 steps
	# '1/32step': A cycle = 2048 * 32 steps
	"""
	
	Motor1.SetMicroStep('hardware' ,'halfstep')    
	Motor1.TurnStep(Dir='forward', steps=3200, stepdelay=0.002)
	time.sleep(0.5)
	Motor1.TurnStep(Dir='backward', steps=3200, stepdelay=0.002)
	time.sleep(0.5)
	Motor1.Stop()
    
except:
    # GPIO.cleanup()
    print ("\nMotor stop")
    Motor1.Stop()
    exit()
