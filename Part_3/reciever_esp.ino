#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  int state;
} struct_message;

struct_message incomingMsg;

// Receiving ESP-NOW data
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&incomingMsg, incomingData, sizeof(incomingMsg));

  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.print("Received message from: ");
  Serial.println(macStr);

  Serial.print("Received State: ");
  Serial.println(incomingMsg.state);


  Serial2.println(incomingMsg.state);


  Serial.println("State value sent to Propeller via Serial2.");
}

void setup() {
  Serial.begin(115200);     
  Serial2.begin(9600);      

  WiFi.mode(WIFI_STA);      

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Ready to receive state value and send to Propeller...");
}

void loop() {
}



#include <esp_now.h> #include <WiFi.h> 
