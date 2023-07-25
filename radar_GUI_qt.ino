#include <Servo.h>

//Servo
Servo myservo; 

//HC-SR04 ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

long duration;
float distance;
int servoSetting;

void setup() {
    //serial
    Serial.begin(9600);
    //servo
    myservo.attach(11);  
    //hc-sr04
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT); 
    myservo.write(0);
}

void loop() {
    moveServo();
    getDistance();
    outputDistance();
    delay(500);
}

//move servo
void moveServo(){
    if (Serial.available()){
        servoSetting = Serial.parseInt();
        myservo.write(servoSetting); 
    }
}

//get hc-sr04 distance
void getDistance(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);  
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
}

//serial print distance
void outputDistance(){
    Serial.print(distance);
    Serial.print(",");
    Serial.flush();
}

