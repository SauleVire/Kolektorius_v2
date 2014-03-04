// ============= KKK valdinlis v2.1 ===============================================


#include <Wire.h>
//#include <DS1307.h>

#include <MenuBackend.h>        
 #include <LiquidCrystal.h>         
  #include <OneWire.h>
   #include <DallasTemperature.h>

    #include <EEPROM.h>
//     #include <PID_v1.h>
#include "definitions.h"
#include "config.h"    

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
      MenuItem P13 = MenuItem("Irasyti nustat",2);
      MenuItem P14 = MenuItem("Nuorinimas    ",2);

   MenuItem P2 = MenuItem("TERMOSTATAS   ",1);
      MenuItem P21 = MenuItem("temperatura 1 ",2);
      MenuItem P22 = MenuItem("temperatura 2 ",2);
      MenuItem P23 = MenuItem("busena        ",2);


/* --- Teraz pozycjonujemy  menu ( zgodnie z ustawieniem podanym powyżej) ------------
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
void menuUseEvent(MenuUseEvent used)      // funkcja klasy MenuBackend - reakcja na wciśnięcie OK
                                          // tutaj właśnie oddajemy menu na rzecz akcji obsługi klawisza OK
{
   Serial.print("pasirinkta:  "); Serial.println(used.item.getName()); // do testów, potem niepotrzebne
   // --- ponizej kilka przykładów obsługi  opcji -----------
   // przykładowa reakcja na wcisnięcie klawisza OK w opcji Otworz :
/* __________________________ NUSTATYMAI Ijungimo skirtumo temperatura          _______________________ */
if (used.item.getName() == "Ijungimo sk. t")   // dokladnie taki sam ciag " Temperatura"
k_ijungimo_skirtumas =  MeniuFunkcija ("Nustatyta=", k_ijungimo_skirtumas, 25, 1, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* __________________________ NUSTATYMAI Isjungimo skirtumo temperatura          _______________________ */     
if (used.item.getName() == "Isjungimo sk.t")   // dokladnie taki sam ciag " Temperatura"
k_isjungimo_skirtumas =  MeniuFunkcija ("Nustatyta=", k_isjungimo_skirtumas, 25, 1, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ NUSTATYMAI Irasymas____________________________________ */
     if (used.item.getName() == "Irasyti nustat")   // dokładnie taki sam ciąg " Temperatura"
      {
                 SaveConfig();
                 lcd.setCursor(0,0);lcd.print(">Irasyta OK        ");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(0,0);lcd.print("                    "); // czyścimy linię
                // lcd.setCursor(0,0);lcd.print("*");lcd.print(eilute1);           // odtwarzamy poprzedni stan na LCD
                // lcd.setCursor(19,0);lcd.print("*");

      }
/* __________________________ Termostatas temperatura 1   _______________________ */
if (used.item.getName() == "temperatura 1 ")   // dokladnie taki sam ciag " Temperatura"
temperatura_1 =  MeniuFunkcija ("Nustatyta=", temperatura_1, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////
/* __________________________ Termostatas temperatura 2  _______________________ */     
if (used.item.getName() == "temperatura 2 ")   // dokladnie taki sam ciag " Temperatura"
temperatura_2 =  MeniuFunkcija ("Nustatyta=", temperatura_2, 99, -25, ">Temperatura OK"); 
     ///////////////////////////////////////////////////////////////////     
/* __________________________ Termostatas busena  _______________________ */     
if (used.item.getName() == "busena        ") 
 {
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print("Busena-");if (T_busena == 1) lcd.print("saldymas"); else lcd.print("sildymas"); //("Nustatyta=   "); 
        //lcd.setCursor(10,0);lcd.print(KeiciamaReiksme); // rodoma esama reikšmė
        int  veiksmas=-1; delay(1000);         // 
                                           
        while(veiksmas!=4)                   // 
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);  
                                            
           if(klaviaturos_pasikeitimas!=veiksmas)           
             {
             if (veiksmas==1) {T_busena++; if(T_busena>2) T_busena=2; lcd.setCursor(10,0);
                                                 lcd.setCursor(8,1); lcd.print("saldymas"); delay(200);}
             if(veiksmas==2)  {T_busena--; if(T_busena<1) T_busena=1; lcd.setCursor(10,0);
                                                 lcd.setCursor(8,1); lcd.print("sildymas"); delay(200);}
             if(veiksmas==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print("Busena        OK"); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print("                "); // 0
                 //lcd.setCursor(1,0);lcd.print(eilute1);           // 0
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;
 }
 
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
    lcd.setCursor(0,2);lcd.print("                    "); // išvaloma 3 eilutė, kai atsiduriama meniu nr.1
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
 LoadConfig(); 

  /* ********************************************************* */

  pinMode(BackLight_Pin, OUTPUT);
    digitalWrite(BackLight_Pin,HIGH);
  eilute1=new char[20]; 
  eilute2=new char[20];
  eilute3=new char[20];  

                        
  Serial.begin(9600);   
  lcd.begin(16, 2);    
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
  pinMode(Rele_K,OUTPUT);pinMode(Rele_T,OUTPUT);
  digitalWrite(Rele_K,HIGH);digitalWrite(Rele_T,HIGH);
  menuSetup(); 
//  menu.moveUp();      
  Temperaturu_matavimas_1();

 
  LCD_T_sablonas();
  Temperaturu_vaizdavimas();
    Ekrano_pasvietimo_ijungimo_laikas = millis();
    temperaturu_matavimo_laikas_1 = millis();


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


// matuojamos temperatūros nurodytais laiko intervalais (temperaturu_matavimo_pertrauka)
/* +++++++++++++++++++++++++++ PIRMAS LYGIS ++++++++++++++++++++++++++++++++++++ */ 
if (millis() > temperaturu_matavimo_laikas_1 ) { 
  temperaturu_matavimo_laikas_1 = millis() + temperaturu_matavimo_pertrauka_1;
  Temperaturu_matavimas_1();}
//------------------ kolektoriaus siurblio ir termostato valdymas-----------------------//
if (millis() > Reliu_junginejimo_laikas ) 
 {
   Reliu_junginejimo_laikas=millis()+Reliu_junginejimo_pertrauka;
   if (K-B>=k_ijungimo_skirtumas) digitalWrite(Rele_K,LOW);
   if (K-B<=k_isjungimo_skirtumas) digitalWrite(Rele_K,HIGH);
   //Jei šildymo režimas
   if T_busena == 1 
   {
    if (T >= temperatura_1) digitalWrite(Rele_T,LOW);
 }

}// === PABAIGA ===========================================================
