	   int MeniuFunkcija (String tekstas1, int KeiciamaReiksme, int MaxReiksme, int MinReiksme, String tekstas2)
	        {
        lcd.setCursor(0,0);lcd.write(7);     
        lcd.setCursor(1,1);lcd.print(tekstas1); //("Nustatyta=   "); 
        lcd.setCursor(10,0);lcd.print(KeiciamaReiksme); // rodoma esama reikšmė
        int  veiksmas=-1; delay(1000);         // 
                                           
        while(veiksmas!=4)                   // 
         {
           klaviaturos_pasikeitimas=-1; 
           veiksmas=Klaviaturos_skaitymas(Key_Pin); //delay(300);  
                                            
           if(klaviaturos_pasikeitimas!=veiksmas)           
             {
             if (veiksmas==1) {KeiciamaReiksme++; if(KeiciamaReiksme>MaxReiksme) KeiciamaReiksme=MaxReiksme; lcd.setCursor(10,0);
                                                 lcd.print(KeiciamaReiksme); delay(200);}
             if(veiksmas==2)  {KeiciamaReiksme--; if(KeiciamaReiksme<MinReiksme) KeiciamaReiksme=MinReiksme; lcd.setCursor(10,0);
                                                 lcd.print(KeiciamaReiksme); delay(200);}
             if(veiksmas==4) // 0
               {
                 lcd.setCursor(0,0); lcd.print(tekstas2); delay(2000); // 0
                 lcd.setCursor(0,0); lcd.print("                "); // 0
                 //lcd.setCursor(1,0);lcd.print(eilute1);           // 0
               }
             } 
         } klaviaturos_pasikeitimas=veiksmas;  // aktualizacja klaviaturos_pasikeitimasiennej klaviaturos_pasikeitimas, po to aby reagowac tylko na klaviaturos_pasikeitimasiany stanu klawiatury
         // tu WAZNY MOMENT - konczy sie petla while i zwracamy sterowanie do gl�wnej petli loop()
         return KeiciamaReiksme;
      }
