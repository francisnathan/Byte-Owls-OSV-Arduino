//ARDUINO PIN NUMBERS
int MotorOneInt1 = 3;
int MotorOneInt2 = 4;
int MotorTwoInt3 = 5;
int MotorTwoInt4 = 6;

boolean ran = false;

void setup() {
  Serial.begin(9600);
  pinMode(MotorOneInt1, OUTPUT);
  pinMode(MotorOneInt2, OUTPUT);
  pinMode(MotorTwoInt3, OUTPUT);
  pinMode(MotorTwoInt4, OUTPUT);
}

void loop() {
  MotorTurnLeft(MotorOneInt1, MotorOneInt2, MotorTwoInt3, MotorTwoInt4, 2);
  delay(3000);
  MotorTurnRight(MotorOneInt1, MotorOneInt2, MotorTwoInt3, MotorTwoInt4, 2);
  delay(3000);
}

//Spin motors to move backward.
int MotorTurnLeft(int inputPin1, int inputPin2, int inputPin3, int inputPin4,  int sec){
 
  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, HIGH);

  digitalWrite(inputPin3, HIGH);
  digitalWrite(inputPin4, LOW);  

  delay(sec*1000);

  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, LOW);

  digitalWrite(inputPin3, LOW);
  digitalWrite(inputPin4, LOW);

}

//Spin motors to move forwards.
int MotorTurnRight(int inputPin1, int inputPin2, int inputPin3, int inputPin4,  int sec){
 
  digitalWrite(inputPin1, HIGH);
  digitalWrite(inputPin2, LOW);

  digitalWrite(inputPin3, LOW);
  digitalWrite(inputPin4, HIGH);  

  delay(sec*1000);

  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, LOW);

  digitalWrite(inputPin3, LOW);
  digitalWrite(inputPin4, LOW);

}
