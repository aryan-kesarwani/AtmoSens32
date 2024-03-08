#include <WiFi.h>
#include "time.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <HTTPClient.h>

#define BMP_SDA 21
#define BMP_SCL 22

const char* resource = "/trigger/bmp280_inside/with/key/d4lFrUg06HCEZN2DuoyoL5";
// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

const char* ssid       = "ARYA";
const char* password   = "2004@ankit";

const char* ntpServer = "asia.pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbxqiz_gq7ldLcDXmXwAjn1OvuDWHcSsnsb1-hmbpVgTrHp3FdG5UQRheqIZ1a_rAN0Z";    // change Gscript ID with yours

Adafruit_BMP280 bmp280;
int Outside = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing BMP280");

  boolean status = bmp280.begin(0x76);

  if (!status) {

  Serial.println("BMP280 Not connected!");

  }
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
//upload data to google sheet
  
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //disconnect WiFi as it's no longer needed
 
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50]; //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A-%B-%d-%Y-%H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"temprature=" + String(bmp280.readTemperature()) + "&altitude=" + String(bmp280.readAltitude(1011.8)) + "&pressure=" + String(bmp280.readPressure()/100) + "&wbp=" + String(bmp280.waterBoilingPoint((bmp280.readPressure()/100))) + "&outside=" + String(Outside);
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
    Serial.println (asString);
  }
///////
  
  float temp = bmp280.readTemperature();

  float pressure = (bmp280.readPressure()/100);

  float altitude = bmp280.readAltitude(1011.8);

  float water_boiling_point = bmp280.waterBoilingPoint(pressure);

  Serial.print ("temperature: ");
  Serial.println (temp);
  Serial.print ("Altitude: ");
  Serial.println (altitude);
  Serial.print ("Pressure: ");
  Serial.println (pressure);
  Serial.print ("Water Boiling Point: ");
  Serial.println (water_boiling_point);
  
  delay(25000);

}