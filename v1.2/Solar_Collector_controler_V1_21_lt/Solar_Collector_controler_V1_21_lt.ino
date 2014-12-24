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
//#define PCB_VERSIJA 121 // PCB versija v1.21 su Arduino Nano (su CH340G mikroschema)

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
#define Key_Pin 7    // analog pin assigned for button reading
#define BackLight_Pin 9 //LCD backlight pin (standart LCD KeeyPad use pin 10)
#define ONE_WIRE_BUS1 2 // Collector
#define ONE_WIRE_BUS2 8 // Boiler
#define ONE_WIRE_BUS3 A3 // Thermostat
#endif

//______________________________________________________________________________________//

#include <Wire.h>
 #include "MenuBackend.h"  
 // Thank wojtekizk, for example
 // http://majsterkowo.pl/forum/menubackend-jak-sie-w-nim-odnalezc-t1549.html
  #include <LiquidCrystal.h>         
 #include <OneWire.h>
#include <DallasTemperature.h>
 #include <EEPROM.h>
#include "definitions.h"

  // --- define their own characters for the LCD arrows: down, left, right, top and power 
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
uint8_t arrowUp[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
    // definition pin for LCD (check the pins in your LCD)
LiquidCrystal lcd(A5, 3, 4, 5, 6, 7);
/* ------------------ R T C ---------------------- */
/* --------------------- RTC END ---------------- */
char *LCD_string_1;                      // First string text displayed on the LCD
char *LCD_string_2;                      // Second string text displayed on the LCD
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
   MenuItem P1 =  MenuItem("KOLEKTORIUS   ",1);
      MenuItem P11 = MenuItem("Skirtumas on  ",2);
      MenuItem P12 = MenuItem("Skirtumas off ",2);
      MenuItem P13 = MenuItem("Siurblio ij   ",2);

   MenuItem P2 = MenuItem("TERMOSTATAS   ",1);
      MenuItem P21 = MenuItem("temperatura 1 ",2);
      MenuItem P22 = MenuItem("temperatura 2 ",2);
      MenuItem P23 = MenuItem("Busena        ",2);

   MenuItem P3 = MenuItem("NUSTATYMAI    ",1);
      MenuItem P31 = MenuItem("Irasyti       ",2);
      MenuItem P32 = MenuItem("Iprasti nustat",2);
      MenuItem P33 = MenuItem("Sviesumas     ",2);

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
        P32.add(P33);P32.addLeft(P3);  // 
        P33.add(P31);P33.addLeft(P3);  // 
      menu.getRoot().add(P1);
      P3.addRight(P1);                 //
      
}
// -----------  -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // feature class MenuBackend - after pressing OK
                                          // Here is the menu we offer for shares of handling the OK button
{
   Serial.print("pasirinkta:  "); Serial.println(used.item.getName()); // test and then unnecessary
// --- Below are some of service options ----------- 
/* __________________________Settings brithness __________________ */
  if (used.item.getName() == "Sviesumas     ")
  {
  lcd.setCursor(0,1);lcd.write(7);     // simbolis aukštyn/žemyn
  lcd.print("                  ");lcd.setCursor(1,1);lcd.print("Sviesumas"); // keiciamos reikšmes pavadinimas
  lcd.setCursor(12,1);lcd.print(lcd_backlight);lcd.print("0% ");lcd.write(7);                        // dabartine reikšme
  int  action=-1;delay(1000);                                             // pagalbinis kintamasis, kontroliuojantis while cikla
                                                                         // jei jums nereikia keisti, spauti OK po 1 sek. ir grižti i meniu  
  while(action!=4)                   // Šis ciklas bus kartojamas, kol paspausite mygtuka OK
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin);//delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis ponizej przy 3 róznych definicjach funkcji czytaj
           if(Keyboard_change!=action)                    // ruszamy do pracy tylko wtedy gdy Keyboard_changeienil sie stan klawiatury
             {lcd.setCursor(12,1);
             if (action==1) {lcd_backlight++; analogWrite(BackLight_Pin,lcd_backlight*25);delay(300);}
               // jesli akcja=1 (czyli wcisnieto klawisz w góre to zwiekszono temperature
               // ustawiono max próg i wyswietlono obecna temperature
             if(action==2)  {lcd_backlight--;analogWrite(BackLight_Pin,lcd_backlight*25);delay(300);}
if (lcd_backlight > 10)  lcd_backlight = 1;
if (lcd_backlight < 1)  lcd_backlight = 10;
            if (lcd_backlight < 10) lcd.print(" ");
            lcd.print(lcd_backlight);
            if (lcd_backlight == 0) lcd.print("");
            lcd.print("0% ");

               // jesli akcja=2 (czyli wcisnieto klawisz w dól to mniejszono temperature
               // ustawiono min próg i wyswietlono obecna temperature
             if(action==4) // jesli wcisnieto OK 
               {
                 lcd.setCursor(0,0);lcd.print(">Sviesumas  OK");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(0,1);lcd.print("                "); // czyscimy linie
                 menu.moveDown();
               //  lcd.setCursor(1,0);lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } Keyboard_change=action;  // aktualizacja Keyboard_changeiennej Keyboard_change, po to aby reagowac tylko na Keyboard_changeiany stanu klawiatury
         // tu WAZNY MOMENT - konczy sie petla while i zwracamy sterowanie do glównej petli loop()
      } 
      //////////////////////////////////////////////////////////////////
      /* ______________________ SETTINGS Save _______________________ */
// Save to EEPROM
     if (used.item.getName() == "Irasyti       ")   // exactly the same string "Save          "
      {
                 SaveConfig();
                 lcd.setCursor(0,0);lcd.print(">Irasyta OK     ");delay(2000); // show OK for 2 sec
                 lcd.setCursor(0,0);lcd.print("              "); // clear line
                 lcd.setCursor(0,0);lcd.print("*");lcd.print(LCD_string_1);           // reconstruct the previous state at LCD
                 lcd.setCursor(15,0);lcd.print("*");
                menu.moveDown();

      }
      //////////////////////////////////////////////////////////////
      /* __________________________ SETTINGS default ____________ */
// Save to EEPROM
     if (used.item.getName() == "Iprasti nustat")   // exactly the same string "Save          "
      {
                 lcd.setCursor(0,0);lcd.print(">Iprasti OK   ");delay(2000); // show OK for 2 sec
                 lcd.setCursor(0,0);lcd.print("              "); // clear line
                 lcd.setCursor(0,0);lcd.print("*");lcd.print(LCD_string_1);           // reconstruct the previous state at LCD
                 lcd.setCursor(15,0);lcd.print("*");
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
Pump_power_on_difference =  MeniuFunkcija ("Parinkti=    ", Pump_power_on_difference, 25, 1, ">Skirtumas  OK");
     ///////////////////////////////////////////////////////////////////
/* __________________________ Collector _______________________ */ 
// OFF - the difference between the temperature            
if (used.item.getName() == "Skirtumas off ")   // exactly the same string "Difference off"
Pump_power_off_difference =  MeniuFunkcija ("Parinkti=    ", Pump_power_off_difference, 25, 1, ">>Skirtumas  OK"); 
     ///////////////////////////////////////////////////////////////////     

/* __________________________ Collector Manual pump on ____________________________________ */
if (used.item.getName() == "Siurblio ij   ") 
 {       
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print("Siurblys"); 
        if (Manual_pump_status == true) lcd.print(" -on "); // 
        if (Manual_pump_status == false) lcd.print(" -off"); //
        int  action=-1; delay(1000);         // 
                                           
        while(action!=4)                   // 
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin); //delay(300);  
                                            
           if(Keyboard_change!=action)           
             {
if(action==1) {Manual_pump_status = false; lcd.setCursor(11,1);lcd.print("off");delay(200);}
if(action==2) {Manual_pump_status = true;  lcd.setCursor(11,1);lcd.print("on ");delay(200);}
             if(action==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print(">Siurblys   OK"); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print("              "); // 0
                 lcd.setCursor(1,0);lcd.print(LCD_string_1);           // 0
                 menu.moveDown();
               }
             } 
         } Keyboard_change=action;
 }
/* __________________________ Termostat temperature 1   _______________________ */
if (used.item.getName() == "temperatura 1 ")   // exactly the same string "temperature 1 "
temperature_1 =  MeniuFunkcija ("temp 1=       ", temperature_1, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* __________________________ Termostat temperature 2  _______________________ */     
if (used.item.getName() == "temperatura 2 ")   // exactly the same string "temperature 2 "
temperature_2 =  MeniuFunkcija ("temp 2=       ", temperature_2, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ Termostat status  _______________________ */     
if (used.item.getName() == "Busena        ") 
 {       
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print("Busena-"); 
        if (Thermostat_status == 1) lcd.print("sildymas"); // heating
        if (Thermostat_status == 2) lcd.print("saldymas"); // freezing
        if (Thermostat_status == 3) lcd.print("isjungta"); // turned off
//      lcd.print(Busena(Thermostat_status,termostato_status_name));
        int  action=-1; delay(1000);         // 
                                           
        while(action!=4)                   // 
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin); //delay(300);  
                                            
           if(Keyboard_change!=action)           
             {
             if (action==1) {Thermostat_status++; if(Thermostat_status>3) Thermostat_status=1; 
                                                 lcd.setCursor(8,1); 
                                               //  lcd.print(Busena(Thermostat_status,termostato_status_name));
                                                 if (Thermostat_status == 1) lcd.print("sildymas"); // heating
                                                 if (Thermostat_status == 2) lcd.print("saldymas"); // freezing
                                                 if (Thermostat_status == 3) lcd.print("isjungta"); // turned off
                                            delay(200);}
             if(action==2)  {Thermostat_status--; if(Thermostat_status<1) Thermostat_status=3; 
                                                 lcd.setCursor(8,1); 
                                              //   lcd.print(Busena(Thermostat_status,termostato_status_name));
                                                 if (Thermostat_status == 1) lcd.print("sildymas"); // heating
                                                 if (Thermostat_status == 2) lcd.print("saldymas"); // freezing
                                                 if (Thermostat_status == 3) lcd.print("isjungta"); // turned off 
                                               delay(200);}
             if(action==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print(">Busena      OK"); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print("              "); // 0
                 lcd.setCursor(1,0);lcd.print(LCD_string_1);           // 0
                 menu.moveDown();
               }
             } 
         } Keyboard_change=action;
 }
 
}
// --- Reakcja na wci�ni�cie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  if(changed.to.getName()==menu.getRoot())
  {
    InMenu =false;
    Serial.println("Now we are on MenuRoot");
    LCD_T_template();
    Temperature_Imaging();
  }
  /* it really is only useful here in shortkey and is used primarily to enrich the 
  menu with arrow symbols depending on what is selected. Everything here is going 
  on is displayed on the LCD.
  */
  int c=changed.to.getShortkey();                         // shortkey charge (1,2,3, or 4)
  lcd.clear();                                            // clear lcd
  lcd.setCursor(0,0); 
  if(c==1)                                                // If this menu Main contacts (shortkey = 1) are:
    {InMenu =true;
    lcd.write(3);                                         // Left arrow 
    strcpy(LCD_string_1,changed.to.getName());            // Create a string in the first line
    lcd.print(LCD_string_1);                              // Display it 
    lcd.setCursor(15,0);lcd.write(4);                     // Right arrow 
    lcd.setCursor(0,1);lcd.write(5);                      // Down arrow 
    lcd.setCursor(15,1);lcd.write(5);                     // Down arrow 
    }
    if(c==2)                                              // if the submenu for the child - (shortkey = 2) are:
    {InMenu =true;
    lcd.print("*");                                       // draw a star
    strcpy(LCD_string_2,changed.to.getName());            // create a string in the first line
    lcd.print(LCD_string_1);                              // print it
    lcd.setCursor(15,0);lcd.print("*");                   // draw a star
    lcd.setCursor(0,1);lcd.write(6);                      // the second line and arrow return (arrowBack)
    lcd.print(changed.to.getName());                      // display name of "child"
    lcd.setCursor(15,1);lcd.write(7);                     // arrow up-down
    }
    if(c==3)                                              // if the child has a child - (shortkey = 3) are:
    {InMenu =true;
    lcd.print("*");                                       // draw a star
    strcpy(LCD_string_2,changed.to.getName());            // the name of the menu options to the variable line 2
    lcd.print(LCD_string_1);                              // and display the first line of
    lcd.setCursor(15,0);lcd.print("*");                   // draw a star
    lcd.setCursor(0,1);lcd.write(6);                      // the second line and arrow arrowBack
    lcd.print(changed.to.getName());                      // display the grandson of the second line
    lcd.setCursor(15,1);lcd.write(4);                     // arrow to the right because they are the grandchildren
    }
    
    if(c==4)                                              // if grandchild (shortkey = 4) are:
    {InMenu =true;
    lcd.print("*");                                       // draw a star
    lcd.print(LCD_string_2);                              // in the first line of the display child (or parent grandchild)
    lcd.setCursor(15,0);lcd.print("*");                   // draw a star
    lcd.setCursor(0,1);lcd.write(6);                      // the second line and arrow arrowBack
    lcd.print(changed.to.getName());                      // display grandson
    lcd.setCursor(15,1);lcd.write(7);                     // arrow up-down
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
  LCD_string_1=new char[16]; 
  LCD_string_2=new char[16];
                        
  Serial.begin(9600);   
  lcd.begin(16, 2);    
  lcd.clear();



  lcd.createChar(3,arrowLeft);    // LCD symbol left
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  lcd.createChar(1,arrowUp);
    lcd.setCursor(0,0); 
    flashbacklight();
    lcd.print("www.SauleVire.lt"); // advertisement
    flashbacklight();
    lcd.print("www.SauleVire.lt"); // advertisement
    lcd.setCursor(0,1); 
    lcd.print("     v1.21      "); delay(3799);
    flashbacklight();
    lcd.print("www.SauleVire.lt"); // advertisement
    flashbacklight();
 lcd.clear();
   Collector_sensor.begin();Boiler_sensor.begin();
   Thermostat_sensor.begin();
   
  pinMode(Relay_Collector,OUTPUT);pinMode(Relay_Thermostat,OUTPUT);
  digitalWrite(Relay_Collector,HIGH);digitalWrite(Relay_Thermostat,HIGH);
  menuSetup(); 
//  menu.moveUp();      
  Temperature_measurements_1();

 
  LCD_T_template();
//  Temperature_Imaging();
    LCD_switching_on_Time = millis();
    temperature_measurement_time_1 = millis();


  }  // setup() ...************ END **************...
  // ************************ START void loop() *******************************
void loop()    
{
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
if ((x != -1) && (Backlighting == false)){ analogWrite(BackLight_Pin,lcd_backlight*25);
											digitalWrite(13,HIGH); // only for test
                                            Backlighting = true;}
// If the menu is inactive for some time, it returns to the main program
//if ((x != -1) && (InMenu == true))Menu_time_spent_inactive = millis();
//    else {if (millis()- Menu_time_spent_inactive > inactive_menu)
//          menu.toRoot();
//}
  x=Read_keyboard(Key_Pin);delay(30);             // read the state of the keyboard:
  if(Keyboard_change!=x)                               // if there was a change in the state are:
    {
      switch(x)                           // check what was pressed
      {
      case 0: menu.moveRight();break;     // If pressed, move it in the right menu to the right 
      case 1: menu.moveUp();break;        // Menu to top 
      case 2: menu.moveDown();break;      // Menu Down 
      case 3: menu.moveLeft();break;      // Menu to the left 
      case 4: menu.use();break;           // pressed OK, so jump to the function menuUseEvent (MenuUseEvend used)
                                          // This function is just serve our menu, check here
                                          // Which option is selected, and here we create the code to the event handler.
      }
    } Keyboard_change=x;                 //Assign the value of x variable amended so that the long pressing the 
                                         // Same key did not result in the re-generation event. 
                                         // Program responds to a change in the keyboard.
// If you are not currently within the menu is of a continuous program
if (InMenu == false){
  // time interval used for the LCD refresh
  if (millis() > LCD_Update_Time ) { 
  LCD_Update_Time = millis() + LCD_Update_Interval;
  LCD_T_template();
  Temperature_Imaging();

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
void LCD_T_template(){
//  analogWrite(BackLight_Pin,lcd_backlight*25);
  lcd.setCursor(0,0); lcd.print("K"); lcd.setCursor(6,0); lcd.print(" S");
  lcd.setCursor(0,1); lcd.print("B"); lcd.setCursor(6,1); lcd.print(" T");
}

void Temperature_Imaging(){
lcd.setCursor(1,0); lcd.print(K); if (K-B>0) {lcd.setCursor(8,0); lcd.print("+");lcd.print((K-B),1);}
                                             else {lcd.setCursor(8,0); lcd.print((K-B),1);}
                                  if (K-B>=Pump_power_on_difference)  {lcd.setCursor(14,0);lcd.print("K");lcd.write(1);}  
                                  if (K-B<=Pump_power_off_difference) {lcd.setCursor(14,0);lcd.print("K");lcd.write(5);}  
                                             
lcd.setCursor(1,1); lcd.print(B); lcd.setCursor(8,1); lcd.print(T,1);//(int(K + 0.5));
  if (Thermostat_status == 1) 
   {// If the heating mode (Thermostat_status = 1)
    if (T <= temperature_1) {lcd.setCursor(14,1);lcd.print("H");lcd.write(1);}  
    if (T >= temperature_2) {lcd.setCursor(14,1);lcd.print("H");lcd.write(5);}  
   }
   if (Thermostat_status == 2) 
    {// If the freezing mode (Thermostat_status = 2)
     if (T >= temperature_1) {lcd.setCursor(14,1);lcd.print("F");lcd.write(1);}  
     if (T <= temperature_2) {lcd.setCursor(14,1);lcd.print("F");lcd.write(5);}  
    }
    if (Thermostat_status == 3) 
     {lcd.setCursor(13,1);lcd.print("off");}
}

   int MeniuFunkcija (String text_1, int  Converted_Value, int Max_Value, int Min_Value, String text_2)
	        {
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print(text_1); //("Nustatyta=   "); 
        lcd.setCursor(11,1);lcd.print( Converted_Value); // shows the current value
        int  action=-1; delay(1000);         // 
                                           
        while(action!=4)                   // 
         {
           Keyboard_change=-1; 
           action=Read_keyboard(Key_Pin); //delay(300);  
                                            
           if(Keyboard_change!=action)           
             {
             if (action==1) { Converted_Value++; if( Converted_Value>Max_Value)  Converted_Value=Max_Value; lcd.setCursor(11,1);
                                                    if( Converted_Value<10) lcd.print(" ");
                                                      lcd.print( Converted_Value); delay(200);}
             if(action==2)  { Converted_Value--; if( Converted_Value<Min_Value)  Converted_Value=Min_Value; lcd.setCursor(11,1);
                                                     if( Converted_Value<10) lcd.print(" ");
                                                       lcd.print( Converted_Value); delay(200);}
             if(action==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print(text_2); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print("                "); // 0
                 lcd.setCursor(0,0);lcd.print("*");lcd.print(LCD_string_1);           // reconstruct the previous state at LCD
                 lcd.setCursor(15,0);lcd.print("*");
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
