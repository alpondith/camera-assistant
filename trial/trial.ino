#include "esp_camera.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "esp_http_server.h"
#include "esp_timer.h"

#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

#include <base64.h>

static const char* TAG = "ESP32-CAM";

camera_config_t config;

#include <WiFi.h>

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
void setup()
{
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);

  setupWifi();

  setupCamera();
}


//*********************************************//
//************   MAIN LOOP    *****************//
//*********************************************//
void loop()
{
  // Capture and encode image every 5 seconds
  delay(10000);
  captureAndEncode();
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
void setupCamera()
{
  // Camera config
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
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
    return;
  }
}

String captureAndEncode()
{
  // Capture image
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Failed to capture image");
    return "";
  }

  // Encode to base64
  String encoded = base64::encode(fb->buf, fb->len);

  // Print to serial console
  Serial.println(encoded);

  // Free memory
  esp_camera_fb_return(fb);

  return encoded;
}
