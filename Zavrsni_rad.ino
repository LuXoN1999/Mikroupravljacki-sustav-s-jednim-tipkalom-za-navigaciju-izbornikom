#include <LiquidCrystal.h>
#include <String.h>
#define buttonPin 2
#define triggerPin 3
#define echoPin 4
#define brightnessPin 5
#define buzzerPin 9
#define RGBRedPin 13
#define RGBGreenPin 12
#define RGBBluePin 11
#define registerSelectPin A0
#define enablePin A1
#define digitalPin4 A2
#define digitalPin5 A3
#define digitalPin6 A4
#define digitalPin7 A5
LiquidCrystal lcd(registerSelectPin, enablePin, digitalPin4, digitalPin5, digitalPin6, digitalPin7);

//varijable za ocitavanje klika
boolean lastButtonState = HIGH, buttonState=HIGH, flag1, flag2;
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;  
byte numberOfClicks; 
int timeDifference; 
long double pressTime, releaseTime; 

//varijable glavnog izbornika
boolean menuDistanceTargeted=true, menuBrightnessTargeted=false, menuRGBColorTargeted=false, menuLoudnessTargeted=false;
boolean menuMode=true;
String menuCurrentTargeted="Change Distance";

//varijable za ultrazvucni senzor
boolean triggerState = LOW;
int offTime = 2,onTime = 10,distance;
unsigned long previousMicros = 0;
long duration;

//varijable za glasnocu,svjetnlinu,glasnocu i stanje RBG LED-ice
String RGBstate="Red",defaultRGBstate="Red";
int buzzerVolume=50,defaultBuzzerVolume=50;
int optionDistance=7,defaultDistance=7;
int optionBrightness=100,defaultBrightness=100;


void setup() {
  pinMode(brightnessPin,OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(RGBRedPin,OUTPUT);
  pinMode(RGBGreenPin,OUTPUT);
  pinMode(RGBBluePin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
}

void loop() {
  analogWrite(brightnessPin,optionBrightness);
  eventRecogniser();
  ultrasonicSensorReading();
  }

void eventRecogniser(){
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
    }
  }
  if (buttonState == 0 && flag2 == 0)
  {
    pressTime = millis();
    flag1 = 0;
    flag2 = 1;
    numberOfClicks++;
  }
  if (buttonState == 1 && flag1 == 0)
  {
    releaseTime = millis();
    flag1 = 1;
    flag2 = 0;
    timeDifference = releaseTime - pressTime;
  }

  if ((millis() - pressTime) > 400 && buttonState == 1)   
  {
    if (numberOfClicks == 1)
    {
      if (timeDifference >= 400) {hold(); }
      else { buttonSingleClick(); }
    }
    else if (numberOfClicks == 2 ){buttonDoubleClick();}
    else if (numberOfClicks == 3) {buttonTripleClick();}
    numberOfClicks = 0;
    }
  lastButtonState = reading;
  }

void buttonSingleClick()
{
  if(menuMode){
    switchMainElements();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(menuCurrentTargeted);
  }
  else{
    if(menuDistanceTargeted){
      optionDistance+=1;
      displayDistance();
       }
    else if(menuRGBColorTargeted){
      RGBSwitchForward();
      displayRGBColor();
      }
    else if(menuBrightnessTargeted){ 
      if(optionBrightness<250){
        optionBrightness+=25;
        displayBrightness();
      }
    }
    else{ 
      if(buzzerVolume<250){
        buzzerVolume+=25;
        displayVolume();
      }
    }
  }
}


void buttonDoubleClick()
{
  if(menuMode){
    menuMode=false;
    if(menuDistanceTargeted){displayDistance();}
    else if(menuRGBColorTargeted){displayRGBColor();}
    else if(menuLoudnessTargeted){displayVolume();}
    else{ displayBrightness();}
    }
    else{
      if(menuDistanceTargeted){
        if(optionDistance>1){
          optionDistance-=1;
        }
        displayDistance();
        }
      else if(menuRGBColorTargeted){ 
        RGBSwitchBackward();
        displayRGBColor();
        }
      else if(menuBrightnessTargeted){ 
        if(optionBrightness>25){
          optionBrightness-=25;
          displayBrightness();
        }
      }
      else{ 
        if(buzzerVolume>0){
          buzzerVolume-=25;
          displayVolume();
        }
      }
    }
}
void buttonTripleClick()
{
  if(!menuMode){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Returning to");
    lcd.setCursor(0,1);
    lcd.print("   main menu...");
    delay(1500);
    lcd.clear();
    lcd.print(menuCurrentTargeted);
    }
  menuMode=true;
}

void hold(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reseting to");
  lcd.setCursor(0,1);
  lcd.print("default values...");
  delay(1500);
  lcd.clear();
  RGBstate=defaultRGBstate;
  optionDistance=defaultDistance;
  buzzerVolume=defaultBuzzerVolume;
  optionBrightness=defaultBrightness;
  menuMode=true;
  lcd.print(menuCurrentTargeted);
}

void RGBSwitchForward(){
  if(RGBstate=="Red"){RGBstate="Green";}
  else if(RGBstate=="Green"){RGBstate="Blue";}
  else{ RGBstate="Red";}
}

void RGBSwitchBackward(){
  if(RGBstate=="Red"){RGBstate="Blue";}
  else if(RGBstate=="Green"){RGBstate="Red";}
  else{ RGBstate="Green";}
}

void switchMainElements(){
    if(menuDistanceTargeted){
      menuDistanceTargeted=false;
      menuRGBColorTargeted=true;
      menuCurrentTargeted="Change RGB Color";
    }
    else if(menuRGBColorTargeted){
      menuRGBColorTargeted=false;
      menuLoudnessTargeted=true;
      menuCurrentTargeted="Change Volume";
    }
    else if(menuLoudnessTargeted){
      menuLoudnessTargeted=false;
      menuBrightnessTargeted=true;
      menuCurrentTargeted="Change Brightness";
    }
    else{
      menuBrightnessTargeted=false;
      menuDistanceTargeted=true;
      menuCurrentTargeted="Change Distance";
    }
}

void ultrasonicSensorReading(){
    unsigned long currentMicros = micros();     
    if(triggerState == LOW && currentMicros - previousMicros >= offTime)     
     {      
       triggerState = HIGH;   
       previousMicros = currentMicros; 
       digitalWrite(triggerPin, triggerState);  
     }    
     else if(triggerState == HIGH && currentMicros - previousMicros >= onTime)    
     {           
       triggerState = LOW;   
       previousMicros = currentMicros;       
       digitalWrite(triggerPin, triggerState);
     }    
    duration = myPulseIn(echoPin,HIGH,1000);   
    distance = ((duration*0.034)/2);
    if(distance < optionDistance && distance != 0){
      turnOnLED(RGBstate);
      analogWrite(buzzerPin,buzzerVolume);
    }
    else{
      turnOffLED();
      analogWrite(buzzerPin,0);
    }
}


unsigned long myPulseIn(int pin, int value, int timeout) {
  unsigned long currentMicros = micros();
  while(digitalRead(pin) == value) {
    if (micros() - currentMicros > (timeout*1000)) {
      return 0;
      }
  }
  currentMicros = micros();
  while (digitalRead(pin) != value) { 
    if (micros() - currentMicros > (timeout*1000)) { 
      return 0;
    }
  }
  currentMicros = micros();
  while (digitalRead(pin) == value) { 
    if (micros() - currentMicros > (timeout*1000)) {
      return 0;
    }
  }
  return micros()-currentMicros;
}


void turnOnLED(String color){
  if(color=="Red"){
     digitalWrite(RGBRedPin,HIGH);
     digitalWrite(RGBGreenPin,LOW);
     digitalWrite(RGBBluePin,LOW);
  }
  else if(color=="Green"){
    digitalWrite(RGBGreenPin,HIGH);
    digitalWrite(RGBBluePin,LOW);
    digitalWrite(RGBRedPin,LOW);
  }
  else{
    digitalWrite(RGBBluePin,HIGH);
    digitalWrite(RGBRedPin,LOW);
    digitalWrite(RGBGreenPin,LOW);
  }
}

void turnOffLED(){
  digitalWrite(RGBRedPin,LOW);
  digitalWrite(RGBGreenPin,LOW);
  digitalWrite(RGBBluePin,LOW);
}


void displayRGBColor(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Color: ");
  lcd.print(RGBstate);
}

void displayDistance(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Distance: ");
  lcd.print(optionDistance);
  lcd.print("cm");
}

void displayVolume(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Volume: ");
  if(buzzerVolume==0){
    lcd.print("MUTE");
  }
  else if(buzzerVolume==250){
    lcd.print("MAX");
  }
  else{
    lcd.print(buzzerVolume);
  }
}

void displayBrightness(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Brightness: ");
  if(optionBrightness==25){
    lcd.print("MIN");
  }
  else if(optionBrightness==250){
    lcd.print("MAX");
  }
  else{
    lcd.print(optionBrightness);
  }
}



  
