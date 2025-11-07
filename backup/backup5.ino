#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 60
#define tournageVitesse 90
#define tournageVitesseSlow 70

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
      start=millis();
      if(direction=="gauche") gauche(tournageVitesse);
      else droite(tournageVitesse);
      delay(150);
    }
  }
  avancer(baseVitesse); delay(160);
}

void rechercheLigne(){
  Serial.println("→ Ligne perdue, lancement recherche");

  stop();
  delay(100);

  // Étape 1 : reculer légèrement
  reculer(baseVitesse);
  delay(250);
  stop();
  delay(100);

  bool ligneTrouvee = false;
  unsigned long startRecherche = millis();

  // Étape 2 : tourner sur place (360° max ~2,5s)
  Serial.println("→ Recherche en rotation droite");
  while(millis() - startRecherche < 2500){
    droite(tournageVitesseSlow);
    readSensor();
    if(sensor[0]==0 || sensor[1]==0 || sensor[2]==0 || sensor[3]==0){
      ligneTrouvee = true;
      break;
    }
  }

  stop();
  delay(100);

  // Étape 3 : si ligne non trouvée, reculer et tourner dans l’autre sens
  if(!ligneTrouvee){
    Serial.println("→ Ligne toujours perdue, deuxième tentative");
    reculer(baseVitesse);
    delay(200);
    stop();
    delay(100);

    startRecherche = millis();
    while(millis() - startRecherche < 2500){
      gauche(tournageVitesseSlow);
      readSensor();
      if(sensor[0]==0 || sensor[1]==0 || sensor[2]==0 || sensor[3]==0){
        ligneTrouvee = true;
        break;
      }
    }
  }

  stop();

  if(ligneTrouvee){
    Serial.println("→ Ligne retrouvée !");
    avancer(baseVitesse);
    delay(250);
  } else {
    Serial.println("→ Ligne introuvable après 2 essais, attente...");
    stop();
  }
}

void loop() {
  readSensor();

  const int ligne = 0;
  const int fond = 1;
  bool acted = false;

  // Cas intersection gauche
  if(sensor[0]==ligne && sensor[1]==ligne && sensor[2]==ligne){
    Serial.println("→ Intersection gauche");
    virageOptimise("gauche");
    acted = true;
  }
  // Cas intersection droite
  else if(sensor[1]==ligne && sensor[2]==ligne && sensor[3]==ligne){
    Serial.println("→ Intersection droite");
    virageOptimise("droite");
    acted = true;
  }
  // Cas croisement complet
  else if(sensor[0]==ligne && sensor[1]==ligne && sensor[2]==ligne && sensor[3]==ligne){
    Serial.println("→ Plein croisement, priorité à gauche");
    virageOptimise("gauche");
    acted = true;
  }
  // Cas normal (2 extrêmes utilisés)
  else if(sensor[0]==fond && sensor[3]==fond){
    Serial.println("→ Avancer tout droit");
    avancer(baseVitesse);
    acted = true;
  }
  // Cas penché gauche
  else if(sensor[0]==ligne && sensor[3]==fond){
    Serial.println("→ Penché vers la gauche");
    avancerPencheGauche(baseVitesse);
    acted = true;
  }
  // Cas penché droite
  else if(sensor[3]==ligne && sensor[0]==fond){
    Serial.println("→ Penché vers la droite");
    avancerPencheDroite(baseVitesse);
    acted = true;
  }

  if(!acted){
    Serial.println("→ Ligne perdue, stop");
    stop();
  }

  delay(30);
}