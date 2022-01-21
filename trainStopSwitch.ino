#include <analogWrite.h>

int irV1;
int potVal;
int potPin = 39;
int irPin1 = 4;
int switchPin = 13;
int dacOut = 25;

//train speeds
int stopped = 130;
int fullForward = 60;
int fullReverse = 190;
int slowForward = 100;
int slowReverse = 160;

boolean inStation = false;

void setup() {
  pinMode(dacOut, OUTPUT);
  pinMode(irPin1, INPUT);
  pinMode(switchPin, OUTPUT);
  Serial.begin(9600);
  dacWrite(25, fullForward);
}

void loop() {
  irV1 = analogRead(irPin1);

  //Detecting if IR sensor has been tripped
  if (irV1 > 1200) {
    inStation = true;

    //stops train
    dacWrite(25, stopped);
    delay(3000);
  }

  while (inStation == true) {
    potVal = analogRead(potPin);

    //using a 10k pot to mimic the switch. The train won't move unless
    //Pot is turned all the way one way or the other
    if (potVal >= 4000) {
      digitalWrite(switchPin, HIGH);
      dacWrite(25, fullForward);
      delay(5000);
      inStation = false;
    }
    if (potVal <= 500) {
      digitalWrite(switchPin, LOW);
      dacWrite(25, fullForward);
      delay(5000);
      inStation = false;
    }
  }
}
