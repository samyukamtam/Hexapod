#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_PWMServoDriver.h>

// HC-05 Bluetooth Module connected to RX=2, TX=3
SoftwareSerial BTSerial(2, 3);

class spiderClass{
  private:
    Adafruit_PWMServoDriver spiderDriver;

    int legN = 6;
    float rSwing = 30, tSwing = 0;

    float theta1val[6] = {0};
    float theta2val[6] = {0};

    int dirOffset(int i){
      return (-30+60*i)*(PI/180);
    }

    float theta2PWM(float theta){
      return 2.5*theta + 375;
    }
  public:
    initiate(){
      spiderDriver = Adafruit_PWMServoDriver();
      spiderDriver.begin();
      spiderDriver.setPWMFreq(60);
    }

    resetServos(){
      for (int i = 0; i < legN; i++){
        setLeg(i, 0, 0);
      }
    }

    setDirn(float dirn){
      tSwing = dirn;
    }

    setLeg(int i, float theta_1, float theta_2){
      float PWM_1 = theta2PWM(theta_1);
      float PWM_2 = theta2PWM(-theta_2);

      spiderDriver.setPWM(i+6, 0, PWM_1);
      spiderDriver.setPWM(i, 0, PWM_2);
    }

    hardcodedGait(){
      //setLeg(i, theta1val[i] - rSwing*cos(tSwing - dirOffset(i)), 0);
      int groupP[] = {0, 2, 4};
      int groupQ[] = {1, 3, 5};

      int delayTime = 300;
      int walkHeight = -20;
      int highHeight = 40;
      
      for (int n = 0; n < 3; n++){
        setLeg(groupP[n], -0.5*rSwing*cos(tSwing - dirOffset(groupP[n])), highHeight);
      }
      delay(delayTime);
      
      for (int n = 0; n < 3; n++){
        setLeg(groupQ[n], 0, walkHeight);
      }
      delay(delayTime);

      for (int n = 0; n < 3; n++){
        setLeg(groupP[n], -rSwing*cos(tSwing - dirOffset(groupP[n])), walkHeight);
      }
      delay(delayTime);

      for (int n = 0; n < 3; n++){
        setLeg(groupQ[n], -0.5*rSwing*cos(tSwing - dirOffset(groupP[n])), highHeight);
      }
      delay(delayTime);

      for (int n = 0; n < 3; n++){
        setLeg(groupP[n], 0, walkHeight);
      }
      delay(delayTime);

      for (int n = 0; n < 3; n++){
        setLeg(groupQ[n], -rSwing*cos(tSwing - dirOffset(groupP[n])), walkHeight);
      }
      delay(delayTime);
    }
};

int deadFlag = 0;
spiderClass mySpider;

void setup() {
    Serial.begin(9600);  // Serial Monitor
    BTSerial.begin(9600); // Bluetooth HC-05 default baud rate

    Serial.println("Bluetooth Ready! Waiting for commands...");

    mySpider.initiate();
    mySpider.setDirn(PI);
    mySpider.resetServos();

    delay(300);
}

void loop() {
    //static char receivedChar;
    if (BTSerial.available()) {
        char receivedChar = BTSerial.read();  // Read incoming data
        Serial.print("Received: ");
        Serial.println(receivedChar);  // Print to Serial Monitor

        // Perform actions based on received data
        if (receivedChar == 'w') {
            Serial.println("Moving Forward");
            mySpider.setDirn(PI);
            // Add motor control logic here
        } 
        else if (receivedChar == 'a') {
            Serial.println("Turning Left");
            mySpider.setDirn(PI/2);
            // Add motor control logic here
        } 
        else if (receivedChar == 's') {
            Serial.println("Moving Backward");
            mySpider.setDirn(0);
            // Add motor control logic here
        } 
        else if (receivedChar == 'd') {
            Serial.println("Turning Right");
            mySpider.setDirn(3*PI/2);
            // Add motor control logic here
        } 
        else {
            deadFlag = 1;
        }
    }
    if(deadFlag == 0){
      mySpider.hardcodedGait();
    }
    else{
      mySpider.resetServos();
    }
}
