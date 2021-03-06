// ============= KKK valdinlis v2.1 ===============================================

#include <Wire.h>
 #include <MenuBackend.h>        
  #include <LiquidCrystal.h>         
 #include <OneWire.h>
 #include <EEPROM.h>
#include "definitions.h"
#include <EtherCard.h>
int plokste=10;
#define DEBUGds18b20 // for temperature measurement testing 
OneWire  ds(2);  // on pin 2
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

byte Ethernet::buffer[350]; // tcp/ip send and receive buffer

unsigned long Ethernet_timer;
//unsigned long Ethernet_timer1;

char website[] PROGMEM = "saulevire.lt";
//char website1[] PROGMEM = "emoncms.org";
boolean dns_error = false;  
class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    const char* buffer() { return buf; }
    byte length() { return fill; }
    void reset()
    { 
      memset(buf,NULL,sizeof(buf));
      fill = 0; 
    }
    virtual size_t write (uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    byte fill;
    char buf[150];
    private:
};
PacketBuffer str;  

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
   Serial.print(F("pasirinkta:  ")); Serial.println(used.item.getName()); // do test�w, potem niepotrzebne
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
    Serial.println(F("Dabar esame MenuRoot"));
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
#ifdef DEBUGds18b20                        
  Serial.begin(9600);   
    Serial.println(F("\n[memCheck]"));
		Serial.println(freeRam());
#endif
  lcd.begin(16, 2);    
  lcd.clear();
    pinMode(BackLight_Pin, OUTPUT);
    digitalWrite(BackLight_Pin,HIGH);
    lcd.setCursor(0,0); 
    lcd.print("www.SauleVire.lt");
    lcd.setCursor(0,1); 
    lcd.print(" v1.2 ethernet"); delay(1999);

 /* ********************************************************* */
 LoadConfig(); 
  /* ********************************************************* */
  if (ether.begin(sizeof Ethernet::buffer, mymac,plokste) == 0) {
    Serial.println(F("Failed to access Ethernet controller"));
//    ethernet_error = 1; 
}
#if STATIC
  ether.staticSetup(myip, gwip, dnsip);
#else
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
    
#endif
  ether.printIp(F("IP:"), ether.myip);
  ether.printIp(F("GW:"), ether.gwip);  
  ether.printIp(F("DNS:"), ether.dnsip);  

  if (!ether.dnsLookup(website)){
    Serial.println(F("DNS failed"));
    dns_error=true;}
    
  ether.printIp(F("SRV:"), ether.hisip);
//  if (!ether.dnsLookup(website1)){
//    Serial.println(F("DNS failed"));
//    dns_error=true;}
    
//  ether.printIp(F("SRV1:"), ether.hisip);
   /* ********************************************************* */


  eilute1=new char[20]; 
  eilute2=new char[20];

  lcd.createChar(3,arrowLeft);    // LCD �enklas kair�n
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  lcd.createChar(1,arrowUp);

 lcd.clear();
  // K_sensor.begin();B_sensor.begin(); T_sensor.begin();
   
  pinMode(13,OUTPUT);digitalWrite(13,LOW); // tik testas 
  pinMode(Rele_K,OUTPUT);pinMode(Rele_T,OUTPUT);
  digitalWrite(Rele_K,HIGH);digitalWrite(Rele_T,HIGH);
  menuSetup(); 
//  menu.moveUp();      


//  dallas(ONE_WIRE_BUS1);// jei davikliai ant atskiru isvadu
//  dallas(ONE_WIRE_BUS2);// jei davikliai ant atskiru isvadu
//  dallas(ONE_WIRE_BUS3);// jei davikliai ant atskiru isvadu
K=Dallas(sensorK);
B=Dallas(sensorB);
T=Dallas(sensorT);
 
  LCD_T_sablonas();
  Temperaturu_vaizdavimas();
    Ekrano_pasvietimo_ijungimo_laikas = millis();
    temperaturu_matavimo_laikas_1 = millis();


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
  
#ifdef DEBUGds18b20
//unsigned long stop = millis();
//Serial.print("Temperaturu matavimo laikas: ");  Serial.println(stop - start);
Serial.print(F("K/ "));Serial.print(K);Serial.print(F(" B-"));Serial.print(B);Serial.print(F(" T-"));Serial.println(T);
Serial.print(F("T_busena- "));Serial.println(T_busena);
Serial.print(F("temperatura_1- "));Serial.println(temperatura_1);
Serial.print(F("temperatura_2- "));Serial.println(temperatura_2);

Serial.print(F("k_ijungimo_skirtumas- "));Serial.print(k_ijungimo_skirtumas);
Serial.print(F("  k_isjungimo_skirtumas- "));Serial.println(k_isjungimo_skirtumas);

Serial.print(F("millis- "));Serial.println(millis()/1000);
#endif
  }
} 


// matuojamos temperat�ros nurodytais laiko intervalais (temperaturu_matavimo_pertrauka)
/* +++++++++++++++++++++++++++ PIRMAS LYGIS ++++++++++++++++++++++++++++++++++++ */ 
if (millis() > temperaturu_matavimo_laikas_1 ) { 
  temperaturu_matavimo_laikas_1 = millis() + temperaturu_matavimo_pertrauka_1;
//K=    dallas(ONE_WIRE_BUS1);// jei davikliai ant atskiru isvadu
//B=    dallas(ONE_WIRE_BUS2);// jei davikliai ant atskiru isvadu
//T=    dallas(ONE_WIRE_BUS3);// jei davikliai ant atskiru isvadu
K=Dallas(sensorK);
B=Dallas(sensorB);
T=Dallas(sensorT);

#ifdef DEBUGds18b20
Serial.print(F("K="));Serial.println(K);
Serial.print(F("B="));Serial.println(B);
Serial.print(F("T="));Serial.println(T);
                Serial.println(F("\n[memCheck]"));
		Serial.println(freeRam());

#endif

}
     

ether.packetLoop(ether.packetReceive());
  if ((millis()-Ethernet_timer)>5000) {
    Ethernet_timer = millis();
  str.reset();
  //  str.print(basedir); 
  str.print(F("/stebesena/input/post?"));
  str.print(F("apikey=zemaicio-kolektorius_a91373db012b"));
  // str.print(apikey);
  str.print(F("&json={Z-boileris:"));  str.print(K);
  str.print(F(",Z-kolektorius:"));  str.print(B);
  str.print(F(",Z-termostatas:"));  str.print(T);
  str.print(F("}\0"));
  Serial.println(F("Request sent"));
  Serial.println(str.buf);
  ether.browseUrl(PSTR(""),str.buf, website, my_result_cb);

/*
Stash::prepare(PSTR("GET /stebesena/input/post?apikey=zemaicio-kolektorius_a91373db012b&json={Z-boileris:$H,Z-kolektorius:$H} HTTP/1.0" "\r\n"
//Stash::prepare(PSTR("GET /input/post?apikey=5f95184d8faf89cfe4ea1e0fd9aad868&json={boileris:$H,kolektorius:$H} HTTP/1.0" "\r\n"
    "Host: $F" "\r\n" "\r\n"),
    B_t, K_t, website, my_callback);
 ether.tcpSend();
*/
}
/*
ether.packetLoop(ether.packetReceive());
  if ((millis()-Ethernet_timer1)>7000) {
    Ethernet_timer1 = millis();
      str.reset();
//  str.print(basedir); 
  str.print(F("/input/post?")); str.print(F("8795903a6c0611123007be8abef16e60"));
 // str.print(apikey);
  str.print(F("&json={Z-boileris1:"));  str.print(K);
  str.print(F(",Z-kolektorius1:"));  str.print(B);
  str.print(F(",Z-termostatas1:"));  str.print(T);
  str.print(F("}\0"));
  Serial.println(F("Request sent"));
  Serial.println(str.buf);
  ether.browseUrl(PSTR(""),str.buf, website1, my_result_cb);

  }
*/
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
}// === PABAIGA ===========================================================
