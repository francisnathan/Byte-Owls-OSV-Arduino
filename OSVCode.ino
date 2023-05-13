#include "Enes100.h"
#include <ezButton.h>


//Arduino Pin Numbers
int digitalDutyCycleReadPin = A0;
int reedSensorReadPin = 8;
int limitSwitchReadPin = 9;

int MotorOneInt1 = 4;
int MotorOneInt2 = 5;
int MotorTwoInt3 = 6;
int MotorTwoInt4 = 7;

int rightTrigPin = A1;
int rightEchoPin = A2;

int leftTrigPin = A3;
int leftEchoPin = A4;

int ArmInt1 = 12;
int ArmInt2 = 13;

bool recalibrated = false;
bool atPylon = false;
bool CompletedMission = false;
bool Navigating = false;
bool Limbo = false;

//important mission data
float startX = -1.0;
float startY = -1.0;
float startTheta = -1.0;

ezButton limitSwitch(limitSwitchReadPin);

void setup() {
    // Initialize Enes100 Library 
  // Team Name, Mission Type, Marker ID, TX Pin, RX Pin
  Enes100.begin("BYTE OWLS", DATA, 9, 10, 11);
  // Transmit the magnetism of the data mission (in this case, not magnetic)
  
  Serial.begin(9600);
  //limitSwitch.setDebounceTime(10); //10 milliseconds
  pinMode(digitalDutyCycleReadPin, INPUT);
  pinMode(reedSensorReadPin, INPUT_PULLUP);

  
  pinMode(MotorOneInt1, OUTPUT);
  pinMode(MotorOneInt2, OUTPUT);
  pinMode(MotorTwoInt3, OUTPUT);
  pinMode(MotorTwoInt4, OUTPUT);

  while (!Enes100.updateLocation()) {
    // OSV's location was not found
    Enes100.println("404 Not Found");
  }

  Enes100.updateLocation();
  startX = Enes100.location.x;
  startY = Enes100.location.y;
  startTheta = Enes100.location.theta;
  
  driveToMissionSite();
  MotorMoveForward();
 
}

void loop() {
  //repeated checks for switch
  limitSwitch.loop();

  if(!CompletedMission){

    delay(25);
    MotorOff();
    delay(500);
    Enes100.updateLocation();

    int state = limitSwitch.getState();

    if(state == LOW) {
      atPylon = true;
    }

    /*
    if(!recalibrated){
      if(startY < 1.0){
        if((abs(Enes100.location.y - 1.35) < 0.05)){
          recalibratePylon();
        }
      } else {
        if((abs(Enes100.location.y - .65) < 0.05)){
          recalibratePylon();
        }
      }
      recalibrated = true;
      
    }*/

    if(atPylon){
      MotorOff();

      dropArmIntoPylon(8.5);
      delay(2000);
      int dutyCycle = readDutyCycle();
      bool magnetic = readReedSensor();
      
      if(magnetic == LOW){
        Enes100.mission(MAGNETISM, MAGNETIC);      
      } else {
        Enes100.mission(MAGNETISM, NOT_MAGNETIC);      
      }
      
      Enes100.mission(CYCLE, dutyCycle);
      Enes100.println("Success!");
      raiseArmIntoPylon(6);
      
      CompletedMission = true;
      Navigating = true;
      atPylon = false;

      //OSV moves until checkpoint position (1,1)
      MotorMoveBackwardUntilY(1.0);
      MotorTurnRightUntilTheta(0.0);
      MotorMoveForwardUntilX(1.0);
      MotorOff();
      dropArmIntoPylon(7);      
      MotorMoveForward();
    }

    MotorMoveForward();
    
  } /*else if(Navigating){
    if(Enes100.location.x <= 3.90){
      MotorOff();      
      Enes100.updateLocation();
      //Restriction for left and right walls
      if(Enes100.location.y - 0.2 > 0.25 && Enes100.location.y + 0.2 < 1.75){
        
        //Both ultrasonics detect an obstacle
        if(readRightUltraSonic() < 5 && readLeftUltraSonic() < 5){
          Enes100.updateLocation();
          MotorMoveBackwardUntilX(Enes100.location.x - 0.1);
          if(Enes100.location.y > 1.0){
            MotorTurnRightUntilTheta(-PI/2.0);
            Enes100.updateLocation();
            MotorMoveForwardUntilY(Enes100.location.y - 0.1);
          } else if(Enes100.location.y <= 1.0){
            MotorTurnRightUntilTheta(PI/2.0);
            Enes100.updateLocation();
            MotorMoveForwardUntilY(Enes100.location.y + 0.1);
          }
          Enes100.updateLocation(); 
          MotorTurnRightUntilTheta(0.0);
          Enes100.updateLocation();
          

        } 
        //Detect obstacle on the left
        else if (readRightUltraSonic() > 5 && readLeftUltraSonic() < 5){
          Enes100.updateLocation();
          MotorMoveBackwardUntilX(Enes100.location.x - 0.1);
          MotorTurnRightUntilTheta(-PI/2.0);
          MotorMoveForwardUntilY(Enes100.location.y - 0.1);   
          MotorTurnRightUntilTheta(0.0);
          MotorMoveForward();
        }
        //Detect obstacle on the right
        else if (readRightUltraSonic() < 5 && readLeftUltraSonic() > 5){
          Enes100.updateLocation();
          MotorMoveBackwardUntilX(Enes100.location.x - 0.1);
          MotorTurnRightUntilTheta(PI/2.0);
          MotorMoveForwardUntilY(Enes100.location.y + 0.1);
          MotorTurnRightUntilTheta(0.0);
          MotorMoveForward();
        }

        delay(750);
        MotorMoveForward();
        delay(250);
      } else {


      }
    } else {
      Navigating = false;
      Limbo = true;
      MotorOff();
    }

  } else if(Limbo){
    
  }*/
}

int MotorMoveForward(){
  digitalWrite(MotorOneInt1, HIGH);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, HIGH);
  digitalWrite(MotorTwoInt4, LOW);

}

int MotorMoveBackward(){
  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, HIGH);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, HIGH);  
}

int MotorOff(){
  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, LOW);
}

void driveToMissionSite(){
    if(startY < 1.0){
      MotorTurnRightUntilTheta((PI/2.0));
      MotorMoveForward();
      delay(1500);
      MotorOff();
      Enes100.updateLocation();    
      while(!(abs(Enes100.location.theta - (PI/2.0)) < 0.05)){
      digitalWrite(MotorOneInt1, LOW);
      digitalWrite(MotorOneInt2, HIGH);

      digitalWrite(MotorTwoInt3, HIGH);
      digitalWrite(MotorTwoInt4, LOW);
      delay(10);
      MotorOff();
      delay(500);
      Enes100.updateLocation();  
    } 
    MotorOff();
    MotorMoveForward();
    delay(250);
    
    while(!(abs(Enes100.location.theta - (PI/2.0)) < 0.03)){
      digitalWrite(MotorOneInt1, HIGH);
      digitalWrite(MotorOneInt2, LOW);

      digitalWrite(MotorTwoInt3, LOW);
      digitalWrite(MotorTwoInt4, HIGH);
      delay(5);
      MotorOff();
      delay(500);
      Enes100.updateLocation();  
    } 
    MotorOff();

  } else {
      MotorTurnRightUntilTheta((-PI/2.0));
      MotorMoveForward();
      delay(1500);
      MotorOff();
      Enes100.updateLocation();
      while(!(abs(Enes100.location.theta - (-PI/2.0)) < 0.05)){
      digitalWrite(MotorOneInt1, LOW);
      digitalWrite(MotorOneInt2, HIGH);

      digitalWrite(MotorTwoInt3, HIGH);
      digitalWrite(MotorTwoInt4, LOW);
      delay(10);
      MotorOff();
      delay(500);
      Enes100.updateLocation();  
    } 
    MotorOff();
    MotorMoveForward();
    delay(250);
    
    while(!(abs(Enes100.location.theta - (-PI/2.0)) < 0.03)){
      digitalWrite(MotorOneInt1, HIGH);
      digitalWrite(MotorOneInt2, LOW);

      digitalWrite(MotorTwoInt3, LOW);
      digitalWrite(MotorTwoInt4, HIGH);
      delay(5);
      MotorOff();
      delay(500);
      Enes100.updateLocation();  
    } 
    MotorOff();
  }
}

//Turns right until OSV faces a certain theta.
int MotorTurnRightUntilTheta(float theta){
 
  while(!(abs(Enes100.location.theta - theta) < 0.1)){
    digitalWrite(MotorOneInt1, HIGH);
    digitalWrite(MotorOneInt2, LOW);

    digitalWrite(MotorTwoInt3, LOW);
    digitalWrite(MotorTwoInt4, HIGH);
    delay(25);
    MotorOff();
    delay(500);
    Enes100.updateLocation();  
  } 

  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, LOW); 

}

//Turns left until OSV faces a certain theta.
int MotorTurnLeftUntilTheta(float theta){
  while(!(abs(Enes100.location.theta - theta) < 0.1)){
    digitalWrite(MotorOneInt1, LOW);
    digitalWrite(MotorOneInt2, HIGH);

    digitalWrite(MotorTwoInt3, HIGH);
    digitalWrite(MotorTwoInt4, LOW);
    delay(25);
    MotorOff();
    delay(500);
    Enes100.updateLocation();  
  } 
  MotorOff();

}

int MotorMoveForwardUntilX(float x){

  while(!(abs(Enes100.location.x - x) < 0.05)){
    digitalWrite(MotorOneInt1, HIGH);
    digitalWrite(MotorOneInt2, LOW);

    digitalWrite(MotorTwoInt3, HIGH);
    digitalWrite(MotorTwoInt4, LOW); 
    Enes100.updateLocation(); 
  } 

  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, LOW); 
}

int MotorMoveBackwardUntilX(float x){

  while(!(abs(Enes100.location.x - x) < 0.05)){
    digitalWrite(MotorOneInt1, LOW);
    digitalWrite(MotorOneInt2, HIGH);

    digitalWrite(MotorTwoInt3, LOW);
    digitalWrite(MotorTwoInt4, HIGH);
    Enes100.updateLocation(); 
  } 

  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, LOW); 
}

int MotorMoveForwardUntilY(float y){

  while(!(abs(Enes100.location.y - y) < 0.05)){
    digitalWrite(MotorOneInt1, HIGH);
    digitalWrite(MotorOneInt2, LOW);

    digitalWrite(MotorTwoInt3, HIGH);
    digitalWrite(MotorTwoInt4, LOW);
    Enes100.updateLocation(); 
  } 

  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, LOW); 
}

int MotorMoveBackwardUntilY(float y){

  while(!(abs(Enes100.location.y - y) < 0.05)){
    digitalWrite(MotorOneInt1, LOW);
    digitalWrite(MotorOneInt2, HIGH);

    digitalWrite(MotorTwoInt3, LOW);
    digitalWrite(MotorTwoInt4, HIGH); 
    Enes100.updateLocation(); 
  } 

  digitalWrite(MotorOneInt1, LOW);
  digitalWrite(MotorOneInt2, LOW);

  digitalWrite(MotorTwoInt3, LOW);
  digitalWrite(MotorTwoInt4, LOW); 
}

void dropArmIntoPylon(int sec){
  digitalWrite(ArmInt1, HIGH);
  digitalWrite(ArmInt2, LOW);
  
  delay(sec*1000);

  digitalWrite(ArmInt1, LOW);
  digitalWrite(ArmInt2, LOW); 
}

void raiseArmIntoPylon(int sec){
  digitalWrite(ArmInt1, LOW);
  digitalWrite(ArmInt2, HIGH);
  
  delay(sec*1000);

  digitalWrite(ArmInt1, LOW);
  digitalWrite(ArmInt2, LOW); 
}

int readDutyCycle(){
  unsigned long duration;
  int dutyCycleSum = 0;
  //Number of interations (seconds to measure over) to ensure correct Duty Cycle is read.
  int numIterations = 5;
  for(int i = 0; i < numIterations; i++){
    duration = pulseInLong(digitalDutyCycleReadPin, HIGH);

    //Duty Cycle Calculation
    int dutyCycleRead = (int) ((duration/100000.0) / (1.0/100.0));
    dutyCycleSum += dutyCycleRead;
  }

  return (dutyCycleSum/numIterations)*10;
}

int readReedSensor(){
  return digitalRead(reedSensorReadPin);
}

int readRightUltraSonic(){
  // Clears the trigPin
  digitalWrite(rightTrigPin, LOW);
  delayMicroseconds(2);
  // Sets the rightTrigPin on HIGH state for 10 micro seconds
  digitalWrite(rightTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(rightTrigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(rightEchoPin, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2;
  
  return distance;
}

int readLeftUltraSonic(){
  // Clears the trigPin
  digitalWrite(leftTrigPin, LOW);
  delayMicroseconds(2);
  // Sets the rightTrigPin on HIGH state for 10 micro seconds
  digitalWrite(leftTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(leftTrigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(leftEchoPin, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2;
  
  return distance;
}

void recalibratePylon(){
  Enes100.updateLocation();
  MotorOff();
  if(startY < 1.0){
    if(Enes100.location.theta > ((PI/2.0) + .05)){
      MotorTurnRightUntilTheta((PI/2.0));
    } else if (Enes100.location.theta > ((PI/2.0) - .05)){
      MotorTurnLeftUntilTheta((PI/2.0));
    }
    MotorMoveForward();
  } else {
    if(Enes100.location.theta > ((-PI/2.0) + .05)){
      MotorTurnRightUntilTheta((-PI/2.0));
    } else if (Enes100.location.theta > ((-PI/2.0) - .05)) {
      MotorTurnLeftUntilTheta((-PI/2.0));
    }
    MotorMoveForward();
  }
}



