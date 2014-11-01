/* @@@@@@@@@@@@@@@@@@@@@@ testavimui @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
// #define DEBUG 
// #define SetWaitForConversionFALSE // pagreitintas ds18b20 temperatūrų matavimas

#define DEBUGds18b20 // temperatūrų matavimo testavimui 
// #define DEBUGbusena

/* ********************** Laikai *************************************** */
unsigned long Meniu_praleistas_neaktyvus_laikas;
#define neaktyvus_meniu 20000
unsigned long temperaturu_matavimo_laikas_1 = 0;
unsigned long Reliu_junginejimo_laikas = 0;
unsigned long temperaturu_matavimo_laikas_3 = 0;
#define temperaturu_matavimo_pertrauka_1 5000
#define Reliu_junginejimo_pertrauka 5000
#

unsigned long Ekrano_pasvietimo_ijungimo_laikas;
unsigned long  Ekrano_pasvietimo_pertrauka = 600000;
unsigned long Ekrano_rodmenu_atnaujinimo_laikas = 0;
#define Ekrano_rodmenu_atnaujinimo_pertrauka 5000


/* ************** LKLAVIATUROS KINTAMIEJI ************************************* */
#define Key_Pin 0
volatile int klaviaturos_pasikeitimas =-1;               // Tikrinti ar pasikeitė klaviatūros reikšmė
volatile int x=-1;                 // 
volatile int stan_Analog;          // 
/* ********** LCD aprasymas ******************* */
#define BackLight_Pin 8 //LCD pasvietimo isvadas (standart LCD KeeyPad use pin 10)
byte lcd_pasvietimas = 10; // lcd_pasvietimas * 25 = MAX LCD apsviestumas
boolean Backlighting = true; // zyme ekrano pasvietimui
//Siurblio įjungimo simbolis (rodyklė į viršų)
byte arrow_up[8]={ B00100,B01110,B11111,B00100,B00100,B00100,B00100,B00100};
//Siurblio įšungimo simbolis (rodyklė į žemyn)
byte arrow_down[8]={ B00100,B00100,B00100,B00100,B00100,B11111,B01110,B00100};

/* ******************** reles *********************************** */
#define Rele_K A1 // Kolektorius
#define Rele_T A2 // Termostatas
/* ************************** davikliai *********************** */
//#define ONE_WIRE_BUS1 2 // Kolektorius
//#define ONE_WIRE_BUS2 9 // Boileris
//#define ONE_WIRE_BUS3 A3 // Termostatas
//ROM = 28 22 8 CC 4 0 0 4D
//ROM = 28 4E 47 CC 4 0 0 75
//ROM = 28 F5 EE CB 4 0 0 CC
byte sensorK[8] = {0x28, 0x22, 0x08, 0xCC, 0x04, 0x00, 0x00, 0x4D};
byte sensorB[8] = {0x28, 0x4e, 0x47, 0xcc, 0x04, 0x00, 0x00, 0x75};
byte sensorT[8] = {0x28, 0xf5, 0xee, 0xcb, 0x04, 0x00, 0x00, 0xcc};

boolean Conv_start1 = false;
boolean Conv_start2 = false;
boolean Conv_start3 = false;
byte Busy1 = 0;
byte data1[2];
byte Busy2 = 0;
byte data2[2];
byte Busy3 = 0;
byte data3[2];
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire K_t(ONE_WIRE_BUS1); //katilo isejimas
//OneWire B_t(ONE_WIRE_BUS2); //saules kolektorius
//OneWire T_t(ONE_WIRE_BUS3); //pamaisymo voztuvas

// Pass our oneWire reference to Dallas Temperature. 
//DallasTemperature K_sensor(&K_t);
//DallasTemperature B_sensor(&B_t);
//DallasTemperature T_sensor(&T_t);

//kintamieji saugoti ismatuotoms temperaturu reiksmems
float K,B,T; 
/* ********** kintamieji Kieto kuro katilui ******************* */
/* ********** kintamieji Dujiniam katilui ******************* */
/* ********** kintamieji saulės kolektoriui ******************* */
byte k_ijungimo_skirtumas = 5;
byte k_isjungimo_skirtumas = 3;
boolean k_uzsalimas = false;
boolean SK_nuorinimas = false; // Žymė rankiniam SK siurblio valdymui (nuorinimas)
/* ********** kintamieji Termostatui ******************* */
byte temperatura_1 = 20;
byte temperatura_2 = 25;
byte T_busena =1;
/* *************************************************************** */
/* ********** kintamieji Akumuliacinei talpai ******************* */
/* ********************************************************************** */
/* ************************************************************************ */
/* ********** kintamieji siurbliams ******************* */
/* ---------------- kintamieji motorizuotiems vožtuvams -------------------- */


