#include <EEPROM.h>

#define capteurExtremeGauche 2
#define capteurCentralGauche 3
#define capteurCentral 12
#define capteurCentralDroit 4
#define capteurExtremeDroit 5

#define baseVitesse 55
#define tournageVitesse 50
#define tournageVitesseMax 75

#define ENA 10 
#define IN1 6
#define IN2 7
#define ENB 11 
#define IN3 9
#define IN4 8

char chemin[501];
int cheminLen = 0;
char dernierDirection = '\0';
char etat = '+';
bool finDeParcours = false;

int sensor[5] = {0,0,0,0,0};

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
  chemin[0] = '\0';
}

/* === Fonctions de base de mouvement === */
void avancer_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void avancer(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void reculer_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void reculer(int v){ reculer_(); analogWrite(ENA,v); analogWrite(ENB,v); }

void gauche_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); }
void gauche(int v){ gauche_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gaucheLent(int v){ avancer_(); analogWrite(ENA,v*0.9); analogWrite(ENB,v); }

void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droiteLent(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v*0.9); }

void stop(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); analogWrite(ENA,0); analogWrite(ENB,0); }

// LOAD EEPROM

void sauvegarderEEPROM() {
  for (int i = 0; i < cheminLen; i++) {
    EEPROM.write(i, chemin[i]);
  }
  EEPROM.write(cheminLen, '\0');
}

void chargerEEPROM() {
  int i = 0;
  char c;
  while ((c = EEPROM.read(i)) != '\0' && i < 500) {
    chemin[i++] = c;
  }
  cheminLen = i;
  chemin[cheminLen] = '\0';
}

char symetrie(char d) {
  if (d == 'g') return 'd';
  if (d == 'd') return 'g';
  if (d == 'a') return 'a';
  if (d == 'G') return 'D';
  if (d == 'D') return 'G';
  if (d == 'A') return 'A';
  return d;
}

// void enregistrerChemin(char c) {
//   // 1️⃣ On garde toujours le chemin complet en RAM
//   if (cheminLen < 500) {
//     chemin[cheminLen++] = c;
//     chemin[cheminLen] = '\0';
//   }

//   // 2️⃣ Lecture du dernier index EEPROM
//   int lenEEPROM = 0;
//   while (EEPROM.read(lenEEPROM) != '\0' && lenEEPROM < 500) lenEEPROM++;

//   // 3️⃣ Lecture du dernier caractère stocké
//   char dernierEEPROM = (lenEEPROM > 0) ? EEPROM.read(lenEEPROM - 1) : '\0';

//   // 4️⃣ Logique d’optimisation
//   if (etat == '+') {
//     // On ajoute normalement
//     EEPROM.write(lenEEPROM, c);
//     EEPROM.write(lenEEPROM + 1, '\0');
//   } 
//   else if (etat == '-') {
//     // Si symétrie détectée, on retire le dernier
//     if (dernierEEPROM == symetrie(c)) {
//       lenEEPROM--;
//       EEPROM.write(lenEEPROM, '\0');
//     } else {
//       // Changement d’état → on ajoute à nouveau
//       etat = '+';
//       EEPROM.write(lenEEPROM, c);
//       EEPROM.write(lenEEPROM + 1, '\0');
//     }
//   }

//   // 5️⃣ Si on rencontre un 'U' → on inverse l’état
//   if (c == 'U') {
//     etat = (etat == '+') ? '-' : '+';
//   }

//   Serial.print("Chemin RAM: ");
//   Serial.println(chemin);

//   Serial.print("EEPROM: ");
//   for (int i = 0; i < lenEEPROM + 1; i++) {
//     char cc = EEPROM.read(i);
//     if (cc == '\0') break;
//     Serial.print(cc);
//   }
//   Serial.println();
// }

void enregistrerChemin(char c) {
  if (cheminLen < 500) {
    chemin[cheminLen++] = c;
    chemin[cheminLen] = '\0';
  }
}

/* === Lecture capteurs === */
void readSensor(){
  sensor[0]=digitalRead(capteurExtremeGauche);
  sensor[1]=digitalRead(capteurCentralGauche); 
  sensor[2]=digitalRead(capteurCentral); 
  sensor[3]=digitalRead(capteurCentralDroit);
  sensor[4]=digitalRead(capteurExtremeDroit);
}

char getSymmetricDirection(char* chemin) {
  int posU = -1;
  for (int i = cheminLen - 1; i >= 0; i--) {
    if (chemin[i] == 'U') { posU = i; break; }
  }
  if (posU == -1) return '\0';
  char apresU[100] = "";
  int lenA = 0;
  for (int i = posU + 1; i < cheminLen; i++) {
    char c = chemin[i];
    if (c == 'a' || c == 'g' || c == 'd' || c == 'A' || c == 'G' || c == 'D') {
      apresU[lenA++] = c;
    }
  }
  apresU[lenA] = '\0';
  int n = lenA;
  if (n == 0) return '\0';
  char avantU[100] = "";
  int lenB = 0;
  for (int i = 0; i < posU; i++) {
    char c = chemin[i];
    if (c == 'a' || c == 'g' || c == 'd' || c == 'A' || c == 'G' || c == 'D') {
      avantU[lenB++] = c;
    }
  }
  
  avantU[lenB] = '\0';
  if (lenB < n + 1) return '\0';
  int indexAvant = lenB - (n + 1);
  char dir = avantU[indexAvant];
  if (dir == 'g' || dir == 'G' || dir == 'a' || dir == 'A' || dir == 'd' || dir == 'D') return dir;
  return '\0';
}



/* === Virage 90° === */

void tourner(char direction) {
  if (direction == 'G') {
    gauche(tournageVitesse);delay(100);
    gaucheLent(baseVitesse);delay(100);
    gauche(tournageVitesse);
  }
  else if (direction == 'D') {
    droite(tournageVitesse);delay(100);
    droiteLent(baseVitesse);delay(100);
    droite(tournageVitesse);
  }
}

void virage90(char direction) {
  unsigned long start = millis();
  const int timeout = 3000;
  dernierDirection = direction;

  if (direction == 'G') {
    // tourner(direction);
    gaucheLent(tournageVitesse);delay(100);
    while (millis() - start < timeout) { // tourne environ 90°
      gauche(tournageVitesse);
      readSensor();
      if ((sensor[3]==LIGNE || sensor[4]==LIGNE) && (sensor[0]==FOND && sensor[1]==FOND)) {
        break;
      }
      if (sensor[3]==LIGNE)droiteLent(baseVitesse);
      // delay(10);
    }
  } 
  else if (direction == 'D') {
    // tourner(direction);
    gaucheLent(tournageVitesse);delay(100);
    while (millis() - start < timeout) {
      droite(tournageVitesse);
      readSensor();
      if ((sensor[1]==LIGNE || sensor[0]==LIGNE) && (sensor[2]==FOND && sensor[3]==FOND)) {
        break;
      }
      if (sensor[1]==LIGNE)gaucheLent(baseVitesse);
      // delay(10);
    }
  }
  // stop();
}

/* === Recherche de ligne perdue === */
void rechercherLigne() {
  unsigned long start = millis();
  if (dernierDirection == 'G') {
    droite(tournageVitesseMax);delay(250);
    // dernierDirection = 'D';
    while (millis() - start < 3000) {
      readSensor();
      if (sensor[1]==LIGNE || sensor[0]==LIGNE) return;
    }
  } else {
    gauche(tournageVitesseMax);delay(250);
    // dernierDirection = 'G';
    while (millis() - start < 3000) {
      readSensor();
      if (sensor[3]==LIGNE || sensor[4]==LIGNE) return;
    }
  }
  // stop();
}


/* === BOUCLE PRINCIPALE === */
void loop() {
  readSensor();

  char dirSym = getSymmetricDirection(chemin);
  if (sensor[0]==LIGNE && (sensor[1]==LIGNE || sensor[2]==LIGNE || sensor[3]==LIGNE) && sensor[4]==LIGNE) {
    if(dirSym=='D' || dirSym=='d'){
      virage90('D');enregistrerChemin('D');
      dernierDirection = 'D';
    }else if (dirSym=='G' || dirSym=='A' || dirSym=='g'){
      virage90('G');
      dernierDirection = 'G';enregistrerChemin('G');
    }else {
      virage90('D');
      dernierDirection = 'D';enregistrerChemin('D');
    }
  }

  else if (sensor[4] == LIGNE) {
    virage90('D');enregistrerChemin('d');
    dernierDirection = 'D';
  }
  else if (sensor[0] == LIGNE) {
    avancer(baseVitesse);delay(100);stop();
    readSensor();
    if (sensor[1]==LIGNE || sensor[2]==LIGNE || sensor[3]==LIGNE){
      if (sensor[3] == LIGNE) {
        droiteLent(baseVitesse);
      }
      else if (sensor[1] == LIGNE) {
        gaucheLent(baseVitesse);
      }
      else if (sensor[2] == LIGNE)  avancer(baseVitesse);
      dernierDirection = 'A';enregistrerChemin('A');
    }
    else {
      reculer(baseVitesse);delay(100);
      virage90('G');
      dernierDirection = 'G';enregistrerChemin('g');
    }
  }

  // --- Ligne centrale ---
  else if (sensor[2] == LIGNE) {
    if (sensor[3] == LIGNE) {
      droiteLent(baseVitesse);
    }
    else if (sensor[1] == LIGNE) {
      gaucheLent(baseVitesse);
    }
    else avancer(baseVitesse);
    dernierDirection = 'A';enregistrerChemin('a');
  }
  
  // --- Ligne perdue ---
  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    rechercherLigne();
    dernierDirection = "U";enregistrerChemin('U');
  }
  else {
    avancer(baseVitesse);
    dernierDirection = 'A';enregistrerChemin('a');
  }
}
