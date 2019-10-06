#include <ESP8266WiFi.h>
#include "config.h"
#include "AdafruitIO_WiFi.h"

//CLOUD INTERFACE 
//#define WIFI_SSID  "WWCode"
//#define WIFI_PASS  "Hopper Lovelace Borg"
#define WIFI_SSID  "Zapproved Guest"
#define WIFI_PASS  "process search review produce"
#define IO_USERNAME "stellacodes1"
#define IO_KEY "b7f2d853992342c48170dad730fb69f4"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//flags
int NightModeFlag = 0;

//LED Pins
int LivingRoomLightPin = 15;
int BROneLightPin = 0;
int BRTwoLightPin = 16;

//Motion
int MotionPin = 2;

//Switch Pins
int LivingRoomSwitchPin = 13;
int BROneSwitchPin = 12;
int BRTwoSwitchPin = 14;

//States
int LivingSwitchState = LOW;
int BROneSwitchState = LOW;
int BRTwoSwitchState = LOW;
int MotionState = LOW;

bool currentLR = false;
bool lastLR = false;
bool currentMR = false;
bool lastMR = false;
bool currentKR = false;
bool lastKR = false;
bool currentMotion = false;
bool lastMotion = false;

// set up the 'digital' feed
AdafruitIO_Feed *digital = io.feed("digital");
AdafruitIO_Feed *LivingIn = io.feed("digitalIn");
AdafruitIO_Feed *MasterIn = io.feed("master-bedroom");
AdafruitIO_Feed *kidIn = io.feed("kids-bedroom");
AdafruitIO_Feed *motionIn = io.feed("motion");

void setup() {

  pinMode(LivingRoomLightPin, OUTPUT);
  pinMode(LivingRoomSwitchPin, INPUT);
  pinMode(BROneLightPin, OUTPUT);
  pinMode(BROneSwitchPin, INPUT);
  pinMode(BRTwoLightPin, OUTPUT);
  pinMode(BRTwoSwitchPin, INPUT);
  pinMode(MotionPin, INPUT);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
    // wait for serial monitor to open
  while(! Serial);

 Serial.print("Connecting to Adafruit IO");
  io.connect();

  digital->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  digital->get();
}

void loop() {    
  int tempLRSwitchState = GetInputState(LivingRoomSwitchPin);
  int tempBROneSwitchState = GetInputState(BROneSwitchPin);
  int tempBRTwoSwitchState = GetInputState(BRTwoSwitchPin);
  MotionState = GetInputState(MotionPin);
//  Serial.println(MotionState);
  io.run();
    
  LivingSwitchState = SetSwitch(tempLRSwitchState,LivingSwitchState,LivingRoomLightPin,false,true);
  BROneSwitchState = SetSwitch(tempBROneSwitchState,BROneSwitchState,BROneLightPin,true,false);
  BRTwoSwitchState = SetSwitch(tempBRTwoSwitchState,BRTwoSwitchState,BRTwoLightPin,true,false);

  if (digitalRead(LivingRoomLightPin) == HIGH)
    currentLR = true;
  else
    currentLR = false;

  if (currentLR != lastLR){
    lastLR = currentLR;
    Serial.print("sending Living Roombutton -> ");
    Serial.println(currentLR);
    LivingIn->save(currentLR);
  }

   if (digitalRead(BROneLightPin) == HIGH && NightModeFlag == 0)
    currentMR = true;
  else
    currentMR = false;

  if (currentMR != lastMR){
    lastMR = currentMR;
    Serial.print("sending Master bedroom button -> ");
    Serial.println(currentMR);
    MasterIn->save(currentMR);
  }

   if (digitalRead(BRTwoLightPin) == HIGH && NightModeFlag == 0)
    currentKR = true;
  else
    currentKR = false;

  if (currentKR != lastKR){
    lastKR = currentKR;
    Serial.print("sending Kids Room button -> ");
    Serial.println(currentKR);
    kidIn->save(currentKR);
  }

  if (MotionState == HIGH)
    currentMotion = true;
  else
    currentMotion = false;

  if (currentMotion != lastMotion){
    lastMotion = currentMotion;
    Serial.print("sending motion -> ");
    Serial.println(currentMotion);
    motionIn->save(currentMotion);
  }
    
  delay(100);
}
int GetInputState(int pin){
  int read = digitalRead(pin);

  return read;
}

int SetSwitch(int tempState, int switchState, int lightPin, bool IsNightModeEnabled,bool hasMotion){

  if (NightModeFlag == 1 && IsNightModeEnabled == true){
    digitalWrite(lightPin, LOW);
  }
  else if (hasMotion == true && MotionState == HIGH){
    digitalWrite(lightPin, HIGH);
//    Serial.println("motion is high");
    switchState = HIGH;
  }
  else if (tempState != switchState){
    switchState = tempState;
    
    if (switchState == HIGH)
      digitalWrite(lightPin, HIGH);
    else
      digitalWrite(lightPin, LOW);
  }
  return switchState;
}

void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");
 
  if(data->toPinLevel() == HIGH){
    Serial.println("HIGH");
    NightModeFlag = 1;
  }
  else{
    Serial.println("LOW");
    NightModeFlag = 0;
  }
}
