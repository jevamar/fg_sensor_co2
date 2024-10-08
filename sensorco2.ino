#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <GyverOLED.h>
#include <MHZ19.h> // by Jonathan Dempsey
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>

#include "beeper_notes.h"

// Дисплей OLED SH1106
#define OLED_RESET 4
GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;
// SDA - A4 Arduino
// SCL - A5 Arduino

// Кнопка сенсорная
#define PIN_BUTTON 6
unsigned long buttonCounter = 0;
bool buttonShort = false;
bool buttonLong = false;

// Датчики BMP280 или BME280: температура, давление, влажность(только BME280)
#define SEALEVELPRESSURE_HPA (1013.25)  // Давление на уровне моря, для вычесления высоты
#define I2C_BMP280 0x76 
Adafruit_BMP280 bmp; // I2C
Adafruit_BME280 bme;
bool isBMP280 = false;
bool isBME280 = false;
float temp = 0;
float pressure = 0;
float humidity = 0;

float tempCorrection = -0.45; // Коррекция температуры

// Датчик СО2 MH-Z19b
#define RX_PIN 3                                          
#define TX_PIN 2
#define BAUDRATE 9600
MHZ19 myMHZ19;
SoftwareSerial mySerial(RX_PIN, TX_PIN);
int co2ppm = 0;

// Счетчик времени для снятия показаний
unsigned long getDataTimer = 0;
unsigned long timerCalibration = 0;

#define MODE_MAIN 0     // co2, temp, press
#define MODE_ALL 1      // all, type
#define MODE_CALIBRATE 2  // Calibration screen
int mode = 0; 

// Зуммер
#define PIN_BEEPER 10 // D10 Arduino
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
bool isSound = true; // По-умолчанию звук предупреждения отключен
bool isCalibration = false; // Калибровка запущена

#include "display.h"
#include "beeper.h"

void setup() {
  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Touch button
  pinMode(PIN_BUTTON, INPUT);

//  tone(PIN_BEEPER, 250, 1000);

  Serial.begin(9600);
  Serial.println(F("Start Big Brother..."));

  // scannerI2C();

  // OLED
  Serial.println(F("TEST - OLED SH1106 128x64"));
 
  oled.init();  // инициализация
  
  oled.clear();   // очистить дисплей (или буфер)
  oled.update();  // обновить. Только для режима с буфером! OLED_BUFFER

  // --------------------------
  // oled.home();            // курсор в 0,0
  oled.setScale(3);
  //oled.autoPrintln(true);
  oled.setCursor(10, 1);
  oled.print(F("Future\r\nGadgets"));   // печатай что угодно: числа, строки, float, как Serial!
  oled.update();


  delay(3000);
  
  setupBME280();

  setupMHZ19();
}

/**
* Инициализация датчика температуры, влажности и давления
*/
void setupBME280()
{
  // BMP280
  Serial.print(F("TEST - BMP280 sensor: "));
  if (bmp.begin(I2C_BMP280)) {  
    isBMP280 = true;
    Serial.println(F("ok"));
  }else{
    Serial.println(F("not found"));
    // BME280
    Serial.print(F("TEST - BME280 sensor: "));
    if (bme.begin(I2C_BMP280)) {  
      isBME280 = true;  
      Serial.println(F("ok"));
    }else{
      Serial.println(F("not found"));
    }
  }
}

/**
* Инициализация CO2 датчика
*/
void setupMHZ19()
{
  mySerial.begin(BAUDRATE);
  myMHZ19.begin(mySerial);

  char myVersion[4];
  myMHZ19.getVersion(myVersion);

  Serial.print("\nFirmware Version: ");
  for(byte i = 0; i < 4; i++)
  {
    Serial.print(myVersion[i]);
    if(i == 1)
      Serial.print(".");
  }
  Serial.println("");

  Serial.print("Range: ");
  Serial.println(myMHZ19.getRange());
  Serial.print("Background CO2: ");
  Serial.println(myMHZ19.getBackgroundCO2());
  Serial.print("Temperature Cal: ");
  Serial.println(myMHZ19.getTempAdjustment());
  // Отключение автокалибровки
  myMHZ19.autoCalibration(false);     
  Serial.print("ABC Status: "); myMHZ19.getABC() ? Serial.println("ON") :  Serial.println("OFF");  // now print it's status
}


void loop() 
{
  onButton(digitalRead(PIN_BUTTON));

  if (timerCalibration != 0 && millis() - timerCalibration >= 7000) {
    eventStopCalibration();
    timerCalibration = 0;
  }

  if (millis() - getDataTimer >= 5000)
  {
    getTemperatureSensor();
    
    getCO2();
  
    displayMode();

    if(isSound){
      soundIndicateCO2();
    }
  }  
}

void soundIndicateCO2()
{
  if(co2ppm > 1000 && co2ppm < 1500) {
    tone(PIN_BEEPER, 2000, 500);  
  }else if(co2ppm > 1500 && co2ppm < 2000) {
    tone(PIN_BEEPER, 2500);  
    delay(500);
    noTone(PIN_BEEPER);
    delay(200);

    tone(PIN_BEEPER, 2500);  
    delay(500);
    noTone(PIN_BEEPER);
    delay(200);

    tone(PIN_BEEPER, 2500);  
    delay(500);
    noTone(PIN_BEEPER);

  }else if(co2ppm > 2000 && co2ppm < 4999){
    tone(PIN_BEEPER, 3000);  
    delay(800);
    noTone(PIN_BEEPER);
    delay(200);

    tone(PIN_BEEPER, 3000);  
    delay(800);
    noTone(PIN_BEEPER);
    delay(200);

    tone(PIN_BEEPER, 3000);  
    delay(800);
    noTone(PIN_BEEPER); 
  }else if(co2ppm >= 4999){
    march();
  }
}

void soundIndicateCalibrate(int count=3)
{
  for(int i=0;i<count;i++){
    tone(PIN_BEEPER, 4000);  
    delay(300);
    noTone(PIN_BEEPER);
    if(i < count-1){
      delay(200);
    }
  }
}

void onButton(int status)
{
  if(status==HIGH){
    if (buttonShort==false && millis()-buttonCounter>100) {
      buttonShort = true;
      buttonCounter = millis();
    }
    if (buttonShort==true && millis()-buttonCounter>1000) {
      buttonLong = !buttonLong;
      buttonCounter = millis();

      eventButtonLong();
    }
  }else{
    if (buttonShort==true && buttonLong==true) {
      buttonShort = false;            
      buttonLong = false;  
    }
    if (buttonShort==true && buttonLong==false) {
      buttonShort = false;  
      
      eventButtonShort();
    }
  }
}

// Событие короткого нажтия кнопки
void eventButtonShort()
{
  digitalWrite(LED_BUILTIN, HIGH);
  mode++;
  if(mode > MODE_CALIBRATE){
    mode = MODE_MAIN;
  }
  if(isSound){
    tone(PIN_BEEPER, 5000, 100);
  }

  Serial.print("TOUCH mode: ");
  Serial.println(mode);

  digitalWrite(LED_BUILTIN, LOW);

  displayMode();
}

// Событие длиного нажтия кнопки
void eventButtonLong()
{
  if(mode == MODE_CALIBRATE){
    eventStartCalibration();
    return;
  }
  if(mode == MODE_ALL){
    // march();
    playMelody();
    return;
  }
  eventSoundChange();
}

/**
* Событие запуска калибровки датчика CO2
*/
void eventStartCalibration()
{
    isCalibration = true;
    soundIndicateCalibrate(3);

    Serial.println(F("Event calibration"));
    myMHZ19.calibrate();
    Serial.println(F("Start calibration..."));
    
    timerCalibration = millis();
    displayMode();
}

/**
* Событие окончания калибровки
*/
void eventStopCalibration()
{
  isCalibration = false;
  mode = MODE_MAIN;
  displayMode();
  soundIndicateCalibrate(2);
}

/**
* Событие влючения/отключения звука
*/
void eventSoundChange()
{
  isSound=!isSound;
  tone(PIN_BEEPER, 2000, 1000);  
  if(isSound) {
    Serial.println(F("Sound ON"));
  } else {
    Serial.println(F("Sound OFF"));
  }
}

/**
* Получение показние концетрации CO2 и температуры с датчика MH-Z19
*/
void getCO2()
{
  co2ppm = myMHZ19.getCO2(); 
  Serial.print(F("CO2: "));
  Serial.print(co2ppm);
  Serial.println(F(" ppm"));
  
  // Если датчики BMP/E280 не подключены, то брать температуру с датчика CO2
  if(!isBMP280 && !isBME280) {
    temp = myMHZ19.getTemperature();
    Serial.println("MH-Z19 Temp: " + String(temp));        
  }
  
  Serial.println();  
  
  getDataTimer = millis();
}

// Получение данных с температурного датчика
void getTemperatureSensor()
{
    if(isBMP280){
      temp = bmp.readTemperature() + tempCorrection;
    }else if(isBME280){
      temp = bme.readTemperature() + tempCorrection;
    }
    Serial.print(F("Temp: "));
    Serial.print(temp);
    Serial.println(F(" °C"));
    
    if(isBMP280){
      pressure = bmp.readPressure() / 100.0F;
    }else if(isBME280){
      pressure = bme.readPressure() / 100.0F;
    }
    Serial.print(F("P: "));
    Serial.print(pressure);
    Serial.println(F(" hPa"));

    if(isBME280){
      humidity = bme.readHumidity();
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.println(F(" %"));
    }

}

/**
* Сканирование устройств на порте I2C
*/
void scannerI2C()
{
    byte error, address;
    int nDevices;
 
    Serial.println(F("Scanning..."));
 
    nDevices = 0;
    for(address = 8; address < 127; address++ ){
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
 
        if (error == 0){
            Serial.print(F("I2C device found at address 0x"));
            if (address<16)
                Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(F(" !"));
 
            nDevices++;
        }
        else if (error==4) {
            Serial.print(F("Unknow error at address 0x"));
            if (address<16)
                Serial.print(F("0"));
            Serial.println(address, HEX);
        } 
    }
    if (nDevices == 0)
        Serial.println(F("No I2C devices found\n"));
    else
        Serial.println(F("done\n"));  
}
