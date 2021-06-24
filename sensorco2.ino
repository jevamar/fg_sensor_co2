#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <GyverOLED.h>
#include <MHZ19.h> // by Jonathan Dempsey
#include <Adafruit_BMP280.h>


#define OLED_RESET 4

GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;

//Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);
//Adafruit_SH1106G display = Adafruit_SH1106G(64, 128, &Wire);
//Adafruit_SH110X display = Adafruit_SH110X(64, 128, &Wire);

#define PIN_BUTTON 6

#define I2C_BMP280 0x76 
#define RX_PIN 3                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 2                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bme; // I2C

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

int co2ppm = 0;
float temp = 0;
float pressure = 0;

unsigned long getDataTimer = 0;

// bool isBMP280 = true;


void setup() {
  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Touch button
  pinMode(PIN_BUTTON, INPUT);

  Serial.begin(9600);
  Serial.println("Start Big Brother...");

//  scannerI2C();

  // OLED
  Serial.println("128x64 OLED test");
 
  oled.init();  // инициализация
  
  oled.clear();   // очистить дисплей (или буфер)
  oled.update();  // обновить. Только для режима с буфером! OLED_BUFFER

  // --------------------------
  // oled.home();            // курсор в 0,0
  oled.setScale(3);
  //oled.autoPrintln(true);
  oled.setCursor(10, 1);
  oled.print("Future\r\nGadgets");   // печатай что угодно: числа, строки, float, как Serial!
  oled.update();


  delay(3000);


  
  // BMP280
  Serial.println(F("BMP280 test"));
  if (!bme.begin(I2C_BMP280)) {  
    Serial.println(F("BMP280 sensor not found"));
//    isBMP280 = false;
  }

  // CO2
    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
  
}

void loop() 
{

  eventButton(digitalRead(PIN_BUTTON));

  if (millis() - getDataTimer >= 5000)
  {
    printBMP280();
    
    printCO2();
  
    display();
  }  
}


void display()
{
   
  oled.clear();   

  oled.home();
  //oled.setCursorXY(5, 30);

  oled.setScale(2);
  oled.print("CO");
  oled.setScale(1);
  oled.print("2");
  oled.setScale(2);
  oled.print(" ");
  oled.print(co2ppm);
  oled.setScale(1);
  oled.print(" ppm");

  oled.setScale(2);
  oled.print("\r\n");
  oled.setScale(1);
  oled.print("\r\n");
  oled.print("temp ");
  oled.setScale(2);
  oled.print(temp);
  oled.print(" C");

  oled.setScale(2);
  oled.print("\r\n");
  oled.setScale(1);
  oled.print("\r\n");
  oled.print("P ");
  oled.setScale(2);
  oled.print(pressure);
  oled.setScale(1);
  oled.print(" hPa");
    
  oled.update();
}


void displayButton()
{
   
  oled.clear();   

  oled.home();
  //oled.setCursorXY(5, 30);
  oled.setScale(1);
  oled.autoPrintln(true);
  oled.print("Hello, Okarin!\r\n");
  oled.setScale(1);
  oled.print("\r\n       Go to\r\n\r\n");
  oled.setScale(2);
  oled.print("Steins;\r\nGate");
    
  oled.update();
}

void eventButton(int status)
{
    
  if(status==HIGH){
    digitalWrite(LED_BUILTIN, HIGH);
    
    Serial.println("TOUCH");
   
    //printBMP280();
    displayButton();
    delay(1000);
    
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}


void printCO2()
{
        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

        co2ppm = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        
        Serial.print("CO2: ");                      
        Serial.print(co2ppm);                                
        Serial.println(" ppm");                                

//        int8_t Temp;
//        Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
//        Serial.print("Temperature (C): ");                  
//        Serial.println(Temp);                               

        Serial.println();  

        getDataTimer = millis();
}

void printBMP280()
{
    temp = bme.readTemperature();
    Serial.print(F("Temp: "));
    Serial.print(temp);
    Serial.println(" °C");

    pressure = bme.readPressure() / 100.0F;
    Serial.print("P: ");
    Serial.print(pressure);
    Serial.println(" hPa");
//
//    Serial.print("Humidity = ");
//    Serial.print(bme.readHumidity());
//    Serial.println(" %");
}

void scannerI2C()
{
    byte error, address;
    int nDevices;
 
    Serial.println("Scanning...");
 
    nDevices = 0;
    for(address = 8; address < 127; address++ ){
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
 
        if (error == 0){
            Serial.print("I2C device found at address 0x");
            if (address<16)
                Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(" !");
 
            nDevices++;
        }
        else if (error==4) {
            Serial.print("Unknow error at address 0x");
            if (address<16)
                Serial.print("0");
            Serial.println(address,HEX);
        } 
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");  
}
