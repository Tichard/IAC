// FUNCTIONS \\

int play(int data) {
  // fonction qui joue la musique associé a l'id en parametre
  
  String ref = String(String(data) + ".wav");
  // ouverture du fichier
  start :
  File music = SD.open(ref);
  if (!music)
  {
    //si le fichier ne peux pas etre lu, on sort de la fonction
    return 0;
  }

  int volume = 0;

  //on enregistre l'heure de depart
  unsigned long play_start = millis();
  
  // 44100kHz stereo => 88200 sample rate
  // 100 mSec of prebuffering.
  Audio.begin(88200, 100);
  short buffer[sample];

  Serial.println("Playing");

  //tant que le fichier et disponible et que snooze n'est pas appuyé, on joue un sample
  while (music.available() && !get_Snooze())
  {
    // lecture du sample
    music.read(buffer, sizeof(buffer));

    // preparation
    Audio.prepare(buffer, sample, volume / 2);
    // envoie au port audio
    Audio.write(buffer, sample);

    //si le volume n'est pas a son maximum, on le monte
    if (volume < 2 * VOLUME_MAX) {
      volume ++;
    }
  }

  // on ferme le flux du fichier
  music.close();

  // A decommenter pour activer la lecture en boucle:
  
  //if (snooze_pressed == 0){goto start;}
  //else
  //{
  snooze_pressed = 0;

  // on enregistre l'heure de fin
  unsigned long play_end = millis();

  // on calcul le temps de lecture
  play_time = (int)(play_end - play_start)/1000;
  return 1;
  //}
}

void getTable() {
  
  Serial.println("transfer...");

  String trame;
  char data = (char) - 1;

  //tant que ';' n'est pas recu
  while ( data != ';')
  {
    data = BTshield.read();
    // on cree la chaine de donnees recues
    if (data != (char) - 1) {
      trame = String(trame + data);
    }
  }

  // on affiche sur le port de debug les donnees recus
  Serial.println(trame);

  // on enregistre les donnees dans la tables
  for (int i = 0; i < table_size; i++) {
    String temp = trame.substring(0, trame.indexOf(' '));
    table[i] = temp.toInt();
    trame = trame.substring(trame.indexOf(' ') + 1);
  }

  //on aquitte la reception
  acknowledge();
  Serial.println("transfer complete");
  
  //on remet le compteur de jour a zero
  day = 0;
}


void acknowledge()
{
  // on re-cree la chaine recu a partir de la table et on la renvoie 
  String ack;
  for (int i = 0; i < table_size; i++)
  {
    ack = String(ack + table[i] + ' ');
  }

  //ack.trim();
  ack = String(ack + ';');

  BTshield.println(ack);
  Serial.println(ack);
}

int get_Snooze()
{
  // si snooze est appuye, on met la varaiable a jour
  if ( digitalRead(snooze) == HIGH)
  {
    snooze_pressed = 1;
    return 1;
  }
  else
  {
    snooze_pressed = 0;
    return 0;
  }
}
