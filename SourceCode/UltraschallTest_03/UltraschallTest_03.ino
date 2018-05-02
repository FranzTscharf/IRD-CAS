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

#define trigPin1 10
#define echoPin1 11
#define trigPin2 12
#define echoPin2 13

float duration1, duration2, distance1, distance2;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
}

void loop() {
   
  // Write a pulse to the HC-SR04 Trigger Pin
  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  
  // Measure the response from the HC-SR04 Echo Pin
 
  duration1 = pulseIn(echoPin1, HIGH);
  
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  
  distance1 = (duration1 / 2) * 0.0343;

  delayMicroseconds(1);

  // Write a pulse to the HC-SR04 Trigger Pin
  
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  // Measure the response from the HC-SR04 Echo Pin
 
  duration2 = pulseIn(echoPin2, HIGH);
  
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  
  distance2 = (duration2 / 2) * 0.0343;
  
  // Send results to Serial Monitor

  Serial.print("Distance 1 = ");
  if (distance1 >= 400 || distance1 <= 2) {
     Serial.println("Out of range");
  }
  else {
    Serial.print(distance1);
    Serial.println(" cm");
  }

  // Send results to Serial Monitor

  Serial.print("Distance 2 = ");
  if (distance2 >= 400 || distance2 <= 2) {
     Serial.println("Out of range");
  }
  else {
    Serial.print(distance2);
    Serial.println(" cm");
  }
  delay(100);
}
