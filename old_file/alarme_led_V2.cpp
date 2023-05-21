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
String intruction; // instruction module bluetooth
String message;
bool alarme = false;
bool Lumiere = false;
bool continuer = true;
const int TAILLE_MAX = 256;
bool coupeur_pas_detecter = true;

/*/
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

/*/
void animation_led()
{
  if (led_mode == 1 && led_continuer)
  {
    couleur_alternance();
  }
  if (led_mode == 2 && led_continuer)
  {
    couleur_alternance_ale_retour();
  }
  if (led_mode == 3 && led_continuer)
  {
    pallete_couleur_run();
  }
  if (led_mode == 4 && led_continuer)
  {
    clignotant();
  }
  if (led_mode == 8)
  {
    currentPalette = degrader_which_two_colors;
    led_mode = 3;
  }
  if (led_mode == 9)
  {
    currentPalette = RainbowColors_p;
    led_mode = 3;
  }
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
bool condition_pointeur(String rechercher)
{
  if (intruction.indexOf(rechercher) != -1)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void afficher_pointeur()
{
  Serial.println(message);
}
void reset_message()
{
  intruction = String("");
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
void reception_bluetooth()
{
  if (SerialBT.available())
  {
    Serial.print("caractére string: ");
    // Serial.println(SerialBT.readString());
    message = SerialBT.readString();
    Serial.print("message: ");
    Serial.println(message);
    Serial.print("condition: ");
    Serial.println((message != intruction));
    if (message != intruction) // la chaine instruction et différent de message
    {
      intruction = message;
      Serial.print("instruction: ");
      Serial.println(intruction);
      Serial.print("a détecter le mot: ");
      if (condition_pointeur("/stop")) // stop
      {
        Serial.println("fonctions off");
        delay(50);
        etat_velo();
        continuer = false;
        intruction = String("");
        message = String("");
        cycle = 0;
      }
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
void super_alerte(int duree) // super alerte indiauer la duree en ms seconde
{
  int bip = 0;
  continuer = true;
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
void bombe_alarme(float temps) // bip facon bombe indiquer le temps en ms
{
  int bip = 0;
  continuer = true;
  while (bip < 35 && continuer)
  {
    animation_led();
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
    delay(2500);
    int temps_locale = 500;
    while (temps_locale < 13000 && continuer)
    {
      digitalWrite(klaxon, HIGH);
      reception_bluetooth();
      delay(100);
      temps_locale += 100;
    }
    digitalWrite(klaxon, LOW);
    continuer = true;
  }
}

void instruction_controlle()
{
  couleur_loop();
  if (SerialBT.available())
  {
    reception_bluetooth();
    if (condition_pointeur("/alarme_on")) // active alarme
    {
      alarme = true;
      Serial.println("alarme activer");
      SerialBT.print('a'); // confirme alarme activer
      delay(50);
      etat_velo();
    }
    if (condition_pointeur("/alarme_off")) // desactive alarme
    {
      alarme = false;
      Serial.println("alarme desactiver");
      SerialBT.print('b'); // confirme alarme desactiver
      delay(50);
      etat_velo();
      continuer = false;
      cycle = 0;
    }
    if (condition_pointeur("/lumiere_velo_on")) // active lumiere velo
    {
      Lumiere = true;
      digitalWrite(lumiere, HIGH);
      Serial.println("/lumiere activer");
      SerialBT.print('p'); // confirme lumiere activer
    }
    if (condition_pointeur("/lumiere_velo_off")) // desactive lumiere velo
    {
      Lumiere = false;
      digitalWrite(lumiere, LOW);
      Serial.println("/lumiere desactiver");
      SerialBT.print('q'); // confirme lumiere desactiver
    }
    if (condition_pointeur("/super_alerte")) // active alarme continue
    {
      Serial.println("super_alerte activer");
      super_alerte(10000);
    }
    if (condition_pointeur("/klaxon")) // active klaxon
    {
      Serial.println("klaxon activer");
      avertissement_alarme(2);
      reset_message();
    }
    if (condition_pointeur("/bombe_alarme")) // active timeur bombe alarme
    {
      Serial.println("bombe activer");
      bombe_alarme(1000);
      reset_message();
    }
    if (condition_pointeur("/etat")) // optenir etat object
    {
      etat_velo();
      reset_message();
    }
    if (condition_pointeur("/sensible_plus")) // rend l'alarme + sensible
    {
      precision_angle -= 2;
      SerialBT.print("sensible_moin");
      etat_velo();
      reset_message();
    }
    if (condition_pointeur("/sensible_moins")) // rend l'alarme - sensible
    {
      precision_angle += 2;
      SerialBT.print("precision_angle : ");
      etat_velo();
      reset_message();
    }
    if (condition_pointeur("/led_couleur")) //  Si strstr renvoie un pointeur différent de nullptr, cela signifie que le motif a été trouvé // strcmp recherche si les 2 pointeur sont identiques
    {
      int n_led = message.substring(12, 13).toInt();
      int n_r = message.substring(13, 16).toInt();
      int n_g = message.substring(16, 19).toInt();
      int n_b = message.substring(19, 22).toInt();
      Serial.println(n_led);
      Serial.println(n_r);
      Serial.println(n_g);
      Serial.println(n_b);
      if (n_led == 1)
      {
        couleur_1_rea = CRGB(n_r, n_g, n_b);
        couleur_1 = couleur_1_rea;
      }
      if (n_led == 2)
      {
        couleur_2_rea = CRGB(n_r, n_g, n_b);
        couleur_2 = couleur_2_rea;
      }
      reset_message();
    }
    if (condition_pointeur("/led_statue"))
    {
      led_on_off();
      Serial.println("connard");
      reset_message();
    }
    if (condition_pointeur("/led_mode")) // permet de choisi led mode
    {
      /*
      1: couleur_alternance
      2: couleur_alternance_ale_retour
      3: couleur_palette on
      4: clignotant
      8 : pallette couleur degrader
      9 : pallette couleur arc_en_ciels
      10: active ou désactive les leds
      */
      led_mode = message.substring(9, 11).toInt();
      Serial.println(led_mode);
    }
  }
  animation_led();
}
void setup()
{
  couleur_1 = couleur_1_rea;
  couleur_2 = couleur_2_rea;
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLT, MAX_AMPS);
  FastLED.setBrightness(BRIGNESS); // modifie le tableaux
  fill_gradient_RGB(degrader_which_two_colors, 16, couleur_1, CRGB(couleur_1.r / 2, couleur_1.g / 2, couleur_1.b / 2), couleur_2, couleur_1);
  currentPalette = degrader_which_two_colors;
  FastLED.show();
  FastLED.clear();
  currentBlending = LINEARBLEND;
  // begin_led();
  Serial.begin(9600);
  SerialBT.begin("Newtesla"); // nom module bluetooth
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
  instruction_controlle();
  int a = 0;
  mpu6050.update();
  temoin_y = mpu6050.getGyroY();
  temoin_x = mpu6050.getGyroX();

  if (alarme)
  {
    mpu6050.calcGyroOffsets(true);
    while (alarme)
    {
      instruction_controlle();
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
          instruction_controlle();
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
