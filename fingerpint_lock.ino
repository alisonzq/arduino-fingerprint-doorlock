#include <Adafruit_Fingerprint.h>
#include "SR04.h"
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define RELAY_PIN 5
#define RELAY_PIN2 8
//#define RELAY_PIN3 6
#define TRIG_PIN1 13
#define ECHO_PIN1 12
#define TRIG_PIN2 11
#define ECHO_PIN2 10
#define SERVO_PIN 9
#define ACCESS_DELAY 3500 //nombre de temps que la serrure est déverouillé

SoftwareSerial mySerial(2,3); //branches 2 et 3 pour le capteur fingerprint
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo servo;
SR04 ultra1 = SR04(ECHO_PIN1,TRIG_PIN1); //ultrason qui active le capteur fingerprint
SR04 ultra2 = SR04(ECHO_PIN2,TRIG_PIN2); //ultrason en arrière de la porte
LiquidCrystal_I2C lcd(0x27,16,2);

boolean sensorTurnedOn = false;
boolean lcdInitialized = false;


void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN); //servo pin 9
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);  
  //pinMode(RELAY_PIN3, OUTPUT);
    
}


void loop() {
  int d1 = ultra1.Distance();
  int d2 = ultra2.Distance();

  //si la distance est moins de 10, le capteur de doigts s'allume
  if (d1 <= 25  && !sensorTurnedOn) { 
    digitalWrite(RELAY_PIN2, HIGH); //Turn on the relay des capteurs
    //digitalWrite(RELAY_PIN3, HIGH); //Turn on the relay lcd

    if (!lcdInitialized) {
      lcd.init(); 
      lcdInitialized = true; // Set the variable to true so that the LCD is not initialized again
    }

    //lcd.init(); 
    lcd.clear();         
    lcd.backlight(); 
    displayWaitFinger();
    while (!Serial);
    Serial.println("Fingerprint sensor test");
    finger.begin(57600); //Le capteur fingerprint commence s'allume
    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
      sensorTurnedOn = true;
    } else {
      Serial.println("Did not find fingerprint sensor");
      while (1) { delay(1); }
    }
  } else if (d1 > 25 && sensorTurnedOn) {   
    lcd.clear();
    lcd.noBacklight(); // turn off the LCD backlight
    digitalWrite(RELAY_PIN2, LOW);  //Turn off the relay & le fingerprint s'éteint
    Serial.println("turned off");    
    sensorTurnedOn = false; 
  }

  if (getFingerPrint() != -1) //si fingerprint match le verrou s'ouvre
  {
    displayFingerOK();
    openDoor();
    displayLocked();
  }

  if (d2 <= 4) //si la distance du 2e ultrason est moins de 5, le verrou s'ouvre
  {
    openDoor();
  }

  delay(50);
}

void openDoor() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("relay on");
  delay(1500);
  servo.write(25); //et le servo s'active et la porte s'ouvre de 90 degré
  delay(ACCESS_DELAY);
  servo.write(100); //après un nombre de temps "ACCESS_DELAY", la porte ferme à 0 degré
  delay(1000);
  digitalWrite(RELAY_PIN, LOW); //et la serrure se verrouille 
  Serial.println("relay off");
}

int getFingerPrint() 
{
  //displayScanning();
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;    

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;     

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_NOTFOUND) {
    displayInvalidFinger();
    return -1;    
  }

  // found a match!
  Serial.println("found a match!");
  return finger.fingerID;
}


void displayWaitFinger() {
  lcd.clear();         
  lcd.backlight(); 
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("PLACE FINGER");
  lcd.setCursor(0,1);
  lcd.print("ON THE SCANNER");

}



void displayInvalidFinger(){
  lcd.clear();         
  lcd.backlight(); 
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("UM... YOU DON'T");
  lcd.setCursor(0,1);
  lcd.print("HAVE ACCESS!!!");

}

void displayFingerOK(){ 
  lcd.clear();
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("WELCOME");
  lcd.setCursor(0,1);
  lcd.print("DOOR UNLOCKED");
}

void displayLocked(){
  lcd.clear();         
  lcd.backlight(); 
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("DOOR IS");
  lcd.setCursor(0,1);
  lcd.print("LOCKED");
  delay(1000);
}


void displayScanning(){
  lcd.clear();         
  lcd.backlight();      
  lcd.scrollDisplayRight();
  lcd.setCursor(0,0);
  lcd.print("SCANNING...");

}

