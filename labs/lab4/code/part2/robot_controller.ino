#include <WiFi.h>
#include <WebServer.h>
#include "robot_webpage.h"

// AP network credentials
const char *ssid = "ESP32_AP_1";          
const char *password = "1234567890";   

// Static IP configuration for the Access Point
IPAddress local_IP(192, 168, 1, 175);  
IPAddress gateway(192, 168, 1, 175);    
IPAddress subnet(255, 255, 255, 0);      

// Web server setup
WebServer ws(80);

// Pin definitions
const int MOTOR_1_CHANNEL_A = 18;
const int MOTOR_1_CHANNEL_B = 19;
const int MOTOR_1_DIR_1 = 0;
const int MOTOR_1_DIR_2 = 10;
const int MOTOR_1_PWM = 1;

const int MOTOR_2_CHANNEL_A = 4;
const int MOTOR_2_CHANNEL_B = 19;
const int MOTOR_2_DIR_1 = 7;
const int MOTOR_2_DIR_2 = 6;
const int MOTOR_2_PWM = 8;

// Constants for calculations
const float WHEEL_RADIUS = 0.0333; // meters
const float WHEEL_BASE = 0.185;    // meters (distance between wheels)
const float MAX_LINEAR_VELOCITY = 0.5; // m/s
const float MAX_ANGULAR_VELOCITY = 2.0; // rad/s
const int PPR = 710; // Pulses per revolution
const int SAMPLE_INTERVAL = 100; // ms

// Direction mapping structure
struct MotorCommand {
  float linear_vel;    // V in m/s
  float angle;         // θ in radians
  float time_to_turn;  // t in seconds
};

// Map directions to linear velocities and angles
const std::map<String, MotorCommand> DIRECTION_COMMANDS = {
  {"start",      { MAX_LINEAR_VELOCITY,  0.0,    1.0}}, 
  {"stop",       { 0.0,                 0.0,    1.0}},  
  {"forward",    { MAX_LINEAR_VELOCITY,  0.0,    1.0}},
  {"reverse",    {-MAX_LINEAR_VELOCITY,  0.0,    1.0}},
  {"left",       { 0.0,                 PI/2,   1.0}},
  {"right",      { 0.0,                -PI/2,   1.0}},
  {"topLeft",    { MAX_LINEAR_VELOCITY * 0.7,  PI/4,   0.5}},
  {"topRight",   { MAX_LINEAR_VELOCITY * 0.7, -PI/4,   0.5}},
  {"bottomLeft", {-MAX_LINEAR_VELOCITY * 0.7,  PI/4,   0.5}},
  {"bottomRight",{-MAX_LINEAR_VELOCITY * 0.7, -PI/4,   0.5}}
};

// Global variables for both motors
volatile long pulseCount1 = 0, pulseCount2 = 0;
unsigned long lastTime = 0;
float currentRPM1 = 0, currentRPM2 = 0;
float desiredRPM1 = 0, desiredRPM2 = 0;
int pwmValue1 = 0, pwmValue2 = 0;

// PD variables for both motors
struct PDController {
  float integral = 0;
  float prev_error = 0;
  float Kp = 0.5;
  float Kd = 0.01;
};

PDController pd1, pd2;

// Encoder interrupt handlers
void IRAM_ATTR handleEncoder1() {
  
  int A = digitalRead(MOTOR_1_CHANNEL_A);
  int B = digitalRead(MOTOR_1_CHANNEL_B);
  
  if (A == HIGH) {
      if (B == LOW) pulseCount1++;
      else pulseCount1--;
  } else {
      if (B == HIGH) pulseCount1--;
      else pulseCount1++;
  }
}

void IRAM_ATTR handleEncoder2() {
 
  int A = digitalRead(MOTOR_2_CHANNEL_A);
  int B = digitalRead(MOTOR_2_CHANNEL_B);
  
  if (A == HIGH) {
      if (B == LOW) pulseCount2++;
      else pulseCount2--;
  } else {
      if (B == HIGH) pulseCount2--;
      else pulseCount2++;
  }
}

// Calculate RPM for both motors
float calculateRPM(long pulseCount, float elapsedTime) {
    float rpm = (abs(pulseCount) * 60.0) / (PPR * elapsedTime);
    return rpm;
}

// PD control for both motors
int pdControl(float setpoint, float measured_value, PDController& pd) {
  
  float error = setpoint - measured_value;
  pd.integral += error * SAMPLE_INTERVAL / 1000.0;
  float derivative = (error - pd.prev_error) / (SAMPLE_INTERVAL / 1000.0);
  pd.prev_error = error;
  
  float output = pd.Kp * error + pd.Kd * derivative;
  return constrain(output, 0, 255);
}

void calculateWheelVelocities(float linear_vel, float angular_vel, float& omega_left, float& omega_right) {
  
  // Using the differential drive equations from the image
  omega_left = (linear_vel - (angular_vel * WHEEL_BASE / 2.0)) / WHEEL_RADIUS;
  omega_right = (linear_vel + (angular_vel * WHEEL_BASE / 2.0)) / WHEEL_RADIUS;
}

void handleSetMotor() {

  if (server.hasArg("dir")) {
    String direction = server.arg("dir");
    
    if (DIRECTION_COMMANDS.find(direction) != DIRECTION_COMMANDS.end()) {
        MotorCommand cmd = DIRECTION_COMMANDS[direction];

        if (direction == "start") {
          server.send(200, "text/html", "Motors started!");
        }

        // Special handling for stop command
        if (direction == "stop") {
            // Immediately stop both motors
            desiredLeftPWM = 0;
            desiredRightPWM = 0;
            ledcWrite(MOTOR_1_PWM, 0);
            ledcWrite(MOTOR_2_PWM, 0);
            
            server.send(200, "text/html", "Motors stopped!");
            return;
        }
        
        // Convert angle to angular velocity (ω = θ/t)
        float angular_velocity = cmd.angle / cmd.time_to_turn;
        
        // Calculate wheel velocities
        float omega_left, omega_right;
        calculateWheelVelocities(cmd.linear_vel, angular_velocity, omega_left, omega_right);
        
        // Convert to RPM
        desiredRPM1 = (omega_left * 60.0) / (2.0 * PI);
        desiredRPM2 = (omega_right * 60.0) / (2.0 * PI);
        
        server.send(200, "text/html", "OK");
    }
  }
}

void handleRoot() {
  // Send the page back
  ws.send(200, "text/html", body);
}

void setup() {
  Serial.begin(115200);

  // Set up ESP32 as Access Point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());    
    
  // Motor 1 setup
  pinMode(MOTOR_1_CHANNEL_A, INPUT_PULLUP);
  pinMode(MOTOR_1_CHANNEL_B, INPUT_PULLUP);
  pinMode(MOTOR_1_DIR_1, OUTPUT);
  pinMode(MOTOR_1_DIR_2, OUTPUT);
  pinMode(MOTOR_1_PWM, OUTPUT);

  // Motor 2 setup
  pinMode(MOTOR_2_CHANNEL_A, INPUT_PULLUP);
  pinMode(MOTOR_2_CHANNEL_B, INPUT_PULLUP);
  pinMode(MOTOR_2_DIR_1, OUTPUT);
  pinMode(MOTOR_2_DIR_2, OUTPUT);
  pinMode(MOTOR_2_PWM, OUTPUT);
  
  // Attach interrupts for both motors
  attachInterrupt(digitalPinToInterrupt(MOTOR_1_CHANNEL_A), handleEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTOR_1_CHANNEL_B), handleEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTOR_2_CHANNEL_A), handleEncoder2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTOR_2_CHANNEL_B), handleEncoder2, CHANGE);
  
  // Give the pwm pins a frequency and resolution
  ledcAttach(MOTOR_1_PWM, 5000, 8);
  ledcAttach(MOTOR_2_PWM, 5000, 8);

  // Set up web server
  ws.on("/", handleRoot);
  ws.on("/update", handleUpdate);
  ws.begin();  
  
  lastTime = millis();
}

void loop() {
  ws.handleClient();

  if (millis() - lastTime >= SAMPLE_INTERVAL) {
      noInterrupts();
      long currentCount1 = pulseCount1;
      long currentCount2 = pulseCount2;
      pulseCount1 = 0;
      pulseCount2 = 0;
      interrupts();
      
      float elapsedTime = (millis() - lastTime) / 1000.0;
      
      // Calculate current RPM for both motors
      currentRPM1 = calculateRPM(currentCount1, elapsedTime);
      currentRPM2 = calculateRPM(currentCount2, elapsedTime);
      
      // Apply PD control
      pwmValue1 = pdControl(desiredRPM1, currentRPM1, pd1);
      pwmValue2 = pdControl(desiredRPM2, currentRPM2, pd2);
      
      // Set motor directions
      digitalWrite(MOTOR_1_DIR_1, desiredRPM1 >= 0 ? HIGH : LOW);
      digitalWrite(MOTOR_1_DIR_2, desiredRPM1 >= 0 ? LOW : HIGH);
      digitalWrite(MOTOR_2_DIR_1, desiredRPM2 >= 0 ? HIGH : LOW);
      digitalWrite(MOTOR_2_DIR_2, desiredRPM2 >= 0 ? LOW : HIGH);
      
      // Apply PWM
      ledcWrite(MOTOR_1_PWM, pwmValue1);
      ledcWrite(MOTOR_2_PWM, pwmValue2);
      
      // Debug output
      Serial.printf("Motor1 - Desired: %.2f, Current: %.2f, PWM: %d\n", desiredRPM1, currentRPM1, pwmValue1);
      Serial.printf("Motor2 - Desired: %.2f, Current: %.2f, PWM: %d\n", desiredRPM2, currentRPM2, pwmValue2);
      
      lastTime = millis();
  }
}
