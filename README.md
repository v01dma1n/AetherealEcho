# **Aethereal Echo: ESP32-Based Wi-Fi and Ping Monitor**

The Aethereal Echo is a project that combines modern technology with a vintage aesthetic. It utilizes an **ESP32** microcontroller to create a Wi-Fi and ping monitor, all housed within a nearly **100-year-old millivolt meter** original oak wood box.

## **Key Functions**

### **Wi-Fi Connection and Network Configuration**
*   Upon startup, the Aethereal Echo attempts to connect to a preconfigured local **Wi-Fi network**.
*   If the device cannot connect to the configured network, it activates an **Access Point (AP)**, broadcasting an SSID named "aethereal-echo".
*   Users can connect to this Wi-Fi network and access a configuration page at the IP address **192.168.4.1** via a web browser.
*   The configuration page allows users to input the local Wi-Fi network's name (SSID) and password. After submitting this information, the ESP32 will reboot and attempt to connect to the specified Wi-Fi network.

### **Pinging Predefined Servers**
*   The Aethereal Echo periodically pings a configurable server.
*   The time between pings can be set in the device's settings.
*   If a ping is successful, the meter's needle displays the **ping time** on its scale. A short **ping sound** is also played as confirmation.
*   If a ping times out, the meter indicates an "**Echo Timeout**" by moving to the maximum position on its scale (1000 mTicks), accompanied by a 'sad' sound.
*   After several consecutive failed pings, Aethereal Echo initiates an alert with a sound and **alternating red and blue flashing LEDs**.
*   Following an alert, the device automatically switches to the next configured **ping server** and reboots. There are four ping server addresses that can be configured.

### **Wi-Fi Access Monitoring**
*   After a successful initial connection to a Wi-Fi network, the Aethereal Echo continuously monitors its connection status.
*   If the Wi-Fi connection is lost, the device initiates an alert, which includes a sound and alternating red and blue flashing LEDs.
*   The device will attempt to reconnect to the Wi-Fi network or reboot if reconnection fails.

### **Touch Controls**
*   The Aethereal Echo has four touch-sensitive terminals on the front panel.
*   These terminals are used to control the device:
    *   **Activate Access Point**
    *   **Select Target Server IP for Pinging**
    *   **Turn On or Off Ping Sound**
    *   **Turn On or Off Alert Sound**
*   Each function is activated by a **'long touch'**, which is a touch of at least 3 seconds.
*   After a function is activated, the Aethereal Echo will confirm with a sound.
*   Value changes can be achieved by a **'short touch'**, which is a touch of less than 0.5 seconds.
*   The Access Point function does not have 'short touch' settings and is activated immediately by a 'long touch'.
*   Aetheral Echo confirms value change with a sound and meter indication.
*   When changing the target server IP, the device will play a sound, the meter will point to the selected server and a light will turn on in the color associated with the target server.
*   When turning the ping or alert sound on and off, the meter will point to 'Yes' or 'No', and a corresponding sound will play.

#### **Detailed Touch Control Instructions**

*   **Turning on the Access Point:** Use a 'long touch' on the Access Point touch terminal. Upon successful activation, the device will confirm with a sound. While the Access Point is running, you can exit and reboot the device with another 'long touch'. See "Access Point Captive Portal" for details.
*  **Changing the target ping server**: To enter ping server selection mode, use a 'long touch' on the Ping server touch terminal. The device will confirm with a sound, turn a light on in the color of the currently selected ping server, and the meter will indicate the ping server on its scale. To advance to the next ping server, use a 'short touch'. The selection will cycle through the 4 available addresses. You can exit the selection mode with a 'long touch' or by waiting 30 seconds. The device will then resume pinging.
*   **Turning on and off the Ping sound**: To enter the ping sound selection mode, use a 'long touch' on the Ping Sound touch terminal. The device will confirm with a sound, and the meter will indicate the current value ('Yes' or 'No') accompanied by a sound. Use a 'short touch' to toggle the ping sound on and off. The meter will adjust and a corresponding sound will play with each change. You can exit the selection mode with a 'long touch' or by waiting 30 seconds.
*   **Turning on and off the Alert sound**: To enter the alert sound selection mode, use a 'long touch' on the Alert Sound touch terminal. The device will confirm with a sound, and the meter will indicate the current value ('Yes' or 'No') accompanied by a sound. Use a 'short touch' to toggle the alert sound on and off. The meter will adjust and a corresponding sound will play with each change. You can exit the selection mode with a 'long touch' or by waiting 30 seconds.


### Turning on and off top light
-

### Receiving serial log through USB port
- 

