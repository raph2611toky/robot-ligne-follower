#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 60
#define tournageVitesse 85

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

int sensor[4] = {0,0,0,0};

void setup() {
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurCentralGauche, INPUT);
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
void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void stop(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); analogWrite(ENA,0); analogWrite(ENB,0); }

/* === Lecture capteurs === */
void readSensor(){
  sensor[0]=digitalRead(capteurExtremeGauche);
  sensor[1]=digitalRead(capteurCentralGauche);
  sensor[2]=digitalRead(capteurCentralDroit);
  sensor[3]=digitalRead(capteurExtremeDroit);
}

/* === Demi-tour fluide au cul-de-sac === */
void demiTour(){
  Serial.println("→ Cul-de-sac, demi-tour");
  unsigned long start = millis();
  gauche(tournageVitesse);
  while (millis() - start < 3000) {
    readSensor();
    if(sensor[1]==0 || sensor[2]==0) { // ligne retrouvée
      stop();
      return;
    }
  }
  stop();
}

void loop() {
  readSensor();

  const int LIGNE = 0;
  const int FOND = 1;
  
  Serial.print(sensor[0]); Serial.print(" ");
  Serial.print(sensor[1]); Serial.print(" ");
  Serial.print(sensor[2]); Serial.print(" ");
  Serial.println(sensor[3]);


  if (sensor[0]==FOND && (sensor[1]==LIGNE || sensor[2]==LIGNE) && sensor[3]==FOND) {
    avancer(baseVitesse);
  }
  
  else if (sensor[0]==LIGNE && sensor[3]==FOND) {
    gauche(baseVitesse);
  }

  else if (sensor[0]==FOND && sensor[3]==LIGNE) {
    droite(baseVitesse);
  }

  else if (sensor[0]==LIGNE && sensor[1]==FOND && sensor[2]==FOND) {
    gauche(tournageVitesse);
  }

  else if (sensor[3]==LIGNE && sensor[1]==FOND && sensor[2]==FOND) {
    droite(tournageVitesse);
  }

  else if (sensor[0]==LIGNE && sensor[1]==LIGNE && sensor[2]==LIGNE && sensor[3]==LIGNE) {
    avancer(baseVitesse); // traverse tout droit par défaut
  }

  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND) {
    demiTour();
  }

  else {
    avancer(baseVitesse);
  }

  delay(10);
}