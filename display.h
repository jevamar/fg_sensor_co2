#ifndef display_h
#define display_h

// Главный Экран
void display()
{
   
  oled.clear();   

  oled.home();
  //oled.setCursorXY(5, 30);

  // Строка CO2 
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

  // Пустая строка 
  oled.setScale(1);
  oled.print(F("\r\n"));

  // Строка температуры
  oled.print(F("temp "));
  oled.setScale(2);
  oled.print(temp);
  oled.setScale(1);
  oled.print(F(" o"));
  oled.setScale(2);
  oled.print(F("C"));
  oled.setScale(2);
  oled.print(F("\r\n"));

  // Пустая строка 
  oled.setScale(1);
  oled.print(F("\r\n"));

  if(isBME280){
    // Строка Влажности
    oled.setScale(1);
    oled.print(F("     "));
    oled.setScale(2);
    oled.print(round(humidity));
    oled.print(F(" %"));
  } else {
    // Строка давления
    oled.print(F("P "));
    oled.setScale(2);
    oled.print(pressure);
    oled.setScale(1);
    oled.print(F(" hPa"));
  }

  oled.update();
}

// Экран со всеми параметрами
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
  
  oled.print(F("T: "));
  oled.print(temp);
  oled.print(F(" C"));
  oled.print(F("\r\n"));

  oled.print(F("P: "));
  oled.print(pressure);
  oled.print(F(" hPa"));
  oled.print(F("\r\n"));

  if(isBME280){
    oled.print(F("H: "));
    oled.print(humidity);
    oled.println(F(" %"));
  }  

  if(isSound){
    oled.println(F("sound on"));
  }else{
    oled.println(F("sound off"));
  }
  
  oled.update();
}

// Экранс произвольным текстом
// void displayButton()
// {
   
//   oled.clear();   

//   oled.home();
//   //oled.setCursorXY(5, 30);
//   oled.setScale(1);
//   oled.autoPrintln(true);
//   oled.print(F("Hello, Okarin!\r\n"));
//   oled.setScale(1);
//   oled.print(F("\r\n       Go to\r\n\r\n"));
//   oled.setScale(2);
//   oled.print(F("Steins;\r\nGate"));
    
//   oled.update();
// }

void displayMode()
{
  if(mode==MODE_MAIN){
    display();
  }
  if(mode==MODE_ALL){
    displayAll();
  }
  // if(mode==MODE_WELCOME){
  //   displayButton();
  // }
}

#endif
