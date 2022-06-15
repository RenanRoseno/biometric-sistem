#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <time.h>
#include <Keypad.h>

  
const byte n_rows = 4;
const byte n_cols = 4;

char keys[n_rows][n_cols] = {
 
  {'1','2','3','A'},
 
  {'4','5','6','B'},
 
  {'7','8','9','C'},
 
  {'*','0','#','D'}
 
};

byte colPins[n_rows] = {13,12, 14, 27};
byte rowPins[n_cols] = {26, 25, 33, 32};

Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);
// set the LCD number of columns and rows


const char* ssid = "--";
const char* password = "--";
int timezone = -3; //time offset
int dst = 0;

int lcdColumns = 16;
int lcdRows = 2;

bool isBeforeEvening = false;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  // initialize LCD
  //lcd.init();
  // turn on LCD backlight                      
  //lcd.backlight();
  Serial.begin(115200);
  /*WiFi.mode(WIFI_STA);
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
  }*/
}

void loop(){
  //initLCD();
  //showTime();
  showKeyPressed();
}

void initLCD(){
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  lcd.setCursor(0, 0);
  lcd.print("LEITOR BIOMETRICO");
  char saudacao[] = "";
  lcd.setCursor(0, 1);
  
  if(timeinfo->tm_hour > 12){
    lcd.print("Boa tarde");
    isBeforeEvening = true;
  }else {
    if(isBeforeEvening){
      lcd.clear();
     isBeforeEvening = false;
    }
    lcd.print("Bom dia");   
  }

  lcd.setCursor(0, 2);
  lcd.printf("%02d:%02d:%02d", timeinfo->tm_hour, 
           timeinfo->tm_min, timeinfo->tm_sec); 
   
 
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

void showKeyPressed(){
  char myKey = myKeypad.getKey();
 
  if (myKey != NULL){
    Serial.print("Key pressed: ");
    Serial.println(myKey);
  }
}
