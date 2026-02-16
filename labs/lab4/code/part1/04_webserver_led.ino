// took help from ChatGPT for this code

// include the necessary WiFi and Web modules
#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN 4 // define a new macro LED_PIN as GPIO pin 4 where the LED is connected
#define FREQ_MIN 3 // frequency lower cap is 3 Hz
#define FREQ_MAX 20 // frequency upper cap is 20 Hz

// WiFi credentials
const char *ssid = "SAAYUJ_ESP32_AP";
const char *password = "12345678";

WebServer server(80); // create a web server on port 80

int frequency = FREQ_MIN;
int duty_cycle = 0;


// HTML page with sliders for controlling duty cycle and frequency
void handleRoot(){
  String html = "<html><body style='display: flex; height: 100vh; font-size: 18px;'>"
                "<div style='text-align: center; padding: 20px;'>"
                "<h1>Saayuj's LED Control</h1>"
                "<label>Frequency:</label> "
                "<input type=\"range\" id=\"frequency\" min=\"" + String(FREQ_MIN) + "\" max=\"" + String(FREQ_MAX) + "\" value=\"" + String(frequency) + "\" "
                "oninput=\"updateValue('frequency', this.value); document.getElementById('frequencyValue').innerText = this.value + ' Hz';\"><br>"
                "<span id='frequencyValue'>" + String(frequency) + " Hz</span><br><br>"
                
                "<label>Duty Cycle:</label> "
                "<input type=\"range\" id=\"duty\" min=\"0\" max=\"255\" value=\"" + String(duty_cycle) + "\" "
                "oninput=\"updateValue('duty', this.value); document.getElementById('dutyValue').innerText = this.value \"><br>"
                "<span id='dutyValue'>" + String(duty_cycle) + " %</span><br><br>"
                
                "<script>"
                "function updateValue(param, value) {"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('GET', '/update?' + param + '=' + value, true);"
                "  xhr.send();"
                "}"
                "</script>"
                "</div>"
                "</body></html>";
  server.send(200, "text/html", html);
}


// update LED frequency and duty cycle on receiving webpage update
void handleUpdate(){
  if (server.hasArg("frequency")) {
    frequency = server.arg("frequency").toInt();
    ledcAttach(LED_PIN, frequency, 14); // set the LED frequency
    ledcWrite(LED_PIN, duty_cycle);
  }

  if (server.hasArg("duty")) {
    duty_cycle = server.arg("duty").toInt();
    ledcWrite(LED_PIN, duty_cycle); // set the LED duty cycle
  }

  server.send(204); // prevent page from reloading
}


void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200); // initialize the serial monitor at a specific baudrate

  // initialize WiFi as Access Point to be used for the webpage
  WiFi.softAP(ssid, password);
  Serial.print("IP address: "); // print IP address
  Serial.println(WiFi.softAPIP());

  // initialize LED frequency and duty
  ledcAttach(LED_PIN, frequency, 14); 
  ledcWrite(LED_PIN, duty_cycle);

  // set up web server routes using the functions defined
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);

  server.begin();  // start the server
}


void loop(){
  // put your main code here, to run repeatedly:
  server.handleClient(); // handle incoming webpage updates
}
