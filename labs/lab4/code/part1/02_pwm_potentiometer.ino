#define LED_PIN 4 // define a new macro LED_PIN as GPIO pin 4 where the LED is connected
#define POT_PIN 0 // define a new macro POT_PIN as GPIO pin 0 where the potentiometer is connected


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); // define this pin as output
  pinMode(POT_PIN, INPUT); // define this pin as input

  Serial.begin(115200); // initialize the serial monitor at a specific baudrate
  
  ledcAttach(LED_PIN, 20, 12); // setup pin 4 to output a square wave with frequency 20 Hz and PWM resolution of 12 bits
  digitalWrite(LED_PIN, HIGH); // turn on LED
}


void loop() {
  // put your main code here, to run repeatedly:
  int duty_cycle = 0.0246 * analogRead(POT_PIN) - 0.6143; // define duty_cycle as a function of the potentiometer value (found mapping experimentally)
  ledcWrite(LED_PIN, duty_cycle); // set the PWM duty cycle

  Serial.println(duty_cycle); // print the duty_cycle to the serial monitor
}
