/*


#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BTScan.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include "C:\Users\vigre\OneDrive\Documents\PlatformIO\Projects\Alarme_plus_ledV2\include\led.cpp"
#include <string.h>
MPU6050 mpu6050(Wire);
BluetoothSerial SerialBT;

#define klaxon 2
#define lumiere 0
#define led_pin 4
#define TAILLE_POINTEUR 1
long timer = 0;
int temoin_y = 0;
int temoin_x = 0;
int precision_angle = 10;
int avertissement = 0;
int cycle = 0;
char intruction[]="instruction"; // instruction module bluetooth
char *message=NULL;
bool alarme = false;
bool Lumiere = false;
bool continuer = true;
const int TAILLE_MAX = 256;

/*//*

a : alarme activer
b : alarme desactiver
t : optenir etat alarme
e : activer alarme continue
y : activer klaxon 2 bip
o : simulation explision avec bip plus rapide
p : allumer lumiere velos
n : precision moin
m : precision plus
k : stop fonction
///////////////////////////////////////////////
pin  klaxon 2
pin lumiere 0
pin data led 4
*/
/*/
void afficher_pointeur(char* pointeur)
{
  for (int i = 0; TAILLE_POINTEUR < i; i++)
  {
    Serial.print(pointeur[i]);
  }
  Serial.println("");
}

void reset_message()
{
memset(message, 0, TAILLE_POINTEUR * sizeof(char));
}
void reception_bluetooth()
{
  if (SerialBT.available())
  {
    int nb_caracteres = SerialBT.available();
    message = (char *)realloc(message, (nb_caracteres + 1) * sizeof(char));
    int i = 0;
    while (SerialBT.available() && i < nb_caracteres)
    {
      Serial.print("caractére: ");
      Serial.println((char)SerialBT.read());
      message[i] = (char)SerialBT.read();
      i++;
      Serial.print("message: ");
      afficher_pointeur(message);
      Serial.print("condition: ");
      Serial.println((strncmp(message, intruction, nb_caracteres) != 0));
    }
    message[nb_caracteres] ='\0';

    if (strncmp(message, intruction, nb_caracteres) != 0) // la chaine instruction et différent de message
    {
      memcpy(intruction, message, TAILLE_POINTEUR * sizeof(char)); // transfere les donners message a instruction
      Serial.print("instruction: ");
      afficher_pointeur(intruction);
    }
  }
}
void avertissement_alarme(int boucle) // lance une alerte et rajoute +1 a avertissement
{
  int bip = 0;
  while (bip < boucle)
  {
    Serial.println("alerte high");
    digitalWrite(klaxon, HIGH);
    delay(50);
    digitalWrite(klaxon, LOW);
    Serial.println("alerte low");
    delay(50);
    bip++;
  }
}

void etat_alarme() // indique si l'alarme et on ou non
{
  if (alarme)
  {
    SerialBT.println("statue alarme : alarme activer ");
  }
  else
  {
    SerialBT.println("statue alarme : alarme desactiver ");
  }
}
void super_alerte(int duree) // super alerte indiauer la duree en ms seconde
{
  int bip = 0;
  while (bip < duree / 100 && continuer)
  {
    reception_bluetooth();
    Serial.println("SuperAlerte");
    delay(100);
    digitalWrite(klaxon, HIGH);
    mpu6050.update();
    avertissement = 0;
    cycle = 0;
    bip++;
  }

  digitalWrite(klaxon, LOW);
  continuer = true;
}
void etat_lumiere() // indique si la lumiere et on ou non
{
  if (Lumiere)
  {
    SerialBT.println("statue lumiere : lumiere activer ");
  }
  else
  {
    SerialBT.println("statue lumiere : lumiere desactiver ");
  }
}
void bombe_alarme(float temps) // bip facon bombe indiquer le temps en ms
{
  int bip = 0;
  while (bip < 35 && continuer)
  {
    reception_bluetooth();
    digitalWrite(klaxon, HIGH);
    delay(temps);
    temps = temps / 1.15;
    digitalWrite(klaxon, LOW);
    delay(temps);
    Serial.print("bip  temps :");
    Serial.println(temps);
    bip++;
  }
  if (continuer)
  {
    delay(3000);
    int temps_locale = 500;
    while (temps_locale < 13000 && continuer)
    {
      digitalWrite(klaxon, HIGH);
      reception_bluetooth();
      delay(500);
      temps_locale += 500;
    }
    digitalWrite(klaxon, LOW);
    continuer = true;
  }
}
void etat_velo()
{
  etat_alarme();
  SerialBT.print("precision_angle : ");
  SerialBT.println(precision_angle);
  etat_lumiere();
  SerialBT.print("temperature exterieur : ");
  mpu6050.update();
  SerialBT.print(mpu6050.getTemp());
  SerialBT.println("c*");
}

bool condition_pointeur(const char *rechercher)
{
  if (strstr(message, rechercher) != nullptr)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void instruction_controlle()
{
  reception_bluetooth();
  if (condition_pointeur("a")) // active alarme
  {
    alarme = true;
    Serial.println("alarme activer");
    SerialBT.print('a'); // confirme alarme activer
    delay(50);
    etat_velo();
  }
  if (condition_pointeur("b")) // desactive alarme
  {
    alarme = false;
    Serial.println("alarme desactiver");
    SerialBT.print('b'); // confirme alarme desactiver
    delay(50);
    etat_velo();
    continuer = false;
    cycle = 0;
  }

  if (condition_pointeur("k")) // stop
  {
    Serial.println("fonctions off");
    delay(50);
    etat_velo();
    continuer = false;
    cycle = 0;
  }
  if (condition_pointeur("p")) // active lumiere velo
  {
    Lumiere = true;
    digitalWrite(lumiere, HIGH);
    Serial.println("lumiere activer");
    SerialBT.print('p'); // confirme lumiere activer
  }
  if (condition_pointeur("q")) // desactive lumiere velo
  {
    Lumiere = false;
    digitalWrite(lumiere, LOW);
    Serial.println("lumiere desactiver");
    SerialBT.print('q'); // confirme lumiere desactiver
  }

  if (condition_pointeur("e")) // active alarme continue
  {
    Serial.println("super_alerte activer");
    super_alerte(10000);
  }
  if (condition_pointeur("y")) // active klaxon
  {
    Serial.println("klaxon activer");
    avertissement_alarme(2);
    reset_message();
  }
  if (condition_pointeur("o")) // active timeur bombe alarme
  {
    Serial.println("bombe activer");
    bombe_alarme(1000);
    reset_message();
  }
  if (condition_pointeur("t")) // optenir etat object
  {
    etat_velo();
    reset_message();
  }
  if (condition_pointeur("n")) // optenir etat object
  {
    precision_angle -= 2;
    SerialBT.print("precision_angle : ");
    etat_velo();
    reset_message();
  }
  if (condition_pointeur("m")) // optenir etat object
  {
    precision_angle += 2;
    SerialBT.print("precision_angle : ");
    etat_velo();
    reset_message();
  }
  if (strstr(message, "/led") != nullptr) //  Si strstr renvoie un pointeur différent de nullptr, cela signifie que le motif a été trouvé // strcmp recherche si les 2 pointeur sont identiques
  {
    reset_message();
  }
}

void setup()
{
  begin_led();
  Serial.begin(9600);
  SerialBT.begin("tesla"); // nom module bluetooth
  Wire.begin();
  mpu6050.begin();
  int a = 0;
  int mouvement_x = 0;
  int mouvement_y = 0;
  pinMode(klaxon, OUTPUT);
  pinMode(lumiere, OUTPUT);
  pinMode(led_pin, OUTPUT);
  mpu6050.calcGyroOffsets(true);
}

void loop()
{
  reception_bluetooth();
  int a = 0;
  mpu6050.update();
  temoin_y = mpu6050.getGyroY();
  temoin_x = mpu6050.getGyroX();

  if (alarme)
  {
    mpu6050.calcGyroOffsets(true);
    while (alarme)
    {
      reception_bluetooth();
      mpu6050.update();
      if (millis() - timer > 1000)
      {
        Serial.print("cycle :");
        Serial.println(cycle);
        Serial.print("angle temoin y  :");
        Serial.println(temoin_y);
        Serial.print("angle temoin x :");
        Serial.println(temoin_x);
        Serial.print("avertissement :");
        Serial.println(avertissement);

        Serial.println("=======================================================");
        Serial.print("temp : ");
        Serial.println(mpu6050.getTemp());
        Serial.print("accX : ");
        Serial.print(mpu6050.getAccX());
        Serial.print("\taccY : ");
        Serial.print(mpu6050.getAccY());
        Serial.print("\taccZ : ");
        Serial.println(mpu6050.getAccZ());

        Serial.print("gyroX : ");
        Serial.print(mpu6050.getGyroX());
        Serial.print("\tgyroY : ");
        Serial.print(mpu6050.getGyroY());
        Serial.print("\tgyroZ : ");
        Serial.println(mpu6050.getGyroZ());

        Serial.print("accAngleX : ");
        Serial.print(mpu6050.getAccAngleX());
        Serial.print("\taccAngleY : ");
        Serial.println(mpu6050.getAccAngleY());

        Serial.print("gyroAngleX : ");
        Serial.print(mpu6050.getGyroAngleX());
        Serial.print("\tgyroAngleY : ");
        Serial.print(mpu6050.getGyroAngleY());
        Serial.print("\tgyroAngleZ : ");
        Serial.println(mpu6050.getGyroAngleZ());

        Serial.print("angleX : ");
        Serial.print(mpu6050.getAngleX());
        Serial.print("\tangleY : ");
        Serial.print(mpu6050.getAngleY());
        Serial.print("\tangleZ : ");
        Serial.println(mpu6050.getAngleZ());
        Serial.println("=======================================================\n");
        timer = millis();
        a++;
        cycle++;
        if (cycle == 50 && avertissement == 1) // reset avertissement apres 50 cycle
        {
          avertissement = 0;
        }
        if (mpu6050.getGyroX() > temoin_x + precision_angle || mpu6050.getGyroX() < temoin_x - precision_angle || mpu6050.getGyroY() < temoin_y - precision_angle || mpu6050.getGyroY() > temoin_y + precision_angle)
        {
          reception_bluetooth();
          int bip = 0;
          if (avertissement == 1) // si il a avertie 1 fois lance la grosse alarme
          {
            super_alerte(10000); // duree en ms seconde
            temoin_y = mpu6050.getGyroY();
            temoin_x = mpu6050.getGyroX();
            avertissement == 0;
            continue; // permet de sauter l'etape d'apres
          }
          if (avertissement == 0)
          {
            cycle = 0;
            avertissement++;
            avertissement_alarme(3);
          }
        }
      }
    }
  }
}
*/