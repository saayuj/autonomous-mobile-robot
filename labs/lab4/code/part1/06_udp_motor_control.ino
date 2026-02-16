// took help from ChatGPT and the UDP examples for this code

// include the necessary WiFi modules
#include <WiFi.h>
#include <WiFiUdp.h>

#define DIR1_PIN 4 // define a new macro DIR1_PIN as GPIO pin 4 which is connected to DIR1 pin of H-bridge
#define DIR2_PIN 5 // define a new macro DIR2_PIN as GPIO pin 5 which is connected to DIR1' pin of H-bridge
#define PWM_PIN 6 // define a new macro PWM_PIN as GPIO pin 6 which will supply PWM to H-Bridge
#define POT_PIN 0 // define a new macro POT_PIN as GPIO pin 0 where the potentiometer is connected
#define SWITCH_PIN 10 // define a new macro SWITCH_PIN as GPIO pin 10 where the switch is connected

const char *ssid = "GIAS_ESP32_AP"; // partner's ESP ssid
const char *password = "12345678";

WiFiUDP UDPServer;

IPAddress myIP(192, 168, 1, 109); // took my IP address from the IP address list given

// partner port and my port has to be same
const int myPort = 12345;            // Local UDP port
const int partnerPort = 12345;          // Partner UDP port


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

  // define the three pins used for the motor as outputs
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);

  pinMode(POT_PIN, INPUT); // define this pin as input
  pinMode(SWITCH_PIN, INPUT_PULLUP); // define this pin as input with pullup resistor

  ledcAttach(PWM_PIN, 15000, 8); // setup pin 6 to output a square wave with frequency 15 kHz and PWM resolution of 8 bits

  // set initial motor speed (duty cycle) and direction
  ledcWrite(PWM_PIN, 128);
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);
}


void loop() {
  // put your main code here, to run repeatedly:
  int duty_cycle = 0.0246 * analogRead(POT_PIN) - 0.6143; // define duty_cycle as a function of the potentiometer value (found mapping experimentally)

  bool switch_pressed = digitalRead(SWITCH_PIN) == LOW;  // check if switch is pressed

  // send the duty_cycle and switch_pressed to partner via a UDP packet
  UDPServer.beginPacket("192.168.1.108", partnerPort);
  UDPServer.printf("%d,%d", duty_cycle, switch_pressed);
  UDPServer.endPacket();

  Serial.printf("Sent Speed: %d, Direction: %s\n", duty_cycle, switch_pressed ? "Reverse" : "Forward"); // print sent values

  // receive the motor duty cycle and direction from partner via a UDP packet
  if(UDPServer.parsePacket()){
    char incomingPacket[255];
    int len = UDPServer.read(incomingPacket, 255);
    if(len > 0){
      incomingPacket[len] = '\0';

      int received_duty_cycle = 0;
      int received_dir = 0;
      sscanf(incomingPacket, "%d,%d", &received_duty_cycle, &received_dir);

      if(direction){ // turn motor clockwise
        digitalWrite(DIR1_PIN, HIGH);
        digitalWrite(DIR2_PIN, LOW);
      }
      else{ // turn motor anti-clockwise
        digitalWrite(DIR1_PIN, LOW);
        digitalWrite(DIR2_PIN, HIGH);
      }

      ledcWrite(PWM_PIN, received_duty_cycle); // set the PWM duty cycle or motor speed

      Serial.printf("Received Speed: %d, Direction: %s\n", received_duty_cycle, received_dir ? "Reverse" : "Forward");
    }
}
