#define capteurExtremeGauche 2
#define capteurExtremeDroite 5

#define baseVitesse 90
#define tournageVitesse 100

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
void gauche_() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void gauche(int vitesse) {
  gauche_();
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Tourner à droite
void droite_() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void droite(int vitesse) {
  droite_();
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Stopper les moteurs
void stop() {
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

  int noir = 1;
  int blanc = 0;

  Serial.print("Gauche: ");
  Serial.print(gaucheVal);
  Serial.print(" | Droite: ");
  Serial.println(droiteVal);

  if (gaucheVal == blanc && droiteVal == noir) {
    gauche(tournageVitesse);
  }
  
  else if (gaucheVal == noir && droiteVal == blanc) {
    droite(tournageVitesse);
  }

  else if (gaucheVal == noir && droiteVal == noir) {
    avancer(baseVitesse);
  }
  
  else if (gaucheVal == blanc && droiteVal == blanc) {
    stop();
    delay(100); 
    
    avancer(tournageVitesse);
    delay(300);
    
  }

  delay(10);
}
