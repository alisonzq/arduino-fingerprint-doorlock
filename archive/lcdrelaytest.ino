#include <LiquidCrystal_I2C.h>
#include "SR04.h"
#include <Wire.h>

#define RELAY_PIN 8

#define TRIG_PIN1 13
#define ECHO_PIN1 12

SR04 ultra1 = SR04(ECHO_PIN1,TRIG_PIN1); 
LiquidCrystal_I2C lcd(0x27,16,2);

boolean sensorTurnedOn = false;
boolean lcdInitialized = false; // new variable to keep track of whether the LCD has been initialized or not

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  delay(50);
}

void loop() {

  int d1 = ultra1.Distance();
  if (d1 <= 10 && !sensorTurnedOn) { 
    digitalWrite(RELAY_PIN, HIGH); //Turn on the relay
    
    // Initialize the LCD screen if it has not been initialized yet
    if (!lcdInitialized) {
      lcd.init();
      lcd.clear();         
      lcd.backlight();  
      lcdInitialized = true; // Set the variable to true so that the LCD is not initialized again
    }
    
    displayFingerOK();
    delay(1000);
    sensorTurnedOn = true;

  } else if (d1 > 10 && sensorTurnedOn) {
    lcd.clear();  
    lcd.noBacklight();
    Serial.println("turned off");
    digitalWrite(RELAY_PIN, LOW);  //Turn off the relay & le fingerprint s'éteint
    sensorTurnedOn = false; 
  }

}

void displayFingerOK(){

  lcd.clear();
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("WELCOME");
  lcd.setCursor(0,1);
  lcd.print("DOOR UNLOCK");

}
