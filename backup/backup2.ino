#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentral 12
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 70
#define tournageVitesse 65
#define tournageVitesseMax 100

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
void gaucheMort(int v){ gauche_(); analogWrite(ENA,v/2); analogWrite(ENB,v); }
void gaucheLent(int v){ avancer_(); analogWrite(ENA,(v*9)/10); analogWrite(ENB,v); }

void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droiteMort(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v/2); }
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

/* === Virage à 90° (optimisé) === */
void tourner(String direction) {
  if (direction == "GAUCHE") {
    gauche(tournageVitesseMax);delay(300);
    avancer(baseVitesse);delay(300);
    gauche(tournageVitesseMax);
  }
  else if (direction == "DROITE") {
    droite(tournageVitesseMax);delay(300);
    avancer(baseVitesse);delay(300);
    droite(tournageVitesseMax);
  }
}

void virage90(String direction) {
  Serial.print("→ Virage 90° "); Serial.println(direction);
  unsigned long start = millis();

  if (direction == "GAUCHE") {
    tourner("GAUCHE");
    while (true) {
      readSensor();
      if (sensor[1] == LIGNE || sensor[2] == LIGNE || sensor[3] == LIGNE || sensor[4] == LIGNE) break;
      if (millis() - start > 2500) break;
      delay(10);
    }
  } 
  else if (direction == "DROITE") {
    tourner("DROITE");
    while (true) {
      readSensor();
      if (sensor[3] == LIGNE || sensor[2] == LIGNE || sensor[1] == LIGNE || sensor[0] == LIGNE) break;
      if (millis() - start > 2500) break;
      delay(10);
    }
  }
  stop();
}

/* === Demi-tour fluide au cul-de-sac === */
void demiTour(){
  Serial.println("→ Cul-de-sac, demi-tour");
  unsigned long start = millis();
  gauche(tournageVitesse);
  while (millis() - start < 3000) {
    readSensor();
    if (sensor[2]==LIGNE && (sensor[1]==LIGNE || sensor[3]==LIGNE)) {
      stop();
      return;
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
  
  else if (sensor[1] == LIGNE && sensor[2] == LIGNE) {
    gaucheLent(baseVitesse);
    dernierDirection = "GAUCHE";
    action = true;
  }
  else if (sensor[3] == LIGNE && sensor[2] == LIGNE) {
    droiteLent(baseVitesse);
    dernierDirection = "DROITE";
    action = true;
  }

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

  if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    if (dernierDirection == "GAUCHE") gauche(baseVitesse);
    else if (dernierDirection == "DROITE") droite(baseVitesse);
    else demiTour();
    dernierDirection = "U";
    action = true;
  }

  if (!action) {
    avancer(baseVitesse);
    dernierDirection = "AVANT";
  }

  delay(10);
}
