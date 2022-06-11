#ifndef _SETUP_H_
#define _SETUP_H_

#include <FirebaseESP32.h>

//Thay doi wifii
#define WIFI_SSID "KHA"
#define WIFI_PASSWORD "09082017"
#define USER  "/users/anhhao/user's gadget" //change username
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyC4eikPsI3VGlvDF8K6yLu3y4xrNV6pHNA"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smarthrt-5d732-default-rtdb.firebaseio.com/" 

#define SEC(x) x*1000
#define MIN(x) x*60*1000

//number of pin 
int LED[9]={0};
enum PIN {D0=16, D1=5, D2=4,D3=0, D4=2, D5=14, D6=13, D7=12, D8=15};
//Define Firebase Data object
FirebaseData fbdo;
FirebaseData stream;

FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
char path[64];
char name[64];
bool isTemp = false;
//Callback firebase thay doi gia tri


unsigned long prevTime1 = millis();
unsigned long prevTime2 = millis();
void putTemp();
void getPin();



//callback khi firebase thay doi gia tri APP -> FIREBASE
//
void streamCallback(StreamData data)
{
 
  if(data.dataPath().substring(3).equals("btnValue")){

    int PinLed=data.dataPath().substring(1,2).toInt();
    int state=data.stringData().toInt();
    Serial.printf("Receive value: %d from Pin %d\n", state, PinLed);
    if(state>1){
        
    }else{
      digitalWrite(LED[PinLed],state);
    }
    
  }
  

 
}
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

#endif