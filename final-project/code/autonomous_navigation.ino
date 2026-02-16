#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include "vive510.h"

#define signalPin 35
Vive510 vive1(SIGNALPIN1);

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

#define LEFT_SENSOR_ADDRESS 0x30
#define FRONT_SENSOR_ADDRESS 0x31
#define RIGHT_SENSOR_ADDRESS 0X32

#define LEFT_XSHUT_PIN 18
#define FRONT_XSHUT_PIN 6
#define RIGHT_XSHUT_PIN 45

#define MIN_LEFT_DISTANCE 100
#define MAX_LEFT_DISTANCE 150
#define FRONT_WALL_THRESHOLD 200
#define MIN_RIGHT_DISTANCE 100
#define MAX_RIGHT_DISTANCE 150

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

// Define target coordinates
uint16_t target_x = 100.0;
uint16_t target_y = 100.0;

// Current position (to be updated by Vive positioning system)
uint16_t current_x = 0.0;
uint16_t current_y = 0.0;

uint32_t med3filt(uint32_t a, uint32_t b, uint32_t c) {
  uint32_t middle;
  if ((a <= b) && (a <= c))
    middle = (b <= c) ? b : c;  
  
  else if ((b <= a) && (b <= c))
    middle = (a <= c) ? a : c;
  
  else
    middle = (a <= b) ? a : b;
  
  return middle;
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  vive1.begin();
  Serial.println("Vive trackers started");

  Wire.begin(8, 9);
  
  pinMode(LEFT_XSHUT_PIN, OUTPUT);
  pinMode(FRONT_XSHUT_PIN, OUTPUT);
  pinMode(RIGHT_XSHUT_PIN, OUTPUT);
  
  digitalWrite(LEFT_XSHUT_PIN, LOW);
  digitalWrite(FRONT_XSHUT_PIN, LOW);
  digitalWrite(RIGHT_XSHUT_PIN, LOW)
  delay(10);
  
  digitalWrite(LEFT_XSHUT_PIN, HIGH);
  delay(10);
  if (!lox1.begin(LEFT_SENSOR_ADDRESS)) {
    Serial.println("Failed to initialize left sensor!");
    while(1);
  }
  
  digitalWrite(FRONT_XSHUT_PIN, HIGH);
  delay(10);
  if (!lox2.begin(FRONT_SENSOR_ADDRESS)) {
    Serial.println("Failed to initialize front sensor!");
    while(1);
  }

  digitalWrite(RIGHT_XSHUT_PIN, HIGH);
  delay(10);
  if (!lox2.begin(RIGHT_SENSOR_ADDRESS)) {
    Serial.println("Failed to initialize right sensor!");
    while(1);
  }

  Serial.println("Both VL53L0X Sensors Initialized");

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
  
  ledcAttach(PWM_1, 5000, 8);
  ledcAttach(PWM_2, 5000, 8);
  ledcAttach(PWM_3, 5000, 8);
  ledcAttach(PWM_4, 5000, 8);

  // Start moving forward
  moveForward();
}

void loop() {
  // Read sensor data
  VL53L0X_RangingMeasurementData_t measure1, measure2, measure3;
  
  lox1.rangingTest(&measure1, false);
  lox2.rangingTest(&measure2, false);
  lox3.rangingTest(&measure3, false);
  
  int leftDistance = measure1.RangeMilliMeter;
  int frontDistance = measure2.RangeMilliMeter;
  int rightDistance = measure3.RangeMilliMeter;

  // Update current position (replace with actual Vive positioning data)
  updateCurrentPosition();

  // Check if we've reached the target
  if (isNearTarget()) {
    stop();
    Serial.println("Target reached!");
    while(1); // Stop execution
  }

  // Recalculate direction if needed
  recalculateMovement(leftDistance, frontDistance, rightDistance);

  delay(100); // Short delay between readings
}

void moveForward() {
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
  
  Serial.println("Move forward");  
}

void moveBackward() {
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
  
  Serial.println("Move backward");   
}

void turnLeft() {
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
  
  Serial.println("Turn left");
  delay(500); // Adjust this delay to control turn duration
}

void turnRight() {
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
  
  Serial.println("Turn right");
  delay(500); // Adjust this delay to control turn duration
}

void adjustLeft() {
  while (true) {
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
  
    Serial.println("Adjusting left");

    // Read left sensor
    VL53L0X_RangingMeasurementData_t measure;
    lox1.rangingTest(&measure, false);
    int leftDistance = measure.RangeMilliMeter;

    if (leftDistance >= MIN_LEFT_DISTANCE && leftDistance <= MAX_LEFT_DISTANCE) {
      stop();
      Serial.println("Left distance in safe range");
      break;
    }

    delay(50); // Short delay before next reading
  }
}

void adjustRight() {
  while (true) {
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
  
    Serial.println("Adjusting right");

    // Read right sensor
    VL53L0X_RangingMeasurementData_t measure;
    lox3.rangingTest(&measure, false);
    int rightDistance = measure.RangeMilliMeter;

    if (rightDistance >= MIN_RIGHT_DISTANCE && rightDistance <= MAX_RIGHT_DISTANCE) {
      stop();
      Serial.println("Right distance in safe range");
      break;
    }

    delay(50); // Short delay before next reading
  }
}


void stop() {
  ledcWrite(PWM_1, 0);
  ledcWrite(PWM_2, 0);
  ledcWrite(PWM_3, 0);
  ledcWrite(PWM_4, 0);

  digitalWrite(FORWARD_14, LOW);
  digitalWrite(BACKWARD_14, LOW);
  digitalWrite(FORWARD_23, LOW);
  digitalWrite(BACKWARD_23, LOW);
  
  Serial.println("Stop");
}

void updateCurrentPosition() {
  if (vive1.status() == VIVE_RECEIVING) {
    static uint16_t x0, y0, oldx1, oldx2, oldy1, oldy2;

    oldx2 = oldx1;
    oldy2 = oldy1;
    oldx1 = x0;
    oldy1 = y0;
    
    x0 = vive1.xCoord();
    y0 = vive1.yCoord();
    current_x = med3filt(x0, oldx1, oldx2);
    current_y = med3filt(y0, oldy1, oldy2);
    
    Serial.print("Position: X = ");
    Serial.println(current_x);
    Serial.print("Position: Y = ");
    Serial.println(current_y);
    
    digitalWrite(LED_BUILTIN, HIGH);
    
    if (current_x > 8000 || current_y > 8000 || current_x < 1000 || current_y < 1000) {
      current_x = 0;
      current_y = 0;
      digitalWrite(LED_BUILTIN, LOW);
    }
  } 
  
  else {
    digitalWrite(LED_BUILTIN, LOW);
    current_x = 0;
    current_y = 0;
    vive1.sync(5);
  }
}


bool isNearTarget() {
  int32_t x_diff = abs((int32_t)target_x - (int32_t)current_x);
  int32_t y_diff = abs((int32_t)target_y - (int32_t)current_y);
  return (x_diff < 50 && y_diff < 50); 
}

void recalculateMovement(int leftDistance, int frontDistance, int rightDistance) {
  // make adjustments to avoid obstalces if necessary
  if (frontDistance < FRONT_WALL_THRESHOLD) {
    while (frontDistance < FRONT_WALL_THRESHOLD) {
      moveBackward();
      delay(100);
      VL53L0X_RangingMeasurementData_t measure;
      lox2.rangingTest(&measure, false);
      frontDistance = measure.RangeMilliMeter;
    }
    stop();
  }

  if (leftDistance < MIN_LEFT_DISTANCE) adjustRight();
  else if (leftDistance > MAX_LEFT_DISTANCE) adjustLeft();

  if (rightDistance < MIN_RIGHT_DISTANCE) adjustLeft();
  else if (rightDistance > MAX_RIGHT_DISTANCE) adjustRight();

  // goal is to get the values of (x_target - x_current) and (y_target - y_current) to be below threshold
  // go forward or left based on which difference value is higher
  int32_t x_diff = (int32_t)target_x - (int32_t)current_x;
  int32_t y_diff = (int32_t)target_y - (int32_t)current_y;
  uint16_t threshold = 50;

  if (abs(x_diff) > threshold && abs(y_diff) > threshold) {
    if (abs(x_diff) > abs(y_diff)) turnRight();
    else moveForward();
  } 
  
  else if (abs(x_diff) > threshold && abs(y_diff) <= threshold) turnRight();
  else if (abs(x_diff) <= threshold && abs(y_diff) > threshold) moveForward();
  
  else {
    stop();
    Serial.println("Bot is near target");
  }
}