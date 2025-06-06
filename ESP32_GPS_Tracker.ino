#include <WiFi.h>
#include <TinyGPSPlus.h>
#include <ESP_Mail_Client.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// GPS Configuration
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

// WiFi Configuration
const char* ssid = "###################";
const char* password = "@@@@@@@@@@@@@@@@";

// Email Configuration
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "$$$$$$$$$$$$$$$$$$"
#define AUTHOR_PASSWORD "&&&&&&&&&&&&&&&&&&&&&"
#define RECIPIENT_EMAIL "*********************"

// Push Button
#define BUTTON_PIN 4

// Web Server
WebServer server(80);

// SMTP Object
SMTPSession smtp;

// GPS Data Structure
struct GPSData {
  double lat;
  double lng;
  int sats;
  double alt;
  double spd;
  String datetime;
};

GPSData currentGPS;
bool gpsValid = false;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  connectWiFi();
  setupWebServer();
  
  Serial.println("Setup complete");
}

void loop() {
  server.handleClient();
  updateGPSData();
  handleButtonPress();
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void updateGPSData() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        currentGPS.lat = gps.location.lat();
        currentGPS.lng = gps.location.lng();
        currentGPS.sats = gps.satellites.value();
        currentGPS.alt = gps.altitude.meters();
        currentGPS.spd = gps.speed.kmph();
        
        if (gps.date.isValid() && gps.time.isValid()) {
          char datetime[20];
          snprintf(datetime, sizeof(datetime), "%04d-%02d-%02d %02d:%02d:%02d",
                   gps.date.year(), gps.date.month(), gps.date.day(),
                   gps.time.hour(), gps.time.minute(), gps.time.second());
          currentGPS.datetime = datetime;
        }
        
        gpsValid = true;
      } else {
        gpsValid = false;
      }
    }
  }
}

void handleButtonPress() {
  static unsigned long lastPress = 0;
  
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 5000) {
    lastPress = millis();
    
    if (gpsValid) {
      String subject = "GPS Location Update";
      String body = generateEmailBody();
      sendEmail(subject, body);
      Serial.println("Email triggered by button press");
    } else {
      Serial.println("GPS not ready for email");
    }
  }
}

String generateEmailBody() {
  String mapLink = "https://www.google.com/maps?q=" + String(currentGPS.lat, 6) + "," + String(currentGPS.lng, 6);
  
  String body = "GPS Location Info:\n\n";
  body += "Latitude: " + String(currentGPS.lat, 6) + "\n";
  body += "Longitude: " + String(currentGPS.lng, 6) + "\n";
  body += "Satellites: " + String(currentGPS.sats) + "\n";
  body += "Altitude: " + String(currentGPS.alt) + " m\n";
  body += "Speed: " + String(currentGPS.spd) + " km/h\n";
  body += "Timestamp: " + currentGPS.datetime + "\n\n";
  body += "Google Maps: " + mapLink + "\n";
  
  return body;
}

void sendEmail(String subject, String body) {
  Serial.println("Preparing email...");
  
  smtp.debug(1);
  
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "ESP32 GPS Tracker";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = subject;
  message.addRecipient("Recipient", RECIPIENT_EMAIL);
  message.text.content = body.c_str();
  message.text.charSet = "utf-8";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session)) {
    Serial.println("SMTP Connection Error: " + smtp.errorReason());
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email: " + smtp.errorReason());
  } else {
    Serial.println("Email sent successfully");
  }
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = R"=====(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>GPS Tracker</title>
      <style>
        * {
          box-sizing: border-box;
          margin: 0;
          padding: 0;
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        body {
          background: linear-gradient(135deg, #1a2a6c, #b21f1f, #1a2a6c);
          color: white;
          min-height: 100vh;
          padding: 20px;
        }
        .container {
          max-width: 800px;
          margin: 0 auto;
          padding: 20px;
        }
        header {
          text-align: center;
          margin-bottom: 30px;
          padding: 20px;
          background: rgba(0, 0, 0, 0.3);
          border-radius: 15px;
          box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
        }
        h1 {
          font-size: 2.5rem;
          margin-bottom: 10px;
          text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
        }
        .status-card {
          background: rgba(0, 0, 0, 0.4);
          border-radius: 15px;
          padding: 25px;
          margin-bottom: 25px;
          box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }
        .card-grid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
          gap: 20px;
          margin-bottom: 25px;
        }
        .card {
          background: rgba(0, 0, 0, 0.4);
          border-radius: 12px;
          padding: 20px;
          text-align: center;
          box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
        }
        .card h3 {
          font-size: 1.3rem;
          margin-bottom: 15px;
          color: #ffcc00;
        }
        .card p {
          font-size: 1.8rem;
          font-weight: bold;
        }
        .btn-group {
          display: flex;
          justify-content: center;
          gap: 15px;
          flex-wrap: wrap;
        }
        .btn {
          background: linear-gradient(to right, #00c9ff, #92fe9d);
          border: none;
          color: #1a2a6c;
          padding: 15px 30px;
          text-align: center;
          text-decoration: none;
          display: inline-block;
          font-size: 1.2rem;
          font-weight: bold;
          margin: 10px 0;
          cursor: pointer;
          border-radius: 50px;
          box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
          transition: all 0.3s ease;
          min-width: 200px;
        }
        .btn:hover {
          transform: translateY(-3px);
          box-shadow: 0 6px 15px rgba(0, 0, 0, 0.3);
        }
        .btn:active {
          transform: translateY(1px);
        }
        .btn-email {
          background: linear-gradient(to right, #ff416c, #ff4b2b);
          color: white;
        }
        .map-link {
          display: block;
          background: rgba(255, 255, 255, 0.1);
          color: #00c9ff;
          padding: 15px;
          border-radius: 12px;
          margin: 20px 0;
          text-decoration: none;
          word-break: break-all;
          text-align: center;
          font-weight: bold;
          transition: background 0.3s;
        }
        .map-link:hover {
          background: rgba(255, 255, 255, 0.2);
          text-decoration: underline;
        }
        .gps-status {
          display: inline-block;
          width: 20px;
          height: 20px;
          border-radius: 50%;
          margin-left: 10px;
          vertical-align: middle;
        }
        .status-active {
          background: #00ff00;
          box-shadow: 0 0 10px #00ff00;
        }
        .status-inactive {
          background: #ff0000;
          box-shadow: 0 0 10px #ff0000;
        }
        @media (max-width: 600px) {
          .container {
            padding: 10px;
          }
          h1 {
            font-size: 2rem;
          }
          .btn {
            width: 100%;
          }
        }
      </style>
    </head>
    <body>
      <div class="container">
        <header>
          <h1>ESP32 GPS Tracker <span id="gpsStatus" class="gps-status status-inactive"></span></h1>
          <p>Real-time Location Monitoring System</p>
        </header>
        
        <div class="status-card">
          <h2>Current Status</h2>
          <div class="card-grid">
            <div class="card">
              <h3>Satellites</h3>
              <p id="satellites">0</p>
            </div>
            <div class="card">
              <h3>Altitude</h3>
              <p id="altitude">0 m</p>
            </div>
            <div class="card">
              <h3>Speed</h3>
              <p id="speed">0 km/h</p>
            </div>
          </div>
          
          <div class="card">
            <h3>Last Update</h3>
            <p id="timestamp">N/A</p>
          </div>
        </div>
        
        <div class="status-card">
          <h2>Coordinates</h2>
          <p><strong>Latitude: </strong><span id="latitude">0.000000</span></p>
          <p><strong>Longitude: </strong><span id="longitude">0.000000</span></p>
          
          <a id="mapLink" class="map-link" target="_blank">Open in Google Maps</a>
        </div>
        
        <div class="btn-group">
          <button class="btn" onclick="refreshData()">Refresh Data</button>
          <button class="btn btn-email" onclick="sendEmail()">Send Location Email</button>
        </div>
      </div>
      
      <script>
        function updateGPSData() {
          fetch('/gps-data')
            .then(response => response.json())
            .then(data => {
              if(data.valid) {
                document.getElementById('latitude').textContent = data.lat;
                document.getElementById('longitude').textContent = data.lng;
                document.getElementById('satellites').textContent = data.sats;
                document.getElementById('altitude').textContent = data.alt + ' m';
                document.getElementById('speed').textContent = data.spd + ' km/h';
                document.getElementById('timestamp').textContent = data.datetime;
                document.getElementById('mapLink').href = 
                  `https://www.google.com/maps?q=${data.lat},${data.lng}`;
                document.getElementById('gpsStatus').className = 'gps-status status-active';
              } else {
                document.getElementById('gpsStatus').className = 'gps-status status-inactive';
              }
            })
            .catch(error => console.error('Error:', error));
        }
        
        function refreshData() {
          updateGPSData();
        }
        
        function sendEmail() {
          fetch('/send-email')
            .then(response => response.text())
            .then(data => {
              alert(data);
            })
            .catch(error => console.error('Error:', error));
        }
        
        // Update data every 3 seconds
        setInterval(updateGPSData, 3000);
        window.onload = updateGPSData;
      </script>
    </body>
    </html>
    )=====";
    
    server.send(200, "text/html", html);
  });

  server.on("/gps-data", HTTP_GET, []() {
    StaticJsonDocument<200> doc;
    doc["valid"] = gpsValid;
    if (gpsValid) {
      doc["lat"] = String(currentGPS.lat, 6);
      doc["lng"] = String(currentGPS.lng, 6);
      doc["sats"] = currentGPS.sats;
      doc["alt"] = currentGPS.alt;
      doc["spd"] = currentGPS.spd;
      doc["datetime"] = currentGPS.datetime;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/send-email", HTTP_GET, []() {
    if (gpsValid) {
      String subject = "Web Triggered GPS Location";
      String body = generateEmailBody();
      sendEmail(subject, body);
      server.send(200, "text/plain", "Email sent successfully!");
    } else {
      server.send(500, "text/plain", "Error: GPS data not valid");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}
