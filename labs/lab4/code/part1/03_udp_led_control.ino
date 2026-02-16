// took help from ChatGPT and the UDP examples for this code

// include the necessary WiFi modules
#include <WiFi.h>
#include <WiFiUdp.h>

#define LED_PIN 4 // define a new macro LED_PIN as GPIO pin 4 where the LED is connected
#define POT_PIN 0 // define a new macro POT_PIN as GPIO pin 0 where the potentiometer is connected

const char *ssid = "GIAS_ESP32_AP"; // partner's ESP ssid
const char *password = "12345678"; 

WiFiUDP UDPServer;

IPAddress myIP(192, 168, 1, 109); // took my IP address from the IP address list given

// partner port and my port has to be same
const int myPort = 12345;   
const int partnerPort = 12345;             


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // initialize the serial monitor at a specific baudrate

  WiFi.config(local_IP, // device IP address
              IPAddress(192, 168, 1, 109), // gateway (not important for 5100)
              IPAddress(255, 255, 255, 0))) // net mask

  WiFi.begin(ssid, password); // connecting to the Access Point server
  Serial.print("Connecting to AP");

  while (WiFi.status() != WL_CONNECTED) { // just wait if not connected
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to AP"); // print if connected

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  UDPServer.begin(myPort); // starting UDP on this port
  Serial.printf("Starting UDP port %d\n", myPort);

  pinMode(LED_PIN, OUTPUT); // define this pin as output
  pinMode(POT_PIN, INPUT); // define this pin as input

  ledcAttach(LED_PIN, 20, 8); // setup pin 4 to output a square wave with frequency 20 Hz and PWM resolution of 8 bits
}


void loop() {
  // put your main code here, to run repeatedly:
  int duty_cycle = 0.0246 * analogRead(POT_PIN) - 0.6143; // define duty_cycle as a function of the potentiometer value (found mapping experimentally)

  // send the duty_cycle to partner via a UDP packet
  UDPServer.beginPacket("192.168.1.108", partnerPort);
  UDPServer.printf("%d", duty_cycle);
  UDPServer.endPacket();

  // receive the duty cycle from partner via a UDP packet
  if(UDPServer.parsePacket()){
    char incoming_packet[255];
    int len = UDPServer.read(incoming_packet, 255);
    if (len > 0) {
      incoming_packet[len] = '\0';
      
      int received_duty_cycle = atoi(incoming_packet);
      Serial.printf("Received duty cycle: %d\n", received_duty_cycle);

      ledcWrite(LED_PIN, received_duty_cycle); // set the PWM duty cycle
    }
  }
}