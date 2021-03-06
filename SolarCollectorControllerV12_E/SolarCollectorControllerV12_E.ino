// ============= Solar Controller v1.2 ethernet ===============================================
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
All code is copyright Alvydas, alvydas@saulevire.lt (c)2014.

Features:
Keyboard temperatures are changed the pump on / off
Storing values
Thermostat with cooling or heating function
LCD 16x2
5 keys on the keyboard
network controller ENC28J60 module
*************Here is a real example of acting.****************** 
Write API Key 89972c260bdd5663fab6f82bb934fb7e
Read API Key 226ce7742ae343652f76feb819ad50c8
URL sending data:
http://saulevire.lt/stebesena/input/post.json?json={Boiler:99,Collector:88,Termostat:77}&apikey=89972c260bdd5663fab6f82bb934fb7e
Results:
http://saulevire.lt/stebesena/dashboard/view?id=7&apikey=226ce7742ae343652f76feb819ad50c8

Binary Project size: 27500 bytes (of the 30,720 max)
Free Ram 500 bytes (max 2048)
*/
#include <Wire.h>
 #include "MenuBackend.h"  
 // Thank wojtekizk, for example
 // http://majsterkowo.pl/forum/menubackend-jak-sie-w-nim-odnalezc-t1549.html
  #include <LiquidCrystal.h>         
 #include <OneWire.h>
#include <DallasTemperature.h>
 #include <EEPROM.h>
//#include "definitions.h"
#include <EtherCard.h>
/* ************************************************************************************** */
/* ************************************************************************************** */


#define DEBUGds18b20 // for temperature measurement testing 

/* ********************** Laikai *************************************** */
unsigned long Menu_time_spent_inactive; //Meniu_praleistas_neaktyvus_laikas
#define inactive_menu 20000
unsigned long temperature_measurement_time_1 = 0;
unsigned long Relay_switching_time = 0;
unsigned long temperature_measurement_time_3 = 0;
#define Temperature_measurement_interval_1 5000
#define Relay_switching_interval 5000
#

unsigned long LCD_switching_on_Time;
unsigned long  The_LCD_light_Break = 600000;
unsigned long LCD_Update_Time = 0;
#define LCD_Update_Interval 5000


/* ************** Keyboard variables ************************************* */
#define Key_Pin 0
volatile int Keyboard_change =-1;  // Check or change the keyboard value
volatile int x=-1;                 // 
volatile int stan_Analog;          // 
/* ********** LCD description ******************* */
#define BackLight_Pin 8 //LCD backlight pin (standart LCD KeeyPad use pin 10)
byte lcd_backlight = 10; // lcd_backlight * 25 = MAX LCD backlight
boolean Backlighting = true; // mark on the screen backlight
// The pump on indicator (arrow up)
byte arrow_up[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
// Pump shut-off symbol (arrow in the bottom)
byte arrow_down[8]={ B00100,B00100,B00100,B00100,B00100,B11111,B01110,B00100};

/* ******************** Relay *********************************** */
#define Relay_Collector A1 // Collector
#define Relay_Thermostat A2 // Thermostas
/* ************************** davikliai *********************** */
#define ONE_WIRE_BUS1 2 // Collector
#define ONE_WIRE_BUS2 9 // Boiler
#define ONE_WIRE_BUS3 A3 // Thermostat

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
boolean Conv_start1 = false;
boolean Conv_start2 = false;
boolean Conv_start3 = false;
byte Busy1 = 0;
byte data1[2];
byte Busy2 = 0;
byte data2[2];
byte Busy3 = 0;
byte data3[2];

// variables to store the measured temperature values
float K,B,T; 

byte Pump_power_on_difference = 5;
byte Pump_power_off_difference = 3;
boolean k_uzsalimas = false;
boolean S_C_pump_manual_control = false; // Mark  manual pump control 
/* ********** Thermostat variables ******************* */
byte temperature_1 = 20;
byte temperature_2 = 25;
byte Thermostat_status =1;

/* ************************************************************************************** */

/* ************************************************************************************** */
#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192,168,1,2 };
// gateway ip address
static byte gwip[] = { 192,168,1,254 };
// dns ip address
static byte dnsip[] = { 192,168,1,254 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[250]; // tcp/ip send and receive buffer
unsigned long Ethernet_timer;

char website[] PROGMEM = "saulevire.lt";

// This is the char array that holds the reply data
char line_buf[33];

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

// --- create all of the options menu: ---------------------------------------
// de facto create a MenuItem class objects, which inherit the class MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // menu design 
   //                        ("                ")
   MenuItem P1 =  MenuItem("KOLEKTORIUS   ",1);
      MenuItem P11 = MenuItem("Ijungimo sk. t",2);
      MenuItem P12 = MenuItem("Isjungimo sk.t",2);
      MenuItem P13 = MenuItem("Irasyti nustat",2);
      MenuItem P14 = MenuItem("Nuorinimas    ",2);

   MenuItem P2 = MenuItem("TERMOSTATAS   ",1);
      MenuItem P21 = MenuItem("temperatura 1 ",2);
      MenuItem P22 = MenuItem("temperatura 2 ",2);
      MenuItem P23 = MenuItem("busena        ",2);


/* --- Now position the menu (according to the setting specified above) ------------
add - adds vertical addRight - adds a level to the right, to the left adds addLeft
*/
void menuSetup()                       // feature class MenuBackend 
{
      menu.getRoot().add(P1);          // set the root menu, which is the first option
      P1.add(P11);
        P11.add(P12);P11.addLeft(P1);  //  
        P12.add(P13);P12.addLeft(P1);  // 
        P13.add(P14);P13.addLeft(P1);  // 
        P14.add(P11);P14.addLeft(P1);  // 
        
      menu.getRoot().add(P2);
      P1.addRight(P2);                 //
      
      P2.add(P21);                     // 
        P21.add(P22);P21.addLeft(P2);  // 
        P22.add(P23);P22.addLeft(P2);  // 
        P23.add(P21);P23.addLeft(P2);  // 

      menu.getRoot().add(P1);
      P2.addRight(P1);                 //
      
}
// -----------  -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // feature class MenuBackend - after pressing OK
                                          // Here is the menu we offer for shares of handling the OK button
{
   Serial.print(F("pasirinkta:  ")); Serial.println(used.item.getName()); // test and then unnecessary
// --- Below are some of service options ----------- 
/* __________________________ SETTINGS    _______________________ */
//  ON - the difference between the temperature       
if (used.item.getName() == "Ijungimo sk. t")   // exactly the same string "Ijungimo sk. t"
Pump_power_on_difference =  MeniuFunkcija ("Nustatyta=    ", Pump_power_on_difference, 25, 1, ">Temperatura OK");
     ///////////////////////////////////////////////////////////////////
/* __________________________ SETTINGS _______________________ */ 
// OFF - the difference between the temperature            
if (used.item.getName() == "Isjungimo sk.t")   // exactly the same string "Isjungimo sk.t"
Pump_power_off_difference =  MeniuFunkcija ("Nustatyta=    ", Pump_power_off_difference, 25, 1, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ SETTINGS ____________________________________ */
// Save to EEPROM
     if (used.item.getName() == "Irasyti nustat")   // exactly the same string "Irasyti nustat"
      {
                 SaveConfig();
                 lcd.setCursor(0,0);lcd.print(F(">Irasyta OK        "));delay(2000); // show OK for 2 sec
                 lcd.setCursor(0,0);lcd.print(F("                    ")); // clear line
                 lcd.setCursor(0,0);lcd.print(F("*"));lcd.print(LCD_string_1);           // reconstruct the previous state at LCD
                 lcd.setCursor(15,0);lcd.print(F("*"));
                menu.moveDown();

      }
/* __________________________ Termostat temperature 1   _______________________ */
if (used.item.getName() == "temperatura 1 ")   // exactly the same string "temperatura 1 "
temperature_1 =  MeniuFunkcija ("Nustatyta=    ", temperature_1, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* __________________________ Termostat temperature 2  _______________________ */     
if (used.item.getName() == "temperatura 2 ")   // exactly the same string "temperatura 2 "
temperature_2 =  MeniuFunkcija ("Nustatyta=    ", temperature_2, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ Termostat status  _______________________ */     
if (used.item.getName() == "busena        ") 
 {       
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print(F("Busena-")); 
        if (Thermostat_status == 1) lcd.print(F("sildymas")); // heating
        if (Thermostat_status == 2) lcd.print(F("saldymas")); // freezing
        if (Thermostat_status == 3) lcd.print(F("isjungta")); // turned off
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
                                                 if (Thermostat_status == 1) lcd.print(F("sildymas")); // heating
                                                 if (Thermostat_status == 2) lcd.print(F("saldymas")); // freezing
                                                 if (Thermostat_status == 3) lcd.print(F("isjungta")); // turned off
                                            delay(200);}
             if(action==2)  {Thermostat_status--; if(Thermostat_status<1) Thermostat_status=3; 
                                                 lcd.setCursor(8,1); 
                                              //   lcd.print(Busena(Thermostat_status,termostato_status_name));
                                                 if (Thermostat_status == 1) lcd.print(F("sildymas")); // heating
                                                 if (Thermostat_status == 2) lcd.print(F("saldymas")); // freezing
                                                 if (Thermostat_status == 3) lcd.print(F("isjungta")); // turned off 
                                               delay(200);}
             if(action==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print(F("Busena        OK")); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print(F("                ")); // 0
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
    Serial.println(F("Dabar esame MenuRoot"));
    LCD_T_Template();
    temperature_Imaging();
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
//#ifdef DEBUGds18b20                        
  Serial.begin(9600);   
//#endif
  /* ********************************************************* */
 LoadConfig(); 
  /* ********************************************************* */
  if (ether.begin(sizeof Ethernet::buffer, mymac,10) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
#if STATIC
  ether.staticSetup(myip, gwip, dnsip);
#else
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
#endif
  ether.printIp(F("IP:"), ether.myip);
  ether.printIp(F("GW:"), ether.gwip);  
  ether.printIp(F("DNS:"), ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println(F("DNS failed"));
    
  ether.printIp("SRV: ", ether.hisip);
  /* ********************************************************* */

  pinMode(BackLight_Pin, OUTPUT);
    digitalWrite(BackLight_Pin,HIGH);
  LCD_string_1=new char[20]; 
  LCD_string_2=new char[20];

  lcd.begin(16, 2);    
  lcd.clear();



  lcd.createChar(3,arrowLeft);    // LCD symbol "left"
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  lcd.createChar(1,arrowUp);
    lcd.setCursor(0,0); 
    lcd.print(F("www.SauleVire.lt"));
    lcd.setCursor(0,1); 
    lcd.print(F(" v1.2 ethernet")); delay(2999);
 lcd.clear();
  // K_sensor.begin();B_sensor.begin(); T_sensor.begin();
   
  pinMode(13,OUTPUT);digitalWrite(13,LOW); // only for test
  pinMode(Relay_Collector,OUTPUT);pinMode(Relay_Thermostat,OUTPUT);
  digitalWrite(Relay_Collector,HIGH);digitalWrite(Relay_Thermostat,HIGH);
  menuSetup(); 
//  menu.moveUp();      


  dallas(ONE_WIRE_BUS1);
  dallas(ONE_WIRE_BUS2);
  dallas(ONE_WIRE_BUS3);

 
  LCD_T_Template();
  temperature_Imaging();
    LCD_switching_on_Time = millis();
    temperature_measurement_time_1 = millis();

  }  // setup() ...************ END **************...
  // ************************ START void loop() *******************************
void loop()    
{
  // if the screen, without application of the button illuminates more than the tasks, backlight off
      if (millis()- LCD_switching_on_Time > The_LCD_light_Break) { 
      analogWrite(BackLight_Pin, 0);
      Backlighting = false;
      LCD_switching_on_Time = millis();}
 // When you press any key, the screen backlight is turned on when it is turned off
if ((x != -1) && (Backlighting == false)){ analogWrite(BackLight_Pin,lcd_backlight*25);
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
  LCD_T_Template();
  temperature_Imaging();

  //#ifdef DEBUGds18b20
//Serial.println("Temperate_measurement");
//unsigned long start = millis();
//#endif
  
#ifdef DEBUGds18b20
//unsigned long stop = millis();
//Serial.print("Temperature measurement time: ");  Serial.println(stop - start);
Serial.print(F("K/"));Serial.print(K);Serial.print(F(" B/"));Serial.print(B);Serial.print(F(" T/"));Serial.println(T);
Serial.println(F("----"));
Serial.print(F("Thermostat_status- "));Serial.println(Thermostat_status);
Serial.print(F("temperature_1- "));Serial.print(temperature_1);
Serial.print(F("  temperature_2- "));Serial.println(temperature_2);
Serial.println(F("----"));
Serial.print(F("Pump_power_on_difference- "));Serial.print(Pump_power_on_difference);
Serial.print(F("  Pump_power_off_difference- "));Serial.println(Pump_power_off_difference);

Serial.print(F("millis- "));Serial.println(millis()/1000);
#endif
  }
} 


// measured temperature specified time intervals (Temperature_measurement_interval)
/* +++++++++++++++++++++++++++ First level ++++++++++++++++++++++++++++++++++++ */ 
if (millis() > temperature_measurement_time_1 ) { 
  temperature_measurement_time_1 = millis() + Temperature_measurement_interval_1;
K=    dallas(ONE_WIRE_BUS1);
B=    dallas(ONE_WIRE_BUS2);
T=    dallas(ONE_WIRE_BUS3);
#ifdef DEBUGds18b20
Serial.print(F("K= "));Serial.println(K);
Serial.print(F("B= "));Serial.println(B);
Serial.print(F("T= "));Serial.println(T);

    Serial.println(F("\n[memCheck]"));
    Serial.println(freeRam());
#endif

}
//---------------------------------------------------------------------------------//
ether.packetLoop(ether.packetReceive());
  if ((millis()-Ethernet_timer)>60000) {
    Ethernet_timer = millis();
    Serial.println(F("Request sent"));

char string_C[5];
dtostrf(K, 2, 2, string_C);
char string_B[5];
dtostrf(B, 2, 2, string_B);
char string_T[5];
dtostrf(T, 2, 2, string_T);

Stash stash;
byte K_t = stash.create();
stash.print(string_C);
byte B_t = stash.create();
stash.print(string_B);
byte T_t = stash.create();
stash.print(string_T);
stash.save();

Stash::prepare(PSTR("GET /stebesena/input/post?apikey=89972c260bdd5663fab6f82bb934fb7e&json={Boiler:$H,Collector:$H,Termostat:$H} HTTP/1.0" "\r\n"
    "Host: $F" "\r\n" "\r\n"),
    B_t, K_t, T_t, website, my_callback);
 ether.tcpSend();

}
//------------------ collector pump and thermostat control -----------------------//
if (millis() > Relay_switching_time ) 
 {
   Relay_switching_time=millis()+Relay_switching_interval;
   if (K-B>=Pump_power_on_difference) digitalWrite(Relay_Collector,LOW);
   if (K-B<=Pump_power_off_difference) digitalWrite(Relay_Collector,HIGH);
   
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
     {// If you do not need a second relay-mode off
      digitalWrite(Relay_Thermostat,HIGH);
     }
 }
}// === END ===========================================================



// Scan settings 
boolean LoadConfig(){
  if ((EEPROM.read(0) == 27) && (EEPROM.read(1) == 28) && 
     (EEPROM.read(2) == 13) && (EEPROM.read(3) == 18)) {

    if (EEPROM.read(4) == EEPROM.read(5)) Pump_power_on_difference = EEPROM.read(4);  
    if (EEPROM.read(6) == EEPROM.read(7)) Pump_power_off_difference = EEPROM.read(6);
    if (EEPROM.read(8) == EEPROM.read(9)) temperature_1 = EEPROM.read(8);  
    if (EEPROM.read(10) == EEPROM.read(11)) temperature_2 = EEPROM.read(10);
    if (EEPROM.read(12) == EEPROM.read(13)) Thermostat_status = EEPROM.read(12);
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

}
/*----------------------------------------------------------------------------*/
int get_reply_data(word off)
{
  memset(line_buf,NULL,sizeof(line_buf));
  if (off != 0)
  {
    uint16_t pos = off;
    int line_num = 0;
    int line_pos = 0;
    
    // Skip over header until data part is found
    while (Ethernet::buffer[pos]) {
      if (Ethernet::buffer[pos-1]=='\n' && Ethernet::buffer[pos]=='\r') break;
      pos++; 
    }
    pos+=2;
    while (Ethernet::buffer[pos])
    {
      if (line_pos<49) {line_buf[line_pos] = Ethernet::buffer[pos]; line_pos++;} else break;
      pos++; 
    }
    line_buf[line_pos] = '\0';
    return line_pos;
  }
  return 0;
}

static void my_callback (byte status, word off, word len) {

  // get_reply_data gets the data part of the reply and puts it in the line_buf char array
  int lsize =   get_reply_data(off);
  
  Serial.print("Feed value: ");

  int value = atoi(line_buf);
  Serial.println(value);
  
  
}
/* ********************************************************* */
void LCD_T_Template(){
  lcd.setCursor(0,0); lcd.print("K"); lcd.setCursor(6,0); lcd.print(" S");
  lcd.setCursor(0,1); lcd.print("B"); lcd.setCursor(6,1); lcd.print(" T");
}

void temperature_Imaging(){
lcd.setCursor(1,0); lcd.print(K); if (K-B>0) {lcd.setCursor(8,0); lcd.print(" ");lcd.print(K-B);}
                                             else {lcd.setCursor(8,0); lcd.print(K-B);}
lcd.setCursor(1,1); lcd.print(B); lcd.setCursor(8,1); lcd.print(T);//(int(K + 0.5));
}
/* ********************************************************* */
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
/* ************************************************************************** */
float dallas(int x)
{
OneWire ds(x);
  //returns the temperature from one DS18S20 in DEG Celsius
  byte i;
  byte data[12];
  byte addr[8];
  float result;
  if ( !ds.search(addr)) 
  {
      ds.reset_search();
      return -99; // no more sensors on chain, reset search
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) 
  {
      return -88; //CRC is not valid!
  }

  if ( addr[0] != 0x28) 
  {
     return -77; // Device is not recognized
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion
//  delay(850); // Wait some time...
  
  ds.reset();   // byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad command

  for (int i = 0; i < 9; i++)  // Receive 9 bytes
  { 
    data[i] = ds.read();
  }
  
 // ds.reset_search();
  
  // memory consuming but readable code:
/*  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;  // 1/16 = 0.0625
  return TemperatureSum;
  
    
  return ( (data[1] << 8) + data[0] )*0.0625 ;
 */
   unsigned int raw = (data[1] << 8) | data[0];
       byte cfg = (data[4] & 0x60);
       return raw / 16.0;
}
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
