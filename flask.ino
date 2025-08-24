#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "iPhone 16_iOS 18_1733";          // Your hotspot or WiFi network name
const char* password = "hana3427";    // Your WiFi password

// Replace with the IP address of your laptop running Flask.
// For example, if your laptop's IP is 192.168.216.204, then:
const char* flaskServerIP = "192.168.92.111";
const int flaskServerPort = 5000;  // Flask server port

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><head><title>AI Website Generator</title><style>";
  html += "body { background-color: #121212; color: #ffffff; font-family: 'Segoe UI', sans-serif; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; }";
  html += "h2 { color: #00ff88; }";
  html += "form { background: #1e1e1e; padding: 20px; border-radius: 15px; box-shadow: 0 0 15px #00ff88; }";
  html += "input[type='text'] { width: 300px; padding: 10px; font-size: 16px; border: none; border-radius: 10px; margin-bottom: 10px; }";
  html += "input[type='submit'] { background-color: #00ff88; border: none; padding: 10px 20px; border-radius: 10px; font-size: 16px; color: #121212; cursor: pointer; }";
  html += "input[type='submit']:hover { background-color: #00cc66; }";
  html += "</style></head><body>";
  html += "<h2>AI Website Generator</h2>";
  html += "<form action='/generate' method='get'>";
  html += "<input type='text' name='prompt' placeholder='Enter your website prompt...' required />";
  html += "<br><input type='submit' value='Generate' />";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleGenerate() {
  if (!server.hasArg("prompt")) {
    server.send(400, "text/plain", "Prompt not provided.");
    return;
  }
  
  String prompt = server.arg("prompt");
  
  // Build URL for Flask server using your laptop's IP
  String serverURL = "http://";
  serverURL += flaskServerIP;
  serverURL += ":";
  serverURL += String(flaskServerPort);
  serverURL += "/generate?prompt=";
  serverURL += prompt;  // Note: In production, you might need to URL-encode the prompt
  
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, serverURL);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    server.send(200, "text/html", payload);
  } else {
    server.send(500, "text/plain", "Failed to connect to AI backend.");
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected! ESP IP: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/generate", HTTP_GET, handleGenerate);
  server.begin();
}

void loop() {
  server.handleClient();
}
