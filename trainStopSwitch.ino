/*
 * USAGE
 * 
 * To control the track switch and speed of the train, send either
 * 1 or 2 values to the serial port seperated by a comma and ending
 * with a semi-colon.(eg: "50;"  OR  "50,1;" ) - without the quotations. 
 * 
 * The first value is the speed value ranging from -100 to 100.
 * -100 is full speed backward, 100 is full speed forward, 0 is stopped.
 * 
 * The second value is the track switch. 1 is switched to the left, 2 is to the right
 * IMPORTANT: if 0 or no value is sent, the lever position will determine the track switch.
 * 
 * The following values are constantly streamed out over serial as a comma separated string.
 * 1 - lever state (0 = neutral, 1 = pulled, 2 = pushed, 3 = error)
 * 2 - right tunnel IR value (0 to 1023)
 * 3 - station IR value (0 to 1023)
 * 4 - lever stop IR value (0 to 1023)
 * 5 - left tunnel IR value (0 to 1023)
 * 6 - track control state (0 = direct lever control, 1 = forced left, 2 = forced right)
 * 
 * The IR values are used to track the position of the train around the track. 
 * For flexibility, the train route can be controlled remotely in combo with the lever state,
 * or the route be selected by the lever directly.
 * 
 * contact: info@devicist.com
*/




#include <Adafruit_MCP4725.h>

//We're using this chip to allow the arduino to emulate the potentiometer
//reading from the original Train remote
Adafruit_MCP4725 dac;

enum leverStateVals {
  neutral,
  pulled,
  pushed,
  error
};
int leverState = neutral;

enum trackControlStates {
  leverControl,
  forcedLeft,
  forcedRight
};
int trackControlState = leverControl;

int trainSpeed = 0;

enum trackSwitchStates {
  left,
  right
};

//Note - Will change these variable names to match their position on track
int rightTunnelIrVal;
int leftTunnelIrVal;
int stationIrVal;
int leverStopIrVal;

int rightTunnelIrPin = A0;
int leftTunnelIrPin = A3;
int stationIrPin = A1;
int leverStopIrPin = A2;
int pullPin = 11;
int pushPin = 12;

//Pin for the track switch relay
int trackPin = 7;

void setup() {
  pinMode(rightTunnelIrPin, INPUT);
  pinMode(stationIrPin, INPUT);
  pinMode(leverStopIrPin, INPUT);
  pinMode(leftTunnelIrPin, INPUT);
  
  pinMode(trackPin, OUTPUT);

  pinMode(pullPin, INPUT_PULLUP);
  pinMode(pushPin, INPUT_PULLUP);  

  Serial.begin(115200);
  dac.begin(0x62);
}


void loop() {
  setTrainSpeed(trainSpeed);
  setTrack(trackControlState);
  updateLeverState();

  if (Serial.available()) {
    extractCommandValues();
  }
  readAndSendSensors();
  delay(5);
}


void setTrainSpeed(int trainSpeed) {
  //maps inputted speed values from serial port to readable values by the train
  int mapTrainSpeed = map(trainSpeed, 100, -100, 0, 2700);

  //protecting the chip on the train remote
  if (mapTrainSpeed > 2700) mapTrainSpeed = 2700;

  //sends speed to train
  dac.setVoltage(mapTrainSpeed, false);
}

void updateLeverState() {  
  int isPushed = !digitalRead(pullPin);
  int isPulled = !digitalRead(pushPin);  

  if(isPulled && !isPushed){
    leverState = pulled;
  }
  else if(isPushed && !isPulled){
    leverState = pushed;
  }
  else if (!isPulled && !isPushed) {
    leverState = neutral;
  }
  else {
    leverState = error;
  }
}

void setTrack(int trackControlState) {
  switch (trackControlState) {

    case forcedLeft:
      digitalWrite(trackPin, left);
      break;

    case forcedRight:
      digitalWrite(trackPin, right);
      break;

    case leverControl:
      if (leverState == pushed) {
        digitalWrite(trackPin, left);
      }
      else if (leverState == pulled) {
        digitalWrite(trackPin, right);
      }
      else {
        digitalWrite(trackPin, right);
      }
      break;      
  }
}

void readAndSendSensors() {
  Serial.print(leverState);
  Serial.print(',');
  Serial.print(analogRead(rightTunnelIrPin));
  Serial.print(',');
  Serial.print(analogRead(stationIrPin));
  Serial.print(',');
  Serial.print(analogRead(leverStopIrPin));
  Serial.print(',');
  Serial.print(analogRead(leftTunnelIrPin));
  Serial.print(',');
  Serial.println(trackControlState);
}

//function for parsing incoming serial commands
void extractCommandValues() {
  String commandSequence = Serial.readStringUntil(';');
  String speedString = getValueFromString(commandSequence, ',', 0);
  String trackString = getValueFromString(commandSequence, ',', 1);
  trainSpeed = speedString.toInt();
  trackControlState = trackString.toInt();
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
