#define LED_PIN 4 // define a new macro LED_PIN as GPIO pin 4 where the LED is connected
#define SWITCH_PIN 5 // define a new macro SWITCH_PIN as GPIO pin 5 where the switch is connected


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); // define this pin as output
  pinMode(SWITCH_PIN, INPUT_PULLUP); // define this pin as input with pullup resistor
}


void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(SWITCH_PIN) == LOW){ // if switch is pressed
    digitalWrite(LED_PIN, HIGH); // turn on LED
  }
  else{
    digitalWrite(LED_PIN, LOW); // turn off LED
  }
}
