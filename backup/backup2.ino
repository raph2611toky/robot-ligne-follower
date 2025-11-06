#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 60
#define tournageVitesse 80
#define tournageVitesseSlow 70
#define correctionFactor 0.7

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
  analogWrite(ENA, v * correctionFactor);
  analogWrite(ENB, v);
}

void avancerPencheDroite(int v){
  avancer_();
  analogWrite(ENA, v);
  analogWrite(ENB, v * correctionFactor);
}

void virageOptimise(String direction) {
  int ligne = 0;
  int fond = 1;

  // démarrage du virage (petit coup initial pour amorcer)
  if (direction == "gauche") {
    gauche(tournageVitesse);
    delay(180);
  } else {
    droite(tournageVitesse);
    delay(180);
  }

  unsigned long start = millis();
  // on tourne tant que les 2 capteurs centraux ne détectent pas la ligne
  while (true) {
    readSensor();
    Serial.print("VirageOpt - S0:"); Serial.print(sensor[0]);
    Serial.print(" S1:"); Serial.print(sensor[1]);
    Serial.print(" S2:"); Serial.print(sensor[2]);
    Serial.print(" S3:"); Serial.println(sensor[3]);

    // condition d'arrêt : les deux capteurs centraux voient la ligne (ré-alignement)
    if (sensor[1] == ligne && sensor[2] == ligne) {
      stop();
      delay(80);
      Serial.println("Virage optimisé: centrage obtenu.");
      break;
    }

    // si on tourne gauche mais extrême droite voit ligne -> possible erreur, sortir
    if (direction == "gauche" && sensor[3] == ligne) {
      // on a rencontré la ligne à droite pendant un virage gauche -> sortir pour sécurité
      stop();
      delay(60);
      Serial.println("Virage interrompu: ligne droite détectée en cours de virage gauche.");
      break;
    }
    if (direction == "droite" && sensor[0] == ligne) {
      stop();
      delay(60);
      Serial.println("Virage interrompu: ligne gauche détectée en cours de virage droite.");
      break;
    }

    // continuer le virage doucement
    if (direction == "gauche") gauche(tournageVitesse);
    else droite(tournageVitesse);

    // sécurité anti-boucle infinie : si trop long sans retrouver ligne -> reculer légèrement et retenter
    if (millis() - start > 2500) {
      stop();
      delay(50);
      stop();
      delay(80);
      start = millis(); // reset timer pour nouvelle tentative
      Serial.println("VirageOptimise: correction de sécurité (recul) et reprise.");
      // reprendre le virage
      if (direction == "gauche") gauche(tournageVitesse);
      else droite(tournageVitesse);
      delay(150);
    }
  }

  // petit avancement pour bien se replacer sur la ligne
  avancer(baseVitesse);
  delay(160);
}


void loop() {
  readSensor();

  int ligne = 0; // ligne = blanc
  int fond = 1;  // fond = noir

  if (sensor[0] == ligne && sensor[3] == fond) {
    gauche(tournageVitesse);
  }

  else if (sensor[0] == fond && sensor[3] == ligne) {
    droite(tournageVitesse);
  }

  else if (sensor[0] == fond && sensor[3] == fond) {
    avancer(baseVitesse);
  }

  else if (sensor[0] == ligne && sensor[3] == ligne) {
    stop();
    delay(100); 

    avancer(tournageVitesse);
    delay(300);

  }

  delay(10);
}