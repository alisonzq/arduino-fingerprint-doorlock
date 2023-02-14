#include <Adafruit_Fingerprint.h>
#include "SR04.h"

#define TRANSISTOR_PIN 5
#define RELAY_PIN 4
#define ACCESS_DELAY 3000 //nombre de temps déverouillé
#define TRIG_PIN1 13
#define ECHO_PIN1 12
#define TRIG_PIN2 11
#define ECHO_PIN2 10

SoftwareSerial mySerial(2,3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

SR04 ultra1 = SR04(ECHO_PIN,TRIG_PIN); //ultrason qui active le capteur fingerprint
SR04 ultra2 = SR04(ECHO_PIN,TRIG_PIN); //ultrason en arrière de la porte

boolean sensorTurnedOn = false;


void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); //éteint le relay initialement
}


void loop() {
  int d1 = ultra1.Distance();
  int d2 = ultra2.Distance();
  
  //si la distance est moins de 10, le capteur de doigts s'allume
  if (d1 > 10 && !sensorTurnedOn) { 
    digitalWrite(TRANSISTOR_PIN, HIGH); //Turn on the transistor
    while (!Serial);
    Serial.println("Fingerprint sensor test");
    finger.begin(57600);
    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
      sensorTurnedOn = true;
    } else {
      Serial.println("Did not find fingerprint sensor");
      while (1) { delay(1); }
    }
  } else if (d1 <= 10 && sensorTurnedOn) {
    digitalWrite(TRANSISTOR_PIN, LOW);  //Turn off the transistor & le fingerprint s'éteint
    sensorTurnedOn = false; 
  }

  if (getFingerprintID() != -1 || d2 < 10) //si fingerprint match OU la distance du 2e ultrason est moins de 10, le verrou s'ouvre
  {
    digitalWrite(RELAY_PIN, LOW);
    delay(ACCESS_DELAY);
    digitalWrite(RELAY_PIN, HIGH); 
  }  
  delay(50);
}

uint8_t getFingerprintID() { //fonction qui vérifie si le fingerprint est dans la base de donnée
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return p;
  p = finger.image2Tz();

  if (p != FINGERPRINT_OK)  return p;
  p = finger.fingerFastSearch();

  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return -1;
  } else {
    Serial.println("Unknown error");
  }

  return p;
}


