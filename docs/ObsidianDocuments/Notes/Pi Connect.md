**Raspberry Pi Connect** is a fantastic choice for bypassing third-party app drops. Because it is built directly by Raspberry Pi Ltd., it integrates natively with the OS, handles local and cloud routing automatically via WebRTC, and lets you view your Pi's full desktop or terminal straight inside any browser window (like Chrome on your Chromebook).

If you didn't check the automated setup box when writing the SD card, you can easily enable and configure it manually right from your Pi's terminal.

---

### How to Set Up Raspberry Pi Connect

#### Step 1: Update your Pi and install the package

Open a terminal on your Raspberry Pi (or via SSH) and run the following commands to ensure your system components and the Connect package are fully up to date:

```bash
sudo apt update
sudo apt full-upgrade -y
sudo apt install rpi-connect -y

```

#### Step 2: Turn on the service

Enable the Connect daemon for your user session by running:

```bash
rpi-connect on

```

#### Step 3: Sign in and link your device

Initiate the sign-in sequence from the terminal:

```bash
rpi-connect signin

```

* The terminal will output a unique verification URL (looking something like `[https://connect.raspberrypi.com/verify/XXXX-XXXX](https://connect.raspberrypi.com/verify/XXXX-XXXX)`).
* Copy that link, paste it into your browser (on your Chromebook or PC), and log in using your **Raspberry Pi ID** (or create a free account if you haven't already).
* Give your Raspberry Pi a recognizable name when prompted.

You can double-check that it is successfully linked by running:

```bash
rpi-connect status

```

---

### How to Access Your Pi Anytime

Once linked, you no longer need to worry about local IP addresses, port forwarding, or third-party client apps dropping connections:

1. Open a browser tab on any device (your Chromebook, PC, etc.) and go to **[connect.raspberrypi.com](https://www.google.com/search?q=https://connect.raspberrypi.com)**.
2. Log into your Raspberry Pi ID account.
3. You will see your Pi listed. Click **Connect**, and you can choose between:
* **Screen Sharing**: Pulls up the full graphical desktop environment in your browser tab.
* **Remote Shell**: Opens an instantaneous browser-based terminal session if you just need to check code, run `make`, or manage logs.