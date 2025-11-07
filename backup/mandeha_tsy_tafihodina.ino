#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentral 12
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 50
#define tournageVitesse 75

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

int sensor[5] = {0,0,0,0,0};
String dernierDirection = "";

void setup() {
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurCentralGauche, INPUT);
  pinMode(capteurCentral, INPUT);
  pinMode(capteurCentralDroit, INPUT);
  pinMode(capteurExtremeDroit, INPUT);
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  Serial.begin(115200);
  delay(300);
}

/* === Fonctions de base de mouvement === */
void avancer_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void avancer(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void reculer_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void reculer(int v){ reculer_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gauche_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); }
void gauche(int v){ gauche_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gaucheLent(int v){ avancer_(); analogWrite(ENA,(v*2)/3); analogWrite(ENB,v); }
void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droiteLent(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,(v*2)/3); }
void stop(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); analogWrite(ENA,0); analogWrite(ENB,0); }

/* === Lecture capteurs === */
void readSensor(){
  sensor[0]=digitalRead(capteurExtremeGauche);
  sensor[1]=digitalRead(capteurCentralGauche); 
  sensor[2]=digitalRead(capteurCentral); 
  sensor[3]=digitalRead(capteurCentralDroit);
  sensor[4]=digitalRead(capteurExtremeDroit);
}

/* === Virage à 90° (optimisé) === */
void virage90(String direction) {
  const int LIGNE = 0;
  const int FOND = 1;

  Serial.print("→ Virage 90° "); Serial.println(direction);
  unsigned long start = millis();

  if (direction == "GAUCHE") {
    gauche(tournageVitesse);
    delay(200); // amorce de rotation
    while (true) {
      readSensor();
      // quand le capteur central ou central droit retrouve la ligne -> fin du virage
      if (sensor[2] == LIGNE || sensor[3] == LIGNE) break;
      // sécurité anti-boucle infinie
      if (millis() - start > 2500) break;
    }
  } 
  else if (direction == "DROITE") {
    droite(tournageVitesse);
    delay(200);
    while (true) {
      readSensor();
      if (sensor[2] == LIGNE || sensor[1] == LIGNE) break;
      if (millis() - start > 2500) break;
    }
  }

  stop();
  delay(80);
  avancer(baseVitesse);
  delay(120);
  stop();
  Serial.println("↳ Fin du virage 90°");
}

/* === Demi-tour fluide au cul-de-sac === */
void demiTour(){
  Serial.println("→ Cul-de-sac, demi-tour");
  unsigned long start = millis();
  gauche(tournageVitesse);
  while (millis() - start < 3000) {
    readSensor();
    if(sensor[1]==0 || sensor[2]==0) { // LIGNE retrouvée
      stop();
      return;
    }
  }
  stop();
}

/* === BOUCLE PRINCIPALE === */
void loop() {
  readSensor();

  const int LIGNE = 0;
  const int FOND = 1;

  Serial.print(sensor[0]); Serial.print(" ");
  Serial.print(sensor[1]); Serial.print(" ");
  Serial.print(sensor[2]); Serial.print(" ");
  Serial.print(sensor[3]); Serial.print(" ");
  Serial.println(sensor[4]);

  bool action = false;

  // === PRIORITÉ : virages détectés par capteurs extrêmes ===
  if (sensor[0] == LIGNE) {
    virage90("GAUCHE");
    dernierDirection = "GAUCHE";
    action = true;
  }
  else if (sensor[4] == LIGNE) {
    virage90("DROITE");
    dernierDirection = "DROITE";
    action = true;
  }

  // === Ligne droite (centrale) ===
  else if (sensor[2] == LIGNE && sensor[1] == FOND && sensor[3] == FOND) {
    avancer(baseVitesse);
    dernierDirection = "AVANT";
    action = true;
  }

  // === Correction légère gauche/droite ===
  // else if (sensor[1] == LIGNE && sensor[2] == FOND) {
  //   gauche(baseVitesse);
  //   dernierDirection = "GAUCHE";
  //   action = true;
  // }
  // else if (sensor[3] == LIGNE && sensor[2] == FOND) {
  //   droite(baseVitesse);
  //   dernierDirection = "DROITE";
  //   action = true;
  // }

  else if (sensor[0]==FOND && sensor[2]==LIGNE && sensor[4]==FOND) {
    if (sensor[1]==FOND && sensor[3]==LIGNE){
      gaucheLent(baseVitesse);
    }
    if (sensor[1]==LIGNE && sensor[3]==FOND){
      droiteLent(baseVitesse);
    }
    else {
      avancer(baseVitesse);
    }
    dernierDirection = "AVANT";
    action = true;
  }

  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    if (dernierDirection == "GAUCHE") gauche(baseVitesse);
    else if (dernierDirection == "DROITE") droite(baseVitesse);
    else avancer(baseVitesse);
    dernierDirection = "AVANT";
    action = true;
  }

  if (!action) {
    avancer(baseVitesse);
    dernierDirection = "AVANT";
  }

  delay(10);
}
