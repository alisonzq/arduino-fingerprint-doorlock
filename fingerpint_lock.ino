#include <Adafruit_Fingerprint.h> //permet d'utiliser un capteur d'empreintes digitale
#include "SR04.h" //permet d'utiliser des capteurs à ultrasons
#include <Servo.h> //permet de contrôler un servomoteur
#include <LiquidCrystal_I2C.h> //permet d'utiliser un écran LCD en mode I2C
#include <Wire.h> //bibliothèque nécessaire pour communiquer avec le LCD via I2C

#define RELAY_PIN 5 //relai pour contrôler l'ouverture du verrou
#define RELAY_PIN2 8 //relai pour contrôler l'activation de l'écran lcd et le capteur fingerprint
#define TRIG_PIN1 13 //pins pour ultrason  devant la porte
#define ECHO_PIN1 12
#define TRIG_PIN2 11 //pins pour ultrason derrière la porte
#define ECHO_PIN2 10
#define SERVO_PIN 9 //servo moteur
#define RX_PIN 3 //pins pour le capteur finigerprint
#define TX_PIN 2 
#define ACCESS_DELAY 3500 //nombre de temps que la serrure est déverouillé

SoftwareSerial mySerial(TX_PIN,RX_PIN); //initialise une connexion série logicielle pour communiquer avec le capteur d'empreintes digitales
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); //crée une instance de la classe Adafruit_Fingerprint pour interagir avec le capteur d'empreintes digitales
Servo servo; //crée une instance de la classe Servo pour contrôler le servomoteur
SR04 ultra1 = SR04(ECHO_PIN1,TRIG_PIN1); //crée une instance de la classe SR04 pour interagir avec le capteur à ultrasons devant la porte qui permet d'ouvrir le RELAY_PIN2
SR04 ultra2 = SR04(ECHO_PIN2,TRIG_PIN2); //ultrason en arrière de la porte
LiquidCrystal_I2C lcd(0x27,16,2); //crée une instance de la classe LiquidCrystal_I2C pour interagir avec l'écran LCD via I2C

boolean sensorTurnedOn = false; //bool qui nous permettront de garder trace de l'état des composantes - pour le fingerprint
boolean lcdInitialized = false; //pour l'iinitialisation du lcd

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN); //initialise le servomoteur
  pinMode(RELAY_PIN, OUTPUT); //définir les branches comme des sorties pour le contrôle des relais
  pinMode(RELAY_PIN2, OUTPUT);    
}


void loop() {
  int d1 = ultra1.Distance();
  int d2 = ultra2.Distance();

  //si la distance est moins de 25, le capteur de doigts s'allume
  if (d1 <= 25  && !sensorTurnedOn) { 
    digitalWrite(RELAY_PIN2, HIGH); //Turn on the relay des capteurs

    if (lcdInitialized == false) {
      lcd.init(); 
      lcdInitialized = true; // Set the variable to true so that the LCD is not initialized again
    }

    lcd.clear();         
    lcd.backlight(); 
    displayWaitFinger();
    while (!Serial);
    Serial.println("Fingerprint sensor test");
    finger.begin(57600); //Le capteur fingerprint s'allume et peut capter des empreintes
    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
      sensorTurnedOn = true; //la capteur est allumé donc change la variable à vrai
    } else {
      Serial.println("Did not find fingerprint sensor");
      while (1) { delay(1); }
    }
  } else if (d1 > 25 && sensorTurnedOn) {   
    lcd.clear();
    lcd.noBacklight(); // turn off the LCD backlight
    digitalWrite(RELAY_PIN2, LOW);  //Turn off the relay & le fingerprint s'éteint
    Serial.println("turned off");    
    sensorTurnedOn = false; //la capteur est éteiint donc change la variable à faux
  }

  if (getFingerPrint() != -1) //si fingerprint match le verrou s'ouvre
  {
    displayFingerOK(); //display message that it's unlocked and then plays the sequence
    openDoor();
    displayLocked(); //when sequence is runned, displays locked door
  }

  if (d2 <= 4) //si la distance du 2e ultrason est moins de 4 cm, le verrou s'ouvre sans activer fingerprint et lcd
  {
    openDoor();
  }

  delay(50);
}

void openDoor() { // fonction qui roule séquence de l'ouverture de la porte
  digitalWrite(RELAY_PIN, HIGH); //déverouille serrure
  Serial.println("relay on");
  delay(1500); 
  servo.write(25); //et le servo s'active et la porte s'ouvre de 90 degré
  delay(ACCESS_DELAY);
  servo.write(100); //après un nombre de temps "ACCESS_DELAY", la porte ferme
  delay(1000);
  digitalWrite(RELAY_PIN, LOW); //après 1 seconde la serrure se verrouille 
  Serial.println("relay off");
}

int getFingerPrint() //fonction qui authentifie l'empreinte
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

// les prochaines fonctions sont pour afficher du texte sur l'écran LCD
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

