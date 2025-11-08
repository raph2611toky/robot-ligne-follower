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
char cheminAParcourir[501];
int cheminActuel = 0;

char dernierDirection = '\0';
char etat = '+';
bool finDeParcours = false;
bool suivreChemin = false;   // ✅ mode suivi du chemin sauvegardé

int sensor[5] = {0,0,0,0,0};

const int LIGNE = 0;
const int FOND = 1;

/* === Fonctions mouvement === */
void avancer_(){ digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
void avancer(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void reculer_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void reculer(int v){ reculer_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gauche_(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); }
void gauche(int v){ gauche_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void droite_(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
void droite(int v){ droite_(); analogWrite(ENA,v); analogWrite(ENB,v); }
void gaucheLent(int v){ avancer_(); analogWrite(ENA,v*0.9); analogWrite(ENB,v); }
void droiteLent(int v){ avancer_(); analogWrite(ENA,v); analogWrite(ENB,v*0.9); }
void stop(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); analogWrite(ENA,0); analogWrite(ENB,0); }

/* === Lecture capteurs === */
void readSensor(){
  sensor[0]=digitalRead(capteurExtremeGauche);
  sensor[1]=digitalRead(capteurCentralGauche); 
  sensor[2]=digitalRead(capteurCentral); 
  sensor[3]=digitalRead(capteurCentralDroit);
  sensor[4]=digitalRead(capteurExtremeDroit);
}

/* === EEPROM === */
void sauvegarderEEPROMOptimise() {
  char tmp[501];
  int lenTmp = 0;
  for (int i = 0; i < cheminLen; i++) tmp[lenTmp++] = chemin[i];
  tmp[lenTmp] = '\0';
  for (int i = 0; i < lenTmp; i++) EEPROM.write(i, tmp[i]);
  EEPROM.write(lenTmp, '\0');
}

void chargerEEPROM() {
  int i = 0;
  char c;
  while ((c = EEPROM.read(i)) != '\0' && i < 500) {
    cheminAParcourir[i++] = c;
  }
  cheminAParcourir[i] = '\0';
  if (i > 0) suivreChemin = true; // ✅ activer mode suivi
  Serial.print("Chemin chargé EEPROM: ");
  Serial.println(cheminAParcourir);
}

/* === Virage 90° === */
void virage90(char direction) {
  unsigned long start = millis();
  const int timeout = 3000;

  if (direction == 'G') {
    while (millis() - start < timeout) {
      gauche(tournageVitesse);
      readSensor();
      if ((sensor[3]==LIGNE || sensor[4]==LIGNE) && (sensor[0]==FOND && sensor[1]==FOND)) break;
    }
  } 
  else if (direction == 'D') {
    while (millis() - start < timeout) {
      droite(tournageVitesse);
      readSensor();
      if ((sensor[1]==LIGNE || sensor[0]==LIGNE) && (sensor[2]==FOND && sensor[3]==FOND)) break;
    }
  }
}

/* === Recherche ligne perdue === */
void rechercherLigne() {
  unsigned long start = millis();
  if (dernierDirection == 'G') {
    droite(tournageVitesseMax);
    while (millis() - start < 2000) {
      readSensor();
      if (sensor[1]==LIGNE || sensor[0]==LIGNE) return;
    }
  } else {
    gauche(tournageVitesseMax);
    while (millis() - start < 2000) {
      readSensor();
      if (sensor[3]==LIGNE || sensor[4]==LIGNE) return;
    }
  }
}

/* === Enregistrer direction === */
void enregistrerChemin(char c) {
  if (cheminLen < 500) {
    chemin[cheminLen++] = c;
    chemin[cheminLen] = '\0';
  }
}

/* === SETUP === */
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
  chargerEEPROM();  // ✅ chargement au démarrage
}

/* === LOOP === */
void loop() {
  readSensor();

  // === ✅ Gestion des croisements ===
  if (sensor[0]==LIGNE && (sensor[1]==LIGNE || sensor[2]==LIGNE || sensor[3]==LIGNE) && sensor[4]==LIGNE) {
    char directionSuivie = '\0';

    // Si on suit un chemin EEPROM
    if (suivreChemin && cheminAParcourir[cheminActuel] != '\0') {
      directionSuivie = cheminAParcourir[cheminActuel];
      cheminActuel++;

      // Si majuscule, appliquer la direction
      if (directionSuivie == 'G' || directionSuivie == 'D' || directionSuivie == 'A') {
        virage90(directionSuivie);
        dernierDirection = directionSuivie;
        Serial.print("Suivi chemin: ");
        Serial.println(directionSuivie);
      }
      // Si minuscule, garder les priorités (rien à faire)
    } 
    else if (suivreChemin && cheminAParcourir[cheminActuel] == '\0') {
      // Plus de chemin à suivre → exploration libre
      suivreChemin = false;
      Serial.println("Fin du chemin enregistré, reprise exploration.");
    }

    // Si on n’est pas en mode suivi ou plus de chemin → comportement normal
    if (!suivreChemin) {
      virage90('D');
      enregistrerChemin('D');
      dernierDirection = 'D';
    }
  }

  // === Virages normaux ===
  else if (sensor[4] == LIGNE) {
    virage90('D'); enregistrerChemin('d');
    dernierDirection = 'D';
  }
  else if (sensor[0] == LIGNE) {
    virage90('G'); enregistrerChemin('g');
    dernierDirection = 'G';
  }

  // === Ligne centrale ===
  else if (sensor[2] == LIGNE) {
    if (sensor[3] == LIGNE) droiteLent(baseVitesse);
    else if (sensor[1] == LIGNE) gaucheLent(baseVitesse);
    else avancer(baseVitesse);
    dernierDirection = 'A'; enregistrerChemin('a');
  }

  // === Ligne perdue ===
  else if (sensor[0]==FOND && sensor[1]==FOND && sensor[2]==FOND && sensor[3]==FOND && sensor[4]==FOND) {
    rechercherLigne();
    enregistrerChemin('U');
  }

  else {
    avancer(baseVitesse);
    dernierDirection = 'A'; enregistrerChemin('a');
  }
}
