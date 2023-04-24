#define RELAY_PIN 8
#define RELAY_PIN1 9
#define RELAY_PIN2 10


void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(RELAY_PIN1, LOW);
    digitalWrite(RELAY_PIN2, HIGH);
    delay(1000); 
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(RELAY_PIN1, HIGH);
    digitalWrite(RELAY_PIN2, LOW);
    delay(1000); 
       
}
