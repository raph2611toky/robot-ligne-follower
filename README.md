# 🤖 Robot Suiveur de Ligne – Arduino Uno + IR Tracker 4 Canaux + L298N

## 🎯 Objectif du Projet

Ce projet a pour but de construire un **robot suiveur de ligne** autonome basé sur une carte **Arduino Uno**, capable de suivre une ligne noire tracée sur un fond blanc à l’aide d’un **capteur infrarouge (IR Tracker à 4 canaux)**.

Le robot utilise un **module moteur L298N** pour contrôler deux roues motrices. L’intelligence du robot est basée sur l’analyse en temps réel des données IR pour ajuster sa direction (avancer, tourner à gauche, tourner à droite, s'arrêter).

---

## ⚙️ Matériel utilisé

- Arduino Uno
- Capteur suiveur de ligne IR Tracker (4 canaux)
- Driver moteur L298N
- 2 moteurs DC
- Châssis robot avec roues
- Batterie externe (Li-ion ou pack de piles)
---

## 🗺️ Étapes du Projet

| Étape | Description | Statut |
|------:|-------------|--------|
| 1     | Comprendre le fonctionnement de chaque composant (Arduino, IR, L298N) | ✅ En cours |
| 2     | Connecter le **capteur IR Tracker à l'Arduino** et vérifier la lecture des données | 🔄 **Étape actuelle** |
| 3     | Câbler et tester le **driver moteur L298N** avec deux moteurs DC | ⬜ À venir |
| 4     | Programmer la logique de **suivi de ligne** en fonction des données IR | ⬜ À venir |
| 5     | Ajuster la sensibilité des capteurs et les vitesses de moteur | ⬜ À venir |
| 6     | Optimisation des virages (léger, 90°, pivots...) | ⬜ À venir |
| 7     | Intégration complète sur châssis robot avec alimentation autonome | ⬜ À venir |
| 8     | Tests sur piste réelle (ligne noire sur fond blanc) | ⬜ À venir |

---

## 🔍 Étape actuelle : Lecture des données des capteurs IR

### 1. 📦 Connexions

| IR Tracker | Arduino Uno |
|------------|--------------|
| VCC        | 3.3V           |
| GND        | GND          |
| OUT1       | D2           |
| OUT2       | D3           |
| OUT3       | D4           |
| OUT4       | D5           |

> Le capteur renvoie `LOW (0)` lorsqu’il détecte du **noir** et `HIGH (1)` lorsqu’il détecte du **blanc**.

### 2. 📋 Code de test

```cpp
void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);  // OUT1
  pinMode(3, INPUT);  // OUT2
  pinMode(4, INPUT);  // OUT3
  pinMode(5, INPUT);  // OUT4
}

void loop() {
  int c1 = digitalRead(2);
  int c2 = digitalRead(3);
  int c3 = digitalRead(4);
  int c4 = digitalRead(5);

  Serial.print("C1: "); Serial.print(c1);
  Serial.print("  C2: "); Serial.print(c2);
  Serial.print("  C3: "); Serial.print(c3);
  Serial.print("  C4: "); Serial.println(c4);

  delay(200);
}
```