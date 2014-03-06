// ============= KKK valdinlis v2.1 ===============================================
#include <EtherCard.h>

#include <Wire.h>
 #include <MenuBackend.h>        
  #include <LiquidCrystal.h>         
 #include <OneWire.h>
#include <DallasTemperature.h>
 #include <EEPROM.h>
#include "definitions.h"

////////////////// ksduino start ////////////////////////
#define BUFFER_SIZE 200
byte Ethernet::buffer [BUFFER_SIZE];  

#include <KSduino.h>

// User definition -----------------------------------------------------

// Change this values to yours

// Your Arduinos ID & Password
unsigned int deviceID = 0000;
unsigned int devicePwd = 0000;

// Your Arduinos MAC, IP & port used in your local network
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D };
byte ip[] = { 192, 168, 0, 222 };
byte dns[] = { 192, 168, 0, 1 };
byte gateway [] = { 192, 168, 0, 1 };
unsigned int port = 58833;

// Server definition -----------------------------------------------------

// Do not change this values

// KSduino Server address & port
byte serverIp[] = { 178,63,53,233 };
unsigned int serverPort = 9930;

// ----------------------------------------------------------------------------

// Create KSduino class
KSduino ksd (deviceID, devicePwd, serverIp, serverPort);

////////////////// ksduino stop /////////////////////////

  // --- definiujemy dla LCD w�asne znaki strza�ek: d��, lewo, prawo, gora-d�� i powr�t ---
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
uint8_t arrowUp[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
    // definicja pin�w dla LCD (sprawd� piny w swoim LCD)
LiquidCrystal lcd(A5, 3, 4, 5, 6, 7);
/* ------------------ R T C ---------------------- */


/* --------------------- RTC PABAIGA ---------------- */
char *eilute1;                      // pierwsza eilute wy�wietlanego tekstu na LCD
char *eilute2;                      // druga eilute wy�wietlanego tekstu na LCD
    boolean InMenu = false;

// --- tworzymy wszystkie opcje Menu: ---------------------------------------
// de facto tworzymy obiekty klasy MenuItem, kt�re dziedzicz� po klasie MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // konstruktor 
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


/* --- Teraz pozycjonujemy  menu ( zgodnie z ustawieniem podanym powy�ej) ------------
add - dodaje w pionie, addRight - dodaje w poziomie z prawej , addLeft dodaje z lewej
*/
void menuSetup()                       // funkcja klasy MenuBackend 
{
      menu.getRoot().add(P1);          // 
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
// ----------- uff... nareszcie :-) -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // funkcja klasy MenuBackend - reakcja na wci�ni�cie OK
                                          // tutaj w�a�nie oddajemy menu na rzecz akcji obs�ugi klawisza OK
{
//   Serial.print("pasirinkta:  "); Serial.println(used.item.getName()); // do test�w, potem niepotrzebne
   // --- ponizej kilka przyk�ad�w obs�ugi  opcji -----------
   // przyk�adowa reakcja na wcisni�cie klawisza OK w opcji Otworz :
/* __________________________ NUSTATYMAI Ijungimo skirtumo temperatura          _______________________ */
if (used.item.getName() == "Ijungimo sk. t")   // dokladnie taki sam ciag " Temperatura"
k_ijungimo_skirtumas =  MeniuFunkcija ("Nustatyta=    ", k_ijungimo_skirtumas, 25, 1, ">Temperatura OK");
     ///////////////////////////////////////////////////////////////////
/* __________________________ NUSTATYMAI Isjungimo skirtumo temperatura          _______________________ */     
if (used.item.getName() == "Isjungimo sk.t")   // dokladnie taki sam ciag " Temperatura"
k_isjungimo_skirtumas =  MeniuFunkcija ("Nustatyta=    ", k_isjungimo_skirtumas, 25, 1, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ NUSTATYMAI Irasymas____________________________________ */
     if (used.item.getName() == "Irasyti nustat")   // dok�adnie taki sam ci�g " Temperatura"
      {
                 SaveConfig();
                 lcd.setCursor(0,0);lcd.print(">Irasyta OK        ");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(0,0);lcd.print("                    "); // czy�cimy lini�
                 lcd.setCursor(0,0);lcd.print("*");lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
                 lcd.setCursor(15,0);lcd.print("*");
                menu.moveDown();

      }
/* __________________________ Termostatas temperatura 1   _______________________ */
if (used.item.getName() == "temperatura 1 ")   // dokladnie taki sam ciag " Temperatura"
temperatura_1 =  MeniuFunkcija ("Nustatyta=    ", temperatura_1, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* __________________________ Termostatas temperatura 2  _______________________ */     
if (used.item.getName() == "temperatura 2 ")   // dokladnie taki sam ciag " Temperatura"
temperatura_2 =  MeniuFunkcija ("Nustatyta=    ", temperatura_2, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ Termostatas busena  _______________________ */     
if (used.item.getName() == "busena        ") 
 {       
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print("Busena-"); 
        if (T_busena == 1) lcd.print("sildymas");
        if (T_busena == 2) lcd.print("saldymas");
        if (T_busena == 3) lcd.print("isjungta");
//      lcd.print(Busena(T_busena,termostato_busenos_pavadinimas));
        int  veiksmas=-1; delay(1000);         // 
                                           
        while(veiksmas!=4)                   // 
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);  
                                            
           if(klaviaturos_pasikeitimas!=veiksmas)           
             {
             if (veiksmas==1) {T_busena++; if(T_busena>3) T_busena=1; 
                                                 lcd.setCursor(8,1); 
                                               //  lcd.print(Busena(T_busena,termostato_busenos_pavadinimas));
                                                 if (T_busena == 1) lcd.print("sildymas");
                                                 if (T_busena == 2) lcd.print("saldymas");
                                                 if (T_busena == 3) lcd.print("isjungta"); 
                                            delay(200);}
             if(veiksmas==2)  {T_busena--; if(T_busena<1) T_busena=3; 
                                                 lcd.setCursor(8,1); 
                                              //   lcd.print(Busena(T_busena,termostato_busenos_pavadinimas));
                                                 if (T_busena == 1) lcd.print("sildymas");
                                                 if (T_busena == 2) lcd.print("saldymas");
                                                 if (T_busena == 3) lcd.print("isjungta"); 
                                               delay(200);}
             if(veiksmas==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print("Busena        OK"); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print("                "); // 0
                 lcd.setCursor(1,0);lcd.print(eilute1);           // 0
                 menu.moveDown();
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;
 }
 
}
// --- Reakcja na wci�ni�cie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  if(changed.to.getName()==menu.getRoot())
  {
    InMenu =false;
//    Serial.println("Dabar esame MenuRoot");
    LCD_T_sablonas();
    Temperaturu_vaizdavimas();
  }
  /* tak naprawd� to tylko tutaj przydaje si� �w shortkey i s�u�y przede wszystkim do wzbogacenia menu
     o symbole strza�ek w zale�no�ci co wybrano. Wszystko co tutaj si� wyprawia jest pokazywane na LCD. 
  */
  int c=changed.to.getShortkey();                         // pobieramy shortkey (1,2,3, lub4)
  lcd.clear();                                            // bez komentarza 
  lcd.setCursor(0,0); 
  if(c==1)                                                // jeśli to menu głowne (shortkey=1) to:
    {InMenu =true;
    lcd.write(3);                                         // strzałka w lewo
    strcpy(eilute1,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(eilute1);                                    // wyświetlamy ją
    lcd.setCursor(15,0);lcd.write(4);                     // strzałka w prawo
    lcd.setCursor(0,1);lcd.write(5);                      // strzałka w dół
    lcd.setCursor(15,1);lcd.write(5);                     // strzałka w dół
    }
    if(c==2)                                              // jeśli to podmenu dla dziecka - (shortkey=2) to:
    {InMenu =true;
    lcd.print("*");                                       // rysujemy gwiazdkę
    strcpy(eilute2,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(eilute1);                                    // wyświetlamy ją
    lcd.setCursor(15,0);lcd.print("*");                   // gwiazdka 
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka powrotu (arrowBack)
    lcd.print(changed.to.getName());                      // wyświetlamy nazwe "dziecka"
    lcd.setCursor(15,1);lcd.write(7);                     // strzałka góra-dół
    }
    if(c==3)                                              // jeśli dziecko  ma dziecko - (shortkey =3) to:
    {InMenu =true;
    lcd.print("*");                                       // gwiazdka
    strcpy(eilute2,changed.to.getName());                  // kopiujemy akt. nazwe opcji menu do zmiennej linia2
    lcd.print(eilute1);                                    // i wyświetlamy pierwszą linię
    lcd.setCursor(15,0);lcd.print("*");                   // gwiazdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka w drugiej linii
    lcd.setCursor(15,1);lcd.write(4);                     // strzałka w prawo bo są wnuki
    }
    
    if(c==4)                                              // jeśli to wnuk  (shortkey =4) to:
    {InMenu =true;
    lcd.print("*");                                       // gwaizdka
    lcd.print(eilute2);                                    // w pierwszej linii wyświetlamy dziecko ( czyli rodzica wnuka) 
    lcd.setCursor(15,0);lcd.print("*");                   // gwaizdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka
    lcd.setCursor(15,1);lcd.write(7);                     // strzałka góra-dół 
    } 
}

// --- analogin� 5 mygtuk� klaviat�ros nuskaitymo versija, DFRobot --------------------------------------
volatile int Klaviaturos_skaitymas(int analog)
{
   int stan_Analog = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (stan_Analog > 1000) return -1; // riba
   if (stan_Analog < 50)   return 3;  // � de�in� 
   if (stan_Analog < 200)  return 1;  // Vir�un 
   if (stan_Analog < 400)  return 2;  // �emyn� 
   if (stan_Analog < 600)  return 0;  // � kair�  
   if (stan_Analog < 800)  return 4;  // OK 
   return -1;                         // Nepaspaustas
}
// ============================================================================================
// 
void setup()
{
 LoadConfig(); 

  /* ********************************************************* */

  pinMode(BackLight_Pin, OUTPUT);
    digitalWrite(BackLight_Pin,HIGH);
  eilute1=new char[20]; 
  eilute2=new char[20];
                        
  Serial.begin(9600);   
  lcd.begin(16, 2);    
  lcd.clear();



  lcd.createChar(3,arrowLeft);    // LCD �enklas kair�n
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  lcd.createChar(1,arrowUp);
    lcd.setCursor(0,0); 
    lcd.print("www.SauleVire.lt");
    lcd.setCursor(0,1); 
    lcd.print("      v1.2"); delay(2999);
 lcd.clear();
//   K_sensor.begin();B_sensor.begin(); T_sensor.begin();
   
  pinMode(13,OUTPUT);digitalWrite(13,LOW); // tik testas 
  pinMode(Rele_K,OUTPUT);pinMode(Rele_T,OUTPUT);
  digitalWrite(Rele_K,HIGH);digitalWrite(Rele_T,HIGH);
  menuSetup(); 
//  menu.moveUp();      
//  Temperaturu_matavimas_1();

 
  LCD_T_sablonas();
  Temperaturu_vaizdavimas();
    Ekrano_pasvietimo_ijungimo_laikas = millis();
    temperaturu_matavimo_laikas_1 = millis();
  // Start KSduino at the beginning of setup
  ksd.begin(mac,  ip, dns, gateway, port);

  }  // setup() ...************ PABAIGA **************...
  // ************************ PROGRAMOS PRADZIA void loop() *******************************
void loop()    
{
  // jei ekranas, nespaudant mygtuk�, �vie�ia ilgiau negu u�duota, pa�vietimas i�jungiamas
      if (millis()- Ekrano_pasvietimo_ijungimo_laikas > Ekrano_pasvietimo_pertrauka) { 
      analogWrite(BackLight_Pin, 0);
      Backlighting = false;
      Ekrano_pasvietimo_ijungimo_laikas = millis();}
 // Paspaudus bet kok� klavi�� �jungiamas ekrano pa�vietimas, kai jis b�na i�jungtas  
if ((x != -1) && (Backlighting == false)){ analogWrite(BackLight_Pin,lcd_pasvietimas*25);
                                            Backlighting = true;}
// Jei meniu b�na neaktyvus ka�kiek laiko, tai gr��tama � pagrindin� program�
//if ((x != -1) && (InMenu == true))Meniu_praleistas_neaktyvus_laikas = millis();
//    else {if (millis()- Meniu_praleistas_neaktyvus_laikas > neaktyvus_meniu)
//          menu.toRoot();
//}
  x=Klaviaturos_skaitymas(Key_Pin);delay(30);             // odczytujemy stan klawiatury:
  if(klaviaturos_pasikeitimas!=x)                               // jesli by�a klaviaturos_pasikeitimasiana stanu to :
    {
      switch(x)                           // sprawdzamy co nacisnieto 
      {
      case 0: menu.moveRight();break;     // jesli naci�ni�to klawisz w Prawo to przesu� menu w prawo
      case 1: menu.moveUp();break;        // menu do g�ry
      case 2: menu.moveDown();break;      // menu w d��
      case 3: menu.moveLeft();break;      // menu w lewo
      case 4: menu.use();break;           // wci�ni�to OK wi�c skok do funkcji menuUseEvent(MenuUseEvend used)
                                          // to w tej funkcji w�a�nie obs�ugujemy nasze Menu, tu sprawdzamy
                                          // jak� opcj� wybrano i tutaj tworzymy kod do obslugi zdarzenia.
      }
    } klaviaturos_pasikeitimas=x;                               // przypisanie klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas warto�ci x po to, aby dlu�sze wciskanie tego
                                          // samego klawisza nie powodowa�o ponownej generacji zdarzenia. 
                                          // program reaguje na klaviaturos_pasikeitimasian� stanu klawiatury. 
// Jeigu dabar neesame meniu viduje, tai vykdoma nuolatin� programa
if (InMenu == false){
  // laiko intervalas naudojamas LCD vaizdui atnaujinti
  if (millis() > Ekrano_rodmenu_atnaujinimo_laikas ) { 
  Ekrano_rodmenu_atnaujinimo_laikas = millis() + Ekrano_rodmenu_atnaujinimo_pertrauka;
  LCD_T_sablonas();
  Temperaturu_vaizdavimas();

  //#ifdef DEBUGds18b20
//Serial.println("Temperaturu_matavimas");
//unsigned long start = millis();
//#endif
  
#ifdef DEBUGds18b20
//unsigned long stop = millis();
//Serial.print("Temperaturu matavimo laikas: ");  Serial.println(stop - start);
Serial.print("K/ ");Serial.print(K);Serial.print(" B/ ");Serial.print(B);Serial.print(" T/ ");Serial.println(T);
Serial.println("----");
Serial.print("T_busena- ");Serial.println(T_busena);
Serial.print("temperatura_1- ");Serial.print(temperatura_1);
Serial.print("  temperatura_2- ");Serial.println(temperatura_2);
Serial.println("----");
Serial.print("k_ijungimo_skirtumas- ");Serial.print(k_ijungimo_skirtumas);
Serial.print("  k_isjungimo_skirtumas- ");Serial.println(k_isjungimo_skirtumas);

Serial.print("millis- ");Serial.println(millis()/1000);
#endif
  }
} 


// matuojamos temperat�ros nurodytais laiko intervalais (temperaturu_matavimo_pertrauka)
/* +++++++++++++++++++++++++++ PIRMAS LYGIS ++++++++++++++++++++++++++++++++++++ */ 
if (millis() > temperaturu_matavimo_laikas_1 ) { 
  temperaturu_matavimo_laikas_1 = millis() + temperaturu_matavimo_pertrauka_1;
  Temperaturu_matavimas_1();
}
//------------------ kolektoriaus siurblio ir termostato valdymas-----------------------//
if (millis() > Reliu_junginejimo_laikas ) 
 {
   Reliu_junginejimo_laikas=millis()+Reliu_junginejimo_pertrauka;
   if (K-B>=k_ijungimo_skirtumas) digitalWrite(Rele_K,LOW);
   if (K-B<=k_isjungimo_skirtumas) digitalWrite(Rele_K,HIGH);
   
  if (T_busena == 1) 
   {//Jei šildymo režimas (T_busena = 1)
    if (T <= temperatura_1) digitalWrite(Rele_T,LOW);
    if (T >= temperatura_2) digitalWrite(Rele_T,HIGH);
   }
   if (T_busena == 2) 
    {//Jei šaldymo režimas (T_busena = 2)
     if (T >= temperatura_1) digitalWrite(Rele_T,LOW);
     if (T <= temperatura_2) digitalWrite(Rele_T,HIGH);
    }
    if (T_busena == 3) 
     {//Jei antra rele nereikalinga- režimas išjungta
      digitalWrite(Rele_T,HIGH);
     }
 }
 
 
  static byte d_value = LOW;
  char buffer[32];

  // Set value to D1 variable  
  if (d_value == LOW) d_value = HIGH; else  d_value = LOW;      
  
  // Create text buffer with parameter d1
  sprintf (buffer, "d1=%d", d_value);
  
  // Send buffer with parameters & values to KSduino server
  ksd.sendBuffer (buffer);  
  
  // Do delay
  delay (1000);
  
  // KSduino update should be at the end of loop
  ksd.update ();  
/* //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if ( !K_t.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    K_t.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
//    case 0x10:
//      Serial.println("  Chip = DS18S20");  // or old DS1820
//      type_s = 1;
//      break;
    case 0x28:
//      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
//    case 0x22:
//      Serial.println("  Chip = DS1822");
//      type_s = 0;
//      break;
    default:
//      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  K_t.reset();
  K_t.select(addr);
  K_t.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = K_t.reset();
  K_t.select(addr);    
  K_t.write(0xBE);         // Read Scratchpad

//  Serial.print("  Data = ");
//  Serial.print(present,HEX);
//  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = K_t.read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
//  Serial.print(" CRC=");
//  Serial.print(OneWire::crc8(data, 8), HEX);
//  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
//  fahrenheit = celsius * 1.8 + 32.0;
//  Serial.print("  Temperature = ");
//  Serial.print(celsius);
//  Serial.print(" Celsius, ");

 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
}// === PABAIGA ===========================================================
