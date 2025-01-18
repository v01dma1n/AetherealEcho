# AetherealEcho
ESP32-based WFi and ping monitor embedded in vintage millivolt meter.
The purpose of this build was to utilize nearly 100 years old wooden box with a great looking meter.

## Main Functions

### Connecting to WiFi and network access configuration
- The aethereal_echo program implemented with ESP32 connects to a local WiFi after power on.
- If ESP32 cannot connect to the local WiFi, it starts an Access Point with a captive portal. The AP is advertised as aethereal_echo WiFi.
- User needs to connect to the AP WiFi and navigate to the IP address 192.168.4.1 with a browser.
- The page allows entering the WiFi name and password.  After page submission the ESP32 reboots and makes another attempt to connect to local WiFi.

### Pinging predefined servers
- The aethereal_echo pings selected server with a period defined in the configuration.
- If the ping is successful, the meter indicates ping time on the scale and AE confirms with a ping sound
- If the ping times out, the meter indicats "Echo Timeout" at 1000 mTicks
  - After several unsuccessfult pings, aethereal_echo starts an alert with sound and flashing blue and red lights.
  - Afterwards, it switches to the next IP and reboots.

#### Monitoring WiFi access
- After initial successful connection to WiFi aethereal_echo monitors network connection
- If the connecion to WiFi fails, aethereal_echo starts an alert with sound and flashing blue and red lights.
- Afterwards, aethereal_echo reboots.

### Using touch controls
- 

#### Turning on the Access Point
- 

#### Changing the target ping server
- 

#### Turning on and off ping sound
- 

#### Turning on and off alert sound
- 

### Turning on and off top light
- 
### Receiving serial log through USB port
- 




