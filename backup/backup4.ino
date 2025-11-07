#define capteurExtremeGauche 2  // Left most sensor
#define capteurCentralGauche 3
#define capteurCentralDroit 4
#define capteurExtremeDroit 5  // Right most sensor

#define ENA 10  // Left motor speed
#define IN1 6   // Left motor input 1
#define IN2 7   // Left motor input 2
#define ENB 11  // Right motor speed
#define IN3 9   // Right motor input 1
#define IN4 8   // Right motor input 2

// Initial Values of Sensors
int sensor[4] = {0, 0, 0, 0};
const int LIGNE = 0; const int FOND = 1;

const int vitesseFort = 80;
const int vitesseFaible = 60;

// Initial Speed of Motor (tune as needed)
int initial_motor_speed = 90;

// PID Constants (tune these based on testing: start with Kp=25, Ki=0, Kd=15)
float Kp = 25;
float Ki = 0;
float Kd = 15;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;
int flag = 0;

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
  Serial.begin(9600);  // Setting serial monitor at default baud rate
  delay(500);
  Serial.println("Started !!");
  delay(1000);
}

void loop() {
  read_sensor_values();
  Serial.print(error);
  if (error == 100) {  // Make left turn until it detects straight path
    do {
      read_sensor_values();
      analogWrite(ENA, vitesseFort);  // Left Motor Speed
      analogWrite(ENB, vitesseFaible);   // Right Motor Speed
      gauche_();
    } while (error != 0);
  } else if (error == 101) {  // Make right turn if only right path detected
    analogWrite(ENA, vitesseFort);    // Left Motor Speed
    analogWrite(ENB, vitesseFaible);     // Right Motor Speed
    avancer_();
    delay(40);
    stop_bot();
    read_sensor_values();
    if (error == 102) {
      do {
        analogWrite(ENA, vitesseFort);  // Left Motor Speed
        analogWrite(ENB, vitesseFaible);   // Right Motor Speed
        droite_();
        read_sensor_values();
      } while (error != 0);
    }
  } else if (error == 102) {  // Make left turn until straight path
    do {
      analogWrite(ENA, vitesseFort);  // Left Motor Speed
      analogWrite(ENB, vitesseFaible);   // Right Motor Speed
      gauche_();
      read_sensor_values();
      if (error == 0) {
        stop_bot();
        delay(40);
      }
    } while (error != 0);
  } else if (error == 103) {  // Handle dead end or left turn
    if (flag == 0) {
      analogWrite(ENA, vitesseFort);  // Left Motor Speed
      analogWrite(ENB, vitesseFaible);   // Right Motor Speed
      avancer_();
      delay(40);
      stop_bot();
      read_sensor_values();
      if (error == 103) {  // Dead End Reached, Stop!
        stop_bot();
        flag = 1;
      } else {  // Move Left
        analogWrite(ENA, vitesseFort);  // Left Motor Speed
        analogWrite(ENB, vitesseFaible);   // Right Motor Speed
        gauche_();
        delay(40);
        do {
          read_sensor_values();
          analogWrite(ENA, vitesseFort);  // Left Motor Speed
          analogWrite(ENB, vitesseFaible);   // Right Motor Speed
          gauche_();
        } while (error != 0);
      }
    }
  } else {
    calculate_pid();
    motor_control();
  }
}

void read_sensor_values() {
  sensor[0] = digitalRead(capteurExtremeGauche);
  sensor[1] = digitalRead(capteurCentralGauche);
  sensor[2] = digitalRead(capteurCentralDroit);
  sensor[3] = digitalRead(capteurExtremeDroit);

  if ((sensor[0] == LIGNE) && (sensor[1] == FOND) && (sensor[2] == FOND) && (sensor[3] == FOND))
    error = 3;
  else if ((sensor[0] == LIGNE) && (sensor[1] == LIGNE) && (sensor[2] == FOND) && (sensor[3] == FOND))
    error = 2;
  else if ((sensor[0] == FOND) && (sensor[1] == LIGNE) && (sensor[2] == FOND) && (sensor[3] == FOND))
    error = 1;
  else if ((sensor[0] == FOND) && (sensor[1] == LIGNE) && (sensor[2] == LIGNE) && (sensor[3] == FOND))
    error = 0;
  else if ((sensor[0] == FOND) && (sensor[1] == FOND) && (sensor[2] == LIGNE) && (sensor[3] == FOND))
    error = -1;
  else if ((sensor[0] == FOND) && (sensor[1] == FOND) && (sensor[2] == LIGNE) && (sensor[3] == LIGNE))
    error = -2;
  else if ((sensor[0] == FOND) && (sensor[1] == FOND) && (sensor[2] == FOND) && (sensor[3] == LIGNE))
    error = -3;
  else if ((sensor[0] == LIGNE) && (sensor[1] == LIGNE) && (sensor[2] == LIGNE) && (sensor[3] == FOND))  // Turn left
    error = 100;
  else if ((sensor[0] == FOND) && (sensor[1] == LIGNE) && (sensor[2] == LIGNE) && (sensor[3] == LIGNE))  // Turn right
    error = 101;
  else if ((sensor[0] == FOND) && (sensor[1] == FOND) && (sensor[2] == FOND) && (sensor[3] == FOND))  // U turn
    error = 102;
  else if ((sensor[0] == LIGNE) && (sensor[1] == LIGNE) && (sensor[2] == LIGNE) && (sensor[3] == LIGNE))  // Left or stop
    error = 103;
}

void calculate_pid() {
  P = error;
  I = I + previous_I;
  D = error - previous_error;
  PID_value = (Kp * P) + (Ki * I) + (Kd * D);
  previous_I = I;
  previous_error = error;
}

void motor_control() {
  // Calculating the effective motor speed:
  int left_motor_speed = initial_motor_speed - PID_value;
  int right_motor_speed = initial_motor_speed + PID_value;
  // Constrain to PWM range
  left_motor_speed = constrain(left_motor_speed, 0, 255);
  right_motor_speed = constrain(right_motor_speed, 0, 255);

  analogWrite(ENA, left_motor_speed);  // Left Motor Speed
  analogWrite(ENB, right_motor_speed); // Right Motor Speed (adjusted if needed)

  avancer_();  // Set forward direction
}

// Motor direction functions (adapted to your pinout)
void avancer_() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void reculer_() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void droite_() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void gauche_() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stop_bot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}