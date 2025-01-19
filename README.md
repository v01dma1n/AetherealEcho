# Aethereal Echo
Aethereal Echo is ESP32-based WFi and ping monitor embedded in vintage millivolt meter.
The purpose of this build was to utilize nearly 100 years old meter in oak wood box.

## Main Functions

### Wi-Fi Connection and Network Configuration
- The Aethereal Echo, powered by an ESP32, attempts to connect to a local Wi-Fi network upon startup.
- If the device fails to connect to the configured Wi-Fi network, it activates an Access Point (AP) with a captive portal. The AP's SSID is broadcast as "aethereal-echo".
- Users can connect to this Wi-Fi network and navigate to the IP address 192.168.4.1 using a web browser.
- A configuration page is presented allowing users to enter the local Wi-Fi network's name (SSID) and password. Upon submission, the ESP32 will reboot and attempt to connect to the specified Wi-Fi network.

### Pinging Predefined Servers
- Aethereal Echo periodically pings a configurable server, with the interval between pings defined in the settings.◦
- If a ping is successful, the meter's needle displays the ping time on its scale, and a short ping sound is played as confirmation.
- If a ping times out, the meter indicates an "Echo Timeout" by moving to 1000 mTicks (maximum scale position).
- After several consecutive failed pings, Aethereal Echo initiates an alert, which includes sound and alternating red and blue flashing LEDs.
- Following an alert, the device automatically switches to the next configured ping server and reboots.

### Wi-Fi Access Monitoring
- After a successful initial connection to a Wi-Fi network, the Aethereal Echo continuously monitors its connection status.
- If the Wi-Fi connection is lost, Aethereal Echo starts an alert with a sound and alternating red and blue flashing LEDs.
- The device will then attempt to reconnect to the WiFi network or reboot

### Using touch controls
- There are four touch termnials on the fron panel:
  - Activate Access Point
  - Select target server IP for ping
  - Turn on or off ping sound
  - Turn on or off alert sound
- Each of the functions is activated by 'long touch', which is at least 3 seconds
- After the function is activated, Aetheral Echo confirms with a sound.
- Value change can be achieved by 'short touch', which is less than 0.5 second
  - Note: the Access Point function does not have 'short touch' settings and it is activated immediately.
- Aethereal Echo confirms value change with a sound and meter indication
  - The target server IP change is confirmed by sound, meter pointing at given server (beacon) and ligth in corresponding color.
  - The sound on and off selected value is indicated by meter pointing at Yes or No and sound

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

- If ping is successful, the meter displays the value of the response in milliseconds up to 1000 on the logarithmic scale and the sound of 'ping' is played by the buzzer.
- If ping times out, the meter shows 1000 (Ping Timeout) and a sad sound is played.
- After several time outs, aethereal_echo switches to another server and reboots.

>>>>>>> e57f187 (Readme continued)
