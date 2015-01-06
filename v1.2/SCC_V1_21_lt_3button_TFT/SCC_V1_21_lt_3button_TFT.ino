// ============= Solar Controller v1.21 (en) ===============================================
/*
You are free:
to Share — to copy, distribute and transmit the work
to Remix — to adapt the work
Under the following conditions:
Attribution — You must attribute the work in the manner specified by the author or licensor 
(but not in any way that suggests that they endorse you or your use of the work).
Noncommercial — You may not use this work for commercial purposes.
Share Alike — If you alter, transform, or build upon this work, you may distribute 
the resulting work only under the same or similar license to this one.
All code is copyright Alvydas, alvydas (at) saulevire.lt (c)2014.

Valdiklio nustatymai:
Keičiamas temperatūrų skirtumas siurblio įjungimui/išjungimui
Rankinis siurblio įjungimas/išjungimas kolektoriaus nuorinimui
Reikšmių išsaugojimas
Termostatas su šildymo/šaldymo funkcija arba išjungtas
Ekranas LCD 16x2
Keičiamas ekrano pašvietimo ryškumas
5 klavišų klaviatūra valdymui
Jungtis tinklo modulio ENC28J60 pajungimui
*/
//____________________________________________________________________________________//
// ********** Pašalinti komentarus pagal turimą PCB versiją ************************
//#define PCB_VERSIJA 12 // PCB versija v1.2 su Arduino Pro Mini procesoriumi
#define PCB_VERSIJA 121 // PCB versija v1.21 su Arduino Nano (su CH340G mikroschema)


#ifndef PCB_VERSIJA
#error "Pasirinkite PCB_VERSIJA reiksme 26 arba 27 eiluteje"
#endif

#if PCB_VERSIJA == 12
#define Key_Pin 0    // analog pin assigned for button reading
#define BackLight_Pin 8 //LCD backlight pin (standart LCD KeeyPad use pin 10)
#define ONE_WIRE_BUS1 2 // Collector
#define ONE_WIRE_BUS2 9 // Boiler
#define ONE_WIRE_BUS3 A3 // Thermostat
#else
#define Key_Pin A7    // analog pin assigned for button reading
#define BackLight_Pin 9 //LCD backlight pin (standart LCD KeeyPad use pin 10)
#define ONE_WIRE_BUS1 2 // Collector
#define ONE_WIRE_BUS2 8 // Boiler
#define ONE_WIRE_BUS3 A3 // Thermostat
#endif

//______________________________________________________________________________________//
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


#include <Wire.h>
 #include "MenuBackend.h"  
 // Thank wojtekizk, for example
 // http://majsterkowo.pl/forum/menubackend-jak-sie-w-nim-odnalezc-t1549.html
//  #include <LiquidCrystal.h>         
 #include <OneWire.h>
#include <DallasTemperature.h>
 #include <EEPROM.h>
#include "definitions.h"

#include <util/delay.h>
#include <avr/io.h>
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

// tft disply
#define cs   4
#define dc   5
#define rst  3
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);


/* ------------------ R T C ---------------------- */
/* --------------------- RTC END ---------------- */
char *TFT_string_1;                      // First string text displayed on the LCD
char *TFT_string_2;                      // Second string text displayed on the LCD
    boolean InMenu = false;
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// --- create all of the options menu: ---------------------------------------
// de facto create a MenuItem class objects, which inherit the class MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // menu design 
   //                        ("                ")
   MenuItem P1 =  MenuItem("KOLEKTORIUS",1);
      MenuItem P11 = MenuItem("Skirtumas on  ",2);
      MenuItem P12 = MenuItem("Skirtumas off ",2);
      MenuItem P13 = MenuItem("Siurblio ij   ",2);


   MenuItem P2 = MenuItem("TERMOSTATAS   ",1);//"TERMOSTATAS   "
      MenuItem P21 = MenuItem("temperatura 1 ",2);//"temperatura 1 "
      MenuItem P22 = MenuItem("temperatura 2 ",2);//"temperatura 2 "
      MenuItem P23 = MenuItem("Busena        ",2);//"Busena        "


   MenuItem P3 = MenuItem("NUSTATYMAI    ",1);
      MenuItem P31 = MenuItem("Irasyti      ",2);
      MenuItem P32 = MenuItem("Iprasti      ",2);
//      MenuItem P33 = MenuItem("Sviesumas    ",2);


/* --- Now position the menu (according to the setting specified above) ------------
add - adds vertical addRight - adds a level to the right, to the left adds addLeft
*/
void menuSetup()                       // feature class MenuBackend 
{
      menu.getRoot().add(P1);          // set the root menu, which is the first option
      P1.add(P11);
        P11.add(P12);P11.addLeft(P1);  //  
        P12.add(P13);P12.addLeft(P1);  // 
        P13.add(P11);P13.addLeft(P1);  // 

      menu.getRoot().add(P2);
      P1.addRight(P2);                 //
      
      P2.add(P21);                     // 
        P21.add(P22);P21.addLeft(P2);  // 
        P22.add(P23);P22.addLeft(P2);  // 
        P23.add(P21);P23.addLeft(P2);  //

      menu.getRoot().add(P3);
      P2.addRight(P3);                 //
        
      P3.add(P31);                     // 
        P31.add(P32);P31.addLeft(P3);  // 
        P32.add(P31);P32.addLeft(P3);  // 
//        P33.add(P31);P33.addLeft(P3);  // 

      menu.getRoot().add(P1);
      P3.addRight(P1);                 //
      
}

// -----------  -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // feature class MenuBackend - after pressing OK
                                          // Here is the menu we offer for shares of handling the OK button
{
  #ifdef DEBUGds18b20 
  Serial.print("pasirinkta:  "); Serial.println(used.item.getName()); // test and then unnecessary
  #endif
// --- Below are some of service options ----------- 

     /* ______________________ SETTINGS Save _______________________ */
// Save to EEPROM
     if (used.item.getName() == "Exit          ")   // exactly the same string "Save          "
      { menu.moveLeft(); }
      //////////////////////////////////////////////////////////////
      /* ______________________ SETTINGS Save _______________________ */
// Save to EEPROM
     if (used.item.getName() == "Irasyti      ")   // exactly the same string "Save          "
      {
                 SaveConfig();
                 tft.setCursor(0,0);tft.print(">Irasyta OK     ");delay(2000); // show OK for 2 sec
                 tft.setCursor(0,0);tft.print("              "); // clear line
                 tft.setCursor(0,0);tft.print("*");tft.print(TFT_string_1);           // reconstruct the previous state at LCD
                 tft.setCursor(15,0);tft.print("*");
                menu.moveDown();

      }
      //////////////////////////////////////////////////////////////
      /* __________________________ SETTINGS default ____________ */
// Save to EEPROM
     if (used.item.getName() == "Iprasti      ")   // exactly the same string "Save          "
      {
                 tft.setCursor(0,0);tft.print(">Iprasti OK   ");delay(2000); // show OK for 2 sec
                 tft.setCursor(0,0);tft.print("              "); // clear line
                 tft.setCursor(0,0);tft.print("*");tft.print(TFT_string_1);           // reconstruct the previous state at LCD
                 tft.setCursor(15,0);tft.print("*");
Pump_power_on_difference = 6;  
Pump_power_off_difference = 3;
temperature_1 = 20;  
temperature_2 = 25;
Thermostat_status = 3; // off
Manual_pump_status = false; 
lcd_backlight = 5;                 
                 SaveConfig();
                menu.moveDown();

      }
      //////////////////////////////////////////////////////////////
/* __________________________ Collector   _______________________ */
//  ON - the difference between the temperature       
if (used.item.getName() == "Skirtumas on  ")   // exactly the same string "Difference on "
Pump_power_on_difference =  MeniuFunkcija ("Parinkti= ", Pump_power_on_difference, 25, 1, ">Skirtumas OK");
     ///////////////////////////////////////////////////////////////////
/* __________________________ Collector _______________________ */ 
// OFF - the difference between the temperature            
if (used.item.getName() == "Skirtumas off ")   // exactly the same string "Difference off"
Pump_power_off_difference =  MeniuFunkcija ("Parinkti= ", Pump_power_off_difference, 25, 1, ">Skirtumas OK"); 
     ///////////////////////////////////////////////////////////////////     

/* __________________________ Collector Manual pump on ____________________________________ */
if (used.item.getName() == "Skirtumas on  ") 
 {       
        tft.setCursor(0,0);tft.write(7);     
        tft.setCursor(1,1);tft.print(F("Siurblys")); 
        if (Manual_pump_status == true) tft.print(F(" -on ")); // 
        if (Manual_pump_status == false) tft.print(F(" -off")); //
        int  action=-1; delay(1000);         // 
                                           
        while(action!=4)                   // 
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin); //delay(300);  
                                            
           if(Keyboard_change!=action)           
             {
if(action==0) {Manual_pump_status = false; tft.setCursor(11,1);tft.print("off");delay(200);}
if(action==3) {Manual_pump_status = true;  tft.setCursor(11,1);tft.print("on ");delay(200);}
             if(action==4) // 0
               {
                 tft.setCursor(0,0); tft.print(">Siurblys OK"); delay(2000); // 0
                 tft.setCursor(0,0); tft.print("              "); // 0
                 tft.setCursor(1,0);tft.print(TFT_string_1);           // 0
                 menu.moveDown();
               }
             } 
         } Keyboard_change=action;
 }
/* __________________________ Termostat temperature 1   _______________________ */
if (used.item.getName() == "temperatura 1 ")   // exactly the same string "temperature 1 "
temperature_1 =  MeniuFunkcija ("temp 1=    ", temperature_1, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* __________________________ Termostat temperature 2  _______________________ */     
if (used.item.getName() == "temperatura 2 ")   // exactly the same string "temperature 2 "
temperature_2 =  MeniuFunkcija ("temp 2=    ", temperature_2, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ Termostat status  _______________________ */     
if (used.item.getName() == "Busena        ") 
 {       
        tft.setCursor(0,0);tft.write(7);     
        tft.setCursor(1,1);tft.print("Busena-"); 
        if (Thermostat_status == 1) tft.print("sildymas"); // heating
        if (Thermostat_status == 2) tft.print("saldymas"); // freezing
        if (Thermostat_status == 3) tft.print("isjungta"); // turned off
//      tft.print(Busena(Thermostat_status,termostato_status_name));
        int  action=-1; delay(1000);         // 
                                           
        while(action!=4)                   // 
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin); //delay(300);  
                                            
           if(Keyboard_change!=action)           
             {
             if (action==0) {Thermostat_status++; if(Thermostat_status>3) Thermostat_status=1; 
                                                 tft.setCursor(8,1); 
                                               //  tft.print(Busena(Thermostat_status,termostato_status_name));
                                                 if (Thermostat_status == 1) tft.print("sildymas"); // heating
                                                 if (Thermostat_status == 2) tft.print("saldymas"); // freezing
                                                 if (Thermostat_status == 3) tft.print("isjungta"); // turned off
                                            delay(200);}
             if(action==3)  {Thermostat_status--; if(Thermostat_status<1) Thermostat_status=3; 
                                                 tft.setCursor(8,1); 
                                              //   tft.print(Busena(Thermostat_status,termostato_status_name));
                                                 if (Thermostat_status == 1) tft.print("sildymas"); // heating
                                                 if (Thermostat_status == 2) tft.print("saldymas"); // freezing
                                                 if (Thermostat_status == 3) tft.print("isjungta"); // turned off 
                                               delay(200);}
             if(action==4) // 0
               {
                 tft.setCursor(0,0); tft.print(">Busena      OK"); delay(2000); // 0
                 tft.setCursor(0,0); tft.print("              "); // 0
                 tft.setCursor(1,0);tft.print(TFT_string_1);           // 0
                 menu.moveDown();
               }
             } 
         } Keyboard_change=action;
 }
 
}
// --- Reakcja na wci�ni�cie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  menuTimer = millis(); // menu is inactive timer
  timerEnable = 1;
  
  if(changed.to.getName()==menu.getRoot())
  {
    InMenu =false;
    #ifdef DEBUGds18b20
    Serial.println("menuChangeEvent:- Now we are on MenuRoot");
    #endif
   LCD_TFT_template();
//    Temperature_Imaging();
tft.setRotation( 3 );
  tft.fillScreen(ST7735_BLACK);
    TFT_Temperature_Imaging();
  }
  /* it really is only useful here in shortkey and is used primarily to enrich the 
  menu with arrow symbols depending on what is selected. Everything here is going 
  on is displayed on the tft.
  */
  int c=changed.to.getShortkey();                         // shortkey charge (1,2,3, or 4)
//  tft.clear();        // clear lcd
//    tft.fillScreen(ST7735_BLACK);
//  tft.setCursor(0,0); 
    tft.setCursor(0, 0);
  if(c==1)                                                // If this menu Main contacts (shortkey = 1) are:
    {InMenu =true;
//    tft.write(3);                                         // Left arrow 
    strcpy(TFT_string_1,changed.to.getName());            // Create a string in the first line
//    tft.print(TFT_string_1);   
    tft.println(TFT_string_1);    // Display it 
//    tft.setCursor(15,0);tft.write(4);                     // Right arrow 
//    tft.setCursor(0,1);tft.write(5);                      // Down arrow 
//    tft.setCursor(15,1);tft.write(5);                     // Down arrow 
    }
    if(c==2)                                              // if the submenu for the child - (shortkey = 2) are:
    {InMenu =true;
//    tft.print("*");                                       // draw a star
    strcpy(TFT_string_2,changed.to.getName());            // create a string in the first line
tft.println(TFT_string_1);//    tft.print(TFT_string_1);                              // print it
//tft.print(P2);
//    tft.setCursor(15,0);tft.print("*");                   // draw a star
//    tft.setCursor(0,1);tft.write(6);                      // the second line and arrow return (arrowBack)
tft.println(changed.to.getName());//    tft.print(changed.to.getName());                      // display name of "child"
//    tft.setCursor(15,1);tft.write(7);                     // arrow up-down
    }
    if(c==3)                                              // if the child has a child - (shortkey = 3) are:
    {InMenu =true;
//    tft.print("*");                                       // draw a star
    strcpy(TFT_string_2,changed.to.getName());            // the name of the menu options to the variable line 2
tft.println(TFT_string_1);//    tft.print(TFT_string_1);                              // and display the first line of
//    tft.setCursor(15,0);tft.print("*");                   // draw a star
//    tft.setCursor(0,1);tft.write(6);                      // the second line and arrow arrowBack
tft.print(changed.to.getName());//    tft.print(changed.to.getName());                      // display the grandson of the second line
//    tft.setCursor(15,1);tft.write(4);                     // arrow to the right because they are the grandchildren
    }
    
    if(c==4)                                              // if grandchild (shortkey = 4) are:
    {InMenu =true;
//    tft.print("*");                                       // draw a star
tft.println(TFT_string_2);//    tft.print(TFT_string_2);                              // in the first line of the display child (or parent grandchild)
//    tft.setCursor(15,0);tft.print("*");                   // draw a star
//    tft.setCursor(0,1);tft.write(6);                      // the second line and arrow arrowBack
tft.print(changed.to.getName());//    tft.print(changed.to.getName());                      // display grandson
//    tft.setCursor(15,1);tft.write(7);                     // arrow up-down
    } 
}

// --- 5 analog buttons keyboard scan version DFRobot --------------------------------------
volatile int Read_keyboard(int analog)
{
   int stan_Analog = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (stan_Analog > 1000) return -1; // limit
   if (stan_Analog < 50)   return 3;  // right
   if (stan_Analog < 200)  return 1;  // up
   if (stan_Analog < 400)  return 2;  // down
   if (stan_Analog < 600)  return 0;  // left
   if (stan_Analog < 800)  return 4;  // OK 
   return -1;                         // Not pressed
}
// ============================================================================================
// 
void setup()
{
 LoadConfig(); 

  /* ********************************************************* */

  pinMode(BackLight_Pin, OUTPUT);
  digitalWrite(BackLight_Pin,HIGH);
  analogWrite(BackLight_Pin,lcd_backlight*25);
  TFT_string_1=new char[16]; 
  TFT_string_2=new char[16];
                        
    
USART_init(); //Serial.begin(9600); save 244 bytes

   Collector_sensor.begin();Boiler_sensor.begin();
   Thermostat_sensor.begin();
   
  pinMode(Relay_Collector,OUTPUT);pinMode(Relay_Thermostat,OUTPUT);
  digitalWrite(Relay_Collector,HIGH);digitalWrite(Relay_Thermostat,HIGH);
  menuSetup(); 
//  menu.moveUp();      
  Temperature_measurements_1();
//tft.begin();
 
//  Temperature_Imaging();
    LCD_switching_on_Time = millis();
    temperature_measurement_time_1 = millis();
//---------------------------------------------------------
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation( 3 );
LCD_TFT_template();
  delay(500);

//---------------------------------------------------------
  }  // setup() ...************ END **************...
  // ************************ START void loop() *******************************
void loop()    
{
  
// If the menu is inactive for some time, it returns to the main program  
  if(timerEnable == 1 && millis() - menuTimer >=25000){
    Serial.println(F("Timer Clear........."));
     delay(30);

menu.moveLeft();
menu.moveUp();
menu.getRoot();
    timerEnable = 0;
    LCD_TFT_template();
    TFT_Temperature_Imaging();
  }  
  
  
// measured temperature specified time intervals (Temperature_measurement_interval)
/* +++++++++++++++++++++++++++ First level ++++++++++++++++++++++++++++++++++++ */ 
if (millis() > temperature_measurement_time_1 ) { 
  temperature_measurement_time_1 = millis() + Temperature_measurement_interval_1;
  Temperature_measurements_1();}

	
  // if the screen, without application of the button illuminates more than the tasks, backlight off
      if (millis()- LCD_switching_on_Time > The_LCD_light_Break) { 
      analogWrite(BackLight_Pin, 0);
       pinMode(13,OUTPUT);digitalWrite(13,LOW); // only for test
      Backlighting = false;
      LCD_switching_on_Time = millis();}
 // When you press any key, the screen backlight is turned on when it is turned off
if ((buttonPressed != -1) && (Backlighting == false)){ analogWrite(BackLight_Pin,lcd_backlight*25);
											digitalWrite(13,HIGH); // only for test
                                            Backlighting = true;}


//********************************************************************
  buttonPressed=Read_keyboard(Key_Pin);delay(30);       // read the state of the keyboard:
  if(Keyboard_change!=buttonPressed)     {              // if there was a change in the state are:
  navMenu();
  }
Keyboard_change=buttonPressed;                 //Assign the value of x variable amended so that the long pressing the 

//********************************************************************
// If you are not currently within the menu is of a continuous program
if (InMenu == false){
  // time interval used for the LCD refresh
  if (millis() > LCD_Update_Time ) { 
  LCD_Update_Time = millis() + LCD_Update_Interval;
  LCD_TFT_template();
//  Temperature_Imaging();
//  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
//  tft.fillScreen(ST7735_BLACK);
  TFT_Temperature_Imaging();

  //#ifdef DEBUGds18b20
//Serial.println("Temperate_measurement");
//unsigned long start = millis();
//#endif
  
#ifdef DEBUGds18b20
//unsigned long stop = millis();
//Serial.print("Temperature measurement time: ");  Serial.println(stop - start);
Serial.print("K/ ");Serial.print(K);Serial.print(" B/ ");Serial.print(B);Serial.print(" T/ ");Serial.println(T);
Serial.println("----");
Serial.print("Thermostat_status- ");Serial.println(Thermostat_status);
Serial.print("temperature_1- ");Serial.print(temperature_1);
Serial.print("  temperature_2- ");Serial.println(temperature_2);
Serial.println("----");
Serial.print("Pump_power_on_difference- ");Serial.print(Pump_power_on_difference);
Serial.print("  Pump_power_off_difference- ");Serial.println(Pump_power_off_difference);

Serial.print("millis- ");Serial.println(millis()/1000);
Serial.println(freeRam());

#endif
Serial.print("millis- ");Serial.println(millis()/1000);
Serial.println(freeRam());
  }
} 


//------------------ collector pump and thermostat control -----------------------//
if (millis() > Relay_switching_time ) 
 {
   Relay_switching_time=millis()+Relay_switching_interval;
if (Manual_pump_status == true) {digitalWrite(Relay_Collector,LOW);}
else{
   if (K-B>=Pump_power_on_difference) digitalWrite(Relay_Collector,LOW);
   if (K-B<=Pump_power_off_difference) digitalWrite(Relay_Collector,HIGH);
    }
  if (Thermostat_status == 1) 
   {// If the heating mode (Thermostat_status = 1)
    if (T <= temperature_1) digitalWrite(Relay_Thermostat,LOW);  
    if (T >= temperature_2) digitalWrite(Relay_Thermostat,HIGH);
   }
   if (Thermostat_status == 2) 
    {// If the freezing mode (Thermostat_status = 2)
     if (T >= temperature_1) digitalWrite(Relay_Thermostat,LOW);  
     if (T <= temperature_2) digitalWrite(Relay_Thermostat,HIGH); 
    }
    if (Thermostat_status == 3) 
     // If you do not need a second relay-mode off
      digitalWrite(Relay_Thermostat,HIGH);
 }
}// === END ===========================================================
////////////////////////////////////////////////////////////////////////
void Temperature_measurements_1(){
  //____________________________ Start Sensor 1 _________________________________
#ifdef SetWaitForConversionFALSE
  Collector_sensor.setWaitForConversion(false);  // makes it async
#endif
  Collector_sensor.requestTemperatures(); // Send the command to get temperatures
  K=Collector_sensor.getTempCByIndex(0);
//_____________________________ Stop Sensor 1 ___________________________________
  //______________________ Start Sensor 3 ________________________________________
  #ifdef SetWaitForConversionFALSE
  Thermostat_sensor.setWaitForConversion(false);  // makes it async
#endif
  Thermostat_sensor.requestTemperatures(); // Send the command to get temperatures
T=Thermostat_sensor.getTempCByIndex(0);
//___________________ Stop Sensor 3 ______________________________________________
//__________________________________________ Start Sensor 2 _____________________
#ifdef SetWaitForConversionFALSE
  Boiler_sensor.setWaitForConversion(false);  // makes it async
#endif
  Boiler_sensor.requestTemperatures(); // Send the command to get temperatures
  B=Boiler_sensor.getTempCByIndex(0);
//_____________________________________ Stop Sensor 2 ____________________________
}

void LCD_TFT_template(){
//  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
// tft.fillScreen(ST7735_BLACK);
//tft.setRotation( 3 );
// *** Printing static Items on display in the setup void in order to speed up the loop void****
 tft.setTextSize(2);
   tft.drawLine(0, 58, tft.width()-1, 58, ST7735_WHITE);
   tft.setCursor(0, 60);
   tft.setTextColor(ST7735_YELLOW);
   tft.println("K : ");
   tft.setTextColor(ST7735_WHITE);
   tft.setCursor(0, 76);   tft.println("B : ");
   tft.setTextColor(ST7735_YELLOW);
   tft.setCursor(0, 92);   tft.println("T1: ");
   tft.drawLine(0, 108, tft.width()-1, 108, ST7735_WHITE);
   tft.setCursor(0, 110);tft.setTextSize(1);tft.print("freeRam ");tft.print(freeRam());
}
void Temperature_Imaging(){
tft.setCursor(1,0); tft.print(K); if (K-B>0) {tft.setCursor(8,0); tft.print("+");tft.print((K-B),1);}
                                             else {tft.setCursor(8,0); tft.print((K-B),1);}
                                  if (K-B>=Pump_power_on_difference)  {tft.setCursor(14,0);tft.print("K");tft.write(1);}  
                                  if (K-B<=Pump_power_off_difference) {tft.setCursor(14,0);tft.print("K");tft.write(5);}  
                                             
tft.setCursor(1,1); tft.print(B); tft.setCursor(8,1); tft.print(T,1);//(int(K + 0.5));
  if (Thermostat_status == 1) 
   {// If the heating mode (Thermostat_status = 1)
    if (T <= temperature_1) {tft.setCursor(14,1);tft.print("H");tft.write(1);}  
    if (T >= temperature_2) {tft.setCursor(14,1);tft.print("H");tft.write(5);}  
   }
   if (Thermostat_status == 2) 
    {// If the freezing mode (Thermostat_status = 2)
     if (T >= temperature_1) {tft.setCursor(14,1);tft.print("F");tft.write(1);}  
     if (T <= temperature_2) {tft.setCursor(14,1);tft.print("F");tft.write(5);}  
    }
    if (Thermostat_status == 3) 
     {tft.setCursor(13,1);tft.print("off");}
}
void TFT_Temperature_Imaging(){
//  tft.setRotation( 3 );
    tft.setCursor(10,0);
   tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.println("SauleVire.lt");
   tft.setTextColor(ST7735_YELLOW,ST7735_BLACK);
    tft.setTextSize(2); //set text size for all data coming from DHT11
  tft.setCursor(68, 60);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK); // set color for all data coming from DHT11
  tft.print((float)K,2);
  tft.setCursor(68, 76);
   tft.print((float)B,2);
  tft.setCursor(68, 92);
   tft.print(T, 2);

}
   int MeniuFunkcija (String text_1, int  Converted_Value, int Max_Value, int Min_Value, String text_2)
	        {
        tft.setCursor(0,32);//tft.write(7);     
       /* tft.setCursor(1,1);*/tft.print(text_1); //("Nustatyta=   "); 
        tft.setCursor(120,32);tft.print( Converted_Value); // shows the current value
        int  action=-1; delay(1000);         // 
                                           
        while(action!=4)                   // 
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin); //delay(300);  
                                            
           if(Keyboard_change!=action)           
             {
             if (action==0) { Converted_Value++; if( Converted_Value>Max_Value)  Converted_Value=Max_Value; tft.setCursor(120,32);
                                                    if( Converted_Value<10) tft.print(" ");
                                                      tft.print( Converted_Value); delay(200);}
             if(action==3)  { Converted_Value--; if( Converted_Value<Min_Value)  Converted_Value=Min_Value; tft.setCursor(120,32);
                                                     if( Converted_Value<10) tft.print(" ");
                                                       tft.print( Converted_Value); delay(200);}
             if(action==4) // 0
               {
                 tft.setCursor(0,16); tft.print(text_2); delay(2000); // 0
                 tft.setCursor(0,16); tft.print("                "); // 0
                 tft.setCursor(0,16);tft.print("*");tft.print(TFT_string_1);           // reconstruct the previous state at LCD
   //              tft.setCursor(15,0);tft.print("*");
                 menu.moveDown();
               }
             } 
         } Keyboard_change=action;  // Keyboard_change update, in order to react only Keyboard_change keyboard status
         // This is an important moment - while loop ends and turn the control to the main loop loop ()
         return  Converted_Value;
      }
// Scan settings 
boolean LoadConfig(){
  if ((EEPROM.read(0) == 27) && (EEPROM.read(1) == 28) && 
     (EEPROM.read(2) == 13) && (EEPROM.read(3) == 18)) {

    if (EEPROM.read(4) == EEPROM.read(5)) Pump_power_on_difference = EEPROM.read(4);  
    if (EEPROM.read(6) == EEPROM.read(7)) Pump_power_off_difference = EEPROM.read(6);
    if (EEPROM.read(8) == EEPROM.read(9)) temperature_1 = EEPROM.read(8);  
    if (EEPROM.read(10) == EEPROM.read(11)) temperature_2 = EEPROM.read(10);
    if (EEPROM.read(12) == EEPROM.read(13)) Thermostat_status = EEPROM.read(12);
    if (EEPROM.read(14) == EEPROM.read(15)) lcd_backlight = EEPROM.read(14);
    return true;
  }
  return false;
}
// Write settings
void SaveConfig(){
  EEPROM.write(0,27);
  EEPROM.write(1,28);
  EEPROM.write(2,13);
  EEPROM.write(3,18);
  EEPROM.write(4,Pump_power_on_difference);EEPROM.write(5,Pump_power_on_difference);  // 
  EEPROM.write(6,Pump_power_off_difference); EEPROM.write(7,Pump_power_off_difference);  // 
  EEPROM.write(8,temperature_1);EEPROM.write(9,temperature_1);  // 
  EEPROM.write(10,temperature_2); EEPROM.write(11,temperature_2);  // 
  EEPROM.write(12,Thermostat_status); EEPROM.write(13,Thermostat_status);  // 
  EEPROM.write(14,lcd_backlight); EEPROM.write(15,lcd_backlight);  // 

}

void flashbacklight() {
  digitalWrite(BackLight_Pin, LOW);  delay(150);
  digitalWrite(BackLight_Pin, HIGH); delay(150);
}
/////////////////////////////////////////
void navMenu(){
  //MenuItem currentMenu=menu.getCurrent();
  switch (buttonPressed){
  case 4: //select     
  //The current item has an element right, it's a sub menu so nav right.
    if (menu.getCurrent().getRight() != 0){  menu.moveRight();
    #ifdef DEBUGds18b20
                  Serial.print(menu.getCurrent().getName()); Serial.println(" has menu right");
     #endif
   }
    else{  //otherwise, menu has no child and has been pressed. enter the current menu
          menu.use();} break;     //select    
  case 3: menu.moveDown(); break; //right
  case 0: 
        if (menu.getCurrent().getRight() != 0){  menu.moveUp();
        #ifdef DEBUGds18b20
                  Serial.print(menu.getCurrent().getName()); 
                Serial.println(" has SUMmenu right");
              //USART_putstring(" has SUMmenu right\n");
        #endif
            }
    else{  //otherwise, menu has no child and has been pressed. enter the current menu
  
  menu.moveLeft();}break;    //left
  }
}
////////////////////////////////////////////
void USART_init(void){
 
 UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
 UBRR0L = (uint8_t)(BAUD_PRESCALLER);
 UCSR0B = (1<<RXEN0)|(1<<TXEN0);
 UCSR0C = (3<<UCSZ00);
}
void USART_send( unsigned char data){
 
 while(!(UCSR0A & (1<<UDRE0)));
 UDR0 = data;
 
}
 
void USART_putstring(char* StringPtr){
 
while(*StringPtr != 0x00){
 USART_send(*StringPtr);
 StringPtr++;}
 
}
