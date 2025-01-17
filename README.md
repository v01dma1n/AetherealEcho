# AetherealEcho
ESP32-based WFi and ping monitor embedded in vintage millivolt meter.
The purpose of this build was to utilize nearly 100 years old wooden box with a great looking meter.

## Main Functions

### Connecting to WiFi and access configuration
- The aethereal_echo program implemented with ESP32 connects to a local WiFi after power on.
- If ESP32 cannot connect to the local WiFi, it starts an Access Point with a captive portal. The AP is advertised as aethereal_echo WiFi.
- User needs to connect to the AP WiFi and navigate to the IP address 192.168.4.1 with a browser.
- The page allows entering the WiFi name and password.  After page submission the ESP32 reboots and makes another attempt to connect to local WiFi.

### Pinging predefined servers
- The aethereal_echo pings one of the predefined servers.
- If ping is successful, the meter displays the value of the response in milliseconds up to 1000 on the logarithmic scale and the sound of 'ping' is played by the buzzer.
- If ping times out, the meter shows 1000 (Ping Timeout) and a sad sound is played.
- After several time outs, aethereal_echo switches to another server and reboots.

