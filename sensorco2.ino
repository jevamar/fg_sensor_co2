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


// Зуммер
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define c 261
#define d 294
#define e 329
#define f 349
#define g 391
#define gS 415
#define a 440
#define aS 455
#define b 466
#define cH 523
#define cSH 554
#define dH 587
#define dSH 622
#define eH 659
#define fH 698
#define fSH 740
#define gH 784
#define gSH 830
#define aH 880

#define PIN_BEEPER 10
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};


void setup() {
  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Touch button
  pinMode(PIN_BUTTON, INPUT);

//  tone(PIN_BEEPER, 250, 1000);

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

  onButton(digitalRead(PIN_BUTTON));

  if (millis() - getDataTimer >= 5000)
  {
    getTemperatureSensor();
    
    getCO2();
  
    displayMode();
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
  if(mode>2){
    mode=0;
  }
  
  tone(PIN_BEEPER, 5000, 100);
  displayMode();
      
  Serial.print("TOUCH mode: ");
  Serial.println(mode);

  digitalWrite(LED_BUILTIN, LOW);
}

// Событие длиного нажтия кнопки
void eventButtonLong()
{
  if(mode==MODE_WELCOME){
    // march();
    // playMelody();
  }
  tone(PIN_BEEPER, 2000, 1000);  
}


void playMelody()
{
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(PIN_BEEPER, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIN_BEEPER);
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

void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds)
{ 
    digitalWrite(LED_BUILTIN, HIGH);   
    //use led to visualize the notes being played
    
    int x;   
    long delayAmount = (long)(1000000/frequencyInHertz);
    long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
    for (x=0;x<loopTime;x++)   
    {    
        digitalWrite(speakerPin,HIGH);
        delayMicroseconds(delayAmount);
        digitalWrite(speakerPin,LOW);
        delayMicroseconds(delayAmount);
    }    
    
    digitalWrite(LED_BUILTIN, LOW);
    //set led back to low
    
    delay(20);
    //a little delay to make all notes sound separate
} 

void march()
{    
    //for the sheet music see:
    //http://www.musicnotes.com/sheetmusic/mtd.asp?ppn=MN0016254
    //this is just a translation of said sheet music to frequencies / time in ms
    //used 500 ms for a quart note
    
    beep(PIN_BEEPER, a, 500); 
    beep(PIN_BEEPER, a, 500);     
    beep(PIN_BEEPER, a, 500); 
    beep(PIN_BEEPER, f, 350); 
    beep(PIN_BEEPER, cH, 150);
    
    beep(PIN_BEEPER, a, 500);
    beep(PIN_BEEPER, f, 350);
    beep(PIN_BEEPER, cH, 150);
    beep(PIN_BEEPER, a, 1000);
    //first bit
    
    beep(PIN_BEEPER, eH, 500);
    beep(PIN_BEEPER, eH, 500);
    beep(PIN_BEEPER, eH, 500);    
    beep(PIN_BEEPER, fH, 350); 
    beep(PIN_BEEPER, cH, 150);
    
    beep(PIN_BEEPER, gS, 500);
    beep(PIN_BEEPER, f, 350);
    beep(PIN_BEEPER, cH, 150);
    beep(PIN_BEEPER, a, 1000);
    //second bit...
    
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, a, 350); 
    beep(PIN_BEEPER, a, 150);
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, gSH, 250); 
    beep(PIN_BEEPER, gH, 250);
    
    beep(PIN_BEEPER, fSH, 125);
    beep(PIN_BEEPER, fH, 125);    
    beep(PIN_BEEPER, fSH, 250);
    delay(250);
    beep(PIN_BEEPER, aS, 250);    
    beep(PIN_BEEPER, dSH, 500);  
    beep(PIN_BEEPER, dH, 250);  
    beep(PIN_BEEPER, cSH, 250);  
    //start of the interesting bit
    
    beep(PIN_BEEPER, cH, 125);  
    beep(PIN_BEEPER, b, 125);  
    beep(PIN_BEEPER, cH, 250);      
    delay(250);
    beep(PIN_BEEPER, f, 125);  
    beep(PIN_BEEPER, gS, 500);  
    beep(PIN_BEEPER, f, 375);  
    beep(PIN_BEEPER, a, 125); 
    
    beep(PIN_BEEPER, cH, 500); 
    beep(PIN_BEEPER, a, 375);  
    beep(PIN_BEEPER, cH, 125); 
    beep(PIN_BEEPER, eH, 1000); 
    //more interesting stuff (this doesn't quite get it right somehow)
    
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, a, 350); 
    beep(PIN_BEEPER, a, 150);
    beep(PIN_BEEPER, aH, 500);
    beep(PIN_BEEPER, gSH, 250); 
    beep(PIN_BEEPER, gH, 250);
    
    beep(PIN_BEEPER, fSH, 125);
    beep(PIN_BEEPER, fH, 125);    
    beep(PIN_BEEPER, fSH, 250);
    delay(250);
    beep(PIN_BEEPER, aS, 250);    
    beep(PIN_BEEPER, dSH, 500);  
    beep(PIN_BEEPER, dH, 250);  
    beep(PIN_BEEPER, cSH, 250);  
    //repeat... repeat
    
    beep(PIN_BEEPER, cH, 125);  
    beep(PIN_BEEPER, b, 125);  
    beep(PIN_BEEPER, cH, 250);      
    delay(250);
    beep(PIN_BEEPER, f, 250);  
    beep(PIN_BEEPER, gS, 500);  
    beep(PIN_BEEPER, f, 375);  
    beep(PIN_BEEPER, cH, 125); 
           
    beep(PIN_BEEPER, a, 500);            
    beep(PIN_BEEPER, f, 375);            
    beep(PIN_BEEPER, c, 125);            
    beep(PIN_BEEPER, a, 1000);       
    //and we're done \ó/    
}
