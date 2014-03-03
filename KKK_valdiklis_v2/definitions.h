/* @@@@@@@@@@@@@@@@@@@@@@ testavimui @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
 #define DEBUG 
 #define SetWaitForConversionFALSE // pagreitintas ds18b20 temperatūrų matavimas
 #define DEBUGakumuliacine //akumuliacines testavimui
// #define DEBUGboileris // boilerio testavimui
// #define DEBUGkolektorius // saulės kolektoriaus testavimui
 #define DEBUGpv // pamaišymo vožtuvo testavimui
 #define DEBUGds18b20 // temperatūrų matavimo testavimui 
// #define DEBUGbusena

/* ********************** Laikai *************************************** */
unsigned long Meniu_praleistas_neaktyvus_laikas;
#define neaktyvus_meniu 20000
unsigned long temperaturu_matavimo_laikas_1 = 0;
unsigned long temperaturu_matavimo_laikas_2 = 0;
unsigned long temperaturu_matavimo_laikas_3 = 0;
#define temperaturu_matavimo_pertrauka_1 5000
#define temperaturu_matavimo_pertrauka_2 30000
#define temperaturu_matavimo_pertrauka_3 60000

unsigned long Ekrano_pasvietimo_ijungimo_laikas;
unsigned long  Ekrano_pasvietimo_pertrauka = 600000;
unsigned long Ekrano_rodmenu_atnaujinimo_laikas = 0;
#define Ekrano_rodmenu_atnaujinimo_pertrauka 5000
unsigned long AT_rankinio_ijungimo_laikas =0;
unsigned long AT_rankinio_ijungimo_trukme = 300000;
unsigned long B_rankinio_ijungimo_laikas =0;
unsigned long B_rankinio_ijungimo_trukme = 300000;
unsigned long SK_rankinio_ijungimo_laikas =0;
unsigned long SK_rankinio_ijungimo_trukme = 300000;
unsigned long PV_pauze =20000;
unsigned long PV_pauzes_pertrauka =20000;
unsigned long PV_atidarinejimo_laikas =0;
unsigned long PV_uzdarinejimo_laikas =0;
unsigned long PV_atidarinejimo_pertrauka =6000;
unsigned long PV_uzdarinejimo_pertrauka =6000;
byte PV_darinejimas = 60;
#define MV_judejimas 5500


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

/* ************************** davikliai *********************** */
#define ONE_WIRE_BUS1 2 // Kolektorius
#define ONE_WIRE_BUS2 9 // Boileris
#define ONE_WIRE_BUS3 A3 // Termostatas

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire K_t(ONE_WIRE_BUS1); //katilo isejimas
OneWire B_t(ONE_WIRE_BUS2); //saules kolektorius
OneWire T_t(ONE_WIRE_BUS3); //pamaisymo voztuvas

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature K_sensor(&K_t);
DallasTemperature B_sensor(&B_t);
DallasTemperature T_sensor(&T_t);

//kintamieji saugoti ismatuotoms temperaturu reiksmems
float K,B,T; 
/* ********** kintamieji Kieto kuro katilui ******************* */
/* ********** kintamieji Dujiniam katilui ******************* */
/* ********** kintamieji saulės kolektoriui ******************* */
byte k_skirtumas_ON = 5;
byte k_skirtumas_OFF = 3;
boolean k_uzsalimas = false;
boolean SK_rankinis_ijungimas = false; // Žymė rankiniam SK siurblio valdymui (nuorinimas)
/* *************************************************************** */
/* ********** kintamieji Akumuliacinei talpai ******************* */
byte at_ON_T = 90; // temperatūra akumuliacines talpos siurbliui įjungti
byte at_OFF_T = 89; // temperatūra akumuliacines talpos siurbliui įšjungti
byte AT_rankinis_ijungimas = 0; // Žymė rankiniam AT siurblio valdymui
/* ********************************************************************** */

/* ********************************************************************** */

/* ************************************************************************ */
/* ********** kintamieji siurbliams ******************* */
boolean SK_siurblys = false;
boolean PV_siurblys = false;
boolean AT_siurblys = false;
boolean B_siurblys = false;
/* ---------------- kintamieji motorizuotiems vožtuvams -------------------- */

