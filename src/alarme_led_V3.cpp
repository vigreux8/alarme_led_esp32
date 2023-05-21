#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BTScan.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include "C:\Users\vigre\OneDrive\Documents\PlatformIO\Projects\Alarme_plus_ledV2\include\ledv2.cpp"
MPU6050 mpu6050(Wire);
BluetoothSerial SerialBT;
#define klaxon 2
#define lumiere 0
#define led_pin 4
#define TAILLE_POINTEUR 1
unsigned long timer = 0;
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
uint8_t memorie_led_mode;
uint8_t vitesse_degrader = 100;

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
void debeging_print(String nom, int variable)
{
  Serial.print(nom);
  Serial.println(variable);
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
bool condition_pointeur(String rechercher) // savoir si la valeur rechercher se trouve dans le string
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
    //log Serial.print("message: ");
    //log Serial.println(message);
    //log Serial.print("condition: ");
    Serial.println((message != intruction));
    if (message != intruction) // la chaine instruction et différent de message
    {
      intruction = message;
      //log Serial.print("instruction: ");
      //log Serial.println(intruction);
      //log Serial.print("a détecter le mot: ");
      if (condition_pointeur("/stop") || condition_pointeur("/alarme_off")) // stop
      {
        Serial.println("fonctions off");
        etat_velo();
        continuer = false;
        alarme =false;
        intruction = String("");
        message = String("");
        cycle = 0;
      }
    }
  }
}

// retoucher les delais et temps

void avertissement_alarme(int boucle=4, uint8_t duree = 50) // lance une alerte et rajoute +1 a avertissement
{
  memorie_led_mode = led_mode;
  led_mode = 0;
  uint8_t changeur = 0;

  int bip = 0;
  digitalWrite(klaxon,HIGH);
  int long temps_temoin = millis();
  while (bip <= boucle)
  {
    
    if (millis() - temps_temoin >= duree)
    {
      digitalWrite(klaxon, !digitalRead(klaxon));
      // log Serial.println(!digitalRead(klaxon));
      bip++;
      if (digitalRead(klaxon) == HIGH && changeur != 1)
      {
        led_statique(CRGB::Red);
        changeur = 1;
      }
      else if (digitalRead(klaxon) == LOW && changeur != 0)
      {
        led_statique(CRGB::Black);
        changeur = 0;
      }
      temps_temoin = millis();
    }
  }
  led_mode = memorie_led_mode;
}

template<typename F>
void super_alerte(int duree, F fonction=nullptr) // super alerte indiauer la duree en ms seconde
{
  int bip = 0;
  continuer = true;
  unsigned long empreinte_temps = millis();
  digitalWrite(klaxon, HIGH);
  Serial.println("SuperAlerte");
  while (millis() - empreinte_temps <= duree && continuer)
  {
    reception_bluetooth();
    fonction();
    Serial.println("SuperAlerte");
  }
  digitalWrite(klaxon, LOW);
  continuer = false;
}

void bombe_alarme(float temps = 2000) // bip facon bombe indiquer le temps en ms
{
  memorie_led_mode = led_mode;
  led_mode = 0;
  int bip = 0;
  continuer = true;
  static unsigned long temps_temoin = millis();
  uint8_t changeur = 0;

  while (temps >= 1.2 && continuer)
  {
    // objectifs : que la boucle tourne en continue sans arret
    if ((millis() - temps_temoin) >= temps)
    {
      reception_bluetooth();
      if (digitalRead(klaxon) == HIGH && changeur != 1)
      {
        led_statique(CRGB::Red);
        changeur = 1;
      }
      else if (digitalRead(klaxon) == LOW && changeur != 0)
      {
        led_statique(CRGB::White);
        changeur = 0;
      }
      digitalWrite(klaxon, !digitalRead(klaxon));
      FastLED.show();
      Serial.print("bip  temps :");
      Serial.println(temps);
      temps = temps / 1.15;
      temps_temoin = millis();
    }
  }
  digitalWrite(klaxon, LOW);
  led_statique(CRGB(0, 0, 0));
  if (continuer)
  {
    const int TEMPS_FIXE = 10000;
    temps_temoin = millis();
    while ((millis() - temps_temoin) <= 3000 && continuer)
    {
      reception_bluetooth();
    }

    temps_temoin = millis();
    while ((millis() - temps_temoin) <= TEMPS_FIXE && continuer)
    {
      reception_bluetooth();
      led_statique(couleur_1);
      digitalWrite(klaxon, HIGH);
    }
    digitalWrite(klaxon, LOW);
    continuer = false;
    led_mode = memorie_led_mode;
  }
  else
  {
    led_mode = memorie_led_mode;
  }
}

void instruction_controlle()
{
  if (SerialBT.available())
  {
    reception_bluetooth();
    if (condition_pointeur("/alarme_on")) // active alarme
    {
      alarme = true;
      Serial.println("alarme activer");
      SerialBT.print('a'); // confirme alarme activer
      etat_velo();
    }
    if (condition_pointeur("/alarme_off")) // desactive alarme
    {
      alarme = false;
      continuer = false;
      Serial.println("alarme desactiver"); // confirme alarme desactiver
      etat_velo();
      
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
      super_alerte(10000,instruction_controlle);
    }
    if (condition_pointeur("/klaxon")) // active klaxon
    {
      Serial.println("klaxon activer");
      avertissement_alarme(2*2, 50);
      reset_message();
    }
    if (condition_pointeur("/bombe_alarme")) // active timeur bombe alarme
    {
      Serial.println("bombe activer");
      bombe_alarme(2000);
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
      if (led_mode == 3)
      {
        led_mode = 8;
        
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
    pallete_couleur_run(vitesse_degrader);
  }
  if (led_mode == 4 && led_continuer)
  {
    clignotant(100);
  }
  if (led_mode == 8)
  {
    currentPalette = degrader_which_two_colors;
    vitesse_degrader = 100;
    led_mode = 3;
  }
  if (led_mode == 9)
  {
    currentPalette = RainbowColors_p;
    vitesse_degrader = 50;
    led_mode = 3;
  }
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
  couleur_loop();

  if (alarme)
  {
    mpu6050.calcGyroOffsets(true);
    mpu6050.update();
    static unsigned long timer_avertissement;
    temoin_y = mpu6050.getGyroY();
    temoin_x = mpu6050.getGyroX();
    while (alarme)
    {
      instruction_controlle();
      couleur_loop();
      mpu6050.update();
      if (millis() - timer > 1000)
      {/* log
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
        */
        timer = millis();

        if (millis()-timer_avertissement >= 20000 && avertissement == 1) // reset avertissement apres 50 cycle
        {
          Serial.println(avertissement);
          avertissement = 0;
        }
        if (mpu6050.getGyroX() > temoin_x + precision_angle || mpu6050.getGyroX() < temoin_x - precision_angle || mpu6050.getGyroY() < temoin_y - precision_angle || mpu6050.getGyroY() > temoin_y + precision_angle)
        {
          instruction_controlle();
          couleur_loop();
          if (avertissement == 1 &&  millis()-timer_avertissement >=2000 ) // si il a avertie 1 fois lance la grosse alarme
          {
            Serial.println(avertissement);
            super_alerte(10000,instruction_controlle); // duree en ms seconde
            mpu6050.update();
            temoin_y = mpu6050.getGyroY();
            temoin_x = mpu6050.getGyroX();
            avertissement = 0;
            continue; // permet de sauter l'etape d'apres
          }
          else if (avertissement == 0 && millis()-timer_avertissement >=2400)
          {
            avertissement++;
            timer_avertissement = millis();
            avertissement_alarme(4, 50);
          }
        }
      }
    }
  }
}
