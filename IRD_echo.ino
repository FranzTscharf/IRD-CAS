#include <checksum.h>
#include <mavlink.h>
#include <mavlink_helpers.h>
#include <mavlink_types.h>
#include <protocol.h>

#include <NewPing.h>
//#include "C:\Users\shaof\OneDrive\Apuntes\4 Grado\2 Semestre\TFG\Arduino\libraries\mavlink\common\mavlink.h"

//==================================INICIALIZACIONES======================================//
/* Initialization of sensor pins. Bookstore "NewPing"
  NewPing NAME (Trigger, Echo, MAXDIST);
  The value of MAXDIST is the maximum distance measured by the library.
  If an Echo returns a value greater than said distance, it is automatically discarded */
NewPing sonar0(3, 4, 300);
NewPing sonar1(5, 6, 300);
NewPing sonar2(7, 8, 300);
NewPing sonar3(9, 10, 300);
NewPing sonar4(11, 12, 100);
 
// Variable used to control that the HeartBeat is sent every second
unsigned long HeartbeatTime = 0; 

// Variables used to only send one RCOverride each time
// to be modified, and not to saturate the 
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
  if ( (millis() - HeartbeatTime) > 1000 ) {
    HeartbeatTime = millis();
    FHeartBeat();
  }
  FSensores();
  FRCOverride();
}

//===========================================FUNCIONES====================================//
// Task responsible for measuring the sensors
void FSensores() {
  ShiftArrays();
  MedirSensores();
  MediaDistances();
  CheckDistances();
}

// Task that sends the movement commands according to the distances detected by the sensors
void FRCOverride() {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  uint16_t len;
  
  Pitch  = CheckPitch(Pitch);
  Roll   = CheckRoll(Roll);
  
  CompensacionInercia();

  if( Pitch != PitchOutTemp || Roll != RollOutTemp ){
    n = 0;
    PitchOutTemp = Pitch;
    RollOutTemp  = Roll;
  }else{
    n += 1;
    if(n == 4){
      RollOut = RollOutTemp;
      PitchOut = PitchOutTemp;
      RCOverride(&msg, len, buf, PitchOut, RollOut);
    }
  }  
}

// Scrolls each array of Distances in one position
void ShiftArrays() {
  for (uint8_t i = 0; i < NSensores; i++) {
    for (uint8_t j = NDistances - 1; j > 0; j--) {
      Sensor[i].Distances[j] = Sensor[i].Distances[j - 1];
    }
  }
}

//==================================SENSORES=====================================//
// The sensors are measured, and they are placed in position 0 of each array
void MedirSensores() {
  Sensor[0].Distances[0] = sonar0.ping_cm();
  Sensor[1].Distances[0] = sonar1.ping_cm();
  Sensor[2].Distances[0] = sonar2.ping_cm();
  Sensor[3].Distances[0] = sonar3.ping_cm();
  Sensor[4].Distances[0] = sonar4.ping_cm();
}

// The average of all distances is performed. The 0 are discarded
void MediaDistances() {
  for (uint8_t i = 0; i < NSensores; i++) {
    int Total   = 0;
    uint8_t Num = 0;
    for (uint8_t j = 0; j < NDistances; j++) {
      if (Sensor[i].Distances[j] != 0  && Sensor[i].Distances[j] < 300) {
        Total += Sensor[i].Distances[j];
        Num += 1;
      }
    }
    if (Num > 3) {
      Sensor[i].MediaDistances = Total / Num;
    } else {
      Sensor[i].MediaDistances = 0;
    }
  }
  /*Serial.print("\n\rDistances: ");
  Serial.print(Sensor[0].MediaDistances);
  Serial.print(",");
  Serial.print(Sensor[1].MediaDistances);
  Serial.print(",");
  Serial.print(Sensor[2].MediaDistances);
  Serial.print(",");
  Serial.print(Sensor[3].MediaDistances);
  Serial.print(",");
  Serial.print(Sensor[4].MediaDistances);
  Serial.print("cm\n\r");*/
}

// Check if the average obtained is below the threshold.
void CheckDistances() {
  // Minimum of 10 for the distance. there are measurement errors
  for (uint8_t i = 0; i < NSensores; i++) {
    if (Sensor[i].MediaDistances != 0 && Sensor[i].MediaDistances < DistanceClose) {
      Sensor[i].Close = true;
    } else {
      Sensor[i].Close = false;
    }
  }
}

//========================MOVEMENT=========================//
uint16_t CheckPitch(uint16_t Pitch) {
  int16_t Diferencia = Sensor[0].MediaDistances - Sensor[2].MediaDistances;
  if( Sensor[4].MediaDistances > AltMin || Sensor[4].MediaDistances == 0 ) {
    if( abs(Diferencia) > DistMin ) {
       // Difference greater than 30 between both sensors
    if( Sensor[0].Close == true ) {
      // Detect the front
      if( Sensor[2].Close == true ) {
        // Detect the back
        if( Sensor[0].MediaDistances < Sensor[2].MediaDistances ) {
          //The front sensor has a shorter distance
          return( Pitch = ValorRC( Sensor[0].MediaDistances, 1 ) );
        }else{
          //The rear sensor has a shorter distance
          return( Pitch = ValorRC( Sensor[2].MediaDistances, 0 ) );
        }
      }else{
        //Detects the front, but not the rear
        return( Pitch = ValorRC( Sensor[0].MediaDistances, 1 ) );
      }
    }else {
      //Does not detect the front
      if( Sensor[2].Close == true ) {
        //Detect the back
        return( Pitch = ValorRC( Sensor[2].MediaDistances, 0 ) );
      }else{
        //Both have a distance greater than 150
        return( Pitch = 0 );
      }
    }
  }else if( Sensor[0].Close == true && Sensor[2].MediaDistances == 0 ) {
    //Detects the one in front, and the one in the back when not detecting anything, returns 0
    return( Pitch = ValorRC( Sensor[0].MediaDistances, 1 ) );
    }else if ( Sensor[0].MediaDistances == 0 && Sensor[2].Close == true ) {
      //The same but the opposite
      return( Pitch = ValorRC( Sensor[2].MediaDistances, 0 ) );
      }else {
        //It does not detect any. Both at 0
        return( Pitch = 0 );
      }
  }else{
    return( Pitch = 0 );
  }
}

uint16_t CheckRoll(uint16_t Roll) {  
  int16_t Diferencia = Sensor[1].MediaDistances - Sensor[3].MediaDistances;
  if( Sensor[4].MediaDistances > AltMin || Sensor[4].MediaDistances == 0 ) {
    if( abs(Diferencia) > DistMin ) {
      //Difference greater than 20 between distances
      if( Sensor[1].Close == true ) {
        //Detect the right
        if( Sensor[3].Close == true ) {
          //Detect the left
          if( Sensor[1].MediaDistances < Sensor[3].MediaDistances ) {
            //The right sensor has a smaller distance
            return( Roll = ValorRC( Sensor[1].MediaDistances, 0 ) );
          }else{
            //The left sensor has a smaller distance
            return( Roll = ValorRC( Sensor[3].MediaDistances, 1 ) );
          }
        }else{
          //Detect the right, but not the left
          return( Roll = ValorRC( Sensor[1].MediaDistances, 0 ) );
        }
      }else {
        //Does not detect the right
        if( Sensor[3].Close == true ) {
          //Detect the left
          return( Roll = ValorRC( Sensor[3].MediaDistances, 1 ) );
        }else{
          //Both have a distance greater than 150
          return( Roll = 0 );
        }
      }
    }else if( Sensor[1].Close == true && Sensor[3].MediaDistances == 0 ) {
      //Detect the right, and the left when not detecting anything, returns 0
      return( Roll = ValorRC( Sensor[1].MediaDistances, 0 ) );
      }else if ( Sensor[1].MediaDistances == 0 && Sensor[3].Close == true ) {
        //The same but the opposite
        return( Roll = ValorRC( Sensor[3].MediaDistances, 1 ) );
        }else {
          //It does not detect any. Both at 0
          return( Roll = 0 );
        }
  }else {
    return( Roll = 0 );
  }
}

// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "Increase" is to know in what direction it is.
uint16_t ValorRC( uint16_t Distancia, bool Aumentar ) {
  if( Distancia < 30 ) {
    if( Aumentar == true ) {
      return( 1700 );
    }else{
      return( 1300 );
    }
  }else if( Distancia < 90 ) {
    if( Aumentar == true ) {
      return( 1675 );
    }else{
      return( 1325 );
    }
  }else if( Distancia < 150 ) {
    if( Aumentar == true ) {
      return( 1650 );
    }else{
      return( 1350 );
    }
  }
}

void CompensacionInercia(){

  if(PitchOut > 1500 && Sensor[0].Active == false && Sensor[2].Active == false){
    Sensor[0].Active = true;
  }else if(PitchOut < 1500 && PitchOut != 0 && Sensor[2].Active == false && Sensor[0].Active == false){
    Sensor[2].Active = true;
  }else if(PitchOut == 0 && Sensor[0].Active == true && Sensor[0].CompensateTime == 0){
    Sensor[0].CompensateTime = millis();
  }else if(PitchOut == 0 && Sensor[2].Active == true && Sensor[2].CompensateTime == 0){
    Sensor[2].CompensateTime = millis();
  }

  if(RollOut > 1500 && Sensor[3].Active == false && Sensor[1].Active == false){
    Sensor[3].Active = true;
  }else if(RollOut < 1500 && RollOut != 0 && Sensor[1].Active == false && Sensor[3].Active == false){
    Sensor[1].Active = true;
  }else if(RollOut == 0 && Sensor[1].Active == true && Sensor[1].CompensateTime == 0){
    Sensor[1].CompensateTime = millis();
  }else if(RollOut == 0 && Sensor[3].Active == true && Sensor[3].CompensateTime == 0){
    Sensor[3].CompensateTime = millis();
  }

  for(int i = 0; i < 4; i++){
    if(Sensor[i].CompensateTime != 0 && (Sensor[i].CompensateTime + Compensation > millis())){
      switch(i){
        case 0:
          Pitch = 1300;
          break;
        case 1:
          Roll = 1700;
          break;
        case 2:
          Pitch = 1700;
          break;
        case 3:
          Roll = 1300;
          break;
        default:
          break;
      }
    }else if(Sensor[i].CompensateTime != 0){
      switch(i){
        case 0:
        case 2:
          PitchOut = 0;
          Sensor[i].Active = false;
          Sensor[i].CompensateTime = 0;
          break;
        case 1:
        case 3:
          RollOut = 0;
          Sensor[i].Active = false;
          Sensor[i].CompensateTime = 0;
          break;
        default:
          break;
      }
    }
  }
  
}

//============================MAVLINK==========================//
// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  uint16_t len;
  // System ID = 255 = GCS
  mavlink_msg_heartbeat_pack(255, 0, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, 0, 1, 0);

  // Copy the message to send buffer
  len = mavlink_msg_to_send_buffer(buf, &msg);

  // Send the message (.write sends as bytes)
  Serial.write(buf, len);

  //Serial.write("\n\rHeartBeat\n\r");
}

void RCOverride(mavlink_message_t *msg, uint16_t len, uint8_t *buf, uint16_t PitchOut, uint16_t RollOut) {
  // Pack and send the calculated Pitch and Roll data. Only send if the data is new
  mavlink_msg_rc_channels_override_pack(255, 0 , msg, 1, 0, RollOut, PitchOut, 0, 0, 0, 0, 0, 0);
  len = mavlink_msg_to_send_buffer(buf, msg);
  Serial.write(buf, len);
  /*Serial.print("\n\rPitch: ");
  Serial.print(PitchOut);
  Serial.print(",");
  Serial.print(" Roll: ");
  Serial.print(RollOut);*/
}

  /*//Armar Dron
    //Pack the message
    //uint16_t mavlink_msg_command_long_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,uint8_t target_system, uint8_t target_component, uint16_t command, uint8_t confirmation, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
    mavlink_msg_command_long_pack(255, 0, &msg, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0, 1, 0, 0, 0, 0, 0, 0);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    // Send the message (.write sends as bytes)
    Serial.write(buf, len);
    delay(1000);*/

        /*mavlink_msg_rc_channels_override_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
    uint8_t target_system, uint8_t target_component, uint16_t chan1_raw, uint16_t chan2_raw, uint16_t chan3_raw,
    uint16_t chan4_raw, uint16_t chan5_raw, uint16_t chan6_raw, uint16_t chan7_raw, uint16_t chan8_raw)*/
    
  /*Channel 1 = Roll
    Channel 2 = Pitch
    Channel 3 = Throttle
    Channel 4 = Yaw*/

  /*Sensor0 = Delantero
    Sensor1 = Derecha
    Sensor2 = Trasero
    Sensor3 = Izquierda*/
