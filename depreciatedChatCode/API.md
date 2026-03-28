**Last Updated:** March 26, 2026  
**Version:** 1.0

Code

````

---

## **File 2: API.md**

```markdown
# ASCOM Alpaca API Reference

## Overview
The piStepperMotorControl system implements the ASCOM Alpaca protocol, enabling remote control from astronomy software like NINA, APT, SequenceGenerator Pro, and others.

## Connection Information

### Server Details
- **Host:** Your Raspberry Pi IP address (e.g., `192.168.1.100`)
- **Port:** `8080` (default)
- **Protocol:** HTTP
- **Base URL:** `http://<pi-ip>:8080`

### Finding Your Pi's IP Address
```bash
ssh pi@raspberrypi
hostname -I
````

Or from the Pi console:

bash

```
ip addr show | grep inet
```

---

## Management Endpoints

### Server Description

Returns information about the Alpaca server.

**Endpoint:**

Code

```
GET /management/v1/description
```

**Response:**

JSON

```
{
  "Value": {
    "ServerName": "Pi-LX200-Focuser",
    "Manufacturer": "Steele-Astronomy",
    "ManufacturerVersion": "1.1",
    "Location": "Observatory"
  },
  "ClientTransactionID": 0,
  "ServerTransactionID": 1,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

### Configured Devices

Returns list of available devices.

**Endpoint:**

Code

```
GET /management/v1/configureddevices
```

**Response:**

JSON

```
{
  "Value": [
    {
      "DeviceName": "Dual Controller Focuser",
      "DeviceType": "Focuser",
      "DeviceNumber": 0,
      "UniqueID": "Steele-Focuser-01"
    },
    {
      "DeviceName": "Dual Controller Rotator",
      "DeviceType": "Rotator",
      "DeviceNumber": 0,
      "UniqueID": "Steele-Rotator-01"
    }
  ],
  "ClientTransactionID": 0,
  "ServerTransactionID": 1,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

---

## Focuser Endpoints

### Connection Status

**Check if focuser is connected:**

Code

```
GET /api/v1/focuser/0/connected
PUT /api/v1/focuser/0/connected
```

**Parameters:** None for GET  
**PUT Body:** `Connected=true` or `Connected=false`

**Response:**

JSON

```
{
  "Value": true,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

### Device Name

**Get focuser name:**

Code

```
GET /api/v1/focuser/0/name
```

**Response:**

JSON

```
{
  "Value": "Dual-Controller Focuser",
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

### Current Position

**Get current focuser position (in steps):**

Code

```
GET /api/v1/focuser/0/position
```

**Response:**

JSON

```
{
  "Value": 25000,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

**Note:** Position is always in absolute steps from FOC_LIMIT_MIN.

### Movement Status

**Check if focuser is moving:**

Code

```
GET /api/v1/focuser/0/ismoving
```

**Response:**

JSON

```
{
  "Value": false,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

Returns `true` while motor is moving, `false` when stationary.

### Absolute Position

**Check if focuser supports absolute positioning:**

Code

```
GET /api/v1/focuser/0/absolute
```

**Response:**

JSON

```
{
  "Value": true,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

### Maximum Step Count

**Get maximum focuser position (max travel range):**

Code

```
GET /api/v1/focuser/0/maxstep
```

**Response:**

JSON

```
{
  "Value": 50000,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

**Note:** This is set to `FOC_LIMIT_MAX` in config.h (typically 50000).

### Maximum Increment

**Get maximum single step distance:**

Code

```
GET /api/v1/focuser/0/maxincrement
```

**Response:**

JSON

```
{
  "Value": 5000,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

**Note:** NINA uses this for safety checks. Adjustable in AlpacaServer.hpp.

### Move to Position

**Command focuser to absolute position:**

Code

```
PUT /api/v1/focuser/0/move
```

**Parameters:**

- `Position` (required) - Target position in steps (0 to 50000)
- `ClientTransactionID` (optional) - Tracking ID for your request

**Example:**

Code

```
PUT /api/v1/focuser/0/move?Position=30000&ClientTransactionID=1
```

**Response:**

JSON

```
{
  "ClientTransactionID": 1,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

**Important Notes:**

- Movement is asynchronous (returns immediately)
- Check `ismoving` endpoint to track completion
- Position will be clamped to FOC_LIMIT_MIN/MAX automatically

### Halt Movement

**Stop focuser immediately:**

Code

```
PUT /api/v1/focuser/0/halt
```

**Parameters:**

- `ClientTransactionID` (optional)

**Response:**

JSON

```
{
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

---

## Rotator Endpoints

### Connection Status

**Check if rotator is connected:**

Code

```
GET /api/v1/rotator/0/connected
PUT /api/v1/rotator/0/connected
```

**Response:** Same format as focuser

### Device Name

**Get rotator name:**

Code

```
GET /api/v1/rotator/0/name
```

**Response:**

JSON

```
{
  "Value": "Dual-Controller Rotator",
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

### Current Position

**Get current rotator position (in steps):**

Code

```
GET /api/v1/rotator/0/position
```

**Response:**

JSON

```
{
  "Value": 0,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

**Note:** Position range is -6400 to +6400 steps (±2 full rotations).

### Movement Status

**Check if rotator is moving:**

Code

```
GET /api/v1/rotator/0/ismoving
```

**Response:** Same format as focuser

### Can Halt

**Check if rotator supports halt command:**

Code

```
GET /api/v1/rotator/0/canhalt
```

**Response:**

JSON

```
{
  "Value": true,
  "ClientTransactionID": 0,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

### Move to Absolute Position

**Command rotator to absolute angle:**

Code

```
PUT /api/v1/rotator/0/moveabsolute
```

**Parameters:**

- `Position` (required) - Target position in steps
- `ClientTransactionID` (optional)

**Example:**

Code

```
PUT /api/v1/rotator/0/moveabsolute?Position=1600&ClientTransactionID=2
```

**Response:**

JSON

```
{
  "ClientTransactionID": 2,
  "ErrorNumber": 0,
  "ErrorMessage": ""
}
```

**Important Notes:**

- Position will be clamped to ROT_LIMIT_MIN/MAX
- Movement is asynchronous
- 3200 steps = 1 full rotation

### Halt Movement

**Stop rotator immediately:**

Code

```
PUT /api/v1/rotator/0/halt
```

**Response:** Same format as focuser

---

## Integration Examples

### NINA (N.I.N.A.) Setup

1. **Open NINA Equipment Manager**
    
    - Go to Equipment → Add Device
    - Select "Alpaca" device type
    - Click "Search"
2. **Enter Connection Details**
    
    - Server: Your Pi's IP (e.g., 192.168.1.100)
    - Port: 8080
    - Click "Discover Devices"
3. **Select Devices**
    
    - Choose "Dual Controller Focuser"
    - Choose "Dual Controller Rotator"
    - Click "Add"
4. **Configure in NINA**
    
    - Set focuser as your active focuser
    - Set rotator as your active rotator
    - Configure autofocus settings

### Manual API Testing (cURL)

**Test connection:**

bash

```
curl http://192.168.1.100:8080/management/v1/description
```

**Move focuser to position 30000:**

bash

```
curl -X PUT "http://192.168.1.100:8080/api/v1/focuser/0/move?Position=30000"
```

**Get current focuser position:**

bash

```
curl http://192.168.1.100:8080/api/v1/focuser/0/position
```

**Rotate to 45 degrees (1/8 turn = 400 steps):**

bash

```
curl -X PUT "http://192.168.1.100:8080/api/v1/rotator/0/moveabsolute?Position=400"
```

**Stop all movement:**

bash

```
curl -X PUT http://192.168.1.100:8080/api/v1/focuser/0/halt
curl -X PUT http://192.168.1.100:8080/api/v1/rotator/0/halt
```

### Python Example

Python

```
import requests
import time

PI_IP = "192.168.1.100"
BASE_URL = f"http://{PI_IP}:8080"

# Move focuser to position 30000
response = requests.put(
    f"{BASE_URL}/api/v1/focuser/0/move",
    params={"Position": 30000}
)
print(f"Focuser move response: {response.json()}")

# Wait for movement to complete
while True:
    moving_response = requests.get(
        f"{BASE_URL}/api/v1/focuser/0/ismoving"
    )
    if not moving_response.json()["Value"]:
        break
    print("Moving...")
    time.sleep(0.5)

# Get final position
position_response = requests.get(
    f"{BASE_URL}/api/v1/focuser/0/position"
)
print(f"Final position: {position_response.json()['Value']}")

# Rotate
requests.put(
    f"{BASE_URL}/api/v1/rotator/0/moveabsolute",
    params={"Position": 1600}
)
```

---

## Error Handling

### Error Response Format

JSON

```
{
  "ClientTransactionID": 1,
  "ErrorNumber": 1,
  "ErrorMessage": "Position out of range"
}
```

### Common Error Codes

- **0** - No error
- **1** - Invalid parameter
- **2** - Device not connected
- **3** - Position out of range
- **999** - Driver error

### Error Scenarios

**Position out of range:**

- Attempting to move beyond FOC_LIMIT_MIN/MAX or ROT_LIMIT_MIN/MAX
- System automatically clamps to valid range

**Device not connected:**

- Motor driver not responding
- Check power supply and wiring
- Verify pigpiod daemon is running

---

## Response Format Specification

All responses follow ASCOM Alpaca standard:

JSON

```
{
  "Value": <any>,                    // Response data (varies by endpoint)
  "ClientTransactionID": <number>,   // Echo of client's request ID
  "ServerTransactionID": <number>,   // Server's transaction sequence
  "ErrorNumber": <number>,           // 0 = success, non-zero = error
  "ErrorMessage": "<string>"         // Error description (empty if success)
}
```

---

## Performance Characteristics

### Response Times

- **Status queries** (position, ismoving): < 50ms
- **Move commands** (async): < 100ms
- **Network latency** (local network): 1-10ms

### Movement Profiles

- **Typical focus move** (10,000 steps): 3-5 seconds
- **Full rotation** (3200 steps): 2-4 seconds
- **Speed adjustable** via config.h

### Concurrent Requests

- Focuser and rotator can move simultaneously
- Each has independent movement queue
- Requests are processed in order

---

## Debugging & Testing

### Test Server Availability

bash

```
curl -v http://<pi-ip>:8080/management/v1/description
```

### Monitor Real-Time Logs

bash

```
ssh pi@raspberrypi
tail -f night_log.txt
```

### Enable Debug Output

In `AlpacaServer.hpp`, uncomment debug statements:

C++

```
std::cout << "[NET] Focuser Move Threaded -> " << target << std::endl;
```

---

## Best Practices

1. **Always check `ismoving` before issuing new commands**
    
    Python
    
    ```
    while requests.get(f"{BASE_URL}/api/v1/focuser/0/ismoving").json()["Value"]:
        time.sleep(0.1)
    ```
    
2. **Use reasonable position increments**
    
    - Large jumps (> 10000 steps) risk mechanical shock
    - Use MaxIncrement (5000 steps) as guidance
3. **Implement connection retry logic**
    
    Python
    
    ```
    import time
    for attempt in range(3):
        try:
            response = requests.get(f"{BASE_URL}/management/v1/description", timeout=2)
            if response.status_code == 200:
                break
        except:
            time.sleep(1)
    ```
    
4. **Log all movements for troubleshooting**
    
    - Check `night_log.txt` on the Pi
    - Correlate with your observation timeline
5. **Halt before critical operations**
    
    Python
    
    ```
    requests.put(f"{BASE_URL}/api/v1/focuser/0/halt")
    requests.put(f"{BASE_URL}/api/v1/rotator/0/halt")
    time.sleep(0.5)
    ```
    

---

## Limitations & Known Issues

- Movement is asynchronous (no blocking calls)
- No intermediate position updates during movement
- No acceleration/deceleration feedback
- Position accuracy depends on STEPS_PER_KNOB_REV calibration
- Network timeout if Pi loses power/connectivity

---

## Troubleshooting

### "Connection refused" on port 8080

- Verify Pi is running and network is available
- Check firewall: `sudo ufw allow 8080`
- Restart service: `pkill LX200_DualController`

### NINA can't discover devices

- Ensure Alpaca server is running
- Verify IP address and port are correct
- Check that Pi is on same network as NINA machine

### Movements are very slow

- Check config.h motor speed settings
- Verify power supply is adequate (12-24V @ 2A)
- Run calibration test

### Position doesn't reach target

- Verify position is within limits
- Check for mechanical binding
- Run re-seat operation via CLI: `[R]`

---

## API Changelog

### Version 3.0 (Current)

- Full ASCOM Alpaca protocol support
- Improved connection handshaking
- MaxStep and MaxIncrement endpoints
- Better error handling

### Version 2.0

- Basic HTTP server
- Core move commands

### Version 1.0

- Initial release

---

## Additional Resources

- [ASCOM Alpaca Specification](https://ascom-standards.org/Help/Platform/html/T_ASCOM_Alpaca_Discovery.htm)
- [NINA Documentation](https://nighttime-imaging.eu/)
- Project README: [README.md](https://github.com/README.md)
- Configuration Guide: [CONFIGURATION.md](https://github.com/copilot/c/CONFIGURATION.md)

---

**Last Updated:** March 26, 2026  
**API Version:** 3.0  
**Server Version:** 4.11