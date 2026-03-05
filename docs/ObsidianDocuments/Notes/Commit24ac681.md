Backed up to the commit 24ac618 and able to move the motors with a lot of noise.

Executed the commands:

 git fetch
 git checkout 24ac681
 cd build
 make clean
 cmake ..
 make

Executed

sudo ./FocuserMaxSpeed

Moved the focuser motor WAY too fast

sudo ./SlowRotationTest

Moved slower, but still way too fast

sudo ./DualMotorTest

Still too fast, but both NIMA17 pancake motors move.

sudo ./ZeroFocuser

Moved, but again too fast

sudo ./StepperRotateV1

Same, moved focuser but too fast

sudo ./BacklashCheck

Worked but too fast.

Changed the DIP switches to all 1's 1/32 microsteps. Working now.

Did this test again on 2026 02 24. Currently I have the DIP switches set for 1/32 microsteps.

This time I did the commands:

 git checkout 24ac681
 cd build
 make clean
 make


DualMotorTest:       both motors moved...
ZeroFocuser:           focuser motor moved
FocuserMaxSpeed: focuser motor moved (quickly) in both directions
SlowRotationTest:   focuser motor moved (slowly) in both directions
StepperRotateV1:   focuser motor moved in both directions
BacklashCheck:      focuser motor moved in both directions

Notes from 2026 02 24

- Working from the piStepperWorkingBaseline from the build directory
- DualMotorTest
	- Both motors are moving slowly
	- Only one direction moved
- FocuserMaxSpeed
	- Output:
		- pi@raspberrypi:~/piStepperWorkingBaseline/build
		- $ sudo ./FocuserMaxSpeed 
		- --- Robert D. Steele: Ramped High Speed Test ---
		- Target: 6 seconds per knob revolution.
		- Phase 1: 3 Knob Revs COUNTER-CLOCKWISE (Ramped)...
		- Pausing 5 seconds for mechanical settling...
		- Phase 2: 3 Knob Revs CLOCKWISE (Ramped)...
		- Test Complete. All movement was ramped. Motor Released.
	- Could hear the ramp up and ramp down
	- Doing approximately 1 revolution of the focuser in 6 seconds
		- There is 1:3 gear ratio, small gear on the motor and larger on the focuser
- SlowRotationTest
	- Approximately 1 revolution in 30 seconds
	- Motion both directions

- Working from the piStepperMotorControl/build directory
	- DualMotorTest
		- Only motor 2 appears to be moving and only in one direction
	- FocuserMaxSpeed
		- Moving motor 2 not motor 1