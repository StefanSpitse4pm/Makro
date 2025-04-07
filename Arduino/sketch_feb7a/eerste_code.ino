// the setup function runs once when you press reset or power the board

const int buttonPin = 4;
int buttonState = 0;

int ledRed = 13;
int ledGreen = 12;
int ledYellow = 8;

void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
 

  pinMode(buttonPin, INPUT);

}
// the loop functions runs over and over again forever
void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    lichten();
  } else {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
  }

}

void lichten() {
  
 digitalWrite(ledRed, LOW);  
 delay(3000); // Wait for 3000 milliseconds
 digitalWrite(ledRed, HIGH);
 delay(1); // Wait for 1 milliseconds

 
 digitalWrite(ledYellow, LOW);  
 delay(3000); // Wait for 4000 milliseconds
 digitalWrite(ledYellow, HIGH);
 delay(1); // Wait for 1 milliseconds

 digitalWrite(ledGreen, LOW);  
 delay(2000); // Wait for 2000 milliseconds
 digitalWrite(ledGreen, HIGH);
 delay(1); // Wait for 1 milliseconds
}


 












