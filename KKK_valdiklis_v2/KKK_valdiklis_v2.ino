// ============= KKK valdinlis v2.1 ===============================================


#include <Wire.h>
//#include <DS1307.h>

#include <MenuBackend.h>        
 #include <LiquidCrystal.h>         
  #include <OneWire.h>
   #include <DallasTemperature.h>

//    #include <EEPROM.h>
//     #include <PID_v1.h>
#include "definitions.h"
    
//  #define ONE_WIRE_BUS4 42
//Define Variables we'll be connecting to
double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
//PID PV_PID(&Input, &Output, &Setpoint,6,15,3, DIRECT);
//int WindowSize = 10000;
//unsigned long windowStartTime;

  // --- definiujemy dla LCD własne znaki strzałek: dół, lewo, prawo, gora-dół i powrót ---
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
uint8_t arrowUp[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
    // definicja pinów dla LCD (sprawdź piny w swoim LCD)
LiquidCrystal lcd(A5, 3, 4, 5, 6, 7);
/* ------------------ R T C ---------------------- */


/* --------------------- RTC PABAIGA ---------------- */
char *eilute1;                      // pierwsza eilute wyświetlanego tekstu na LCD
char *eilute2;                      // druga eilute wyświetlanego tekstu na LCD
char *eilute3;
    boolean InMenu = false;

// --- tworzymy wszystkie opcje Menu: ---------------------------------------
// de facto tworzymy obiekty klasy MenuItem, które dziedziczą po klasie MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // konstruktor 
   //                        ("                ")
   MenuItem P1 =  MenuItem("KOLEKTORIUS   ",1);
      MenuItem P11 = MenuItem("Ijungimo sk. t",2);
      MenuItem P12 = MenuItem("Isjungimo sk.t",2);

   MenuItem P2 = MenuItem("TERMOSTATAS   ",1);
      MenuItem P21 = MenuItem("Ijungimo t.",2);
      MenuItem P22 = MenuItem("AT isjungimo temp.",2);
      MenuItem P23 = MenuItem("AT rankinis vald.",2);


/* --- Teraz pozycjonujemy  menu ( zgodnie z ustawieniem podanym powyżej) ------------
add - dodaje w pionie, addRight - dodaje w poziomie z prawej , addLeft dodaje z lewej
*/
void menuSetup()                       // funkcja klasy MenuBackend 
{
      menu.getRoot().add(P1);          // 
      P1.add(P11);
        P11.add(P12);P11.addLeft(P1);  //  
        P12.add(P13);P12.addLeft(P1);  // 
        P13.add(P11);P13.addLeft(P1);

      menu.getRoot().add(P2);
      P1.addRight(P2);                 //
      
      P2.add(P21);                     // 
        P21.add(P22);P21.addLeft(P2);  // 
        P22.add(P23);P22.addLeft(P2);  // 
        P23.add(P21);P23.addLeft(P2);
      menu.getRoot().add(P1);
      P2.addRight(P1);                 //
      
}
// ----------- uff... nareszcie :-) -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // funkcja klasy MenuBackend - reakcja na wciśnięcie OK
                                          // tutaj właśnie oddajemy menu na rzecz akcji obsługi klawisza OK
{
   Serial.print("pasirinkta:  "); Serial.println(used.item.getName()); // do testów, potem niepotrzebne
   // --- ponizej kilka przykładów obsługi  opcji -----------
   // przykładowa reakcja na wcisnięcie klawisza OK w opcji Otworz :
/* __________________________ NUSTATYMAI Pasvietimas____________________________________ */
  if (used.item.getName() == "Ijungimo sk. t")
  {
  lcd.setCursor(0,2);lcd.write(7);     // simbolis aukštyn/žemyn
  lcd.print("                  ");lcd.setCursor(1,2);lcd.print("Sviesumas"); // keičiamos reikšmės pavadinimas
  lcd.setCursor(13,2);lcd.print(lcd_pasvietimas);lcd.print("0% ");                        // dabartinė reikšmė
  int  veiksmas=-1;delay(1000);                                             // pagalbinis kintamasis, kontroliuojantis while ciklą
                                                                         // jei jums nereikia keisti, spauti OK po 1 sek. ir grįžti į meniu  
  while(veiksmas!=4)                   // Šis ciklas bus kartojamas, kol paspausite mygtuką OK
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin);//delay(300);   // odczyt stanu klawiatury - funkcja Klaviaturos_skaitymas lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(klaviaturos_pasikeitimas!=veiksmas)                    // ruszamy do pracy tylko wtedy gdy klaviaturos_pasikeitimasienił sie stan klawiatury
             {lcd.setCursor(13,2);
             if (veiksmas==1) {lcd_pasvietimas++; analogWrite(BackLight_Pin,lcd_pasvietimas*25);delay(300);}
               // jesli akcja=1 (czyli wciśnieto klawisz w górę to zwiększono temperaturę
               // ustawiono max próg i wyświetlono obecną temperaturę
             if(veiksmas==2)  {lcd_pasvietimas--;analogWrite(BackLight_Pin,lcd_pasvietimas*25);delay(300);}
            if ((lcd_pasvietimas == 255) || (lcd_pasvietimas == 0)) lcd_pasvietimas = 0;
            if (lcd_pasvietimas > 10) lcd_pasvietimas = 10;
            if (lcd_pasvietimas < 10) lcd.print(" ");
            lcd.print(lcd_pasvietimas);
            if (lcd_pasvietimas == 0) lcd.print("");
            lcd.print("0% ");

               // jesli akcja=2 (czyli wciśnieto klawisz w dół to mniejszono temperaturę
               // ustawiono min próg i wyświetlono obecną temperaturę
             if(veiksmas==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(0,2);lcd.print(">Sviesumas OK");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(0,2);lcd.print("                    "); // czyścimy linię
               //  lcd.setCursor(1,0);lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagować tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         // tu WAŻNY MOMENT - kończy się pętla while i zwracamy sterowanie do głównej pętli loop()
      } 
/* __________________________ NUSTATYMAI Irasymas____________________________________ */
     if (used.item.getName() == "Irasymas")   // dokładnie taki sam ciąg " Temperatura"
      {

                 lcd.setCursor(0,1);lcd.print(">Irasyta OK        ");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(0,1);lcd.print("                    "); // czyścimy linię
                // lcd.setCursor(0,0);lcd.print("*");lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
                // lcd.setCursor(19,0);lcd.print("*");

      }

 // ...
}
// --- Reakcja na wciśnięcie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  if(changed.to.getName()==menu.getRoot())
  {
    InMenu =false;
    Serial.println("Dabar esame MenuRoot");
    LCD_T_sablonas();
    Temperaturu_vaizdavimas();
  }
  /* tak naprawdę to tylko tutaj przydaje się ów shortkey i służy przede wszystkim do wzbogacenia menu
     o symbole strzałek w zależności co wybrano. Wszystko co tutaj się wyprawia jest pokazywane na LCD. 
  */
  int c=changed.to.getShortkey();                         // pobieramy shortkey (1,2,3, lub4)
  lcd.setCursor(0,0); 
  if(c==1)                                                // jeśli to menu głowne (shortkey=1) to:
    {InMenu =true;
    lcd.write(3);                                         // strzałka w lewo
    strcpy(eilute1,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(eilute1);                                    // wyświetlamy ją
    lcd.setCursor(15,0);lcd.write(4);                     // strzałka w prawo
    lcd.setCursor(0,1);lcd.write(5); lcd.print("                  ");   
    lcd.setCursor(15,1);lcd.write(5);                     // strzałka w dół
    lcd.setCursor(0,1);lcd.print("                    "); // išvaloma 3 eilutė, kai atsiduriama meniu nr.1
    }
    if(c==2)                                              // jeśli to podmenu dla dziecka - (shortkey=2) to:
    {InMenu =true;
    lcd.print("*");                                       // rysujemy gwiazdkę
    strcpy(eilute2,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(eilute1);                                    // wyświetlamy ją
    lcd.setCursor(15,0);lcd.print("*");                   // gwiazdka 
    lcd.setCursor(0,1);lcd.write(6); lcd.print("                  ");                     // druga eilute i strzałka powrotu (arrowBack)
    lcd.setCursor(1,1);lcd.print(changed.to.getName());                      // wyświetlamy nazwe "dziecka"
    lcd.setCursor(15,1);lcd.write(7);                     // strzałka góra-dół
    }
    
    if(c==3)                                              // jeśli dziecko  ma dziecko - (shortkey =3) to:
    {InMenu =true;
    lcd.print("*");                                       // gwiazdka
    strcpy(eilute2,changed.to.getName());                  // kopiujemy akt. nazwe opcji menu do klaviaturos_pasikeitimasiennej eilute2
    lcd.print(eilute1);                                    // i wyświetlamy pierwszą linię
    lcd.setCursor(15,0);lcd.print("*");                   // gwiazdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga eilute i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka w drugiej linii
    lcd.setCursor(15,1);lcd.write(4);                     // strzałka w prawo bo są wnuki
    }
    
    if(c==4)                                              // jeśli to wnuk  (shortkey =4) to:
    {InMenu =true;
    lcd.print("*");                                       // gwaizdka
    lcd.print(eilute2);                                    // w pierwszej linii wyświetlamy dziecko ( czyli rodzica wnuka) 
    lcd.setCursor(15,0);lcd.print("*");                   // gwaizdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga eilute i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka
    lcd.setCursor(15,1);lcd.write(7);                     // strzałka góra-dół 
    } 
}

// --- analoginė 5 mygtukų klaviatūros nuskaitymo versija, DFRobot --------------------------------------
volatile int Klaviaturos_skaitymas(int analog)
{
   int stan_Analog = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (stan_Analog > 1000) return -1; // riba
   if (stan_Analog < 50)   return 3;  // Į dešinę 
   if (stan_Analog < 200)  return 1;  // Viršun 
   if (stan_Analog < 400)  return 2;  // Žemyn‚ 
   if (stan_Analog < 600)  return 0;  // Į kairę  
   if (stan_Analog < 800)  return 4;  // OK 
   return -1;                         // Nepaspaustas
}
// ============================================================================================
// 
void setup()
{
  

  /* ********************************************************* */

  pinMode(BackLight_Pin, OUTPUT);
    digitalWrite(BackLight_Pin,HIGH);
  eilute1=new char[20];  // zainicjowanie dynamicznego wskaźnika do tekstu 
  eilute2=new char[20];  // to BARDZO WAŻNE, bo wskażnik dynamiczny musi wskazywać na 
  eilute3=new char[20];   // z góry określone miejsce w pamieci. Gdybyśmy tego nie zrobili
                        // to wcześniej czy później programik mógłby wskoczyć w nieokreślony 
                        //  bliżej obszar pamięci, co może skutkować nieodwracalnymi konsekwencjami
                        // łącznie z przestawieniem Fuse Bitów !!!  
                        // Proszę uważać na wszystkie dynamiczne wskaźniki, TAKA DOBRA RADA :-)
  Serial.begin(9600);   // inicjacja Seriala, głównie do testów 
  lcd.begin(16, 2);     // inicjacja LCD
  lcd.clear();



  lcd.createChar(3,arrowLeft);    // LCD ženklas kairėn
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  lcd.createChar(1,arrowUp);
    lcd.setCursor(0,0); 
    lcd.print("www.SauleVire.lt");
    lcd.setCursor(0,1); 
    lcd.print("valdiklis v1.2"); delay(2999);
 lcd.clear();
   K_sensor.begin();B_sensor.begin();
   T_sensor.begin();
   
  pinMode(13,OUTPUT);digitalWrite(13,LOW); // tik testas 
  menuSetup();          // funkcja klasy MenuBackend - tu tak naprawdę tworzymy nasze menu 
//  menu.moveDown();      // idziemy do pierwszej opcji - PLIK, moveDown bo pierwotnie byliśmy w root
  Temperaturu_matavimas_1();

 
  LCD_T_sablonas();
  Temperaturu_vaizdavimas();
    Ekrano_pasvietimo_ijungimo_laikas = millis();
    temperaturu_matavimo_laikas_1 = millis();

 //////////////////////// LAIKRODIS ///////////////////////////

  }  // setup() ...************ PABAIGA **************...
  // ************************ PROGRAMOS PRADZIA void loop() *******************************
void loop()    
{
  // jei ekranas, nespaudant mygtukų, šviečia ilgiau negu užduota, pašvietimas išjungiamas
      if (millis()- Ekrano_pasvietimo_ijungimo_laikas > Ekrano_pasvietimo_pertrauka) { 
      analogWrite(BackLight_Pin, 0);
      Backlighting = false;
      Ekrano_pasvietimo_ijungimo_laikas = millis();}
 // Paspaudus bet kokį klavišą įjungiamas ekrano pašvietimas, kai jis būna išjungtas  
if ((x != -1) && (Backlighting == false)){ analogWrite(BackLight_Pin,lcd_pasvietimas*25);
                                            Backlighting = true;}
// Jei meniu būna neaktyvus kažkiek laiko, tai grįžtama į pagrindinę programą
//if ((x != -1) && (InMenu == true))Meniu_praleistas_neaktyvus_laikas = millis();
//    else {if (millis()- Meniu_praleistas_neaktyvus_laikas > neaktyvus_meniu)
//          menu.toRoot();
//}
  x=Klaviaturos_skaitymas(Key_Pin);delay(30);             // odczytujemy stan klawiatury:
  if(klaviaturos_pasikeitimas!=x)                               // jesli była klaviaturos_pasikeitimasiana stanu to :
    {
      switch(x)                           // sprawdzamy co nacisnieto 
      {
      case 0: menu.moveRight();break;     // jesli naciśnięto klawisz w Prawo to przesuń menu w prawo
      case 1: menu.moveUp();break;        // menu do góry
      case 2: menu.moveDown();break;      // menu w dół
      case 3: menu.moveLeft();break;      // menu w lewo
      case 4: menu.use();break;           // wciśnięto OK więc skok do funkcji menuUseEvent(MenuUseEvend used)
                                          // to w tej funkcji właśnie obsługujemy nasze Menu, tu sprawdzamy
                                          // jaką opcję wybrano i tutaj tworzymy kod do obslugi zdarzenia.
      }
    } klaviaturos_pasikeitimas=x;                               // przypisanie klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas wartości x po to, aby dluższe wciskanie tego
                                          // samego klawisza nie powodowało ponownej generacji zdarzenia. 
                                          // program reaguje na klaviaturos_pasikeitimasianę stanu klawiatury. 
// Jeigu dabar neesame meniu viduje, tai vykdoma nuolatinė programa
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
Serial.print("millis- ");Serial.println(millis()/1000);
#endif
  }
} 
// Tikrinama ar rankiniu budu neijungtas AKUMULIACINĖS TALPOS ar BOILERIO  siurblys







// matuojamos temperatūros nurodytais laiko intervalais (temperaturu_matavimo_pertrauka)
/* +++++++++++++++++++++++++++ PIRMAS LYGIS ++++++++++++++++++++++++++++++++++++ */ 
if (millis() > temperaturu_matavimo_laikas_1 ) { 
  temperaturu_matavimo_laikas_1 = millis() + temperaturu_matavimo_pertrauka_1;
  Temperaturu_matavimas_1();}
<<<<<<< HEAD:v12/v12.ino
//------------------ kolektoriaus siurblio ir termostato valdymas-----------------------//
if (millis() > Reliu_junginejimo_laikas ) 
 {
   Reliu_junginejimo_laikas=millis()+Reliu_junginejimo_pertrauka;
   if (K-B >= k_ijungimo_skirtumas) digitalWrite(Rele_K,LOW);
   if (K-B <= k_isjungimo_skirtumas) digitalWrite(Rele_K,HIGH);
   //Jei šildymo režimas
   if (T_busena == 1) 
   {
    if (T >= temperatura_1) digitalWrite(Rele_T,LOW);
 }
 }
=======
/* +++++++++++++++++++++++++++ ANTRAS LYGIS ++++++++++++++++++++++++++++++++++++ */ 

/* +++++++++++++++++++++++++++ TREČIAS LYGIS ++++++++++++++++++++++++++++++++++++ */ 


// === LAIKRODIS ===========================================================



>>>>>>> parent of 389fdfb... pirmyn....:KKK_valdiklis_v2/KKK_valdiklis_v2.ino
}// === PABAIGA ===========================================================
