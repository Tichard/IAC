// Include des bibliothèques utilisées
#include <SD.h>
#include <SPI.h>
#include <Audio.h>
#include <Scheduler.h>

// Cablage du Hardware :
/*
   Shield BLuetooth branché en UART3
   RxD -> Tx3
   TxD -> Rx3
   Vcc -> +3.3V
   Gnd -> Gnd

   Shield SD branché en SPI
   MISO -> MOSI(SPI_1)
   MOSI -> MISO(SPI_4)
   SCK -> SCK(SPI_3)
   CS -> Digital_IN_22
   Vcc -> +5V
   Gnd -> Gnd

   Bouton d'interruption "Snooze"
   B+ -> +3.3V
   B- -> Digital_IN_24 ->Rpull->Gnd
*/

// Definition des paramètres et des ports
#define BTshield Serial3
#define SDshield 22
#define baudrate 9600
#define sample 512
#define VOLUME_MAX 1023

#define snooze 24
#define table_size 14


//Declaration des globales
int table[table_size];
int day = 0;
int flag = 0;
int snooze_pressed = 0;
int play_time = 0;


//Function d'initialisation
void setup() {

  //Configuration des UARTs
  Serial.begin(baudrate);
  BTshield.begin(baudrate);
  pinMode(snooze, INPUT);

  // initialisation SD-card
  SD_init:
  Serial.print("Initializing SD card...");
  if (!SD.begin(SDshield)) {goto SD_init;}
  Serial.println("Done.");

  // Lancement de la boucle parallele
  Scheduler.startLoop(loop_getTable);
}

// Code principal
void loop()
{

  // quand on a assez attendu, on joue la sonnerie et actualise les variables
  if (flag >= table[day]*100)
  {
    play(table[day + 1]);
    day+=2;
    if(day == table_size){day=0;}
    flag = 0;
    table[day] = table[day]- play_time;
    play_time = 0;
   }

  //on attend 10ms
  delay(10);
  flag++;

  // On saute a la boucle parallele
  yield();
}

void loop_getTable()
{

  // si on lis 's', on met la table a jour
  if(BTshield.read()== 's')
  {  
    getTable();
  }

  //On revient a la boucle principale
  yield();
}

