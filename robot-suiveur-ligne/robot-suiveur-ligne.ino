#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 90
#define tournageVitesse 100

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

int sensor[4] = {0, 0, 0, 0};

void setup() {
  pinMode(capteurExtremeGauche, INPUT);
  pinMode(capteurCentralGauche, INPUT);
  pinMode(capteurCentralDroit, INPUT);
  pinMode(capteurExtremeDroit, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  delay(500);
  Serial.println("Démarrage... !!");
  delay(1000);
}

// ====================
// FONCTIONS MOTEURS
// ====================
void avancer_() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void avancer(int vitesse) {
  avancer_();
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

void reculer_() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void reculer(int vitesse) {
  reculer_();
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

void droite_() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void droite(int vitesse) {
  droite_();
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

void gauche_() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void gauche(int vitesse) {
  gauche_();
  analogWrite(ENA, vitesse);
  analogWrite(ENB, vitesse);
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ====================
// Lecture capteurs
// ====================
void readSensor() {
  sensor[0] = digitalRead(capteurExtremeGauche);
  sensor[1] = digitalRead(capteurCentralGauche);
  sensor[2] = digitalRead(capteurCentralDroit);
  sensor[3] = digitalRead(capteurExtremeDroit);
}
// ====================
// Virage intelligent optimisé (utilisé pour virages d'intersection/extrêmes)
// direction: "gauche" ou "droite"
// ====================
void virageOptimise(String direction) {
  int ligne = 0;
  int fond = 1;

  if (direction == "gauche") {
    gauche(tournageVitesse);
    delay(180);
  } else {
    droite(tournageVitesse);
    delay(180);
  }

  unsigned long start = millis();
  while (true) {
    readSensor();

    if (sensor[1] == ligne && sensor[2] == ligne) {
      stop();
      delay(80);
      break;
    }

    if (direction == "gauche" && sensor[3] == ligne) {
      stop();
      delay(60);
      break;
    }
    if (direction == "droite" && sensor[0] == ligne) {
      stop();
      delay(60);
      break;
    }

    if (direction == "gauche") gauche(tournageVitesse);
    else droite(tournageVitesse);

    if (millis() - start > 2500) {
      stop();
      delay(50);
      reculer(baseVitesse);
      delay(120);
      stop();
      delay(80);
      start = millis();
      Serial.println("VirageOptimise: correction de sécurité (recul) et reprise.");
      if (direction == "gauche") gauche(tournageVitesse);
      else droite(tournageVitesse);
      delay(150);
    }
  }

  avancer(baseVitesse);
  delay(160);
}

// ====================
// BOUCLE PRINCIPALE (logique exhaustive)
// ====================
void loop() {
  readSensor();

  int ligne = 0; // blanc = ligne
  int fond = 1;  // noir = fond

  Serial.print("S0:"); Serial.print(sensor[0]);
  Serial.print(" S1:"); Serial.print(sensor[1]);
  Serial.print(" S2:"); Serial.print(sensor[2]);
  Serial.print(" S3:"); Serial.println(sensor[3]);

  // --- 1) Intersection complète (4 capteurs sur la ligne) ---
  if (sensor[0]==ligne && sensor[1]==ligne && sensor[2]==ligne && sensor[3]==ligne) {
    stop();
    delay(80);
    Serial.println("Intersection (4) detectée -> priorité GAUCHE");
    virageOptimise("gauche");
    return;
  }

  // --- 2) Cas 3 capteurs ---
  // 0+1+2 (deux gauches + central droit) => virage gauche immédiat
  if (sensor[0]==ligne && sensor[1]==ligne && sensor[2]==ligne) {
    stop();
    delay(50);
    Serial.println("3 capteurs (0,1,2) -> virage GAUCHE immédiat");
    virageOptimise("gauche");
    return;
  }
  // 1+2+3 (central gauche + deux droits) => virage droite immédiat
  if (sensor[1]==ligne && sensor[2]==ligne && sensor[3]==ligne) {
    stop();
    delay(50);
    Serial.println("3 capteurs (1,2,3) -> virage DROITE immédiat");
    virageOptimise("droite");
    return;
  }

  // --- 3) Priorité aux extrêmes ---
  // Si extrême gauche voit la ligne -> virage optimisé gauche
  if (sensor[0]==ligne) {
    stop();
    delay(30);
    Serial.println("Extrême GAUCHE détectée -> virage optimisé GAUCHE");
    virageOptimise("gauche");
    return;
  }
  // Si extrême droite voit la ligne -> virage optimisé droite
  if (sensor[3]==ligne) {
    stop();
    delay(30);
    Serial.println("Extrême DROITE détectée -> virage optimisé DROITE");
    virageOptimise("droite");
    return;
  }

  // --- 4) Cas où extrêmes voient FOND (noir) mais un central voit la ligne :
  // avancer quand même mais pencher vers le capteur central actif (correction douce)
  if (sensor[0]==fond && sensor[3]==fond) {
    // if (sensor[1]==ligne && sensor[2]==fond) {
    //   Serial.println("Central gauche seul -> avancer en penchant GAUCHE");
    //   gauche(baseVitesse);
    //   delay(10);
    //   return;
    // }
    // if (sensor[1]==fond && sensor[2]==ligne) {
    //   Serial.println("Central droit seul -> avancer en penchant DROITE");
    //   droite(baseVitesse); 
    //   delay(10);
    //   return;
    // }
    if (sensor[1]==ligne || sensor[2]==ligne) {
      Serial.println("Centrés (S1+S2) -> avancer");
      avancer(baseVitesse);
      delay(10);
      return;
    }
  }

  // --- 5) Cas centraux seuls (corrrections / avancer) ---
  // Central gauche actif -> correction gauche légère
  if (sensor[1]==ligne && sensor[2]==fond) {
    Serial.println("Correction légère GAUCHE (S1 actif)");
    gauche(baseVitesse);
    delay(10);
    return;
  }
  // Central droit actif -> correction droite légère
  if (sensor[1]==fond && sensor[2]==ligne) {
    Serial.println("Correction légère DROITE (S2 actif)");
    droite(baseVitesse);
    delay(10);
    return;
  }
  // Les deux centraux -> avancer
  if (sensor[1]==ligne && sensor[2]==ligne) {
    Serial.println("Ligne centrée -> avancer");
    avancer(baseVitesse);
    delay(10);
    return;
  }

  // --- 6) Cas mixtes restants (exemples : S0 fond, S1 fond, S2 fond, S3 fond => perdu) ---
  // Si aucun capteur ne voit la ligne -> stop ou recherche (recherche simple)
  if (sensor[0]==fond && sensor[1]==fond && sensor[2]==fond && sensor[3]==fond) {
    Serial.println("Aucun capteur -> perte ligne: petite recherche");
    // petite recherche: pivoter à gauche lentement puis droite si rien
    gauche(tournageVitesse);
    delay(120);
    readSensor();
    if (sensor[1]==ligne || sensor[2]==ligne) {
      stop();
      delay(20);
      return;
    }
    // sinon tenter droite
    droite(tournageVitesse);
    delay(240);
    stop();
    return;
  }

  // --- 7) Cas restants non-couverts (sécurisé) ---
  // On arrête pour sécurité et on laisse la boucle relire très vite
  stop();
  delay(10);
}