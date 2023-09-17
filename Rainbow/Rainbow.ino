#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiManager wifiManager;
HTTPClient client;

char msg[] = "Hello";
unsigned long last;

const int COLOR_COUNT = 8;
int COLORS_RED[COLOR_COUNT];
int COLORS_GREEN[COLOR_COUNT];
int COLORS_BLUE[COLOR_COUNT];
int COLOR_INDEX = 0;

void setup() {
    Serial.begin(115200);
    last = millis();

    #ifdef RGB_BUILTIN
      neopixelWrite(RGB_BUILTIN,0,0,0); // Red
    #endif

    wifiManager.setDebugOutput(true);
    wifiManager.setConnectRetries(2);

    wifiManager.setAPCallback(configModeCallback);

    // uncomment this to clear stored wifi settings
    //wifiManager.resetSettings();

    bool res;
    res = wifiManager.autoConnect("ESP32-SETUP");
    if(!res) {
        Serial.println("Failed to connect to WIFI");
        //ESP.restart();
    }
    else {
        Serial.println("Connected to WIFI");
    }

    // red
    COLORS_RED[0] = RGB_BRIGHTNESS;
    COLORS_GREEN[0] = 0;
    COLORS_BLUE[0] = 0;

    // orange
    COLORS_RED[1] = RGB_BRIGHTNESS;
    COLORS_GREEN[1] = RGB_BRIGHTNESS / 2;
    COLORS_BLUE[1] = 0;

    // yelllow
    COLORS_RED[2] = RGB_BRIGHTNESS;
    COLORS_GREEN[2] = RGB_BRIGHTNESS;
    COLORS_BLUE[2] = 0;

    // green
    COLORS_RED[3] = 0;
    COLORS_GREEN[3] = RGB_BRIGHTNESS;
    COLORS_BLUE[3] = 0;

    // blue
    COLORS_RED[4] = 0;
    COLORS_GREEN[4] = 0;
    COLORS_BLUE[4] = RGB_BRIGHTNESS;

    // indigo
    COLORS_RED[5] = RGB_BRIGHTNESS/3;
    COLORS_GREEN[5] = 0;
    COLORS_BLUE[5] = RGB_BRIGHTNESS/2;

    // violet
    COLORS_RED[6] = RGB_BRIGHTNESS/2;
    COLORS_GREEN[6] = 0;
    COLORS_BLUE[6] = RGB_BRIGHTNESS;

    COLORS_RED[7] = COLORS_RED[0];
    COLORS_GREEN[7] = COLORS_GREEN[0];
    COLORS_BLUE[7] = COLORS_BLUE[0];

}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void tick(){
    //Serial.println("ping");

    long rnd = random(1,10);
    client.begin("https://jsonplaceholder.typicode.com/comments?id=" + String(rnd));

    int httpCode = client.GET();

  if (httpCode > 0){
    String data = client.getString();
    //Serial.println(data);

    char json[500];
    data.toCharArray(json,500);

    StaticJsonDocument<200> doc;
    deserializeJson(doc, json);

    const char* email = doc[0]["email"];

    strcpy(msg,email);

    Serial.println(email);

  } else {
    //Serial.println("HTTP error");
  }

  client.end();
}


void updateColor(){
  #ifdef RGB_BUILTIN

    for (int step=0;step<50;step++){

      float progress = (float)step/50.0;
      float red = (1.0-progress) * COLORS_RED[COLOR_INDEX] + progress*COLORS_RED[COLOR_INDEX+1];
      float green = (1.0-progress) * COLORS_GREEN[COLOR_INDEX] + progress*COLORS_GREEN[COLOR_INDEX+1];
      float blue = (1.0-progress) * COLORS_BLUE[COLOR_INDEX] + progress*COLORS_BLUE[COLOR_INDEX+1];

      // turn down the brightness so it's not so annoying at night...
      red *= 0.05;
      green *= 0.05;
      blue *= 0.05;

      neopixelWrite(RGB_BUILTIN,(int)red,(int)green,(int)blue);
      delay(10);
    }
    COLOR_INDEX++;
    if (COLOR_INDEX + 1 > COLOR_COUNT -1){
      COLOR_INDEX = 0;
    }

  #endif
}

void loop() {

    unsigned long elapsed = millis() - last;
    // call tick every ten seconds
    if (elapsed > 10000){
      tick();
      last = millis();
    }

    updateColor();

}


