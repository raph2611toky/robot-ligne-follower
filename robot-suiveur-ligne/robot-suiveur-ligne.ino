#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentral 12
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 60
#define tournageVitesse 50
#define tournageVitesseMax 55

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

int sensor[5] = {0,0,0,0,0};
String dernierDirection = "";

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
void gaucheLent(int v){ avancer_(); analogWrite(ENA,(v*9)/10); analogWrite(ENB,v); }

void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droiteLent(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,(v*9)/10); }

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
  Serial.print("→ Virage 90° "); Serial.println(direction);
  unsigned long start = millis();
  dernierDirection = direction;

  if (direction == "GAUCHE") {
    tourner(direction);
    while (millis() - start < 1800) { // tourne environ 90°
      readSensor();
      if (sensor[4]==LIGNE) break;
      delay(10);
    }
  } 
  else if (direction == "DROITE") {
    tourner(direction);
    while (millis() - start < 1800) {
      readSensor();
      if (sensor[0]==LIGNE) break;
      delay(10);
    }
  }
  stop();
}

/* === Recherche de ligne perdue === */
void rechercherLigne() {
  Serial.println("→ Ligne perdue, recherche…");
  unsigned long start = millis();
  if (dernierDirection == "GAUCHE") {
    droite(tournageVitesse);
    dernierDirection = "DROITE";
    while (millis() - start < 2500) {
      readSensor();
      if (sensor[2]==LIGNE || sensor[1]==LIGNE || sensor[3]==LIGNE) return;
    }
  } else {
    gauche(tournageVitesse);
    dernierDirection = "GAUCHE";
    while (millis() - start < 2500) {
      readSensor();
      if (sensor[2]==LIGNE || sensor[1]==LIGNE || sensor[3]==LIGNE) return;
    }
  }
  stop();
}

/* === BOUCLE PRINCIPALE === */
void loop() {
  readSensor();

  Serial.print(sensor[0]); Serial.print(" ");
  Serial.print(sensor[1]); Serial.print(" ");
  Serial.print(sensor[2]); Serial.print(" ");
  Serial.print(sensor[3]); Serial.print(" ");
  Serial.println(sensor[4]);

  // --- Correction douce selon capteurs extrêmes ---
  if (sensor[0] == LIGNE || sensor[4] == LIGNE){
    if (sensor[0] == LIGNE && sensor[4] == LIGNE){
      if (dernierDirection=="GAUCHE"){
        virage90("DROITE");
        dernierDirection = "DROITE";
      }
      else if (dernierDirection=="DROITE"){
        virage90("GAUCHE");
        dernierDirection = "GAUCHE";
      }
    }
    if (sensor[0] == LIGNE) {
      virage90("GAUCHE");
      dernierDirection = "GAUCHE";
    }
    else if (sensor[4] == LIGNE) {
      virage90("DROITE");
      dernierDirection = "DROITE";
    }
    delay(50);
  }
  // --- Ligne centrale ---
  else if (sensor[2] == LIGNE) {
    avancer(baseVitesse);
    dernierDirection = "AVANT";
  }
  // --- Légère déviation à gauche ---
  else if (sensor[1] == LIGNE && sensor[2]==FOND) {
    gaucheLent(baseVitesse);
    dernierDirection = "GAUCHE";
  }
  // --- Légère déviation à droite ---
  else if (sensor[3] == LIGNE && sensor[2]==FOND) {
    droiteLent(baseVitesse);
    dernierDirection = "DROITE";
  }

  else if (sensor[0]==FOND && sensor[2]==LIGNE && sensor[4]==FOND) { 
    avancer(baseVitesse); 
    // if (sensor[1]==FOND && sensor[3]==LIGNE){ 
    //   gaucheLent(baseVitesse); 
    // } if (sensor[1]==LIGNE && sensor[3]==FOND){ 
    //   droiteLent(baseVitesse); 
    // } 
    // else { 
    //   avancer(baseVitesse); 
    // } 
    dernierDirection = "AVANT";
  }
  
  // --- Ligne perdue ---
  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    rechercherLigne();
  }
  else {
    avancer(baseVitesse);
    dernierDirection = "AVANT";
  }

  delay(10);
}
