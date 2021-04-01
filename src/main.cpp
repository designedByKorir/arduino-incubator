#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>
#include <DHT.h>
#include <TimerOne.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "RTClib.h"

//macros
#define TWELVE_HRS 3600000UL
unsigned long startTime;
#define ONE_WIRE_BUS 8 //ds18b20 module attach to pin7
#define DHTPIN 9     
#define DHTTYPE DHT11   // DHT 11

 
void beeper();
void takeReading();
void light();
void nowTime();
void turn();
void resolveInputFlags();
void inputAction(int input);
void parameterChange(int key);
void printScreen();
void populate();


 
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {12, 13, A0, A1}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A2, A3, 7, 6}; //connect to the column pinouts of the keypad

RTC_DS3231 rtc;
OneWire oneWire(ONE_WIRE_BUS); // Pass our oneWire reference totempRelay Dallas Temperature. 
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature. 
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Ticker readingTimer(takeReading, 2000); // once , every 2 seconds
Ticker backlightTimer(light, 12000,1); // once, after 12seconds
Ticker timer1(nowTime, 0, 1);  // once, immediately 
Ticker turnOff(turn, 6000, 1);  // once, after 6 seconds
Ticker buzz(beeper, 1000, 0);  // once, immediately 
 


// float temp, humid, t;
const byte tempRelay = 11;
const byte motorRelay = 10;
const byte buzzerPin= 4;

int beeps =0;
int addr = 0;
// boolean stater = HIGH;
int startHour, startMinute, nowHour,nowMinute;
 unsigned long currensMillis ;

const int numOfInputs = 5;  //Input & Button Logic
// int inputState[numOfInputs];
// int lastInputState[numOfInputs] = {LOW,LOW,LOW,LOW,LOW};
bool inputFlags[numOfInputs] = {LOW,LOW,LOW,LOW,LOW};
// long lastDebounceTime[numOfInputs] = {0,0,0,0,0};
// long debounceDelay = 5;

//LCD Menu Logic
const int numOfScreens = 8;
int currentScreen = 6;
String screens[numOfScreens][2] = {{"Temp Off","degC"},{"Temp On","degC"}, {"Turn active","Y/N"}, {"Humidity","%"},{"Frequency","hrs"},
{"turn stop","day"},{"Temperature","degC"}, {"Humidity","%"} };
int parameters[numOfScreens];



void setup() {
  //Serial.begin(9600);
  sensors.begin();          // initialize the bus
  dht.begin();
  lcd.init(); 
  readingTimer.start();
  populate();

 
   timer1.start();
   //  turnOff.start();
    // buzz.start();
    startTime = millis();
takeReading();
  wdt_disable();
  lcd.setCursor(3,0);
  lcd.print("Simplex designs");
  lcd.setCursor(3,1);
  lcd.print("Please wait");
  lcd.setCursor(3,1);
  lcd.print("configuring");
  delay(3000);
  lcd.clear();
  wdt_enable(WDTO_2S);
 pinMode(tempRelay,OUTPUT);
 pinMode(motorRelay,OUTPUT);
 digitalWrite(motorRelay,HIGH);
}

void loop() {
   wdt_reset();
  readingTimer.update();
   timer1.update();
   backlightTimer.update();
   turnOff.update();
   buzz.update();
   char key = keypad.getKey();
 
  if (key){
   noTone(buzzerPin);
   tone(buzzerPin, 440, 200);
 
   lcd.backlight();
   backlightTimer.start();
   
    switch (key) {
    case 'A':   
     if (parameters[2]%2==0){
      digitalWrite(motorRelay,LOW);
      turnOff.start();
    }else{
      lcd.clear();
       lcd.setCursor(0,0);
     lcd.print("Turning Disabled");
         
      } 
      delay(1000);
      lcd.clear();
    break;
    case 'B':   
    parameters[2]+=1;
     EEPROM.write(2, parameters[2]);
     lcd.clear();
     lcd.setCursor(3,1);
     lcd.print("turner :");
     if (parameters[2] %2==1){
        lcd.setCursor(10,1);
     lcd.print("off");
      }
     else{
         lcd.setCursor(10,1);
     lcd.print("on");
      } 
    
      delay(1000);
       lcd.clear();
      break;
    case 'C':    
      buzz.stop();
      break;
    case 'D':   
     // Serial.println("bright");
      break;
     case '2':   
      inputFlags[0] = HIGH;
      break;
     case '4':   
      inputFlags[1] = HIGH;
       break;
       case '5':   
      inputFlags[2] = HIGH;
       break;
       case '6':   
      inputFlags[3] = HIGH;
      break;
       case '8':   
      inputFlags[4] = HIGH;
      break;
  }

  resolveInputFlags();
  }

    if (parameters[6]>=parameters[0]){
       digitalWrite(tempRelay, HIGH);
   
    
    // }else if (temp<lsetpoint){
      }else if (parameters[6]<parameters[1]){

          digitalWrite(tempRelay, LOW);
    
    }
   if (parameters[7]<parameters[3]){
   buzz.start();
    
    }
   if (beeps==1){
    beeper();
    }

  //   lcd.setCursor(1,0);
  // lcd.print("TEMP ");
  // lcd.setCursor(11,0);
  // lcd.print("HUM");
  //  lcd.setCursor(1,1);
  // lcd.print(temp);
  // lcd.setCursor(6,1);
  // lcd.print(char(223));//print the unit" ℃ "
  // lcd.print("C");
  //  lcd.setCursor(11,1);
  // lcd.print(humid);
 if (millis() - startTime > (TWELVE_HRS*parameters[4]))
  {
     digitalWrite(motorRelay,LOW);
    startTime = millis();
     turnOff.start();
  }

}
void beeper(){
    noTone(buzzerPin);
 
  tone(buzzerPin, 440, 200);
  }

void takeReading(){
 sensors.requestTemperatures(); // Send the command to get temperatures
  parameters[6]= int(sensors.getTempCByIndex(0));
    
   parameters[7] = int(dht.readHumidity());  // Read humidity 

  // t = dht.readTemperature();  // Read temperature as Celsius 
  if (isnan(parameters[6]) || isnan(parameters[7])  ) {
   // Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void light() {
   lcd.noBacklight(); 
}
  
  void nowTime(){
 DateTime now = rtc.now();
 startHour=now.hour();
  startMinute=now.minute();
  if (parameters[2]%2==0){
  digitalWrite(motorRelay,LOW);
    turnOff.start();
    }
  }

void turn(){
   digitalWrite(motorRelay,HIGH);
  }

 void resolveInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    if(inputFlags[i] == HIGH) {
      inputAction(i);
      inputFlags[i] = LOW;
      printScreen();
    }
  }
}

void inputAction(int input) {
  if(input == 0) {
    if (currentScreen == 0) {
      currentScreen = numOfScreens-1;
    }else{
      currentScreen--;
    }
  }else if(input == 1) {
    if (currentScreen == numOfScreens-1) {
      currentScreen = 0;
    }else{
      currentScreen++;
    }
  }else if(input == 2) {
    parameterChange(0);
  }else if(input == 3) {
    parameterChange(1);
  }else if(input == 4 && (currentScreen!=6||currentScreen!=7)) {
     EEPROM.write(input, parameters[currentScreen]);
  }
}

void parameterChange(int key) {
  if(key == 0) {
    parameters[currentScreen]++;
  }else if(key == 1) {
    parameters[currentScreen]--;
  }
}
void populate() {
  for(int i = 0; i <  numOfScreens; i++) {
     parameters[i]=EEPROM.read(i);
}
}

void printScreen() {
  lcd.clear();
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(parameters[currentScreen]);
  lcd.print(" ");
  lcd.print(screens[currentScreen][1]);
}


          