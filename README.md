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
- If a ping times out, the meter indicates an "Echo Timeout" by moving to 1000 mTicks (maximum scale position) and a 'sad' sound is played.
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
- To turn on the Access Point, use 'long touch' with Access Point touch terminal.
  - Upon successful Acces Point activation, Aethereal Echo will confirm with a sound.
- While the Access Point is running, you can use 'long touch' to exit and reboot Aethereal Echo
- See Access Point Captive Portal for details on how to use it.

#### Changing the target ping server
- To activave selection of ping server, use 'long touch' with Ping server touch terminal.
  - Upon successful selection mode activation, Aethereal Echo will confirm with a sound.
  - Aethereal Echo will turn ligth to a color of the currently select ping sever and meter will indicate the ping server on the scale
- To advance to the next ping server, use 'short touch'
- Aethereal Echo will turn ligth to a color of the currently select ping sever and meter will indicate the ping server on the scale.
- The ping server address selection happens in round robin fashion for all 4 addresses
- You can use 'long touch' to get back to pinging or just wait 30 seconds for the device to start pinging on its own

#### Turning on and off ping sound
- To activave selection of Ping Sound switching, use 'long touch' with Ping Sound touch terminal.
   - Upon successful selection mode activation, Aethereal Echo will confirm with a sound.
   - Aethereal Echo will point the meter to the currently selected value (Yes/No) and play corresponding sound.
- To switch the Ping Sound on and off, use 'short touch'.  Eevery time the device will adjust the meter and play corresponding sound.
- You can use 'long touch' to get back to pinging or just wait 30 seconds for the device to start pinging on its own
  
#### Turning on and off alert sound
- To activave selection of Alert Sound switching, use 'long touch' with Alert Sound touch terminal.
   - Upon successful selection mode activation, Aethereal Echo will confirm with a sound.
   - Aethereal Echo will point the meter to the currently selected value (Yes/No) and play corresponding sound.
- To switch the Alert Sound on and off, use 'short touch'.  Eevery time the device will adjust the meter and play corresponding sound.
- You can use 'long touch' to get back to pinging or just wait 30 seconds for the device to start pinging on its own

### Turning on and off top light
-

### Receiving serial log through USB port
- 

