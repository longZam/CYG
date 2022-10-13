#include <Servo.h>
#include <stdlib.h>
#include <math.h>
#define PIN_SERVO 10

Servo myservo;

void setup() {
  Serial.begin(115200);
  myservo.attach(PIN_SERVO); 
  myservo.write(90);
  delay(1000);
}

void loop() {
  
}

double logistic(double x)
{
  return 1 / (1 + exp(-x));
}
