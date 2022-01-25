#include <Adafruit_MCP4725.h>

//#include <analogWrite.h>

Adafruit_MCP4725 dac;

int irVal1;
int irVal2;
int irVal3;
int irVal4;
int potVal;

int irPin1 = A0;
int irPin2 = A1;
int irPin3 = A2;
int irPin4 = A3;
int potPin = A4;
int switchPin = 7;
//int dacPin = 25;

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
  //pinMode(dacPin, OUTPUT);

  Serial.begin(115200);
  dac.begin(0x62);
  

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
  int mapTrainSpeed = map(trainSpeed, 0,255,0,2700);
  if(mapTrainSpeed > 2700) mapTrainSpeed = 2700;
  dac.setVoltage(mapTrainSpeed, false);
}

void setTrack(int switchVal){
  if(analogRead(potPin) < 600) {
    switchVal = 1;
    digitalWrite(switchPin, switchVal);
  }
  if(analogRead(potPin) > 800) {
    switchVal = 0;
    digitalWrite(switchPin, switchVal);
  }
  

}

void extractCommandValues() {
    String commandSequence = Serial.readStringUntil(';');
    
    String switchString = getValueFromString(commandSequence, ',', 0);
    String speedString = getValueFromString(commandSequence, ',', 1);
    switchVal = switchString.toInt();
    trainSpeed = speedString.toInt();

   
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
  Serial.println(analogRead(irPin4));  
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
