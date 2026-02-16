#include <WiFi.h>
#include <Wire.h>
#include <WebServer.h>
#include "robot_webpage.h"
#include <ESP32Servo.h>

// AP network credentials
const char *ssid = "ESP32_AP_1";          
const char *password = "1234567890"; 

// Static IP configuration for the Access Point
IPAddress local_IP(192, 168, 1, 175);  
IPAddress gateway(192, 168, 1, 175);    
IPAddress subnet(255, 255, 255, 0);      

// Web server setup
WebServer ws(80);

// Servo setup
int servoPin = 20;  
Servo myServo;  

// I2C setup
#define I2C_SLAVE_ADDR 0x28
#define SDA_PIN 38
#define SCL_PIN 37

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 500; 
volatile uint32_t wifiPacketCount = 0;

// Pin definitions for four motors
#define PWM_1 41
#define PWM_2 4
#define PWM_3 1
#define PWM_4 14
#define FORWARD_1 2
#define BACKWARD_1 42
#define FORWARD_2 12
#define BACKWARD_2 13
#define FORWARD_3 39
#define BACKWARD_3 40
#define FORWARD_4 10
#define BACKWARD_4 11
#define WIFI_CHANNEL 4

#define LEFT_XSHUT_PIN 18
#define FRONT_XSHUT_PIN 6

#define SDA 7
#define SCL 15

void send_I2C_byte(uint8_t data) {
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(data);
  uint8_t error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("Data sent successfully");
    rgbLedWrite(2, 0, 20, 0);  // green
  } else {
    Serial.printf("Error sending data: %d\n", error);
    rgbLedWrite(2, 20, 0, 0);  // red
  }
}

uint8_t receive_I2C_byte() {
  uint8_t bytesReceived = Wire.requestFrom(I2C_SLAVE_ADDR, 1);
  uint8_t byteIn = 0;

  if (bytesReceived > 0) {
    byteIn = Wire.read();
    Serial.printf("Received from slave: 0x%02X\n", byteIn);
  }
  return byteIn;
}

void moveBackward(VL53L0X_RangingMeasurementData_t frontMeasure) {
  ledcWrite(PWM_1, 180);
  ledcWrite(PWM_2, 180);
  ledcWrite(PWM_3, 180);
  ledcWrite(PWM_4, 180);

  digitalWrite(FORWARD_1, LOW);
  digitalWrite(BACKWARD_1, HIGH);
  digitalWrite(FORWARD_2, LOW);
  digitalWrite(BACKWARD_2, HIGH);
  digitalWrite(FORWARD_3, LOW);
  digitalWrite(BACKWARD_3, HIGH);
  digitalWrite(FORWARD_4, LOW);
  digitalWrite(BACKWARD_4, HIGH);
  
  Serial.println("Move backward");

  // Keep adjusting until within range
  while (true) {
    lox1.rangingTest(&frontMeasure, false);
    
    if (frontMeasure.RangeStatus != 4) {
      int frontDistance = frontMeasure.RangeMilliMeter;
      Serial.print("Sensor - Distance (mm): ");
      Serial.println(frontDistance);
      
      if (frontDistance >= FRONT_WALL_THRESHOLD) break; 
      else Serial.println("Sensor - Out of range");
    
    delay(50); 
    }
  }
}

void adjustLeft(VL53L0X_RangingMeasurementData_t leftMeasure) {
  Serial.println("Adjusting to the left");
  
  ledcWrite(PWM_1, 0);
  ledcWrite(PWM_2, 180);
  ledcWrite(PWM_3, 0);
  ledcWrite(PWM_4, 180);

  digitalWrite(FORWARD_1, LOW);
  digitalWrite(BACKWARD_1, LOW);
  digitalWrite(FORWARD_2, HIGH);
  digitalWrite(BACKWARD_2, LOW);
  digitalWrite(FORWARD_3, LOW);
  digitalWrite(BACKWARD_3, LOW);
  digitalWrite(FORWARD_4, HIGH);
  digitalWrite(BACKWARD_4, LOW);

  // Keep adjusting until within range
  while (true) {
    lox1.rangingTest(&leftMeasure, false);
    
    if (leftMeasure.RangeStatus != 4) {
      int leftDistance = leftMeasure.RangeMilliMeter;
      Serial.print("Left Sensor - Distance (mm): ");
      Serial.println(leftDistance);
      
      if (leftDistance >= MIN_WALL_DISTANCE && leftDistance <= MAX_WALL_DISTANCE) break; 
      else Serial.println("Left Sensor - Out of range");
    
    delay(50); 
    } 

  // Return to forward motion
  setMotorSpeed("forward");
  }
}

void adjustRight(VL53L0X_RangingMeasurementData_t leftMeasure) {
  Serial.println("Adjusting to the right");
  
  ledcWrite(PWM_1, 180);
  ledcWrite(PWM_2, 0);
  ledcWrite(PWM_3, 180);
  ledcWrite(PWM_4, 0);

  digitalWrite(FORWARD_1, HIGH);
  digitalWrite(BACKWARD_1, LOW);
  digitalWrite(FORWARD_2, LOW);
  digitalWrite(BACKWARD_2, LOW);
  digitalWrite(FORWARD_3, HIGH);
  digitalWrite(BACKWARD_3, LOW);
  digitalWrite(FORWARD_4, LOW);
  digitalWrite(BACKWARD_4, LOW);

  // Keep adjusting until within range
  while (true) {
    lox1.rangingTest(&leftMeasure, false);
    
    if (leftMeasure.RangeStatus != 4) {
      int leftDistance = leftMeasure.RangeMilliMeter;
      Serial.print("Left Sensor - Distance (mm): ");
      Serial.println(leftDistance);
      
      if (leftDistance >= MIN_WALL_DISTANCE && leftDistance <= MAX_WALL_DISTANCE) break;
      else Serial.println("Left Sensor (Adjust - Out of range");
    }
    
    delay(50);
  }

  // Return to forward motion
  setMotorSpeed("forward");
}

void handleTargets() {
  Serial.println("Attack targets");
  incrementWifiPacketCount();
}


void handleWall() {
  incrementWifiPacketCount();

  // Get ToF sensor measurements
  VL53L0X_RangingMeasurementData_t leftMeasure;
  VL53L0X_RangingMeasurementData_t frontMeasure;

  // Perform ranging measurements for both sensors
  lox1.rangingTest(&leftMeasure, false);
  lox2.rangingTest(&frontMeasure, false);

  // Adjust motion based on sensor outputs
  if (frontMeasure.RangeMilliMeter < FRONT_WALL_THRESHOLD) moveBackward(frontMeasure);
  delay(100);
  
  if (leftMeasure.RangeMilliMeter < MIN_WALL_DISTANCE) adjustRight(leftMeasure);
  delay(100);

  if (leftMeasure.RangeMilliMeter > MAX_WALL_DISTANCE) adjustLeft(leftMeasure);
  delay(100);
  
  setMotorSpeed("forward");
  delay(100);
}

void setMotorSpeed(String direction) {
  if (direction == "forward") {
    // All motors forward
    ledcWrite(PWM_1, 160);
    ledcWrite(PWM_2, 160);
    ledcWrite(PWM_3, 160);
    ledcWrite(PWM_4, 160);

    digitalWrite(FORWARD_1, HIGH);
    digitalWrite(BACKWARD_1, LOW);
    digitalWrite(FORWARD_2, HIGH);
    digitalWrite(BACKWARD_2, LOW);
    digitalWrite(FORWARD_3, HIGH);
    digitalWrite(BACKWARD_3, LOW);
    digitalWrite(FORWARD_4, HIGH);
    digitalWrite(BACKWARD_4, LOW);
  }
  else if (direction == "backward") {
    // All motors backward
    ledcWrite(PWM_1, 160);
    ledcWrite(PWM_2, 160);
    ledcWrite(PWM_3, 160);
    ledcWrite(PWM_4, 160);

    digitalWrite(FORWARD_1, LOW);
    digitalWrite(BACKWARD_1, HIGH);
    digitalWrite(FORWARD_2, LOW);
    digitalWrite(BACKWARD_2, HIGH);
    digitalWrite(FORWARD_3, LOW);
    digitalWrite(BACKWARD_3, HIGH);
    digitalWrite(FORWARD_4, LOW);
    digitalWrite(BACKWARD_4, HIGH);
  }
  else if (direction == "left") {
    // Left turn 
    ledcWrite(PWM_1, 255);
    ledcWrite(PWM_2, 0);
    ledcWrite(PWM_3, 255);
    ledcWrite(PWM_4, 0);

    digitalWrite(FORWARD_1, HIGH);
    digitalWrite(BACKWARD_1, LOW);
    digitalWrite(FORWARD_2, LOW);
    digitalWrite(BACKWARD_2, LOW);
    digitalWrite(FORWARD_3, HIGH);
    digitalWrite(BACKWARD_3, LOW);
    digitalWrite(FORWARD_4, LOW);
    digitalWrite(BACKWARD_4, LOW);
  }
  else if (direction == "right") {
    // Right turn
    ledcWrite(PWM_1, 0);
    ledcWrite(PWM_2, 255);
    ledcWrite(PWM_3, 0);
    ledcWrite(PWM_4, 255);

    digitalWrite(FORWARD_1, LOW);
    digitalWrite(BACKWARD_1, LOW);
    digitalWrite(FORWARD_2, HIGH);
    digitalWrite(BACKWARD_2, LOW);
    digitalWrite(FORWARD_3, LOW);
    digitalWrite(BACKWARD_3, LOW);
    digitalWrite(FORWARD_4, HIGH);
    digitalWrite(BACKWARD_4, LOW);
  }
  else if (direction == "stop") {
    // Stop all motors
    ledcWrite(PWM_1, 0);
    ledcWrite(PWM_2, 0);
    ledcWrite(PWM_3, 0);
    ledcWrite(PWM_4, 0);
  }
}

void handleSetMotor() {
  if (ws.hasArg("dir")) {
    // incrementWifiPacketCount();
    String direction = ws.arg("dir");
    setMotorSpeed(direction);
    ws.send(200, "text/html", "OK");
  }

  incrementWifiPacketCount();
}

void handleSetServo() {
  setMotorSpeed("stop");
  for (int angle = 0; angle <= 180; angle++) {
    myServo.write(angle); 
    delay(5);          
  }

  for (int angle = 180; angle >= 0; angle--) {
    myServo.write(angle);
    delay(5);           
  }

  incrementWifiPacketCount();
}

void handleRoot() {
  ws.send(200, "text/html", body);
  incrementWifiPacketCount();
}

void setup() {
  Serial.begin(115200);   

  Wire.begin(7, 15);

  // Set XSHUT pins as outputs
  pinMode(LEFT_XSHUT_PIN, OUTPUT);
  pinMode(FRONT_XSHUT_PIN, OUTPUT);
  // pinMode(XSHUT_PIN3, OUTPUT);

  // Reset both sensors by setting XSHUT pins low
  digitalWrite(LEFT_XSHUT_PIN, LOW);
  digitalWrite(FRONT_XSHUT_PIN, LOW);
  delay(10);

  // Initialize Sensor 1
  digitalWrite(LEFT_XSHUT_PIN, HIGH);
  delay(10);
  if (!lox1.begin(LEFT_SENSOR_ADDRESS)) {
    Serial.println("Failed to initialize first sensor!");
    while(1);
  }

  // Initialize Sensor 2
  digitalWrite(FRONT_XSHUT_PIN, HIGH);
  delay(10);
  if (!lox2.begin(FRONT_SENSOR_ADDRESS)) {
    Serial.println("Failed to initialize second sensor!");
    while(1);
  }

  // WiFi
  WiFi.softAP(ssid, password, WIFI_CHANNEL);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());   

  // I2C
  Wire.begin(SDA_PIN, SCL_PIN, 40000);
  Serial.println("ESP32-C3 I2C Master initialized");
  Serial.printf("SDA: %d, SCL: %d\n", SDA_PIN, SCL_PIN);
    
  // Setup for all four motors
  pinMode(FORWARD_1, OUTPUT);
  pinMode(BACKWARD_1, OUTPUT);
  pinMode(PWM_1, OUTPUT);

  pinMode(FORWARD_2, OUTPUT);
  pinMode(BACKWARD_2, OUTPUT);
  pinMode(PWM_2, OUTPUT);

  pinMode(FORWARD_3, OUTPUT);
  pinMode(BACKWARD_3, OUTPUT);
  pinMode(PWM_3, OUTPUT);

  pinMode(FORWARD_4, OUTPUT);
  pinMode(BACKWARD_4, OUTPUT);
  pinMode(PWM_4, OUTPUT);
  
  // Set PWM properties for all motors
  ledcAttach(PWM_1, 5000, 8);
  ledcAttach(PWM_2, 5000, 8);
  ledcAttach(PWM_3, 5000, 8);
  ledcAttach(PWM_4, 5000, 8);

  // Initialize all motors to stop
  ledcWrite(PWM_1, 0);
  ledcWrite(PWM_2, 0);
  ledcWrite(PWM_3, 0);
  ledcWrite(PWM_4, 0);

  // Set up the servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Attach the servo to the pin with minimum and maximum pulse widths
  myServo.attach(servoPin, 500, 2400);  

  // Set up web server
  ws.on("/", handleRoot);
  ws.on("/update", handleSetMotor);
  ws.on("/servo", handleSetServo);
  ws.on("/targets", handleTargets); 
  ws.on("/wall", handleWall); 
  ws.begin();  
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSendTime >= sendInterval) {
    // Send WiFi packet count to slave
    if (wifiPacketCount % 5 == 0) send_I2C_byte(wifiPacketCount & 0xFF);
    
    // Receive response from slave
    uint8_t response = receive_I2C_byte();
    
    // Check if bot should stop
    if (response == 0) {
      Serial.println("Received stop signal. Bot stopping.");
      setMotorSpeed("stop");

      while(1) {
        rgbLedWrite(2, 20, 0, 0);  // red
        delay(500);
        rgbLedWrite(2, 0, 0, 0);  // off
        delay(500);
      }
    }
    
    lastSendTime = currentTime;
    rgbLedWrite(2, 0, 0, 0);  // off
  }

  ws.handleClient();
}

// Function to update WiFi packet count (call this when a packet is received)
void incrementWifiPacketCount() {
  wifiPacketCount++;
}