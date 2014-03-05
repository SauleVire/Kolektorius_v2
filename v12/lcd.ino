void LCD_T_sablonas(){
  lcd.setCursor(0,0); lcd.print("K"); lcd.setCursor(6,0); lcd.print(" S");
  lcd.setCursor(0,1); lcd.print("B"); lcd.setCursor(6,1); lcd.print(" T");
}

void Temperaturu_vaizdavimas(){
lcd.setCursor(1,0); lcd.print(K); lcd.setCursor(8,0); lcd.print(K-B);//(int(K + 0.5));
lcd.setCursor(1,1); lcd.print(B); lcd.setCursor(8,1); lcd.print(T);//(int(K + 0.5));
}

int  Busena (int termostato_busenos_reksme, char termostato_busenos_pavadinimas)
   (if (T_busena == 1) termostato_busenos_pavadinimas = 'saldymas';
    if (T_busena == 2) termostato_busenos_pavadinimas = sildymas;
    if (T_busena == 3) termostato_busenos_pavadinimas = isjungta;
return termostato_busenos_pavadinimas;
   )
