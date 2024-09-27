#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <TimeLib.h>
#include <NTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1
#define DHT_PIN       4   // Pin connected to DHT11 (Can Be adjusted)
#define DHT_TYPE      DHT11

#define SDA_PIN 21  // SDA pin for OLED
#define SCL_PIN 22  // SCL pin for OLED

#define TIME_ZONE_OFFSET 0 * 3600 // Offset (Can be changed according to Your timezone ({UTC OFFSET} * 3600)

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TIME_ZONE_OFFSET);

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long previousMillis = 0;
const long interval = 5000; // Interval to switch slides (5 seconds)

time_t getNtpTime() {
  timeClient.update();
  return timeClient.getEpochTime(); 
}

void setup() {
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  delay(200); // Pause for display initialization

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  
  dht.begin();
  timeClient.begin();

  // Synchronize time 
  setSyncProvider(getNtpTime);  
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    display.clearDisplay();
    
    // Slide 1: Display DHT11 data
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("DHT NOT CONNECTED");
      
      
    } else {
      Serial.print("T:");
      Serial.print(temperature);
      Serial.print(" °C, H:");
      Serial.print(humidity);
      Serial.println(" %");

      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("T/H:");
      display.setTextSize(2);
      display.println("");
      display.println(temperature);
      display.print(humidity);
      display.print("%");
    }
    
    display.display();
    delay(4000); // Display DHT data for 4 seconds before switching
    
    display.clearDisplay();
    
    // Slide 2: Display time in 12-hour format from NTP server
    timeClient.update();
    String formattedTime = timeClient.getFormattedTime();
    int hour = timeClient.getHours();
    bool isPM = false;

    if (hour >= 12) {
      isPM = true;
      if (hour > 12) {
        hour -= 12;
      }
    }
    if (hour == 0) {
      hour = 12;
    }

    String am_pm = isPM ? "PM" : "AM";
    formattedTime = String(hour) + timeClient.getFormattedTime().substring(2);
    formattedTime += "" + am_pm;
    
    Serial.print("Time:");
    Serial.println(formattedTime);
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Time:");
    display.setTextSize(2);
    display.println(" ");
    display.println(formattedTime);
    
    display.display();
    delay(4000); // Display time for 4 seconds before switching

    display.clearDisplay();
    
    //TimeLib Functions
    int dayf = day();
    int monthf = month();
    int yearf = year();
    
    // Valid Date check
    if (yearf > 1970) {  
      String formattedDate = String(dayf) + "/" + String(monthf) + "/" + String(yearf);
    
      Serial.print("Date:");
      display.println(" ");
      Serial.println(formattedDate);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Date:");
      display.setTextSize(2);
      display.println(" ");
      display.println(formattedDate);
      
      display.display();
      delay(4000); // Display date for 4 seconds before switching
    } else {
      Serial.println("Waiting for valid date...");
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Fetching Date...");
      display.display();
      delay(1000);
    }
  }
}
