#include <Arduino.h>

#include <Wire.h>
#include "setup.h"

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

  if (!Firebase.RTDB.beginStream(&stream,USER)); //thay doi user name
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
  if(isTemp){
    if (Firebase.RTDB.setString(&fbdo, path, String(random(10,30)))){
      Serial.println("Random TEMP");
    
    }
    else {
      Serial.println("FAILED  to push Temp");
    
    }
  } 
  
}

//Update Pin from Firebase
void getPin() {
  isTemp = false;
  for(int i=0;i<=8;i++){
    
    sprintf(path, "%s/%d/espPin", USER, i); 
    
    if(Firebase.RTDB.getString(&fbdo,path)){
      Serial.println(path);
      String espPin=fbdo.stringData().substring(1,3);
      
      //get Temp pin 
      if(espPin.equals("A0")){
        sprintf(path, "%s/%d/btnValue", USER, i);
        isTemp = true;
      }
      //gan pin theo Enum
      if(espPin.equals("D0")){
        LED[i]=D0;
      }
      if(espPin.equals("D1")){
        LED[i]=D1;
      }
      if(espPin.equals("D2")){
        LED[i]=D2;
      }
      if(espPin.equals("D3")){
        LED[i]=D3;
      }
      if(espPin.equals("D4")){
        LED[i]=D4;
      }
      if(espPin.equals("D5")){
        LED[i]=D5;
      }
      if(espPin.equals("D6")){
        LED[i]=D6;
      }
      if(espPin.equals("D7")){
        LED[i]=D7;
      }
      if(espPin.equals("D8")){
        LED[i]=D8;
      }
      
    }
    
  }
  //setup pinmode
  for(int i=0;i<=8;i++){
    pinMode(LED[i], OUTPUT);
  }
  
}





