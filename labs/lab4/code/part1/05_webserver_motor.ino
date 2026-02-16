// took help from ChatGPT for this code

// include the necessary WiFi and Web modules
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define DIR1_PIN 4 // define a new macro DIR1_PIN as GPIO pin 4 which is connected to DIR1 pin of H-bridge
#define DIR2_PIN 5 // define a new macro DIR2_PIN as GPIO pin 5 which is connected to DIR1' pin of H-bridge
#define PWM_PIN 6 // define a new macro PWM_PIN as GPIO pin 6 which will supply PWM to H-Bridge

// WiFi credentials
const char *ssid = "SAAYUJ_ESP32_AP";
const char *password = "12345678";

WebServer server(80); // create a web server on port 80

// HTML page with slider for controlling motor speed and buttons for controlling motor direction
String htmlPage(){
  String page = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  page += "<title>ESP32 Motor Control</title></head><body><h2>ESP32 Motor Control</h2>";
  page += "<form action=\"/update\" method=\"get\">";
  page += "<label for=\"speed\">Duty Cycle (0%-100%):</label><br>";
  page += "<input type=\"range\" id=\"speed\" name=\"speed\" min=\"0\" max=\"255\" value=\"" + String(duty_cycle) + "\" onchange=\"updateSpeed(this.value)\"><span id=\"speedValue\">" + String(duty_cycle) + "</span><br><br>";
  page += "<button onclick=\"setDirection('forward')\">Forward</button>";
  page += "<button onclick=\"setDirection('reverse')\">Reverse</button>";
  page += "<input type=\"hidden\" id=\"direction\" name=\"direction\" value=\"" + String(direction ? "forward" : "reverse") + "\">";
  page += "<input type=\"submit\" value=\"Update\">";
  page += "</form>";
  page += "<script>function updateSpeed(val) {document.getElementById('speedValue').innerText = val;}";
  page += "function setDirection(dir) {document.getElementById('direction').value = dir;}</script>";
  page += "</body></html>";
  return page;
}


// update motor direction and speed on receiving webpage update
void handleUpdate() {
  if (server.hasArg("speed")) {
    int duty_cycle = server.arg("speed").toInt();
    ledcWrite(PWM_PIN, duty_cycle);  // set the PWM duty cycle or motor speed
  }

  if (server.hasArg("direction")) {
    bool direction = (server.arg("direction") == "forward");

    if(direction){ // turn motor clockwise
      digitalWrite(DIR1_PIN, HIGH);
      digitalWrite(DIR2_PIN, LOW);
    }
    else{ // turn motor anti-clockwise
      digitalWrite(DIR1_PIN, LOW);
      digitalWrite(DIR2_PIN, HIGH);
    }
  }
  
  server.send(200, "text/html", htmlPage()); // Refresh the page
}


// serve the webpage
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}


void setup() {
  // put your setup code here, to run once:
  // define the three pins used for the motor as outputs
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);

  Serial.begin(115200); // initialize the serial monitor at a specific baudrate

  ledcAttach(PWM_PIN, 15000, 8); // setup pin 6 to output a square wave with frequency 15 kHz and PWM resolution of 8 bits

  // initialize WiFi as Access Point to be used for the webpage
  WiFi.softAP(ssid, password);
  Serial.print("IP address: "); // print IP address
  Serial.println(WiFi.softAPIP());

  // set initial motor speed (duty cycle) and direction
  ledcWrite(PWM_PIN, 128);
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);

  // set up web server routes using the functions defined
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);

  server.begin(); // start the server
}


void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient(); // handle incoming webpage updates
}
