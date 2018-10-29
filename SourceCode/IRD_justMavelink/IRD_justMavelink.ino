#include <checksum.h>
#include <mavlink.h>
#include <mavlink_helpers.h>
#include <mavlink_types.h>
#include <protocol.h>
#include <NewPing.h>

#define PING_PIN_1 2 // vorne
#define PING_PIN_2 3 // rechts
#define PING_PIN_3 4 // links
#define PING_PIN_4 5 // links-vorne 45°
#define PING_PIN_5 6 // rechte-vorne 45°
#define PING_PIN_6 7 // vorne-oben

NewPing sonar0(PING_PIN_1,PING_PIN_1 , 300);
NewPing sonar1(PING_PIN_2,PING_PIN_2 , 300);
NewPing sonar2(PING_PIN_3,PING_PIN_3 , 300);
NewPing sonar3(PING_PIN_4,PING_PIN_4 , 300);
NewPing sonar4(PING_PIN_5,PING_PIN_5 , 300);
NewPing sonar5(PING_PIN_6,PING_PIN_6 , 300);

int Rolll;
uint16_t Pitch = 0;
uint16_t Roll = 0;
uint16_t PitchOut = 0;
uint16_t RollOut = 0;
uint16_t PitchOutTemp = 0;
uint16_t RollOutTemp  = 0;
uint8_t n = 0;
int duration, distance;


void setup() {
  Serial.begin (9600);
}

float getDistance(int pin) {
  NewPing sonar(pin, pin, 300);
  delay(10);
  duration = sonar.ping();
  return (duration / US_ROUNDTRIP_CM);
}

void caseDistance(int pin) {
  distance = getDistance(pin);
  if (distance >= 250) {
    Serial.write(250);
  } else if (distance <= 3) {
    Serial.write(1);
  } else {
    Serial.write(distance);
  }
}

void RCOverride(mavlink_message_t *msg, uint16_t len, uint8_t *buf, uint16_t PitchOut, uint16_t RollOut) {
  // Pack and send the calculated Pitch and Roll data. Only send if the data is new
  mavlink_msg_rc_channels_override_pack(1, 0 , msg, 1, 0, RollOut, PitchOut, 0, 0, 0, 0, 0, 0);
  len = mavlink_msg_to_send_buffer(buf, msg);
  Serial.write(buf, len);
}

void calcDistances() {
 
}

void calcPitch() {

}

void calcRoll() {

}

void sendData() {

}

void loop() {
  calcDistances();
  calcPitch();
  calcRoll();
  sendData();
  delay(1);
}
