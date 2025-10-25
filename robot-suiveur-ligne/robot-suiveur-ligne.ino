#define capteurExtremeGauche 2
#define capteurCentreGauche 3
#define capteurCentreDroite 4
#define capteurExtremeDroite 5

#define baseVitesse 100
#define tournageVitesse 95

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

void setup() {
  // Configuration des capteurs
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurCentreGauche, INPUT);
  pinMode(capteurCentreDroite, INPUT);
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
void reculer(int vitesse) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Reculer
void avancer(int vitesse) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Tourner à gauche
void gauche(int vitesse) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

// Tourner à droite
void droite(int vitesse) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
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
  avancer(baseVitesse);
  delay(1000);
  droite(tournageVitesse);
  delay(1000);
  gauche(tournageVitesse);
  delay(1000);
  reculer(baseVitesse);
  delay(1000);
  stopMoteur();
  delay(3000);

}
