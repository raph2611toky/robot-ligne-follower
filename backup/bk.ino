#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentral 12
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 90
#define tournageVitesse 50
#define tournageVitesseMax 75

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

int currentSpeedA = 0;
int currentSpeedB = 0;

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

void setMotorSpeed(int targetA, int targetB) {
  int step = 5; 
  if (currentSpeedA < targetA) currentSpeedA += step;
  else if (currentSpeedA > targetA) currentSpeedA -= step;

  if (currentSpeedB < targetB) currentSpeedB += step;
  else if (currentSpeedB > targetB) currentSpeedB -= step;

  currentSpeedA = constrain(currentSpeedA, 0, 255);
  currentSpeedB = constrain(currentSpeedB, 0, 255);

  analogWrite(ENA, currentSpeedA);
  analogWrite(ENB, currentSpeedB);
}

/* === Fonctions de base de mouvement === */
void avancer_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void avancer() {
  avancer_();
  setMotorSpeed(baseVitesse, baseVitesse);
}

void reculer_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void reculer(int v){ reculer_(); analogWrite(ENA,v); analogWrite(ENB,v); }

void gauche_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); }
void gauche(int v){ gauche_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gaucheLent(){ avancer_(); setMotorSpeed(baseVitesse * 0.8, baseVitesse); }
void gaucheRapide(){ gauche_(); setMotorSpeed(tournageVitesseMax, tournageVitesseMax); }

void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droiteLent(){ avancer_(); setMotorSpeed(baseVitesse, baseVitesse * 0.8); }
void droiteRapide(){ droite_(); setMotorSpeed(tournageVitesseMax, tournageVitesseMax); }

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
    gaucheLent();delay(100);
    gauche(tournageVitesse);
  }
  else if (direction == "DROITE") {
    droite(tournageVitesse);delay(100);
    droiteLent();delay(100);
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
      if (sensor[4]==LIGNE && (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND)) break;
      // delay(10);
    }
  } 
  else if (direction == "DROITE") {
    tourner(direction);
    while (millis() - start < 1800) {
      readSensor();
      if (sensor[0]==LIGNE && (sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND)) break;
      // delay(10);
    }
  }
  stop();
}

/* === Recherche de ligne perdue === */
void rechercherLigne() {
  unsigned long start = millis();
  if (dernierDirection == "GAUCHE") {
    while (millis() - start < 1500) {
      droiteRapide();
      readSensor();
      if (sensor[2]==LIGNE || sensor[1]==LIGNE) return;
    }
  } else {
    while (millis() - start < 1500) {
      gaucheRapide();
      readSensor();
      if (sensor[2]==LIGNE || sensor[3]==LIGNE) return;
    }
  }
  avancer();
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
  else if (sensor[2] == LIGNE) {
    if (sensor[3] == LIGNE) droiteLent();
    else if (sensor[1] == LIGNE) gaucheLent();
    else avancer();
    dernierDirection = "AVANT";
  }
  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    rechercherLigne();
    dernierDirection = "U";
  }
  else {
    avancer();
    dernierDirection = "AVANT";
  }
}

