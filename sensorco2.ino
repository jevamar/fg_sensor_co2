#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <GyverOLED.h>
#include <MHZ19.h> // by Jonathan Dempsey
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>


// Дисплей OLED SH1106
#define OLED_RESET 4
GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;


// Кнопка сенсорная
#define PIN_BUTTON 6


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


// Датчик СО2 MH-Z19b
#define RX_PIN 3                                          
#define TX_PIN 2
#define BAUDRATE 9600
MHZ19 myMHZ19;
SoftwareSerial mySerial(RX_PIN, TX_PIN);
int co2ppm = 0;

// Счетчик времени для снятия показаний
unsigned long getDataTimer = 0;

#define MODE_MAIN 0     // co2, temp, press
#define MODE_ALL 1      // all, type
#define MODE_WELCOME 2  // welcome
int mode = 0; 

void setup() {
  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Touch button
  pinMode(PIN_BUTTON, INPUT);

  Serial.begin(9600);
  Serial.println(F("Start Big Brother..."));

//  scannerI2C();

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

  
  // BMP280
  Serial.print(F("TEST - BMP280 sensor: "));
  if (bmp.begin(I2C_BMP280)) {  
    isBMP280 = true;
    Serial.println(F("ok"));
  }else{
    Serial.println(F("not found"));
    
    Serial.print(F("TEST - BME280 sensor: "));
    if (bme.begin(I2C_BMP280)) {  
      isBME280 = true;  
      Serial.println(F("ok"));
    }else{
      Serial.println(F("not found"));
    }
  }


  // CO2
  mySerial.begin(BAUDRATE);
  myMHZ19.begin(mySerial);
  myMHZ19.autoCalibration();
}

void loop() 
{

  eventButton(digitalRead(PIN_BUTTON));

  if (millis() - getDataTimer >= 5000)
  {
    getTemperatureSensor();
    
    getCO2();
  
    displayMode();
  }  
}

void displayMode()
{
  if(mode==MODE_MAIN){
    display();
  }
  if(mode==MODE_ALL){
    displayAll();
  }
  if(mode==MODE_WELCOME){
    displayButton();
  }
}

void display()
{
   
  oled.clear();   

  oled.home();
  //oled.setCursorXY(5, 30);

  oled.setScale(2);
  oled.print(F("CO"));
  oled.setScale(1);
  oled.print(F("2"));
  oled.setScale(2);
  oled.print(F(" "));
  oled.print(co2ppm);
  oled.setScale(1);
  oled.print(F(" ppm"));

  oled.setScale(2);
  oled.print(F("\r\n"));
  oled.setScale(1);
  oled.print(F("\r\n"));
  oled.print(F("temp "));
  oled.setScale(2);
  oled.print(temp);
  oled.print(F(" C"));

  oled.setScale(2);
  oled.print(F("\r\n"));
  oled.setScale(1);
  oled.print(F("\r\n"));
  oled.print(F("P "));
  oled.setScale(2);
  oled.print(pressure);
  oled.setScale(1);
  oled.print(F(" hPa"));
    
  oled.update();
}

void displayAll()
{
   
  oled.clear();   

  oled.home();
  oled.setScale(1);

  oled.print(F("co2 "));
  oled.print(co2ppm);
  oled.print(F(" ppm"));
  oled.print(F("\r\n"));

  oled.print(F("\r\n"));
  if(isBMP280){
    oled.print(F("BMP280\r\n"));  
  }
  if(isBME280){
    oled.print(F("BME280\r\n"));  
  }
  oled.print(F("\r\n"));
  
  oled.print(F("temp "));
  oled.print(temp);
  oled.print(F(" C"));
  oled.print(F("\r\n"));

  oled.print(F("P "));
  oled.print(pressure);
  oled.print(F(" hPa"));
  oled.print(F("\r\n"));

  if(isBME280){
    oled.print(F("Humidity "));
    oled.print(humidity);
    oled.print(F(" %"));
  }  
  oled.update();
}

void displayButton()
{
   
  oled.clear();   

  oled.home();
  //oled.setCursorXY(5, 30);
  oled.setScale(1);
  oled.autoPrintln(true);
  oled.print(F("Hello, Okarin!\r\n"));
  oled.setScale(1);
  oled.print(F("\r\n       Go to\r\n\r\n"));
  oled.setScale(2);
  oled.print(F("Steins;\r\nGate"));
    
  oled.update();
}

void eventButton(int status)
{
    
  if(status==HIGH){
    mode++;
    if(mode>2){
      mode=0;
    }
    displayMode();
    digitalWrite(LED_BUILTIN, HIGH);
    
    Serial.print("TOUCH mode: ");
    Serial.println(mode);
    
    delay(1000);
    
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}


void getCO2()
{
  co2ppm = myMHZ19.getCO2(); 
  Serial.print(F("CO2: "));
  Serial.print(co2ppm);
  Serial.println(F(" ppm"));
  
  //        int8_t Temp = myMHZ19.getTemperature();
  //        Serial.println("MH-Z19 Temp: " + String(Temp)+ " °C");        
  
  Serial.println();  
  
  getDataTimer = millis();
}

// Получение данных с температурного датчика
void getTemperatureSensor()
{
    if(isBMP280){
      temp = bmp.readTemperature();
    }else if(isBME280){
      temp = bme.readTemperature();
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
