
Pancake motors are prone to overheating. The system implements a **30-second Auto-Release** safety.

## The Logic (`checkTimeout`)
1. Whenever a motor moves, `_lastActivity` is updated.
2. If `30 seconds` pass without a new command, the `EN` (Enable) pin is set to `0` (Off).
3. This releases the "Holding Torque," allowing the motor to cool down.
4. The motor is automatically re-engaged (Powered) the moment a new `moveTo` command arrives.

> [!WARNING]
> If the telescope is pointing at the Zenith (straight up), releasing power might cause a heavy imaging train to "slip" or "creep." Monitor for focus drift during long sessions.