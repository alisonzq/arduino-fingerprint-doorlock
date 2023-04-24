#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include "SR04.h"

#define RELAY_PIN 8
#define RELAY_PIN1 9

#define TRIG_PIN1 13
#define ECHO_PIN1 12

SR04 ultra1 = SR04(ECHO_PIN1,TRIG_PIN1); //ultrason qui active le capteur fingerprint
LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySerial(2,3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

boolean sensorTurnedOn = false;


void setup() {
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(RELAY_PIN1, OUTPUT);
}

void displayFingerOK(){

  lcd.clear();
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("WELCOME");
  lcd.setCursor(0,1);
  lcd.print("DOOR UNLOCK");

}

void loop() {

  int d1 = ultra1.Distance();
  if (d1 <= 10 && !sensorTurnedOn) { 
    digitalWrite(RELAY_PIN, HIGH); //Turn on the relay
    digitalWrite(RELAY_PIN1, HIGH);
    lcd.init();
    lcd.clear();         
    lcd.backlight();  
    displayFingerOK() ;  
    delay(1000);
    sensorTurnedOn = true;

  } else if (d1 > 10 && sensorTurnedOn) {
    digitalWrite(RELAY_PIN, LOW);  //Turn off the relay & le fingerprint s'éteint
    digitalWrite(RELAY_PIN1, LOW); 
    sensorTurnedOn = false; 
  }

}
