#include <esp_now.h>
#include <WiFi.h>


uint8_t receiverMAC[] = {0x7C, 0x9E, 0xBD, 0xF3, 0xE9, 0xC0};


typedef struct struct_message {
 char stateChar; 
} struct_message;


struct_message outgoingMsg;
esp_now_peer_info_t peerInfo;


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
 //Serial.print("Send Status: ");
 //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}
const int flexPin1 = 34;
const int flexPin2 = 35;
const int xPin = 14;
const int yPin = 15;


void setup() {
 Serial.begin(115200);


 //pinMode(flexPin1, INPUT);
 //pinMode(flexPin2, INPUT);
 pinMode(xPin, INPUT);
 pinMode(yPin, INPUT);


 WiFi.mode(WIFI_STA);
 if (esp_now_init() != ESP_OK) {
   //Serial.println("ESP-NOW Init Failed");
   return;
 }


 esp_now_register_send_cb(OnDataSent);


 memcpy(peerInfo.peer_addr, receiverMAC, 6);
 peerInfo.channel = 0;
 peerInfo.encrypt = false;


 if (esp_now_add_peer(&peerInfo) != ESP_OK) {
   //Serial.println("Failed to add peer");
   return;
 }


 //Serial.println("System Initialized.");
}


void loop() {
 int flexValue1 = analogRead(flexPin1);
 int flexValue2 = analogRead(flexPin2);
 int pulseX = pulseIn(xPin, HIGH, 100000L);
 int pulseY = pulseIn(yPin, HIGH, 100000L);


 //Serial.println(flexValue1);
 //Serial.println(flexValue2);


 int flex_1_val;
 int flex_2_val;


 if (flexValue1 > 10) {
   flex_1_val = 1;
 } else {
   flex_1_val = 0;
 }
  if (flexValue2 > 10) {
   flex_2_val = 1;
 } else {
   flex_2_val = 0;
 }


 int x_val = 0;
 int y_val = 0;


 if (pulseX > 6000 && pulseX < 7000) {
   x_val = 0;
 } else if (pulseX > 5000 && pulseX < 6000) {
   x_val = 1;
 } else if (pulseX > 4000 && pulseX < 5000) {
   x_val = 2;
 }


 //Serial.println("3" + x_val);
 if (pulseY > 5000 && pulseY < 6000) {
   y_val = 0;
 } else if (pulseY > 4000 && pulseY < 5000) {
   y_val = 1;
 } else if (pulseY > 2000 && pulseY < 3500) {
   y_val = 2;
 }




 // Encode state
 int state = (flex_1_val * 18) + (flex_2_val * 9) + (x_val * 3) + y_val;
 //Serial.print("Encoded State: ");
 Serial.println(state);


 outgoingMsg.stateChar = (char)state; 


 esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&outgoingMsg, sizeof(outgoingMsg));


 if (result == ESP_OK) {
   //Serial.println("State sent successfully.");
 } else {
   //Serial.println("Error sending state.");
 }


 delay(100);
}



