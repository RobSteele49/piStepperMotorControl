Since you are connecting them directly with no router to hand out addresses, you need to manually assign "Static IPs" to both. We will put them on their own private "island" network using the **192.168.5.x** range.

---

### Phase 1: Configure the Windows Mini PC

Windows needs to know it isn't waiting for a router.

1. Press `Win + R`, type `ncpa.cpl`, and hit Enter.
    
2. Right-click your **Ethernet** adapter and select **Properties**.
    
3. Double-click **Internet Protocol Version 4 (TCP/IPv4)**.
    
4. Select **Use the following IP address** and enter:
    
    - **IP address:** `192.168.5.10`
        
    - **Subnet mask:** `255.255.255.0`
        
    - **Default gateway:** (Leave this blank)
        
5. Click **OK** and **OK**.
    

---

### Phase 2: Configure the Raspberry Pi

Modern Raspberry Pi OS (Bookworm) uses **NetworkManager**. We’ll use the command line tool `nmcli` to set the IP.

1. Open your terminal on the Pi.
    
2. Find your wired connection name (usually "Wired connection 1"):
    
    Bash
    
    ```
    nmcli connection show
    ```
    
3. Set the static IP (replace "Wired connection 1" if yours is named differently):
    
    Bash
    
    ```
    sudo nmcli connection modify "Wired connection 1" ipv4.addresses 192.168.5.11/24 ipv4.method manual
    ```
    
4. Apply the changes:
    
    Bash
    
    ```
    sudo nmcli connection up "Wired connection 1"
    ```
    

---

### Phase 3: The Moment of Truth (The Ping)

Now, let’s see if they can "hear" each other across the cable.

**From the Raspberry Pi terminal:**

Bash

```
ping -c 4 192.168.5.10
```

_If you see "64 bytes from...", the Pi can see the PC._

**From the Windows Command Prompt (cmd):**

Bash

```
ping 192.168.5.11
```

_If you see "Reply from...", the PC can see the Pi._

---

### Troubleshooting: "Request Timed Out"

If the Pi can ping Windows, but Windows **cannot** ping the Pi, it is almost always the **Windows Firewall**.

- Windows often treats direct Ethernet connections as "Public" (untrusted) and blocks Pings (ICMP).
    
- **The Quick Test:** Temporarily disable the Windows Firewall for "Public Networks" and try the ping again. If it works, you just need to add an exception for ICMP rather than leaving the firewall off.
    

### Important Note on Internet

Because you've set a static IP with no gateway, the Pi will **not** get internet through that Ethernet cable. If you need to download libraries or updates, make sure the Pi is also connected to your house **WiFi**. The Pi is smart enough to use WiFi for the internet and Ethernet for NINA simultaneously.

**Would you like me to show you how to check if both the WiFi and Ethernet are active at the same time on the Pi?**