// *****************************************************************************************************
//                                  W W W . S a u l e V i r e . l t
// ********************************************************************************************************                                  
// Projektas pavadinimu "pasidaryk pats mikrprocesorinį karšto vandens saulės kolektoriaus valdiklį".
// Visi failai yra ir čia:- http://fritzing.org/projects/hot-water-solar-controller/
// ********************************************************************************************************

#include <MenuBackend.h>                 // dołączenie biblioteki
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
boolean bBlink = true;
// --- definiujemy dla LCD własne znaki strzałek: dół, lewo, prawo, gora-dół i powrót ---
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
int backLight = 8;    // išvadas 13 valdo ekrano apšvietimą
LiquidCrystal lcd(A5, 3, 4, 5, 6, 7);
volatile int zm =-1;               // to dla kontroli zmiany stanu klawiatury
volatile int x=-1;                 // zmienna pomocnicza
volatile int stan_Analog;          // wartość na wejściu Analogowym dla klawiaturki analogowej
char *linia1;                      // pierwsza linia wyświetlanego tekstu na LCD
char *linia2;                      // druga linia wyświetlanego tekstu na LCD
boolean InMenu = false;
// --- tworzymy wszystkie opcje Menu: ---------------------------------------
// de facto tworzymy obiekty klasy MenuItem, które dziedziczą po klasie MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // konstruktor 
   //                        ("                ")
   MenuItem P1 =  MenuItem("Ij.temperatura",1);
   MenuItem P2 = MenuItem("Is.temperatura",1);
   MenuItem P3 = MenuItem("  Nuorinimas",1);
   MenuItem P4 = MenuItem(" Termostatas",1);

/* --- Teraz pozycjonujemy  menu ( zgodnie z ustawieniem podanym powyżej) ------------
add - dodaje w pionie, addRight - dodaje w poziomie z prawej , addLeft dodaje z lewej
*/
void menuSetup()                       // funkcja klasy MenuBackend 
{
      menu.getRoot().add(P1);          // ustawiamy korzeń Menu, czyli pierwszą opcję
      P1.addRight(P2);                 // po prawej dla PLIK jest EDYCJA
      P2.addRight(P3); 
      P3.addRight(P4);      // na prawo od EDYCJA jest SZKIC 
      P4.addRight(P1);
}
// ----------- uff... nareszcie :-) -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // funkcja klasy MenuBackend - reakcja na wciśnięcie OK
                                          // tutaj właśnie oddajemy menu na rzecz akcji obsługi klawisza OK
{
   Serial.print("Pasirinkta:  "); Serial.println(used.item.getName()); // do testów, potem niepotrzebne
   
   // --- ponizej kilka przykładów obsługi  opcji -----------
   // przykładowa reakcja na wcisnięcie klawisza OK w opcji Otworz :
//   if (used.item.getName() == "    Otworz")   // Uwaga - dokładnie taki sam ciąg "    Otworz" jak w menu !!!
                                              // bo przecież getName() pobiera nazwę
     
    // A teraz coś ambitniejszego :-), bo przekazujemy sterowanie klawiaturką do innej procedury,
    // w tym przykładzie programik czeka aż ustawisz jakąś temperaturę i po wciśnięciu OK wraca do pętli głównej
      if (used.item.getName() == "Ij.temperatura")   // dokładnie taki sam ciąg " Temperatura"
      {
        int temp=21;                         // przykładowo 21 st. C
        lcd.setCursor(0,1);lcd.write(7);     // wyswietlamy nasz symbol strzałki góra-dół
        lcd.print("              ");lcd.setCursor(1,1);lcd.print("Nust.temp.= "); // tekst dla użytkownika
        lcd.setCursor(13,1);lcd.print(temp); // wyświetlamy akt. temperaturę
        int  akcja=-1;delay(1000);         // zmienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(akcja!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           zm=-1; 
           akcja=czytaj_1(0);//delay(300);   // odczyt stanu klawiatury - funkcja czytaj_1 lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(zm!=akcja)                    // ruszamy do pracy tylko wtedy gdy zmienił sie stan klawiatury
             {
             if (akcja==1) {temp++;if(temp>99)temp=99;lcd.setCursor(13,1);lcd.print(temp);delay(300);}
               // jesli akcja=1 (czyli wciśnieto klawisz w górę to zwiększono temperaturę
               // ustawiono max próg i wyświetlono obecną temperaturę
             if(akcja==2)  {temp--;if(temp<10)temp=10;lcd.setCursor(13,1);lcd.print(temp);delay(300);}
               // jesli akcja=2 (czyli wciśnieto klawisz w dół to mniejszono temperaturę
               // ustawiono min próg i wyświetlono obecną temperaturę
             if(akcja==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(0,1);lcd.print("*Temperatura OK");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(1,1);lcd.print("              "); // czyścimy linię
                 lcd.setCursor(1,0);lcd.print(linia1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } zm=akcja;  // aktualizacja zmiennej zm, po to aby reagować tylko na zmiany stanu klawiatury
         // tu WAŻNY MOMENT - kończy się pętla while i zwracamy sterowanie do głównej pętli loop()
      }
// a tutaj obsługa pozostałych opcji :-)  
// ...
      if (used.item.getName() == "Is.temperatura")   // dokładnie taki sam ciąg " Temperatura"
      {
        int temp=21;                         // przykładowo 21 st. C
        lcd.setCursor(0,1);lcd.write(7);     // wyswietlamy nasz symbol strzałki góra-dół
        lcd.print("              ");lcd.setCursor(1,1);lcd.print("Nust.temp.= "); // tekst dla użytkownika
        lcd.setCursor(13,1);lcd.print(temp); // wyświetlamy akt. temperaturę
        int  akcja=-1;delay(1000);         // zmienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(akcja!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           zm=-1; 
           akcja=czytaj_1(0);//delay(300);   // odczyt stanu klawiatury - funkcja czytaj_1 lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(zm!=akcja)                    // ruszamy do pracy tylko wtedy gdy zmienił sie stan klawiatury
             {
             if (akcja==1) {temp++;if(temp>99)temp=99;lcd.setCursor(13,1);lcd.print(temp);delay(300);}
               // jesli akcja=1 (czyli wciśnieto klawisz w górę to zwiększono temperaturę
               // ustawiono max próg i wyświetlono obecną temperaturę
             if(akcja==2)  {temp--;if(temp<10)temp=10;lcd.setCursor(13,1);lcd.print(temp);delay(300);}
               // jesli akcja=2 (czyli wciśnieto klawisz w dół to mniejszono temperaturę
               // ustawiono min próg i wyświetlono obecną temperaturę
             if(akcja==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(0,1);lcd.print("*Temperatura OK");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(1,1);lcd.print("              "); // czyścimy linię
                 lcd.setCursor(1,0);lcd.print(linia1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } zm=akcja;  // aktualizacja zmiennej zm, po to aby reagować tylko na zmiany stanu klawiatury
         // tu WAŻNY MOMENT - kończy się pętla while i zwracamy sterowanie do głównej pętli loop()
      }

// ...
      if (used.item.getName() == " Termostatas")   // dokładnie taki sam ciąg " Temperatura"
      {
        int temp=21;                         // przykładowo 21 st. C
        lcd.setCursor(0,1);lcd.write(7);     // wyswietlamy nasz symbol strzałki góra-dół
        lcd.print("              ");lcd.setCursor(1,1);lcd.print("Nust.temp.= "); // tekst dla użytkownika
        lcd.setCursor(13,1);lcd.print(temp); // wyświetlamy akt. temperaturę
        int  akcja=-1;delay(1000);         // zmienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(akcja!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           zm=-1; 
           akcja=czytaj_1(0);//delay(300);   // odczyt stanu klawiatury - funkcja czytaj_1 lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(zm!=akcja)                    // ruszamy do pracy tylko wtedy gdy zmienił sie stan klawiatury
             {
             if (akcja==1) {temp++;if(temp>99)temp=99;lcd.setCursor(13,1);lcd.print(temp);delay(300);}
               // jesli akcja=1 (czyli wciśnieto klawisz w górę to zwiększono temperaturę
               // ustawiono max próg i wyświetlono obecną temperaturę
             if(akcja==2)  {temp--;if(temp<10)temp=10;lcd.setCursor(13,1);lcd.print(temp);delay(300);}
               // jesli akcja=2 (czyli wciśnieto klawisz w dół to mniejszono temperaturę
               // ustawiono min próg i wyświetlono obecną temperaturę
             if(akcja==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(0,1);lcd.print("*Temperatura OK");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(1,1);lcd.print("              "); // czyścimy linię
                 lcd.setCursor(1,0);lcd.print(linia1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } zm=akcja;  // aktualizacja zmiennej zm, po to aby reagować tylko na zmiany stanu klawiatury
         // tu WAŻNY MOMENT - kończy się pętla while i zwracamy sterowanie do głównej pętli loop()
      }

}
// --- Reakcja na wciśnięcie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  if(changed.to.getName()==menu.getRoot()){InMenu =false;
//                                           Serial.println("Dabar esame MenuRoot");
//                                           lcd.setCursor(0,1);lcd.print("    MenuRoot");
                                           }
  /* tak naprawdę to tylko tutaj przydaje się ów shortkey i służy przede wszystkim do wzbogacenia menu
     o symbole strzałek w zależności co wybrano. Wszystko co tutaj się wyprawia jest pokazywane na LCD. 
  */
  int c=changed.to.getShortkey();                         // pobieramy shortkey (1,2,3, lub4)
  lcd.clear();                                            // bez komentarza 
  lcd.setCursor(0,0); 
  if(c==1)                                                // jeśli to menu głowne (shortkey=1) to:
    {InMenu =true;
    lcd.write(3);                                         // strzałka w lewo
    strcpy(linia1,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(linia1);                                    // wyświetlamy ją
    lcd.setCursor(15,0);lcd.write(4);                     // strzałka w prawo
    lcd.setCursor(0,1);lcd.write(5);                      // strzałka w dół
    lcd.setCursor(15,1);lcd.write(5);                     // strzałka w dół
    }
    if(c==2)                                              // jeśli to podmenu dla dziecka - (shortkey=2) to:
    {InMenu =true;
    lcd.print("*");                                       // rysujemy gwiazdkę
    strcpy(linia2,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(linia1);                                    // wyświetlamy ją
    lcd.setCursor(15,0);lcd.print("*");                   // gwiazdka 
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka powrotu (arrowBack)
    lcd.print(changed.to.getName());                      // wyświetlamy nazwe "dziecka"
    lcd.setCursor(15,1);lcd.write(7);                     // strzałka góra-dół
    }
    if(c==3)                                              // jeśli dziecko  ma dziecko - (shortkey =3) to:
    {InMenu =true;
    lcd.print("*");                                       // gwiazdka
    strcpy(linia2,changed.to.getName());                  // kopiujemy akt. nazwe opcji menu do zmiennej linia2
    lcd.print(linia1);                                    // i wyświetlamy pierwszą linię
    lcd.setCursor(15,0);lcd.print("*");                   // gwiazdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka w drugiej linii
    lcd.setCursor(15,1);lcd.write(4);                     // strzałka w prawo bo są wnuki
    }
    
    if(c==4)                                              // jeśli to wnuk  (shortkey =4) to:
    {InMenu =true;
    lcd.print("*");                                       // gwaizdka
    lcd.print(linia2);                                    // w pierwszej linii wyświetlamy dziecko ( czyli rodzica wnuka) 
    lcd.setCursor(15,0);lcd.print("*");                   // gwaizdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka
    lcd.setCursor(15,1);lcd.write(7);                     // strzałka góra-dół 
    }
}
/* --- ponżej funkcja odczytująca stan klawiatury -------------------------------------------
przygotowałem 3 różne wersje:
1) dla klawiaturki analogowej z shielda LCDanalogKey firmy DFRobot
2) dla joysticka ( 2 wejscia Analogowe i 1 pin cyfrowy )
   Uwaga dla tej wersji opcji musisz dopisać w funkcji setup konfigurację dla pin, np. tak:
   pinMode(1,INPUT);digitalWrite(1,HIGH);
3) dla 5-ciu pojedynczych przycisków ( potrzeba 5 pinów cyfrowych)
   Uwaga dla tej wersji opcji musisz dopisać w funkcji setup konfigurację dla pinów, np. tak:
   pinMode(1,INPUT);digitalWrite(1,HIGH);
   pinMode(2,INPUT);digitalWrite(2,HIGH);
   pinMode(3,INPUT);digitalWrite(3,HIGH); 
   pinMode(11,INPUT);digitalWrite(11,HIGH); 
   pinMode(12,INPUT);digitalWrite(12,HIGH); 
*/
// --- wersja dla klawiatury 5-cio przyciskowej DFRobot --------------------------------------
volatile int czytaj_1(int analog)
{
   int stan_Analog = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (stan_Analog > 1000) return -1; // riba
   if (stan_Analog < 50)   return 3;  // desine  
   if (stan_Analog < 150)  return 1;  // viršus 
   if (stan_Analog < 320)  return 2;  // apačia 
   if (stan_Analog < 500)  return 0;  // kaire  
   if (stan_Analog < 750)  return 4;  // OK 
   return -1;                         // nieko
}

// ============================================================================================
// 


#define Pump A1 //Siurblio valdymo relės išvadas
#define Relay2 A2 //Termostato valdymo relės išvadas
int analogPin =  A7; //daugiasūkio rezistoriaus pajungimo išvadas
// versijos 10 pasirinkimo pabaiga


// versijos 11 pasirinkimo pabaiga


#define SERIAL_BAUD_RATE 9600
// Temperatūros jutiklių duomenų laidai prijungti prie Arduino išvadų 8,9,10
#define Collector 2 //Kolektoriaus jutiklio išvadas
#define Boiler 9 //Boilerio jutiklio išvadas
#define Temp_3 A3 //termostato jutiklio išvadas


int relayPin = Pump;
int relay2Pin = Relay2;
static long timer_pump=0;
static long timer_Relay2=0;
static long timer_freezing=0;

static long timer_Collector_Error=0;
static long timer_Boiler_Error=0;
//Laiko intervalai
// REQUEST_Pump_Control minimalus siurblio reles veikimo laiko intervalas, pasikeitus temperatūrų reikšmėms
// REQUEST_Relay2_Control minimalus termostato reles veikimo laiko intervalas, pasikeitus temperatūrų reikšmėms
// REQUEST_Collector_Error laiko intervalas veiksmui, jei kolektoriaus temperatūros jutiklis rodo klaidą
// REQUEST_Boiler_Error laiko intervalas veiksmui, jei boilerio temperatūros jutiklis rodo klaidą
#define REQUEST_Pump_Control 10000          // 10000 millis= 10 sekundžių
#define REQUEST_Relay2_Control 10000       // 15000 millis= 15 sekundžių
#define REQUEST_Collector_Error 600000    // 600000 millis= 10minučių
#define REQUEST_Boiler_Error 10000       // 30000 millis= 30 sekundžių
#define REQUEST_freezing 5000          // 5000 millis= 5 sekundės

int freezing = 1; //kintamasis apsaugai nuo užšalimo įjungti (freezing = 1) arba išjungti (freezing = 0)
int Temp3_daviklis = 1 ; //kintamasis trečio daviklio naudojimui 0- nenaudojamas, 1- naudojamas
int Differential_ON=5; //Čia užduodama prie kokio temperatūrų skirtumo įjungiama kolektoriaus siurblio relė
int Differential_OFF=3; //prie tokio skirtumo siurblio relė išjungiama
int Collector_Error, Boiler_Error,Temp_3_Error; // jutiklio būsenos "klaida" žymė
float Collector_tempC, Boiler_tempC, Sensor3_tempC; //kintamieji temperatūros matavimų reikšmėms saugoti
float Saved_Collector_tempC, Saved_Boiler_tempC;
//Siurblio įjungimo simbolis (rodyklė į viršų)
byte arrow_up[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
//Siurblio įšungimo simbolis (rodyklė į žemyn)
byte arrow_down[8]={ B00100,B00100,B00100,B00100,B00100,B11111,B01110,B00100};

      // nustatomos reikšmės valdiklio "susikalbėjimui" su jutikliais DS18B20
      OneWire Collector_T(Collector);
      OneWire Boiler_T(Boiler);
      OneWire Temp_T(Temp_3);
      DallasTemperature CollectorSensor(&Collector_T);
      DallasTemperature BoilerSensor(&Boiler_T);
      DallasTemperature TempSensor(&Temp_T);
      //masyvai jutiklių adresams saugoti
      DeviceAddress Collector_Sensor_Address;
      DeviceAddress Boiler_Sensor_Address;
      DeviceAddress Temp_3_Sensor_Address;

// LCD ekrano nustatymai
// prijungimas:
// rs (LCD išvadas 4) - Arduino išvadas 2
// enable (LCD išvadas 6) - Arduino išvadas 3
// LCD išvadas 15 - Arduino išvadas 13
// LCD išvadai d4, d5, d6, d7 - Arduino išvadai 4, 5, 6, 7


// Kintamasis termostato užduotai reikšmei saugoti
int val = 0;
//Programos nustatymų pradžia
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pinMode(Pump, OUTPUT);
  pinMode(Relay2, OUTPUT);
  digitalWrite(Pump, HIGH);
  digitalWrite(Relay2, HIGH);
  // Arduino išvadų paruošimas darbui su LCD ekranu:
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, HIGH); // Įjungiamas apšvietimas.
  lcd.createChar(1, arrow_down);
  lcd.createChar(2, arrow_up);

  lcd.begin(16, 2);
  // Reklaminio užrašo išvedimas į ekraną (TAI LABAI SVARBU :-).
          lcd.print("www.SauleVire.lt");
          lcd.setCursor(0,1);lcd.print("      v1.2      ");
          Serial.println();
          Serial.println("www.SauleVire.lt");
		Serial.print("Siurblys isijungs, kai temperaturu skirtumas bus - ");  Serial.println(Differential_ON);
		Serial.print("Siurblys issijungs, kai temperaturu skirtumas bus - ");  Serial.println(Differential_OFF);
		if (freezing == 1) {Serial.println("IJUNGTA kolektoriaus apsauga nuo uzsalimo");}
		else {Serial.println("Kolektoriaus apsauga nuo uzsalimo NEIJUNGTA");}
          delay(5000); //Reklama trunka tik 5 sekundes.
          lcd.clear();
CollectorSensor.begin();
          if (!CollectorSensor.getAddress(Collector_Sensor_Address, 0))
          Serial.println("neveikia kolektoriaus temperaturos daviklis");
          // nustatomas didžiausias jutiklio tikslumas- 12 bitų.
          CollectorSensor.setResolution(Collector_Sensor_Address, 12);
BoilerSensor.begin();
          if (!BoilerSensor.getAddress(Boiler_Sensor_Address, 0))
          Serial.println("Neveikia boilerio temperaturos daviklis");
          // nustatomas didžiausias jutiklio tikslumas- 12 bitų.
          BoilerSensor.setResolution(Boiler_Sensor_Address, 12);
TempSensor.begin();
          if (!TempSensor.getAddress(Temp_3_Sensor_Address, 0))
          Serial.println("Neveikia Temp_3 temperaturos daviklis");
          // nustatomas didžiausias jutiklio tikslumas- 12 bitų.
          TempSensor.setResolution(Temp_3_Sensor_Address, 12);          
} 
//____________________________
//Programos nustatymų pabaiga
//____________________________
void loop() { //Prasideda pagrindinė programa
bBlink = ((bBlink) ? false : true);
 Serial.println();
 Serial.print("Siurblys isijungs, kai temperaturu skirtumas bus - ");  Serial.println(Differential_ON);
 Serial.print("Siurblys issijungs, kai temperaturu skirtumas bus - ");  Serial.println(Differential_OFF);
 
if (freezing == 1) {lcd.setCursor(14,0);
        Serial.println("Kolektoriaus apsauga nuo uzsalimo I_J_U_N_G_T_A");
 	lcd.print("*");
 }else {
        Serial.println("Kolektoriaus apsauga nuo uzsalimo N_E_I_J_U_N_G_T_A");
	lcd.setCursor(14,0);
	lcd.print(" ");}
//if (InMenu == false){
// ******************* trečiojo daviklio naudojimas  P_R_A_D_Ž_I_A *****************************************
 if (Temp3_daviklis == 1){
 val = (analogRead(analogPin) * 100.0) / 1024; //nuskaitoma kintamu rezistoriumi nustatyta termostato reikšmė ir
lcd.setCursor(7,1);
   lcd.print("N");
   lcd.setCursor(8,1);
   lcd.print(val); //parodoma ekrane
   
if (!TempSensor.getAddress(Temp_3_Sensor_Address, 0)) {
   lcd.setCursor(10,1);
  if (bBlink) lcd.print("KLAIDA!");
  else lcd.print("-------");
   Serial.println("3 daviklio temperatura: K_L_A_I_D_A");
   Temp_3_Error=1;
}
else 
{
  TempSensor.requestTemperatures();
  Sensor3_tempC=TempSensor.getTempC(Temp_3_Sensor_Address);
  Serial.print("Nustatyta termostato reiksme "); Serial.println(val); //spausdinama į terminalą, jei prijungtas kompiuteris
  Sensor3_tempC=TempSensor.getTempC(Temp_3_Sensor_Address);
  
  if (Sensor3_tempC==-127){lcd.setCursor(10,1);
  if (bBlink)lcd.print("KLAIDA");
  else lcd.print("------"); 
  Serial.println(" K L A I D A !"); Temp_3_Error=1;
  } else {
  lcd.setCursor(10,1);lcd.print(" ");lcd.print(Sensor3_tempC);
  Serial.print("Ismatuota termostato reiksme "); Serial.println(Sensor3_tempC);}
    if (millis() > timer_Relay2 + REQUEST_Relay2_Control) {
  timer_Relay2 = millis();
// Jei potenciometru nustatyta temperatura mazesne, negu ismatuota 3-ciuoju davikliu, RELE IJUNGIAMA  
  if (Sensor3_tempC >= val)  Rele_2_ijungta(); 
  if (Sensor3_tempC <= val)  Rele_2_isjungta();
// Jei potenciometru nustatyta temperatura Didesne, negu ismatuota 3-ciuoju davikliu, RELE IJUNGIAMA  
//  if (Sensor3_tempC <= val)  Rele_2_ijungta(); 
//  if (Sensor3_tempC >= val)  Rele_2_isjungta();
   
 }
}} 
// ************************** trečiojo daviklio naudojimas  P_A_B_A_I_G_A *************************

  if (!BoilerSensor.getAddress(Boiler_Sensor_Address, 0)) {
  lcd.setCursor(0,1);
  if (bBlink) lcd.print("KLAIDA!"); 
  else lcd.print("-------");
  Serial.println("Boilerio Temperatura: K_L_A_I_D_A");
  Boiler_Error=1;
  }
else{
BoilerSensor.requestTemperatures();
Serial.print("Boilerio temperatura: ");
Boiler_tempC=BoilerSensor.getTempC(Boiler_Sensor_Address);
if (Boiler_tempC==-127.00){
  lcd.setCursor(0,1);
  if (bBlink) lcd.print("KLAIDA!"); 
  else lcd.print("-------");
  Serial.println(" K L A I D A !");
  Boiler_Error=1;
}
else{
Serial.println(Boiler_tempC);
  lcd.setCursor(0,1);
  lcd.print("B");
  lcd.setCursor(1,1);
  lcd.print(Boiler_tempC);
  lcd.print(" ");
  Boiler_Error = 0;
}}
 
if (!CollectorSensor.getAddress(Collector_Sensor_Address, 0)) {
   lcd.setCursor(0,0);
  if (bBlink) lcd.print("KLAIDA!");
  else lcd.print("-------");
   Serial.println("Kolektoriaus Temperatura: K_L_A_I_D_A");
   Collector_Error=1;
}else{
  CollectorSensor.requestTemperatures(); // Komanda jutikliui parodyti išmatuotą temperatūrą
Serial.print("Kolektoriaus Temperatura: ");
Collector_tempC=CollectorSensor.getTempC(Collector_Sensor_Address);
if (Collector_tempC==-127.00){
  lcd.setCursor(0,0);
  if (bBlink) lcd.print("KLAIDA!");
  else lcd.print("-------");
  Serial.println(" K L A I D A !");
  Collector_Error=1;
  }else{
Serial.println(Collector_tempC);
  lcd.setCursor(0,0);
  lcd.print("K");
  if (Collector_tempC >= 0) lcd.print(" "); 
  else lcd.setCursor(1,0);
  lcd.print(Collector_tempC);
  lcd.setCursor(8,0);
  if (Collector_tempC-Boiler_tempC >=0) lcd.print(" ");
  lcd.print(Collector_tempC-Boiler_tempC);
  if (Collector_tempC-Boiler_tempC <=10) lcd.print(" ");
  Collector_Error = 0;
  }}


// Tikrinama ar reikia apsaugos nuo užšalimo PRADŽIA
if (millis() > timer_freezing + REQUEST_freezing) {
    timer_freezing = millis();
if (freezing == 1) { //jei apsauga įjungta,
	//tikrinama ar kolektoriaus temperatūra teigiama
if ((Collector_tempC  <= 0 ) && (Collector_Error == 0)) { //jei temperatūra neigiama, 
   Siurblys_ijungtas();       //įjungiamas siurblys
   Serial.println("I_J_U_N_G_T_A_S siurblys apsaugai nuo uzsalimo");
   } 
      }
} 
// Tikrinimo ar reikia apsaugos nuo užšalimo PABAIGA

//__________________________________________________
//Tikrinama kokia turį būti siurblio būsena. Pradžia
//--------------------------------------------------
//kintamasis REQUEST_Pump_Control nurodo kas kiek laiko tai daryti
if (millis() > timer_pump + REQUEST_Pump_Control) {
    timer_pump = millis();
if (Boiler_Error == 0){ // jei boilerio daviklis geras
if (Collector_Error == 0){ // jei kolektoriaus daviklis geras
//tikrinama at kolektorius nėra šiltesnis už boilerį	
if (Collector_tempC - Boiler_tempC >= Differential_ON) {
   Siurblys_ijungtas();
   Serial.println("___________________________");
   Serial.println("Siurblys- _I_J_U_N_G_T_A_S_");
   Serial.println("___________________________");
}
//tikrinama ar kolektoriaus ir boilerio temperatūrų skirtumes nėra mažesnis už nustatytą
if (Collector_tempC - Boiler_tempC <= Differential_OFF) { 
   Siurblys_isjungtas();
   Serial.println("_________________________");
   Serial.println("Siurblys- _N_E_D_I_R_B_A_");
   Serial.println("_________________________");
}

//______________________________________________
//Kolektoriaus jutiklių klaidų tikrinimo pražia
//----------------------------------------------
}else{Serial.println("************* Kolektoriaus daviklio zyme- BLOGAS! ********************************");
// Jei kolektoriaus temperatūros jutiklis neveikia, įjungiamas siurblys. 
// Kas kiek laiko nurodo kintamasis REQUEST_Collector_Error
// Jei boilerio temperatūra mažėja, siurblys išjungiamas.
if (millis() > timer_Collector_Error + REQUEST_Collector_Error) {
   timer_Collector_Error = millis();
   Saved_Boiler_tempC = Boiler_tempC; //Atsimenama boilerio temperatūra
   Siurblys_ijungtas();
   lcd.setCursor(8,0); lcd.print(" ??.??");
   Serial.println("____________________________________________________________");
   Serial.println("***** kolektoriaus apsauga nuo perkaitimo, siurblys ijungtas");
   Serial.println("____________________________________________________________");
   Serial.println(Saved_Boiler_tempC);
}else{ if (Saved_Boiler_tempC - Boiler_tempC >= 0.5 )
       {Siurblys_isjungtas(); timer_Collector_Error = millis();
       }
}
//______________________________________________
  //Boilerio jutiklių klaidų tikrinimo pražia
  //----------------------------------------------

}
}
else{ Serial.println("************* Boilerio daviklio zyme- BLOGAS! ********************************");
      Serial.println("* Siurblys bus ijungtas, kai kolektoriaus temperatura pakils iki 95 laipsniu *");
      Serial.println("* Siurblys bus isjungtas, kai kolektoriaus temperatura nukris iki 85 laipsniu *");
      Serial.println("******************************************************************************");
      if (Collector_tempC >= 95) Siurblys_ijungtas();
      if (Collector_tempC <= 85) Siurblys_isjungtas();
  // if (Collector_tempC >=95){Siurblys_ijungtas();} //Jei kolektoriaus temperatūra didesnė negu 95 laipsniai, įjungiamas siurblys 

}
} //__________________________________________________
   //
   //--------------------------------------------------

}//Pagrindinės programos pabaiga
//}
//_______________________________________
//      P A P R O G R A M Ė S 
//siurblio ir termostato relių junginėjimui
//---------------------------------------
//siurblio įjungimas ir atitinkamo ženklo ekrane rodymas
void Siurblys_ijungtas()  {
 digitalWrite(Pump, LOW);
 digitalWrite(13, HIGH);
   lcd.setCursor(15,0);
   lcd.write((uint8_t)2);//char up to LCD
//   lcd.setCursor(15,1);
//   lcd.write((uint8_t)2);//char up to LCD
}
//siurblio įšjungimas ir atitinkamo ženklo ekrane rodymas
void Siurblys_isjungtas(){
digitalWrite(Pump, HIGH);
   lcd.setCursor(15,0);
   lcd.write((uint8_t)1);//char down to LCD
//   lcd.setCursor(15,1);
//   lcd.write((uint8_t)1); //char down to LCD
}
//termostato relės įjungimas ir atitinkamo ženklo ekrane rodymas
void Rele_2_ijungta()  {
digitalWrite(Relay2, LOW);
digitalWrite(13, HIGH);
   lcd.setCursor(9,1);
   lcd.write((uint8_t)2);//char up to LCD
}
//termostato reles įsjungimas ir atitinkamo ženklo ekrane rodymas
void Rele_2_isjungta(){
digitalWrite(Relay2, HIGH);
   lcd.setCursor(9,1);
   lcd.write((uint8_t)1); //char down to LCD
}

