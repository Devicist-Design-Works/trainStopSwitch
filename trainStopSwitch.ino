#include <analogWrite.h>


int irVal1;
int irVal2;
int irVal3;
int irVal4;
int potVal;

int irPin1 = 2;
int irPin2 = 35;
int irPin3 = 34;
int irPin4 = 36;
int potPin = 39;
int switchPin = 13;
int dacPin = 25;

//train speeds
const int stopped = 130;
const int fullForward = 60;
const int fullReverse = 190;
const int slowForward = 100;
const int slowReverse = 160;
int trainSpeed = stopped;
int switchVal = 0;

bool inStation = false;


void setup() {
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(irPin3, INPUT);
  pinMode(irPin4, INPUT);    
  pinMode(potPin, INPUT);        
  pinMode(switchPin, OUTPUT);
  pinMode(dacPin, OUTPUT);

  Serial.begin(115200);

}


void loop() {
  setTrainSpeed(trainSpeed);
  setTrack(switchVal);
 
  if(Serial.available()){
    extractCommandValues();
  }
  readAndSendSensors();  
  Serial.println(switchVal);
 
}

void setTrainSpeed(int trainSpeed) {
  dacWrite(dacPin, trainSpeed);
}

void setTrack(int switchVal){
  digitalWrite(switchPin, switchVal);

}

void extractCommandValues() {
    String commandSequence = Serial.readStringUntil(';');
    
    String switchString = getValueFromString(commandSequence, ',', 0);
    String speedString = getValueFromString(commandSequence, ',', 1);
    switchVal = switchString.toInt();
    trainSpeed = speedString.toInt();

    Serial.println(switchVal);
}

void readAndSendSensors() {
//  Serial.print(analogRead(potPin));
//  Serial.print(',');
//  Serial.print(analogRead(irPin1));
//  Serial.print(',');
//  Serial.print(analogRead(irPin2));
//  Serial.print(',');
//  Serial.print(analogRead(irPin3));
//  Serial.print(',');
//  Serial.println(analogRead(irPin4));  
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
