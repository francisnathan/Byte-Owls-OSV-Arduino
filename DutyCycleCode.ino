//ARDUINO PIN NUMBERS
int digitalDutyCycleReadPin = 8;

//MISSION-SPECIFIC TASK DATA
int dutyCycle = -1;

void setup() {
  Serial.begin(9600);
  pinMode(digitalDutyCycleReadPin, INPUT);

}

void loop() {
  dutyCycle = readDutyCycle(digitalDutyCycleReadPin);
  Serial.println(dutyCycle);

}

//Reads the Duty Cycle from the Pylon through the given Pin.
int readDutyCycle(int readPin){
  unsigned long duration;
  int dutyCycleSum = 0;
  //Number of interations (seconds to measure over) to ensure correct Duty Cycle is read.
  int numIterations = 5;
  for(int i = 0; i < numIterations; i++){
    duration = pulseInLong(readPin, HIGH);

    //Duty Cycle Calculation
    int dutyCycleRead = (int) ((duration/100000.0) / (1.0/100.0));
    dutyCycleSum += dutyCycleRead;
  }

  return (dutyCycleSum/numIterations);
}

