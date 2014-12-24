//MENU SETUP
#include <MenuBackend.h>
//initialize menu
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);
//initialize menuitems
MenuItem exit1 = MenuItem(menu,"EXIT-1",1);
MenuItem configure = MenuItem(menu,"CONFIGURE",1);
  MenuItem config_heater = MenuItem(menu,"CONF HEAT",2); 
   MenuItem heater_on = MenuItem(menu, "HEAT ON TEMP",3);
   MenuItem heater_off = MenuItem(menu, "HEAT OFF TEMP",3);
   MenuItem heater_disable = MenuItem(menu,"HEAT DISABLE",3);
   MenuItem exit3 = MenuItem(menu,"EXIT-3",3);
  MenuItem config_fan = MenuItem(menu,"CONF FANS",2); 
   MenuItem fan_on = MenuItem(menu, "FAN ON TEMP",3);
   MenuItem fan_off = MenuItem(menu, "FAN OFF TEMP",3);
   MenuItem fan_on_rh = MenuItem(menu, "FAN ON RH",3);
   MenuItem fan_off_rh = MenuItem(menu, "FAN OFF RH",3);   
   MenuItem fan_disable = MenuItem(menu,"FAN DISABLE",3);
   MenuItem exit4 = MenuItem(menu,"EXIT-4",3);
  MenuItem config_mist = MenuItem(menu,"CONF MIST",2); 
   MenuItem mist_on = MenuItem(menu, "MIST ON RH",3);
   MenuItem mist_off = MenuItem(menu, "MIST OFF RH",2);
   MenuItem mist_disable = MenuItem(menu,"MIST DISABLE",3);
   MenuItem exit5 = MenuItem(menu,"EXIT-5",3);   
  MenuItem exit2 = MenuItem(menu,"EXIT-2",2);

unsigned long menuTimer = millis();              
byte timerEnable = 1;
//END MENU  

/*LCD*/
#include <LiquidCrystal.h>
LiquidCrystal lcd(A5, 3, 4, 5, 6, 7); //Data,Clk,Enable Assuming that the header is connected to the same digital IO pin.
/* ********** LCD description ******************* */
byte lcd_backlight = 10; // lcd_backlight * 25 = MAX LCD backlight
//byte LCD_brightness = 10; // lcd_pasvietimas * 25 = MAX LCD apsviestumas
boolean Backlighting = true; // mark on the screen backlight
#define BackLight_Pin 9 //LCD backlight pin (standart LCD KeeyPad use pin 10)
//END LCD

#define Key_Pin A7    // analog pin assigned for button reading

//INTERRUPT BUTTONS
int pin = 13;
int buttonPin = A7;
volatile byte buttonAct = 0;
volatile int buttonValue = 0;
//volatile int buttonPressed = 0;
volatile int lastButtonPressed = 0;
volatile int state = LOW;
volatile int lastState = HIGH;

//DEBUG
#include <MemoryFree.h> 
int tempMemCount = 0;
//

/* ************** Keyboard variables ************************************* */
volatile int Keyboard_change =-1;  // Check or change the keyboard value
volatile int buttonPressed=-1;                 // 
volatile int stan_Analog;          // 

void setup()
{
  pinMode(BackLight_Pin, OUTPUT);
  digitalWrite(BackLight_Pin,HIGH);
  analogWrite(BackLight_Pin,lcd_backlight*25);  
  //MENU SETUP
  //configure menu
  menu.getRoot().add(configure).addAfter(exit1).addAfter(configure); 
  configure.addRight(exit2).addAfter(config_heater).addAfter(config_fan).addAfter(config_mist).addAfter(exit2);
  config_heater.addRight(exit3).addAfter(heater_on).addAfter(heater_off).addAfter(heater_disable).addAfter(exit3);
config_fan.addRight(exit4).addAfter(fan_on).addAfter(fan_off).addAfter(fan_on_rh).addAfter(fan_off_rh).addAfter(fan_disable).addAfter(exit4);
config_mist.addRight(exit5).addAfter(mist_on).addAfter(mist_off).addAfter(mist_disable).addAfter(exit5);


  //END MENU SETUP
  Serial.begin(9600);
  Serial.println("STARTED");
  pinMode(pin, OUTPUT);
  

  lcd.begin(16,2);               // initialize the lcd
  delay(400); // wait to start lcd writing
  lcd.home ();                   // go home
  lcd.print("Button Interrupt");

}
void buttonStat(){ 
  Serial.print(buttonPressed); 
  Serial.print(" ");
  Serial.println(lastButtonPressed);
}
void loop(){
                                         // Same key did not result in the re-generation event. 
                                         // Program responds to a change in the keyboard.
  if(timerEnable == 1 && millis() - menuTimer >=9000){
     Serial.print("freeMemory() reports "); Serial.println( getFreeMemory() );
    Serial.println("Timer Clear.........");
     delay(300);
//  [color=yellow] // menu.moveToLevel(0);
    
    while(menu.getCurrent().getLevel() > 1){
       if(menu.getCurrent().getLeft() != 0){
         menu.moveLeft();
         Serial.println("  L  ");
       }else{
         menu.moveUp();
         Serial.println("  U  ");
       }  
    }//[/color]
    
    //  menu.moveRelativeLevels(-1);
    timerEnable = 0;
    lcd.clear();                   // go home
    lcd.print("Button Interrupt");
    delay(60);
    buttonAct = 0;
  }
  buttonPressed=Read_keyboard(Key_Pin);delay(30);       // read the state of the keyboard:
  if(Keyboard_change!=buttonPressed)     {              // if there was a change in the state are:
  navMenu();
  }
Keyboard_change=buttonPressed;                 //Assign the value of x variable amended so that the long pressing the 

  delay(100);
  tempMemCount++;
  if(tempMemCount > 29){ //once a sec will do
   Serial.print("freeMemory() reports ");    Serial.println( getFreeMemory() );
    tempMemCount = 0;  
  }
}
//////////////////////////////////
//----------------------------------------------------
// --- 5 analog buttons keyboard scan version DFRobot --------------------------------------
volatile int Read_keyboard(int analog)
{
//   state = !state;
//  digitalWrite(pin, state);
   int buttonValue = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (buttonValue > 1000) return buttonPressed=-1; // limit
   if (buttonValue < 50)   return buttonPressed=3;  // right
   if (buttonValue < 200)  return buttonPressed=1;  // up
   if (buttonValue < 400)  return buttonPressed=2;  // down
   if (buttonValue < 600)  return buttonPressed=0;  // left
   if (buttonValue < 800)  return buttonPressed=4;  // OK 
   return buttonPressed=-1;                         // Not pressed
}
//----------------------------------------------------
/////////////////////////////////////////
void navMenu(){
  //MenuItem currentMenu=menu.getCurrent();
  switch (buttonPressed){
  case 4: //select     buvo 3
  //The current item has an element right, it's a sub menu so nav right.
    if (menu.getCurrent().getRight() != 0){  menu.moveRight();
                  Serial.print(menu.getCurrent().getName()); Serial.println("has menu right");}
    else{  //otherwise, menu has no child and has been pressed. enter the current menu
          menu.use();} break;     //select    
  case 3: menu.moveDown(); break; //down
  case 0: menu.moveUp();break;    //up
  }
}
////////////////////////////////////////////

/*
  This is an important function
 Here we get a notification whenever the user changes the menu
 That is, when the menu is navigated
 */
void menuChangeEvent(MenuChangeEvent changed){
  menuTimer = millis(); 
  timerEnable = 1;


  lcd.clear();                   // go home
  lcd.setCursor ( 0, 0 );        // go to the next line

  lcd.print("1)");
  // lcd.print(menu.getCurrent().getName());
  lcd.print(changed.from.getName());
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print("2)");    
  lcd.print(changed.to.getName());

  Serial.print("Menu change FROM:TO ");
  Serial.print(changed.from.getName());
  Serial.print(":");
  Serial.println(changed.to.getName());
}
////////////////////////////////////////

/*This is where you define a behaviour for a menu item
 */
void menuUseEvent(MenuUseEvent used){
  Serial.print("Menu use: ");
  Serial.println(used.item.getName());
  lcd.clear();                   // go home
  lcd.print("USED: "); 
  lcd.print(used.item.getName());

  if (used.item.getName() == "EXIT"){
    Serial.println("exit call");
         menu.moveLeft();  
  }
}

