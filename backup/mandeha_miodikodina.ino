#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentral 12
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 70
#define tournageVitesse 50
#define tournageVitesseMax 75

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

String dernierDirection = "";

int sensor[5] = {0,0,0,0,0};

const int LIGNE = 0;
const int FOND = 1;

void setup() {
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurCentralGauche, INPUT);
  pinMode(capteurCentral, INPUT);
  pinMode(capteurCentralDroit, INPUT);
  pinMode(capteurExtremeDroit, INPUT);
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  Serial.begin(9600);
  delay(1000);
}

/* === Fonctions de base de mouvement === */
void avancer_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void avancer(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void reculer_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void reculer(int v){ reculer_(); analogWrite(ENA,v); analogWrite(ENB,v); }

void gauche_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); }
void gauche(int v){ gauche_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gaucheLent(int v){ avancer_(); analogWrite(ENA,v*0.8); analogWrite(ENB,v); }

void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droiteLent(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v*0.8); }

void stop(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); analogWrite(ENA,0); analogWrite(ENB,0); }

/* === Lecture capteurs === */
void readSensor(){
  sensor[0]=digitalRead(capteurExtremeGauche);
  sensor[1]=digitalRead(capteurCentralGauche); 
  sensor[2]=digitalRead(capteurCentral); 
  sensor[3]=digitalRead(capteurCentralDroit);
  sensor[4]=digitalRead(capteurExtremeDroit);
}

/* === Virage 90° === */

void tourner(String direction) {
  if (direction == "GAUCHE") {
    gauche(tournageVitesse);delay(100);
    gaucheLent(baseVitesse);delay(100);
    gauche(tournageVitesse);
  }
  else if (direction == "DROITE") {
    droite(tournageVitesse);delay(100);
    droiteLent(baseVitesse);delay(100);
    droite(tournageVitesse);
  }
}

void virage90(String direction) {
  unsigned long start = millis();
  const int timeout = 3000;
  dernierDirection = direction;

  if (direction == "GAUCHE") {
    // tourner(direction);
    gaucheLent(tournageVitesse);delay(100);
    while (millis() - start < timeout) { // tourne environ 90°
      gauche(tournageVitesse);
      readSensor();
      if ((sensor[3]==LIGNE || sensor[4]==LIGNE) && (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND)) {
        if (sensor[3]==LIGNE)droiteLent(baseVitesse);
        break;
      }
      // delay(10);
    }
  } 
  else if (direction == "DROITE") {
    // tourner(direction);
    gaucheLent(tournageVitesse);delay(100);
    while (millis() - start < timeout) {
      droite(tournageVitesse);
      readSensor();
      if ((sensor[1]==LIGNE || sensor[0]==LIGNE) && (sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND)) {
        if (sensor[1]==LIGNE)gaucheLent(baseVitesse);
        break;
      }
      // delay(10);
    }
  }
  // stop();
}

/* === Recherche de ligne perdue === */
void rechercherLigne() {
  unsigned long start = millis();
  if (dernierDirection == "GAUCHE") {
    droite(tournageVitesseMax);delay(150);
    // dernierDirection = "DROITE";
    while (millis() - start < 3000) {
      readSensor();
      if (sensor[1]==LIGNE || sensor[0]==LIGNE) return;
    }
  } else {
    gauche(tournageVitesseMax);delay(150);
    // dernierDirection = "GAUCHE";
    while (millis() - start < 3000) {
      readSensor();
      if (sensor[3]==LIGNE || sensor[4]==LIGNE) return;
    }
  }
  // stop();
}

/* === BOUCLE PRINCIPALE === */
void loop() {
  readSensor();

  if (sensor[4] == LIGNE) {
    virage90("DROITE");
    dernierDirection = "DROITE";
  }
  else if (sensor[0] == LIGNE) {
    virage90("GAUCHE");
    dernierDirection = "GAUCHE";
  }

  // --- Ligne centrale ---
  else if (sensor[2] == LIGNE) {
    if (sensor[3] == LIGNE) {
      droiteLent(baseVitesse);
    }
    else if (sensor[1] == LIGNE) {
      gaucheLent(baseVitesse);
    }
    else avancer(baseVitesse);
    dernierDirection = "AVANT";
  }
  
  // --- Ligne perdue ---
  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    rechercherLigne();
    dernierDirection = "U";
  }
  else {
    avancer(baseVitesse);
    dernierDirection = "AVANT";
  }
}
