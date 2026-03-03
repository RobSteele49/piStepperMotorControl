I am working on a focusers/rotator for my 10" LX200 optical tube that is mounted on a Celestron CGEM Equatorial mount. My long term goal is to run the telescope remotely via the NINA interface. I have miniPC mounted to the telescope along with a Raspberry Pi 4. The scope is mounted about 50 feet from the house with 110 power and a wifi extender connected to my Starlink router at the house.

I've got the software with the help of Gemini working and is in a repository https://github.com/RobSteele49/piStepperMotorControl
As this is a public accessible github repository I was told you could view the code. As of right now the code to move the focuser is working and I can control the focuser from an ASCOM Device Hub or NINA either wired directly between my mini PC and the Raspberry Pi 4 or via a WiFi connection. The Pi4 has a Stepper Hat which is working quite well. I'm using two NIMA 17 pancake motors.  I'm still bench testing but getting very close to moving outside. A few items I'd like to work on would include:

- Getting the code documented.
- Building a button interface. My goal is to have two buttons dedicated to the focusers and two dedicated to the camera rotator.
	- Focuser
		- For the focuser I was going to have one button dedicated to clockwise rotation and the other to counter-clockwise rotation.
		- If the button is held for <3 seconds it will move 'very' slowing between 3 and 6 seconds it will spend up to 1 rotation in 3 seconds, and if held greater than 6 seconds it will spend up to 1 rotation per second. (These are approximate and I'm hoping you can make some suggestions to improve things)
		- For the rotator I was thinking that if held <3 seconds it would be 'very' slowing and if > 3 seconds if would move at the rate of 1 rotation in 6 seconds. The maximum rotation allowed would be 180 degrees so I don't get cables wrapped up.
		- I would want to 3d print a button control box that would have cables connected to the raspberry pi and would have a spot to stay at the telescope.
	- Rotator
		- I have basic control working of this motor but not from NINA
		- I've started on a design but this will wait until I have button control working.
	- Button Control
		- I'm not very familiar with setting up button i/f on the Raspberry Pi 4 so I'm hoping for your help.
		- The buttons I'm planning on using were purchased from Amazon are are "# 5pcs 12mm 2A 12V/24V/125V/250V AC Prewired Waterproof Push Button Momentary Stainless Steel Push Button On Off (High Head)"
		- I don't have a box designed yet, so if you could create a openSCAD design of one that meets my requirements that would be a great help.
		- I do have the ability to take an openSCAD design and print it and/or use FreeCAD to turn it into a STEP file and make edits on the part wit onShape.

So getting the code documented, a button case, instructions for how to connect the buttons, along with the software would really help.

As I have working code in my repository I want to be very careful to not break it. Would using the pull mechanism and/or creating a fork be the safest way to to this work? If has been several years since I used GitHub for any serious development so I feel like I'm getting my feet wet.

Initial response from ChatGPT:

Rob — this is a very solid project description. Clear goal, real constraints, and you’re thinking about failure modes (cable wrap). Good engineering instincts.

Before I suggest improvements, I’m going to tighten this into a structured system view and then ask you the **right missing questions**. These are the kinds of things that determine whether this becomes “works” or “rock solid observatory-grade.”

---

# 🔭 System Summary (What I Understand)

**Optics:** 10" Meade LX200 Classic OTA  
**Mount:** Celestron CGEM  
**Imaging Software:** NINA  
**Control Hub:** ASCOM Device Hub  
**Compute:**

- MiniPC on scope (runs NINA)
    
- Raspberry Pi 4 (motor control)
    
- WiFi via Starlink + extender
    
- 110VAC at pier
    

**Motors:**

- 2 × NEMA 17 pancake steppers
    
- Stepper HAT on Pi
    
- Focuser working via ASCOM
    
- Rotator partially working
    

**Next goals:**

1. Code documentation
    
2. Physical button interface (4 buttons total)
    
3. 3D printed control box
    
4. Safe Git workflow
    
5. Avoid breaking working code
    

---

# 🔎 Important Missing Questions

These matter more than they look.

---

## 1️⃣ Absolute vs Relative Positioning

Does your focuser:

- Keep absolute position across power cycles?
    
- Or is it relative-only?
    

If power drops mid-session (Starlink hiccup, breaker trip), what happens?

For a rotator especially, **absolute position matters**.

Do you have:

- Limit switches?
    
- Home switch?
    
- Encoder?
    
- Or purely open-loop step counting?
    

---

## 2️⃣ Mechanical Reduction

What’s your gear ratio between motor and focuser shaft?

At 1 rotation/sec:

- Is that actually safe for the mechanics?
    
- Or will you induce vibration into the imaging train?
    

Astro reality: you rarely need high speed at the scope.

---

## 3️⃣ Microstepping

What microstep setting are you using on the Stepper HAT?

Earlier you mentioned 1/32 for precision. For focusing on an LX200:

- Depth of focus at f/10 is fairly forgiving.
    
- Torque margin matters more than resolution.
    

You might be over-optimizing precision and losing torque.

---

## 4️⃣ Cable Wrap Strategy (Rotator)

You said max 180°.

But what happens if:

- NINA rotates 170°
    
- Then user hits manual button 30°?
    

Will you:

- Track cumulative angle?
    
- Enforce hard limits in software?
    
- Or just “hope”?
    

This needs a state machine.

---

## 5️⃣ Button Control Philosophy

Your time-based speed tiers (<3s, 3–6s, >6s) are clever — but slightly brittle.

Better approach:

### Option A – Acceleration Ramp (Better UX)

Instead of time buckets:

- When button pressed → start slow
    
- Every 300ms increase speed
    
- Cap at max speed
    
- When released → decelerate
    

This feels much more natural.

Time buckets feel artificial.

---

## 6️⃣ Electrical Interface for Buttons

Those stainless momentary buttons:

Are they:

- NO (Normally Open)?
    
- With LED ring?
    
- What voltage is the LED rated for?
    

Very important:  
Pi GPIO is **3.3V only**.

We must:

- Use internal pull-ups
    
- Or external resistors
    
- Or opto-isolate (if long cable runs)
    

How long will the cable from box to Pi be?

50 feet? 2 feet?

Long runs = noise risk (especially near steppers).

---

## 7️⃣ Real Observatory Question

Is this system:

- Used only while you're present?
    
- Or truly unattended remote?
    

Because unattended changes everything:

- Watchdog
    
- Logging
    
- Motor stall detection
    
- Safe reboot state
    

---

# 🧠 Architectural Recommendation

You’re at the exact point where architecture matters.

I recommend separating:

```
MotorControl Layer
    ↓
Device Logic Layer (Focuser / Rotator state machines)
    ↓
Interface Layer
    - ASCOM
    - Buttons
    - Network
```

Buttons should NOT directly drive motors.

They should issue high-level commands:

```
focuser.move(direction, speed_profile)
```

That prevents breaking your working ASCOM interface.

---

# 🧰 Git Strategy (Very Important)

You asked:

> Should I fork? Pull? What's safest?

Since this is your repo:

### Best practice:

On Pi:

```bash
git checkout -b feature/button-interface
```

Work there.

When stable:

```bash
git checkout main
git merge feature/button-interface
```

Then push.

You do NOT need a fork.

Forking is for working on someone else’s repository.

---

# 🧩 OpenSCAD Box

Before I design it, I need:

- Button diameter (12mm mounting hole confirmed?)
    
- Panel thickness recommendation?
    
- Cable gland or strain relief?
    
- Size constraints?
    
- Weatherproof level?
    

You mentioned waterproof buttons — is the enclosure outdoors permanently?

---

# 🚀 What I Suggest Next

Let’s do this in phases:

Phase 1 – Lock Architecture  
Phase 2 – GPIO wiring design  
Phase 3 – Button speed control algorithm  
Phase 4 – OpenSCAD enclosure  
Phase 5 – Code documentation structure  
Phase 6 – Git workflow cleanup

---

# The 5 Questions I Need Answered First

1. Do you have any limit switches or encoders?
    
2. What microstepping setting are you currently using?
    
3. How long will the button cable run be?
    
4. Are the buttons illuminated?
    
5. Is this unattended or supervised operation?
    

Answer those and we’ll tighten this into a professional-grade design.

You’re very close to something really nice here.