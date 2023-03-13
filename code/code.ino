#include <WiFi.h>
#include "esp_camera.h"


//*********************************************//
//************WIFI CREDENTIALS*****************//
//*********************************************//
const char* ssid = "black_sky";
const char* password = "blackmirror";


//*********************************************//
//************  MAIN SETUP    *****************//
//*********************************************//
void setup() {
  
  Serial.begin(115200);

  setupWifi();
  
  setupCamera();
  
  
}


//*********************************************//
//************   MAIN LOOP    *****************//
//*********************************************//
void loop() {
  Serial.println("Loop running");
  delay(3000);
}


//*********************************************//
//************   WIFI SETUP   *****************//
//*********************************************//
void setupWifi(){
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("*********************************************");
  Serial.println("WiFi connected");
  Serial.println("*********************************************");
  
}


//*********************************************//
//************   CAMERA SETUP *****************//
//*********************************************//
void setupCamera(){
  
  // Camera setup
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // Camera start
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("*********************************************");
  Serial.println("Camera Setup Complete");
  Serial.println("*********************************************");
  
}
