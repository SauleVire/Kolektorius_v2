void LCD_T_sablonas(){
  lcd.setCursor(0,0); lcd.print("K"); lcd.setCursor(6,0); lcd.print(" S");
  lcd.setCursor(0,1); lcd.print("B"); lcd.setCursor(6,1); lcd.print(" T");
}

void Temperaturu_vaizdavimas(){
lcd.setCursor(1,0); lcd.print(K); lcd.setCursor(8,0); lcd.print(K-B);//(int(K + 0.5));
lcd.setCursor(1,1); lcd.print(B); lcd.setCursor(8,1); lcd.print(T);//(int(K + 0.5));
}
/*
String  Busena (int termostato_busenos_reksme, String termostato_busenos_pavadinimas)
   {if (termostato_busenos_reksme == 1) termostato_busenos_pavadinimas = "saldymas";
    if (termostato_busenos_reksme == 2) termostato_busenos_pavadinimas = "sildymas";
    if (termostato_busenos_reksme == 3) termostato_busenos_pavadinimas = "isjungta";
return termostato_busenos_pavadinimas;
}
*/
