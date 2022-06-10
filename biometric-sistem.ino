#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <time.h>

const char *ssid = "--";
const char *password = "--";
const int delay1000 = 1000;
//Functions
void initLCD();
void clearLCD();
void showTime();

// Timer attributes
int timezone = -3;
int dst = 0;
bool isBeforeEvening = false;

// LCD fields attributes
int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup()
{
  // LCD
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  // Timer
  configTime(timezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(delay1000);
  }
}

void loop()
{
  initLCD();
  showTime();
}

void initLCD()
{
  time_t now;
  struct tm *timeinfo;
  time(&now);
  timeinfo = localtime(&now);

  lcd.setCursor(0, 0);
  lcd.print("LEITOR BIOMETRICO");
  lcd.setCursor(0, 1);

  if (timeinfo->tm_hour > 12)
  {
    lcd.print("Boa tarde");
    isBeforeEvening = true;
  }
  else
  {
    if (isBeforeEvening)
    {
      lcd.clear();
      isBeforeEvening = false;
    }
    lcd.print("Bom dia");
  }

  lcd.setCursor(0, 2);
  lcd.printf("%02d:%02d:%02d", timeinfo->tm_hour,
             timeinfo->tm_min, timeinfo->tm_sec);
}

void clearLCD()
{
  lcd.clear();
}

void showTime()
{
  time_t now;
  struct tm *timeinfo;
  time(&now);
  timeinfo = localtime(&now);

  if (timeinfo->tm_hour < 12)
  {
    Serial.println("Bom dia");
  }
  else
  {
    Serial.println("Boa tarde");
  }
  Serial.println(timeinfo->tm_hour);
  Serial.printf("The time is: %02d:%02d:%02d\n", timeinfo->tm_hour,
                timeinfo->tm_min, timeinfo->tm_sec);

  delay(delay1000);
}
