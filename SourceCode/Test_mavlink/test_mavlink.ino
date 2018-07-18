#include <checksum.h>
#include <mavlink.h>
#include <mavlink_helpers.h>
#include <mavlink_types.h>
#include <protocol.h>

#include <NewPing.h>
//include "C:\Users\shaof\OneDrive\Apuntes\4 Grado\2 Semestre\TFG\Arduino\libraries\mavlink\common\mavlink.h"

//==================================INICIALIZACIONES======================================//
/* Initialization of sensor pins. Bookstore "NewPing"
    NewPing NAME (Trigger, Echo, MAXDIST);
    The value of MAXDIST is the maximum distance measured by the library.
    If an Echo returns a value greater than said distance, it is automatically discarded */
NewPing sonar0(3, 4, 300);
NewPing sonar1(5, 6, 300);
NewPing sonar2(7, 8, 300);
NewPing sonar3(9, 10, 300);
NewPing sonar4(11, 12, 100);      //wir haben 6 sensoren, also noch einen hinzufügen

// Variable used to control that the HeartBeat is sent every second
unsigned long HeartbeatTime = 0;

// Variables used to only send one RCOverride each time
// to be modified, and not to saturate the
int Rolll;
uint16_t Pitch = 0;
uint16_t Roll  = 0;
uint16_t PitchOut = 0;
uint16_t RollOut  = 0;
uint16_t PitchOutTemp = 0;
uint16_t RollOutTemp  = 0;
uint8_t n         = 0;

#define NDistances 5
#define DistanceClose   100 // Distance at which the control starts to act
#define AltMin          70 // Height at which the control begins to act
#define DistMin         50 // Minimum difference between two distances of the same axis to move.
#define Compensation    800 // Time acting inertial compensation in ms

// Record to save the data of each sensor
struct Sensores {
  uint16_t Distances[NDistances]  = {0};
  uint16_t MediaDistances          = 0;
  bool Close                        = false; //search for
  bool Active                       = false;
  unsigned long CompensateTime       = 0;
};

// The variables of each sensor are started
#define NSensores 5
Sensores Sensor[NSensores];

//====================================PROGRAM============================================//

void setup() {
  Serial.begin(57600);
}

void loop() {
  Rolll=1800;
  for(int i=0;i<1000;i++){
  FRCOverride();
  }
  Rolll=500;
  for(int i=0;i<1000;i++){
  FRCOverride();
  }
}


// Task that sends the movement commands according to the distances detected by the sensors
void FRCOverride() {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  uint16_t len;
  RCOverride(&msg, len, buf, PitchOut, RollOut);
}

void RCOverride(mavlink_message_t *msg, uint16_t len, uint8_t *buf, uint16_t PitchOut, uint16_t RollOut) {
  // Pack and send the calculated Pitch and Roll data. Only send if the data is new
  mavlink_msg_rc_channels_override_pack(1, 0 , msg, 1, 0, Rolll, 1500, 0, 0, 0, 0, 0, 0);
  len = mavlink_msg_to_send_buffer(buf, msg);
  Serial.write(buf, len);
}

