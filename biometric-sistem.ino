#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <time.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h> //https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
#include <IOXhop_FirebaseESP32.h>                           
#include <ArduinoJson.h>      

#define FIREBASE_HOST "https://biometric-sistem-default-rtdb.firebaseio.com/"    
#define FIREBASE_AUTH "pyQfKSOtmaOfTHA0stGjnJwMolhpybh1U9ksfnOp"   

//Senha padrão do sensor de digitais
const uint32_t password = 0x0;
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, password);

  
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


const char* ssid = "HotnetNayane";
const char* passwordW = "@.NR1402";
int timezone = -3; //time offset
int dst = 0;

int lcdColumns = 16;
int lcdRows = 2;

bool isBeforeEvening = false;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
//   initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
    setupFingerprintSensor();
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passwordW);
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
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop(){
  initLCD();
  menu();
  //showTime();
  //showKeyPressed();
  /*
   int i = command.toInt();

  //Verifica qual o número digitado e chama a função correspondente
  switch (i)
  {
    case 1:
      storeFingerprint();
      break;
    case 2:
      checkFingerprint();
      break;
    case 3:
      printStoredFingerprintsCount();
      break;
    case 4:
      deleteFingerprint();
      break;
    case 5:
      emptyDatabase();
      break;
    default:
      Serial.println(F("Opção inválida"));
      break;
  }
   */
}

void setupFingerprintSensor()
{
  //Inicializa o sensor
  fingerprintSensor.begin(57600);

  //Verifica se a senha está correta
  if(!fingerprintSensor.verifyPassword())
  {
    //Se chegou aqui significa que a senha está errada ou o sensor está problemas de conexão
    Serial.println(F("Não foi possível conectar ao sensor. Verifique a senha ou a conexão"));
    while(true);
  }
}


void initLCD(){
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  lcd.setCursor(0, 0);
  lcd.print("  LEITOR BIOMETRICO");
  char saudacao[] = "";
  lcd.setCursor(0, 1);
  
  if(timeinfo->tm_hour > 11){
    lcd.print("Boa tarde! ");
    isBeforeEvening = true;
  }else {
    if(isBeforeEvening){
      lcd.clear();
     isBeforeEvening = false;
    }
      lcd.print("  Bom dia! ");   
  }
  
  lcd.printf("%02d:%02d:%02d", timeinfo->tm_hour, 
           timeinfo->tm_min, timeinfo->tm_sec); 
  lcd.setCursor(0, 2);
  lcd.print("1.Verificar, 2.Criar");
}

void menu(){
  boolean isDigitado = true;
  do{
    char keyPressed = myKeypad.getKey();
    if(keyPressed != NULL){
    isDigitado = false;
    //Serial.println(keyPressed);
    selectOption(keyPressed);
    }
  }while(isDigitado);
}
void selectOption(char option){
  
  //char myKey = myKeypad.getKey();
  int op = option - '0'; 
  Serial.println(op);
  switch(op){
    case 1 :
    Serial.println("Verificar");
    break;
    case 2:
    Serial.println("Cadastrar");
    break;
    defaultM:
    Serial.println("Deafult");
    break;
  }
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
void storeFingerprint()
{
  Serial.println(F("Qual a posição para guardar a digital? (1 a 149)"));

  //Lê o que foi digitado no monitor serial
 // String strLocation = getCommand();

  //Transforma em inteiro
  int location = 1; //strLocation.toInt();

  //Verifica se a posição é válida ou não
  if(location < 1 || location > 149)
  {
    //Se chegou aqui a posição digitada é inválida, então abortamos os próximos passos
    Serial.println(F("Posição inválida"));
    return;
  }

  Serial.println(F("Encoste o dedo no sensor"));

  //Espera até pegar uma imagem válida da digital
  while (fingerprintSensor.getImage() != FINGERPRINT_OK);
  
  //Converte a imagem para o primeiro padrão
  if (fingerprintSensor.image2Tz(1) != FINGERPRINT_OK)
  {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro image2Tz 1"));
    return;
  }
  
  Serial.println(F("Tire o dedo do sensor"));

  delay(2000);

  //Espera até tirar o dedo
  while (fingerprintSensor.getImage() != FINGERPRINT_NOFINGER);

  //Antes de guardar precisamos de outra imagem da mesma digital
  Serial.println(F("Encoste o mesmo dedo no sensor"));

  //Espera até pegar uma imagem válida da digital
  while (fingerprintSensor.getImage() != FINGERPRINT_OK);

  //Converte a imagem para o segundo padrão
  if(fingerprintSensor.image2Tz(2) != FINGERPRINT_OK)
  {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro image2Tz 2"));
    return;
  }

  //Cria um modelo da digital a partir dos dois padrões
  if(fingerprintSensor.createModel() != FINGERPRINT_OK)
  {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro createModel"));
    return;
  }

  //Guarda o modelo da digital no sensor
  if(fingerprintSensor.storeModel(location) != FINGERPRINT_OK)
  {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro storeModel"));
    return;
  }

  //Se chegou aqui significa que todos os passos foram bem sucedidos
  Serial.println(F("Sucesso!!!"));
}

//Verifica se a digital está cadastrada
void checkFingerprint()
{
  Serial.println(F("Encoste o dedo no sensor"));

  //Espera até pegar uma imagem válida da digital
  while (fingerprintSensor.getImage() != FINGERPRINT_OK);

  //Converte a imagem para o padrão que será utilizado para verificar com o banco de digitais
  if (fingerprintSensor.image2Tz() != FINGERPRINT_OK)
  {
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro image2Tz"));
    return;
  }

  //Procura por este padrão no banco de digitais
  if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK)
  {
    //Se chegou aqui significa que a digital não foi encontrada
    Serial.println(F("Digital não encontrada"));
    return;
  }

  //Se chegou aqui a digital foi encontrada
  //Mostramos a posição onde a digital estava salva e a confiança
  //Quanto mais alta a confiança melhor
  Serial.print(F("Digital encontrada com confiança de "));
  Serial.print(fingerprintSensor.confidence);
  Serial.print(F(" na posição "));
  Serial.println(fingerprintSensor.fingerID);
}

void printStoredFingerprintsCount()
{
  //Manda o sensor colocar em "templateCount" a quantidade de digitais salvas
  fingerprintSensor.getTemplateCount();

  //Exibe a quantidade salva
  Serial.print(F("Digitais cadastradas: "));
  Serial.println(fingerprintSensor.templateCount);
}

void deleteFingerprint()
{
  Serial.println(F("Qual a posição para apagar a digital? (1 a 149)"));

  //Lê o que foi digitado no monitor serial
  String strLocation = "1";

  //Transforma em inteiro
  int location = strLocation.toInt();

  //Verifica se a posição é válida ou não
  if(location < 1 || location > 149)
  {
    //Se chegou aqui a posição digitada é inválida, então abortamos os próximos passos
    Serial.println(F("Posição inválida"));
    return;
  }

  //Apaga a digital nesta posição
  if(fingerprintSensor.deleteModel(location) != FINGERPRINT_OK)
  {
    Serial.println(F("Erro ao apagar digital"));
  }
  else
  {
    Serial.println(F("Digital apagada com sucesso!!!"));
  }
}

void emptyDatabase()
{
  Serial.println(F("Tem certeza? (s/N)"));

  //Lê o que foi digitado no monitor serial
//  String command = getCommand();

  //Coloca tudo em maiúsculo para facilitar a comparação
 String  command = "S"; //.toUpperCase();

  //Verifica se foi digitado "S" ou "SIM"
  if(command == "S" || command == "SIM")
  {
    Serial.println(F("Apagando banco de digitais..."));

    //Apaga todas as digitais
    if(fingerprintSensor.emptyDatabase() != FINGERPRINT_OK)
    {
      Serial.println(F("Erro ao apagar banco de digitais"));
    }
    else
    {
      Serial.println(F("Banco de digitais apagado com sucesso!!!"));
    }
  }
  else
  {
    Serial.println(F("Cancelado"));
  }
}
