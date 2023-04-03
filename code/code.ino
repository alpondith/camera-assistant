#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <base64.h>

#include "esp_camera.h"
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


//*********************************************//
//************WIFI CREDENTIALS*****************//
//*********************************************//
const char* ssid = "black_sky";
const char* password = "blackmirror";


//*********************************************//
//********* IMAGE UPLOAD API ENDPOINT *********//
//*********************************************//
const char* endpoint = "http://gassistant.insoulit.com/api/image";


//*********************************************//
//************  MAIN SETUP    *****************//
//*********************************************//
void setup() {
  
  Serial.begin(115200);

  setupWifi();
  
  setupCamera();

  setupFileSystem();
  
  
}


//*********************************************//
//************   MAIN LOOP    *****************//
//*********************************************//
void loop() {
  Serial.println("Loop running");
  takePicture();
  uploadPicture();
  delay(10000);
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


//*********************************************//
//*********   FILE SYSTEM SETUP ***************//
//*********************************************//
void setupFileSystem(){
  
  // Initialize the SPIFFS file system
  if (!SPIFFS.begin(true)) {
    Serial.println("*********************************************");
    Serial.println("An error occurred while mounting SPIFFS");
    Serial.println("*********************************************");
    return;
  }  

  Serial.println("*********************************************");
  Serial.println("SPIFFS Filesystem setup complete.");
  Serial.println("*********************************************");
  
}


//*********************************************//
//************   TAKE PHOTO   *****************//
//*********************************************//
void takePicture(){
  // Take a photo
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Save the photo to the SPIFFS file system with a fixed filename
  char* filename = "/image.jpg";
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  
  file.write(fb->buf, fb->len);
  file.close();
  esp_camera_fb_return(fb);



  // Check if the file exists
  if (SPIFFS.exists(filename)) {
    Serial.println("Image saved to /image.jpg");
  } else {
    Serial.println("Failed to save image");
  }
    
}

//*********************************************//
//************   UPLOAD IMAGE *****************//
//*********************************************//

void uploadPicture() {
// Open the image file
char* filename = "/image.jpg";
File file = SPIFFS.open(filename, FILE_READ);
if (!file) {
  Serial.println("Failed to open file for reading");
  return;
}

// Create an HTTPClient object
HTTPClient http;

// Set the API endpoint URL
http.begin(endpoint);


  http.addHeader("Accept", "application/json");
//  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

 //  http.addHeader("Content-Type", "multipart/form-data");
 //  http.addHeader("camera_key", "aCPeaY8K4p1EYMzqyTRoEX2gNktOyOoXg3c08Gxi");
 String httpRequestData = "camera_key=aCPeaY8K4p1EYMzqyTRoEX2gNktOyOoXg3c08Gxi";

 // Send the file as the request body
 int httpCode = http.POST(httpRequestData);


// Check the HTTP response code
if (httpCode == HTTP_CODE_OK) {
  Serial.println("Image sent successfully");
} else {
  Serial.print("HTTP error: ");
  Serial.println(httpCode);
  String response = http.getString();
  Serial.println(response);
}

// Close the file and the HTTP connection
file.close();
http.end();
}
