#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 90
#define tournageVitesse 140
#define tournageVitesseFast 180

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

int sensor[4] = {0,0,0,0};

void setup(){
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurCentralGauche, INPUT);
  pinMode(capteurCentralDroit, INPUT);
  pinMode(capteurExtremeDroit, INPUT);
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  Serial.begin(115200);
  delay(200);
}

void avancer_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void avancer(int vitesse){ avancer_(); analogWrite(ENA, vitesse); analogWrite(ENB, vitesse); }
void reculer_(){ digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
void reculer(int vitesse){ reculer_(); analogWrite(ENA, vitesse); analogWrite(ENB, vitesse); }
void droite_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
void droite(int vitesse){ droite_(); analogWrite(ENA, vitesse); analogWrite(ENB, vitesse); }
void gauche_(){ digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void gauche(int vitesse){ gauche_(); analogWrite(ENA, vitesse); analogWrite(ENB, vitesse); }
void stop(){ digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); analogWrite(ENA,0); analogWrite(ENB,0); }

void readSensor(){
  sensor[0]=digitalRead(capteurExtremeGauche);
  sensor[1]=digitalRead(capteurCentralGauche);
  sensor[2]=digitalRead(capteurCentralDroit);
  sensor[3]=digitalRead(capteurExtremeDroit);
}

// avancer penché gauche / droite
void avancerPencheGauche(int v){
  avancer_();
  analogWrite(ENA, v/2);
  analogWrite(ENB, v);
}
void avancerPencheDroite(int v){
  avancer_();
  analogWrite(ENA, v);
  analogWrite(ENB, v/2);
}

void virageOptimise(String direction){
  int ligne = 0;
  int fond = 1;

  if (direction == "gauche") gauche(tournageVitesse);
  else droite(tournageVitesse);
  delay(180);

  unsigned long start = millis();
  while(true){
    readSensor();
    if(sensor[1]==ligne && sensor[2]==ligne) break;
    if(direction=="gauche" && sensor[3]==ligne) break;
    if(direction=="droite" && sensor[0]==ligne) break;
    if(direction=="gauche") gauche(tournageVitesse);
    else droite(tournageVitesse);
    if(millis()-start > 2500){
      stop(); delay(50);
      reculer(baseVitesse); delay(120);
      stop(); delay(80);
      start=millis();
      if(direction=="gauche") gauche(tournageVitesse);
      else droite(tournageVitesse);
      delay(150);
    }
  }
  avancer(baseVitesse); delay(160);
}

void loop() {
  readSensor();

  const int ligne = 0;
  const int fond = 1;
  bool acted = false;

  // --- 1. Ligne droite : deux capteurs centraux sur la ligne ---
  if(sensor[1]==ligne && sensor[2]==ligne && sensor[0]==fond && sensor[3]==fond) {
    avancer(baseVitesse);
    acted = true;
  }

  // --- 2. Correction légère ---
  else if(sensor[1]==ligne && sensor[2]==fond && sensor[0]==fond && sensor[3]==fond) {
    avancerPencheGauche(baseVitesse);
    acted = true;
  }
  else if(sensor[2]==ligne && sensor[1]==fond && sensor[0]==fond && sensor[3]==fond) {
    avancerPencheDroite(baseVitesse);
    acted = true;
  }

  // --- 3. Virage modéré (2-3 capteurs) ---
  else if(sensor[0]==ligne && (sensor[1]==ligne || sensor[2]==ligne)) {
    // tourne jusqu’à retrouver ligne centrale
    while(true){
      readSensor();
      gauche(tournageVitesse);
      if(sensor[1]==ligne && sensor[2]==ligne) break;
    }
    acted = true;
  }
  else if(sensor[3]==ligne && (sensor[2]==ligne || sensor[1]==ligne)) {
    while(true){
      readSensor();
      droite(tournageVitesse);
      if(sensor[1]==ligne && sensor[2]==ligne) break;
    }
    acted = true;
  }

  // --- 4. Intersection : tous les capteurs sur ligne ---
  else if(sensor[0]==ligne && sensor[1]==ligne && sensor[2]==ligne && sensor[3]==ligne) {
    avancer(baseVitesse); // traverse un peu l’intersection
    // Choix direction (exemple : priorité à gauche)
    while(true){
      readSensor();
      gauche(tournageVitesse);
      if(sensor[1]==ligne && sensor[2]==ligne) break;
    }
    acted = true;
  }

  // --- 5. Fin de ligne (cul-de-sac) ---
  else if(sensor[0]==fond && sensor[1]==fond && sensor[2]==fond && sensor[3]==fond) {
    // vérifier si c’est temporaire (petit saut de ligne) ou vrai cul-de-sac
    unsigned long t0 = millis();
    bool retrouve_ = false;
    while(millis() - t0 < 300) {
      readSensor();
      if(sensor[1]==ligne || sensor[2]==ligne) { retrouve_=true; break; }
    }
    if(!retrouve_) {
      // vrai cul-de-sac : demi-tour fluide
      while(true){
        readSensor();
        gauche(tournageVitesseFast);
        if(sensor[1]==ligne && sensor[2]==ligne) break;
      }
    }
    acted = true;
  }

  // --- 6. Par défaut ---
  if(!acted) {
    avancer(baseVitesse);
  }
}
