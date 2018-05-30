#include <NewPing.h>


/*
  HC-SR04 Basic Demonstration
  HC-SR04-Basic-Demo.ino
  Demonstrates functions of HC-SR04 Ultrasonic Range Finder
  Displays results on Serial Monitor

  DroneBot Workshop 2017
  http://dronebotworkshop.com
*/

// This uses Serial Monitor to display Range Finder distance readings

// Hook up HC-SR04 with Trig to Arduino Pin 10, Echo to Arduino pin 13

#define PING_PIN1 2
#define PING_PIN2 3
#define PING_PIN3 4
#define PING_PIN4 5
#define PING_PIN5 6

float duration, distance;

void setup() {
  Serial.begin (9600);
  //setup the input and output pin's
}
float getDistance(int pin){
  NewPing sonar(pin, pin);
  delay(10);
  duration = sonar.ping();
  return (duration / US_ROUNDTRIP_CM);
}
void caseDistance(int pin){
  
  delayMicroseconds(1);
  Serial.print("Distance " + String(pin) + " = ");
  distance = getDistance(pin);
  if (distance >= 400) {
     Serial.println("Out of range - too far away");
  } else if(distance <= 20) {
    Serial.println("Out of range - to close");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  
}

void loop() {
  caseDistance(PING_PIN1);  
  caseDistance(PING_PIN2); 
  caseDistance(PING_PIN3); 
  caseDistance(PING_PIN4); 
  caseDistance(PING_PIN5);   
  delay(1);
}
