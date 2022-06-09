#include <Arduino.h>

#include <Wire.h>

#include <FirebaseESP32.h>

//Thay doi wifi
#define WIFI_SSID "Linh Pru"
#define WIFI_PASSWORD "Linh1321"

#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define API_KEY "AIzaSyC4eikPsI3VGlvDF8K6yLu3y4xrNV6pHNA"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smarthrt-5d732-default-rtdb.firebaseio.com/" 

#define SEC(x) x*1000
#define MIN(x) x*60*1000

//number of pin 
int LED[9]={-1};

//Define Firebase Data object
FirebaseData fbdo;
FirebaseData stream;

FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
char path[64];
char name[64];

//Callback firebase thay doi gia tri
void streamCallback(StreamData data);

void streamTimeoutCallback(bool timeout);

unsigned long prevTime1 = millis();
unsigned long prevTime2 = millis();
void putTemp();
void getPin() ;
int tempPin;

void setup() {
  randomSeed(42); 
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (!Firebase.RTDB.beginStream(&stream, "/users/anhhao/user's gadget")) //thay doi user name
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  //Update Pin 
  getPin();
  
}

void loop() {

  unsigned long  currentTime = millis();
  if(currentTime-prevTime1>SEC(5)){  // send data every 5s 
    putTemp();
    prevTime1=currentTime;
  }
  
  //change time value MIN define at line 21
  if(currentTime-prevTime2>MIN(5)){  // update pin every 5p
    getPin();
    prevTime2=currentTime;
  }
  
  


}


//Random Temp Data and send to Firebase
void putTemp(){
  

  if (Firebase.RTDB.setString(&fbdo, path, String(random(10,30)))){
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

//Update Pin from Firebase
void getPin() {
  for(int i=0;i<=8;i++){
    
    sprintf(path, "users/anhhao/user's gadget/%d/espPin", i); //thay doi username
    
    if(Firebase.RTDB.getString(&fbdo,path)){
      Serial.println(path);
      String espPin=fbdo.stringData().substring(1,3);
      
      //get Temp pin 
      if(espPin.equals("A0")){
        tempPin=i;
      }
      //assigning pin according to ESP32 pinout
      if(espPin.equals("D0")){
        LED[i]=16;
      }
      if(espPin.equals("D1")){
        LED[i]=5;
      }
      if(espPin.equals("D2")){
        LED[i]=4;
      }
      if(espPin.equals("D3")){
        LED[i]=0;
      }
      if(espPin.equals("D4")){
        LED[i]=2;
      }
      if(espPin.equals("D5")){
        LED[i]=14;
      }
      if(espPin.equals("D6")){
        LED[i]=12;
      }
      if(espPin.equals("D7")){
        LED[i]=13;
      }
      if(espPin.equals("D8")){
        LED[i]=15;
      }
      
    }
    
  }
  for(int i=0;i<=8;i++){
    pinMode(LED[i], OUTPUT);
  }
  sprintf(path, "users/anhhao/user's gadget/%d/btnValue", tempPin); //thay doi user name
  
  
}

//callback khi firebase thay doi gia tri APP -> FIREBASE
//
void streamCallback(StreamData data)
{
  Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  printResult(data); //see addons/RTDBHelper.h

  //Kiem tra gia tri pin thay doi hay add/remove button
  
  if(data.dataPath().substring(3).equals("btnValue")){

    int PinLed=data.dataPath().substring(1,2).toInt();
    int state=data.stringData().toInt();
    if(state>1){
      if(LED[PinLed]!=16) //khac D0
        analogWrite(LED[PinLed], state);  //esp8266
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

