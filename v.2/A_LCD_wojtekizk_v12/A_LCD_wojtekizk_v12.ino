/*
Przykład użycia biblioteki MenuBackend do obsługi wielopoziomowego menu:
...by wojtekizk (wk@warcaby.lh.pl) przygotowany na forum majasterkowo.pl
//----------------------------------------------------------------------------------
Opis:
Generalnie tworzenie menu w oparciu o bibliotekę MenuBackend jest bardzo proste,
jeśli pamietać o kilku zasadach:
1) Najważniejsza jest konfiguracja Menu w funkcji menuSetup()
2) Poruszanie po menu powinno byc zgodne ze schematem menu, tzn. jesli
   coś jest submenu, to szukasz tego klawiszem "w prawo", a równorzędne "dół - góra"
3) Warto dla użytkownika wydrukować taka mapkę przy klawiaturce. 
4) Obsługę wszystkich opcji wrzucaj do funkcji void menuUseEvent(MenuUseEvent used)
   Tam zapuszczasz pętle while lub do while i czytasz w niej stan klawiaturki.
   W tej pętli modyfikujesz zachowanie programiku w zależności od tego, co wciśniesz. 
   Jeśli wciśniesz OK to wychodzisz z pętli while i de facto wracasz do loop-a.
Wszystkie ważniejsze kwestie opatrzono dość czytelnym komentarzem.   
*/
// ============= MenuBackend tutorial ===============================================
#include <MenuBackend.h>                 // dołączenie biblioteki
#include <LiquidCrystal.h>               // obsługa wyświetlacza LCD
// --- definiujemy dla LCD własne znaki strzałek: dół, lewo, prawo, gora-dół i powrót ---
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
LiquidCrystal lcd(A5, 3, 4, 5, 6, 7);    // definicja pinów dla LCD (sprawdź piny w swoim LCD)
volatile int zm =-1;               // to dla kontroli zmiany stanu klawiatury
volatile int x=-1;                 // zmienna pomocnicza
volatile int stan_Analog;          // wartość na wejściu Analogowym dla klawiaturki analogowej
char *linia1;                      // pierwsza linia wyświetlanego tekstu na LCD
char *linia2;                      // druga linia wyświetlanego tekstu na LCD
boolean InMenu = false;
// --- tworzymy wszystkie opcje Menu: ---------------------------------------
// de facto tworzymy obiekty klasy MenuItem, które dziedziczą po klasie MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // konstruktor 
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
                                           Serial.println("Dabar esame MenuRoot");
                                           lcd.clear();lcd.setCursor(0,1);lcd.print("    MenuRoot");
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
void setup()
{
  linia1=new char[16];  // zainicjowanie dynamicznego wskaźnika do tekstu 
  linia2=new char[16];  // to BARDZO WAŻNE, bo wskażnik dynamiczny musi wskazywać na 
                        // z góry określone miejsce w pamieci. Gdybyśmy tego nie zrobili
                        // to wcześniej czy później programik mógłby wskoczyć w nieokreślony 
                        //  bliżej obszar pamięci, co może skutkować nieodwracalnymi konsekwencjami
                        // łącznie z przestawieniem Fuse Bitów !!!  
                        // Proszę uważać na wszystkie dynamiczne wskaźniki, TAKA DOBRA RADA :-)
  Serial.begin(9600);   // inicjacja Seriala, głównie do testów 
  lcd.begin(16, 2);     // inicjacja LCD
  lcd.createChar(3,arrowLeft);    // tworzymy w pamięci LCD 5 własnych znaków dla strzałek
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  /* tu przykładowe piny cyfrowe dla 3 wersji funkcji czytaj_3(1,2,3,11,12)
   pinMode(1,INPUT);digitalWrite(1,HIGH);
   pinMode(2,INPUT);digitalWrite(2,HIGH);
   pinMode(3,INPUT);digitalWrite(3,HIGH); 
   pinMode(11,INPUT);digitalWrite(11,HIGH); 
   pinMode(12,INPUT);digitalWrite(12,HIGH); 
  */ 
  pinMode(0,OUTPUT);digitalWrite(0,LOW); // do testów 
  menuSetup();          // funkcja klasy MenuBackend - tu tak naprawdę tworzymy nasze menu 
  menu.moveDown();      // idziemy do pierwszej opcji - PLIK, moveDown bo pierwotnie byliśmy w root
                        // to tak jak w Awatarze drzewa rosną korzeniami do góry :-)
 int backLight = 8;
 pinMode(backLight, OUTPUT);
 digitalWrite(backLight, HIGH); // Įjungiamas apšvietimas. 
}
// --- I nadszedł czas na neverending story :-) -------------------------------------------- 
void loop()    
{
  x=czytaj_1(0);delay(50);             // odczytujemy stan klawiatury:
  /*
  Ja używam funkcji czytaj_1() bo mam akurat klawiaturkę podpiętą pod A0
  Jeśli masz inna klawiaturkę to użyj funkcji czytaj_2 lub czytaj_3 - patrz opis
  Ponadto musisz pamietać że w funkcji obsługo klawisza OK - menuUseEvent(MenuUseEvent used)
  także musisz użyć odpowiedniej wersji funkcji czytaj !!!
  */
  if(zm!=x)                               // jesli była zmiana stanu to :
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
    } zm=x;                               // przypisanie zmiennej zm wartości x po to, aby dluższe wciskanie tego
                                          // samego klawisza nie powodowało ponownej generacji zdarzenia. 
                                          // program reaguje na zmianę stanu klawiatury. 
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
if (InMenu == false){
  Serial.println("Dabar esame MenuRoot");
lcd.setCursor(0,1);lcd.print("    MenuRoot");
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

}
// === KONIEC ===========================================================


