#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>


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

#define trigPin1 2
#define echoPin1 3
#define trigPin2 4
#define echoPin2 5
#define trigPin3 6
#define echoPin3 7
#define trigPin4 8
#define echoPin4 9
#define trigPin5 10
#define echoPin5 11

float duration, distance;

void setup() {
  Serial.begin (9600);
  //setup the input and output pin's
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);

}
float getDistance(int trigPin,int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 80000);
  return (duration / 2) * 0.0343;
}
void caseDistance(int trigPin,int echoPin){
  
  delayMicroseconds(1);
  Serial.print("Distance " + String(echoPin) + " = ");
  distance = getDistance(trigPin,echoPin);
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

  caseDistance(trigPin1,echoPin1);
  caseDistance(trigPin2,echoPin2);
  caseDistance(trigPin3,echoPin3);
  caseDistance(trigPin4,echoPin4);
  caseDistance(trigPin5,echoPin5);       
  delay(1);
}
