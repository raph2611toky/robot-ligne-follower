#define capteurExtremeGauche 2
#define capteurExtremeDroite 5

#define baseVitesse 160
#define tournageVitesse 200
#define tournageVitesseLow 150

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

void setup() {
  // Configuration des capteurs
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurExtremeDroite, INPUT);

  // Configuration des moteurs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
}

// ====================
// FONCTIONS MOTEURS
// ====================

// Avancer
void avancer(int vitesse) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Reculer
void reculer(int vitesse) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Tourner à gauche
void gauche(int vitesse) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, tournageVitesseLow);
  analogWrite(ENB, tournageVitesse);
}

// Tourner à droite
void droite(int vitesse) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, tournageVitesse);
  analogWrite(ENB, tournageVitesseLow);
}

// Stopper les moteurs
void stopMoteur() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ====================
// BOUCLE PRINCIPALE
// ====================

void loop() {
  int gaucheVal = digitalRead(capteurExtremeGauche);
  int droiteVal = digitalRead(capteurExtremeDroite);

  Serial.print("Gauche: ");
  Serial.print(gaucheVal);
  Serial.print(" | Droite: ");
  Serial.println(droiteVal);

  if (gaucheVal == 0 && droiteVal == 0) {
    avancer(baseVitesse);
  }
  
  else if (gaucheVal == 1 && droiteVal == 0) {
    gauche(tournageVitesse);
  }
  
  else if (gaucheVal == 0 && droiteVal == 1) {
    droite(tournageVitesse);
  }
  
  else if (gaucheVal == 1 && droiteVal == 1) {
    stopMoteur();
    delay(100); 
    
    gauche(tournageVitesse);
    delay(300);
    
  }

  delay(10);
}
