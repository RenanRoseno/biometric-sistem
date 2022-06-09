#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <time.h>

// set the LCD number of columns and rows
const char* ssid = "HotnetNayane";
const char* password = "@.NR1402";
int timezone = -3; //time offset
int dst = 0;

int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  configTime(timezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
  Serial.print(".");
  delay(1000);
  }
}

void loop(){
  initLCD();
  showTime();
}

void initLCD(){
  lcd.setCursor(0, 0);
  lcd.print("F diugodiugo :( ");
}

void clearLCD(){
   lcd.clear(); 
}

void showTime(){
     time_t now;
    struct tm * timeinfo;
    time(&now);
    //timeinfo = gmtime(&now); 
    timeinfo = localtime(&now);

    if(timeinfo->tm_hour < 12){
      Serial.println("Bom dia");      
    }else {
      Serial.println("Boa tarde"); 
    }
    Serial.println(timeinfo->tm_hour);
    Serial.printf("The time is: %02d:%02d:%02d\n", timeinfo->tm_hour, 
           timeinfo->tm_min, timeinfo->tm_sec); 
    //  , timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year-100);
     
    //Serial.println(ctime(&now));
    delay(1000);
}
