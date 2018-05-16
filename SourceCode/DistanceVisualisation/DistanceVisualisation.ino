//#include <Array.h>

//#include <StaticThreadController.h>
//#include <Thread.h>
//#include <ThreadController.h>
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
#define ping_pin 7
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
#define median_number 1
int distance[3]; // [anzahl der sensoren]
int median_helper[median_number]; // [stärke der mittlungen]
float duration;

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
int getmedian( int values[],int sizee){
  int median;
  bubble_sort(values,sizee);
  return values[(int) (sizee/2)];
}
float getDistance(int trigPin,int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 100000);
  return (duration / 2) * 0.0343;
}
void caseDistance(int trigPin,int echoPin, int id){
  
  //Serial.print("  Distance " + String(echoPin) + " = ");
  for(int i=0;i<median_number;i++){
    median_helper[i] = getDistance(trigPin,echoPin);
  }
  distance[id-1]=getmedian(median_helper,median_number);
}

void loop() {

  caseDistance(trigPin1,echoPin1,1); //ids beginnen bei 1
  caseDistance(trigPin2,echoPin2,2);
  caseDistance(trigPin3,echoPin3,3);
  //caseDistance(trigPin4,echoPin4);
  //caseDistance(trigPin5,echoPin5);
  for(int i=0 ; i<3 ;i++){
    if((int)distance[i]!=0){
     Serial.write((int)distance[i]);                      // ausgabe-> processing
     // Serial.print(distance[i]); Serial.print(" ");      // ausgabe arduino-konsole
    }
  } 
  //Serial.println(" ");                                  //ausgabe arduini-konsole
  //delay(10);
}
