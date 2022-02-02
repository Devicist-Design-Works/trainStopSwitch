//-----USAGE-----//
//To control the track switch and speed of the train, send 2 values through
//the serial port in this format: switchVal,trainSpeed;

//-----VALUES----//
//switchVal - either 0 or 1
//trainSpeed - range -100 to 100
//-100 = full speed backwards
//0 = stopped
//100 = full speed forwards


#include <Adafruit_MCP4725.h>

//We're using this chip to allow the arduino to emulate the potentiometer
//reading from the original Train remote
Adafruit_MCP4725 dac;

//Note - Will change these variable names to match their position on track
int irVal1;
int irVal2;
int irVal3;
int irVal4;

//Analog Value of Potentiometer attached to Lever
int potVal;

int irPin1 = A0;
int irPin2 = A1;
int irPin3 = A2;
int irPin4 = A3;
int potPin = A4;

//Pin for the track switch relay
int switchPin = 7;

int trainSpeed = 0;
int switchVal = 0;

void setup() {
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(irPin3, INPUT);
  pinMode(irPin4, INPUT);
  pinMode(potPin, INPUT);
  pinMode(switchPin, OUTPUT);

  Serial.begin(115200);
  dac.begin(0x62);

}


void loop() {
  setTrainSpeed(trainSpeed);
  setTrack(switchVal);

  if (Serial.available()) {
    extractCommandValues();
  }
  readAndSendSensors();
  Serial.println(switchVal);

}

void setTrainSpeed(int trainSpeed) {
  //maps inputted speed values from serial port to readable values by the train
  int mapTrainSpeed = map(trainSpeed, 100, -100, 0, 2700);

  //protecting the chip on the train remote
  if (mapTrainSpeed > 2700) mapTrainSpeed = 2700;

  //sends speed to train
  dac.setVoltage(mapTrainSpeed, false);
}

void setTrack(int switchVal) {

  switch (switchVal) {
    case 0:
      if (analogRead(potPin) < 600) {
        switchVal = 1;
        digitalWrite(switchPin, switchVal);
      }
      if (analogRead(potPin) > 800) {
        switchVal = 0;
        digitalWrite(switchPin, switchVal);
      }
      break;

    case 1:
      digitalWrite(switchPin, 0);
      break;

    case 2:
      digitalWrite(switchPin, 1);
      break;
  }
}

//function for parsing incoming serial commands
void extractCommandValues() {
  String commandSequence = Serial.readStringUntil(';');
  String speedString = getValueFromString(commandSequence, ',', 0);
  String switchString = getValueFromString(commandSequence, ',', 1);
  trainSpeed = speedString.toInt();
  switchVal = switchString.toInt();
}

void readAndSendSensors() {
  Serial.print(analogRead(potPin));
  Serial.print(',');
  Serial.print(analogRead(irPin1));
  Serial.print(',');
  Serial.print(analogRead(irPin2));
  Serial.print(',');
  Serial.print(analogRead(irPin3));
  Serial.print(',');
  Serial.print(analogRead(irPin4));
  Serial.print(',');
  Serial.println(analogRead(switchVal));
}


// Serial helper to extract values form sequence (eg: 1,2,3;)
String getValueFromString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
