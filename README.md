# ESP32 GPS Tracker with Web and Email 🌍

**Real-Time Location Monitoring with Web Interface and Email Notifications**

## Repository Structure
- `README.md`: This documentation file.
- `LICENSE`: MIT License file for legal use.
- `ESP32_GPS_Tracker.ino`: The main Arduino sketch containing the project code.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Applications](#applications)
- [Prerequisites](#prerequisites)
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
- [Usage](#usage)
- [Customization](#customization)
- [Troubleshooting](#troubleshooting)
- [Deployment Considerations](#deployment-considerations)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Overview
This project is an ESP32-based GPS tracker designed for real-time location monitoring. It seems likely to offer a robust solution for tracking locations using a GPS module, with data accessible via a web interface and email notifications. The system connects to WiFi to host a web server, displays GPS data (latitude, longitude, satellites, altitude, speed, and timestamp), and allows users to send location updates via email using a push button or web interface. Emails include a Google Maps link for easy visualization, making it ideal for various tracking applications.

## Features
- **Real-Time GPS Tracking**: Acquires latitude, longitude, satellites, altitude, speed, and timestamp using a GPS module (e.g., Neo-6M).
- **Web Interface**: Displays GPS data with automatic updates every 3 seconds, including a Google Maps link.
- **Email Notifications**: Sends location updates via email, triggered by a push button or web interface.
- **Push Button Control**: Triggers email notifications with a 5-second press.
- **WiFi Connectivity**: Connects to a local WiFi network to host the web server.
- **Google Maps Integration**: Provides clickable links to view locations on Google Maps.

## Applications
This project can be applied in various scenarios:
- **Vehicle Tracking**: Monitor cars, bikes, or fleet vehicles for real-time location updates, useful for logistics or personal use.
- **Personal Safety**: Share your location with family or friends during travel or emergencies for enhanced security.
- **Asset Monitoring**: Track valuable items or equipment to prevent loss or theft in logistics or personal settings.
- **IoT Projects**: Integrate location data into IoT systems for smart applications, such as home automation or industrial monitoring.
- **Outdoor Activities**: Log routes for hiking, cycling, or other adventures, ideal for outdoor enthusiasts.

## Prerequisites
To use this project, you should have:
- Basic knowledge of Arduino programming and ESP32 development.
- Familiarity with connecting hardware components (e.g., GPS modules, push buttons).
- A Gmail account for email notifications (with 2FA app password if enabled).
- Access to a WiFi network for connectivity.

## Hardware Setup

### Required Components
| Component                | Description                                      |
|--------------------------|--------------------------------------------------|
| **ESP32 Development Board** | Main microcontroller (e.g., ESP32 DevKitC).       |
| **GPS Module**           | Neo-6M or similar, compatible with 3.3V.         |
| **Push Button**          | Simple button for triggering email notifications. |
| **Jumper Wires**         | For connecting components.                       |
| **Breadboard** (optional)| For easier prototyping.                          |
| **Power Source**         | USB cable or 5V battery for portable use.        |

### Connections
| Component       | ESP32 Pin       | Notes                                      |
|-----------------|-----------------|--------------------------------------------|
| **GPS Module**  |                 |                                            |
| TX (Transmit)   | RXD2 (pin 16)   | Connects to ESP32's serial receive pin.    |
| RX (Receive)    | TXD2 (pin 17)   | Connects to ESP32's serial transmit pin.   |
| GND             | GND             | Common ground.                            |
| VCC             | 3.3V            | Verify module's voltage (Neo-6M uses 3.3V).|
| **Push Button** |                 |                                            |
| One leg         | Pin 4 (BUTTON_PIN) | Uses internal pull-up resistor (`INPUT_PULLUP`). |
| Other leg       | GND             | No external resistor needed.               |
| **Power**       |                 |                                            |
| USB/Battery     | 5V/VIN          | Ensure sufficient current for ESP32 and GPS.|

### Notes
- Ensure the GPS module has a clear view of the sky for optimal satellite acquisition.
- Verify the GPS module’s voltage requirements; some modules may require 5V instead of 3.3V.
- Use a stable power source to avoid interruptions during operation.

## Software Setup

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) installed.
- ESP32 board support added to Arduino IDE.

### Installing ESP32 Board Support
1. Open Arduino IDE.
2. Go to `File` > `Preferences`.
3. In "Additional Board Manager URLs," add: `https://raw.githubusercontent.com/espressif/arduino-esp32/master/package_esp32_index.json`.
4. Click "OK."
5. Go to `Tools` > `Board` > `Boards Manager`.
6. Search for "ESP32" and install the latest version by Espressif Systems.

### Installing Required Libraries
1. Open Arduino IDE.
2. Go to `Sketch` > `Include Library` > `Manage Libraries`.
3. Search and install:
   - [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) by Mikal Hart (parses GPS data).
   - [ESP Mail Client](https://github.com/mobizt/ESP-Mail-Client) by Mobizt (handles email sending).
   - [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoit Blanchon (manages JSON data for web server).
4. Note: The `WebServer` library is included in the ESP32 Arduino core.

### Configuring the Code
1. Open `ESP32_GPS_Tracker.ino` in Arduino IDE.
2. Update WiFi credentials:
   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_Password";
   ```
3. Update email credentials:
   ```cpp
   #define AUTHOR_EMAIL "your_email@gmail.com"
   #define AUTHOR_PASSWORD "your_app_password"
   #define RECIPIENT_EMAIL "recipient_email@gmail.com"
   ```
4. For Gmail with two-factor authentication (2FA), generate an [app password](https://support.google.com/accounts/answer/185833) from your Google Account settings and use it for `AUTHOR_PASSWORD`.

### Uploading to ESP32
1. Connect the ESP32 to your computer via USB.
2. Select your ESP32 board from `Tools` > `Board` (e.g., "ESP32 Dev Module").
3. Select the correct port from `Tools` > `Port`.
4. Click the upload button to flash the sketch.

## Usage

### Accessing the Web Interface
1. After uploading, open the Serial Monitor in Arduino IDE (set to 115200 baud) to view the ESP32’s IP address printed during setup (e.g., "Connected! IP: 192.168.1.100").
2. Open a web browser on a device connected to the same WiFi network and navigate to the IP address (e.g., `http://192.168.1.100`).
3. The web interface displays:
   - **GPS Status**: Green (active) if data is valid, red (inactive) if not.
   - **Satellites**: Number of satellites in view.
   - **Altitude**: Current altitude in meters.
   - **Speed**: Current speed in km/h.
   - **Timestamp**: Current date and time.
   - **Latitude and Longitude**: Current coordinates.
   - **Google Maps Link**: Clickable link to view the location on Google Maps.
4. The page refreshes every 3 seconds for real-time updates.
5. Use the "Refresh Data" button to manually update the display.
6. Use the "Send Location Email" button to send an email with the current location.

### Using the Push Button
- Press and hold the push button for more than 5 seconds.
- If GPS data is valid, an email with the current location and a Google Maps link is sent to the configured recipient.

### Expected Output
- **Serial Monitor**: Displays connection status, IP address, and GPS data updates.
- **Web Interface**: Shows real-time GPS data with a user-friendly layout.
- **Email**: Contains location details (latitude, longitude, altitude, speed, timestamp) and a Google Maps link.

## Customization
You can modify the project to suit specific needs:
- **Change Email Recipient**: Update `RECIPIENT_EMAIL` in the code.
- **Change WiFi Network**: Update `ssid` and `password` variables.
- **Modify Web Interface**: Edit the HTML and JavaScript in `setupWebServer()` to change the layout or add features.
- **Adjust Update Interval**: Modify the JavaScript `setInterval(updateGPSData, 3000)` to change the refresh rate.
- **Use Different GPS Module**: Adjust pin assignments (e.g., RXD2, TXD2) and library if using a module other than Neo-6M.
- **Add Sensors**: Integrate additional sensors (e.g., temperature, humidity) and display their data on the web interface.

## Troubleshooting
| Issue                     | Possible Cause                     | Solution                                                                 |
|---------------------------|------------------------------------|--------------------------------------------------------------------------|
| **GPS Data Not Valid**    | Incorrect wiring or no satellite fix | Verify wiring (TX to RXD2, RX to TXD2, GND, VCC). Ensure clear sky view. Wait 1-5 minutes for a fix. |
| **Email Not Sending**     | Incorrect credentials or no internet | Check `AUTHOR_EMAIL` and `AUTHOR_PASSWORD`. Use an app password for Gmail 2FA. Confirm WiFi connectivity. |
| **Web Server Not Accessible** | WiFi not connected or firewall issues | Verify WiFi connection via Serial Monitor. Ensure port 80 is open. Check IP address. |
| **Button Not Triggering Email** | Debounce issue or invalid GPS data | Ensure button is held for 5+ seconds. Check GPS validity in Serial Monitor. |

## Deployment Considerations
For real-world use:
- **Power Supply**: Use a USB cable for development or a 5V battery for portability. Ensure sufficient current for the ESP32 and GPS module.
- **Enclosure**: Protect components with a weatherproof enclosure for outdoor use.
- **Antenna**: Position the GPS module’s antenna for optimal satellite reception.
- **Security**: Secure your WiFi network and keep email credentials confidential. Avoid hardcoding sensitive data in shared code.

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch for your changes.
3. Make and test your modifications.
4. Submit a pull request with a clear description of your changes.
Ensure code is well-documented and follows the project’s structure.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact
For questions, issues, or feature requests, please create a [GitHub issue](https://github.com/AbdulHasib-IRE/ESP32-GPS-Tracker/issues).
