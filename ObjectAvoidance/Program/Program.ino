//this is the arduino code for the object avoidance robot program 
//using some information from the prebuilt code to get the sensor pin number info
#include <IRremote.hpp>

// motor speeds (PWM)
#define ENA1 5   //left front
#define ENB1 6   // right front
#define ENA2 10  // left back
#define ENB2 12  // right back

//direction control 
#define IN1 7    // left front 
#define IN2 8    // left front 
#define IN3 4    // right front 
#define IN4 11   // right front 

#define IN5 A0   // right back
#define IN6 A1   //right back 
#define IN7 A2   // left back 
#define IN8 A3   // left back 

#define STBY 3   
#define TRIG_PIN 13      // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 12      // Arduino pin tied to echo pin on the ultrasonic sensor.

// IR remote
const int IR_RECEIVER_PIN = 9;

//buton hex codes 
const uint16_t BTN_UP = 0x46;
const uint16_t BTN_LEFT= 0x44;
const uint16_t BTN_RIGHT = 0x43;
const uint16_t BTN_DOWN = 0x15;
const uint16_t BTN_OK = 0x40;
const uint16_t BTN_1 = 0x16;
const uint16_t BTN_2 = 0x19;
const uint16_t BTN_3 = 0x0D;
const uint16_t BTN_4 = 0x0C;
const uint16_t BTN_5= 0x18;
const uint16_t BTN_6 = 0x5E;
const uint16_t BTN_7 = 0x08;
const uint16_t BTN_8 =0x1C;
const uint16_t BTN_9 = 0x5A;
const uint16_t BTN_0 = 0x52;
const uint16_t BTN_STAR = 0x42;
const uint16_t BTN_HASH = 0x4A;
//threshold distance for object detected (cm)
const long OBJECT_THRESHOLD = 35; 

// movement 
const int MOTOR_SPEED = 100; 
const int TURN_SPEED = 100; 

//button state 
bool buttonPressed = false;
unsigned long lastPressTime = 0;
const unsigned long RELEASE_DELAY = 120; // ms to wait before stopping after last signal

void setup(){
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN);
  Serial.println("IR Remote Control Ready");

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // PWM Pins
  pinMode(ENA1, OUTPUT);
  pinMode(ENB1, OUTPUT);
  pinMode(ENA2, OUTPUT);
  pinMode(ENB2, OUTPUT);

  //direction 
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT); pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT); pinMode(IN8, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // test ultrasonic sensor 
  if(testUltrasonicSensor()){
    Serial.println("ultrasonic sensor is connected");
  }else{
    Serial.println("ultrasonic sensor not connected ");
  }
  stopMotors();

}

bool testUltrasonicSensor(){
  // Send a pulse to the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  if(duration == 0){
    return false; // Sensor not connected or not working
  }else{
    return true; // Sensor is working
  }
}

long getDistanceCM(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance= duration * 0.034 / 2; // Calculate distance in cm
  return distance;
}
bool detectingObstacle= false;

void loop(){

  if(detectingObstacle){
    detectObstacle();  // keep calling detectObstacle() 
    delay(100);  
  }

  if(IrReceiver.decode()){
    handleIRCommand(IrReceiver.decodedIRData.command);
    lastPressTime= millis();
    buttonPressed= true;
    IrReceiver.resume();
  }else if(buttonPressed && (millis() - lastPressTime > RELEASE_DELAY)) {
    stopMotors();
    buttonPressed = false;
    Serial.println("Button released");
  }

  long distance = getDistanceCM();  // get the current distance

  if(distance < OBJECT_THRESHOLD){
    Serial.println("Object detected!");
  }

  //delays 
  delay(500); 
}

void handleIRCommand(uint16_t command) {
  Serial.print("Received: 0x");
  Serial.println(command, HEX);
  
  switch(command){
    case BTN_UP:
      if(!buttonPressed){
        Serial.println("forward");
        moveForward();
      }
      break;
    case BTN_LEFT:
      if(!buttonPressed){
        Serial.println("left");
        turnLeft();
      }
      break;
    case BTN_RIGHT:
      if(!buttonPressed){
        Serial.println("right");
        turnRight();
      }
      break;
    case BTN_DOWN:
      if(!buttonPressed){
        Serial.println("back");
        moveBackward();
      }
      break;
    case BTN_OK:
      Serial.println("stop");
      stopMotors();
      detectingObstacle = false;  // stop obstacle detction 

      break;
    case BTN_1: 
      Serial.println("Button 1 pressed");
      detectingObstacle = true; 
    default:
      Serial.println("Unknown button pressed");
  }
}
void detectObstacle() {
  long distance = getDistanceCM();  // distance to the nearest object

  if(distance< OBJECT_THRESHOLD){
    stopMotors(); 
    delay(500);  
    //go capture image and run through object identification 

    //check left side 
    turnLeft(); 
    delay(500); 
    long leftDistance = getDistanceCM();  // check distance to the left

    stopMotors();  
    delay(500);  

    // check the right side 
    turnRight();  
    delay(500);  
    long rightDistance = getDistanceCM();  // check distance on the right

    stopMotors();  
    delay(500);  

    if(leftDistance > OBJECT_THRESHOLD && rightDistance > OBJECT_THRESHOLD){
      moveForward();  // move forward if no obstacles on both sides
    }else if(leftDistance > OBJECT_THRESHOLD) {
      turnLeft();
      delay(1000);  
      moveForward();  
    }else if (rightDistance > OBJECT_THRESHOLD){
      turnRight();
      delay(1000);  
      moveForward(); 
    }else{
      // both left and right are blocked so move backward 
      moveBackward();
      delay(1000);  
      stopMotors(); 
      delay(500); 
    }
  }else{
    //no obstacle detected in front, keep moving forward
    moveForward();
  }
}


/* -------------------------------
 * Motor Control Functions
 * ------------------------------*/
void moveForward() {
  digitalWrite(IN2, HIGH); 
  digitalWrite(IN1, HIGH); 
  analogWrite(ENA1, MOTOR_SPEED); 
  analogWrite(ENB1, MOTOR_SPEED);
}

void moveBackward() {
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, LOW);
  analogWrite(ENA1, MOTOR_SPEED); 
  analogWrite(ENB1, MOTOR_SPEED);
}

void turnRight() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN7, LOW); digitalWrite(IN8, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  digitalWrite(IN5, HIGH); digitalWrite(IN6, LOW);
  analogWrite(ENA1, MOTOR_SPEED); analogWrite(ENB1, MOTOR_SPEED);
}

void turnLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN7, HIGH); digitalWrite(IN8, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  digitalWrite(IN5, LOW); digitalWrite(IN6, HIGH);
  analogWrite(ENA1, MOTOR_SPEED); analogWrite(ENB1, MOTOR_SPEED);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  digitalWrite(IN5, LOW); digitalWrite(IN6, LOW);
  digitalWrite(IN7, LOW); digitalWrite(IN8, LOW);
  analogWrite(ENA1, 0); analogWrite(ENB1, 0);
}
