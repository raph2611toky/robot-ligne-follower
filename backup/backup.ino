#define S1 2
#define S2 3
#define S3 4
#define S4 5

#define ENA 10
#define IN1 6
#define IN2 7
#define ENB 11
#define IN3 9
#define IN4 8

#define baseVitesse 90
#define tournageVitesse 100

#define ligne 0 // blanc
#define fond 1  // noir

char chemin[200];
int indexChemin = 0;

void setup() {
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
}

void avancer(int v) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, v);
  analogWrite(ENB, v);
}

void stopMoteurs() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void gauche(int v) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, v);
  analogWrite(ENB, v);
}

void droite(int v) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, v);
  analogWrite(ENB, v);
}

// Demi-tour sur place
void demiTour() {
  droite(tournageVitesse);
  delay(650); // ajuster selon ta vitesse / ton robot
  stopMoteurs();
}

void loop() {
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);

  Serial.print(s1); Serial.print(s2); Serial.print(s3); Serial.println(s4);

  // === SUIVI NORMAL DE LIGNE ===
  if ((s2 == ligne && s3 == ligne) && (s1 == fond && s4 == fond)) {
    avancer(baseVitesse);
  }
  else if (s1 == ligne || s2 == ligne) {
    gauche(tournageVitesse);
  }
  else if (s3 == ligne || s4 == ligne) {
    droite(tournageVitesse);
  }

  // === INTERSECTION (T ou +) ===
  else if (s1 == ligne && s2 == ligne && s3 == ligne && s4 == ligne) {
    stopMoteurs();
    delay(100);
    avancer(baseVitesse);
    delay(200);

    int s1b = digitalRead(S1);
    int s2b = digitalRead(S2);
    int s3b = digitalRead(S3);
    int s4b = digitalRead(S4);

    // Priorité à gauche
    if (s1b == ligne || s2b == ligne) {
      gauche(tournageVitesse);
      delay(400);
      chemin[indexChemin++] = 'G';
    }
    // Sinon tout droit
    else if (s2b == ligne && s3b == ligne) {
      avancer(baseVitesse);
      chemin[indexChemin++] = 'S';
    }
    // Sinon droite
    else if (s3b == ligne || s4b == ligne) {
      droite(tournageVitesse);
      delay(400);
      chemin[indexChemin++] = 'D';
    }
    // Sinon cul-de-sac
    else {
      demiTour();
      chemin[indexChemin++] = 'U';
    }
  }

  // === FIN DE LIGNE (cul-de-sac) ===
  else if (s1 == fond && s2 == fond && s3 == fond && s4 == fond) {
    stopMoteurs();
    delay(200);
    demiTour();
    chemin[indexChemin++] = 'U';
  }

  delay(10);
}
