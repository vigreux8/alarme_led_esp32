#include <Arduino.h>
#include <FastLED.h>
#define NUM_LED 31
#define DATA_PIN 27
#define COLOR_ORDER RGB
#define CHIPSET WS2812B
#define BRIGNESS 150 // elle vont entre 0 et 255 comme beaucoup de paramétre de fastled
#define VOLT 12
#define MAX_AMPS 500 // milliamps
#define UPDATES_PER_SECOND 100
int sens = 0;
bool led_continuer = true;
u_int8_t led_mode = 3;
int long attente = 0;
bool cycle_0 = true;      // utiliser dans clignotant
bool couleur_0 = true;    // utiliser dans clignotant
int index_clignotant = 0; // utiliser dans clignotant
CRGBPalette16 degrader_which_two_colors;
CRGBPalette16 currentPalette;
TBlendType currentBlending;

CRGB couleur_1_rea = CRGB{255, 0, 0};
CRGB couleur_2_rea = CRGB(255, 255, 255);
CRGB couleur_1 = CRGB(255, 0, 0);
CRGB couleur_2 = CRGB(255, 255, 255);

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

CRGB leds[NUM_LED];
//
void begin_led()
{
  couleur_1 = couleur_1_rea;
  couleur_2 = couleur_2_rea;
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLT, MAX_AMPS);
  FastLED.setBrightness(BRIGNESS); // modifie le tableaux
  fill_gradient_RGB(degrader_which_two_colors, 16, couleur_1, CRGB(couleur_1.r / 2, couleur_1.g / 2, couleur_1.b / 2), couleur_2, couleur_1);
  currentPalette = degrader_which_two_colors;
  FastLED.clear();
  currentBlending = LINEARBLEND;
}

void test()
{
}
void couleur_alternance_ale_retour()
{
  // permet de passer 0  ou plusieur fonction
  FastLED.setBrightness(BRIGNESS);
  static unsigned long empreinte_temps;
  int8_t t = 20;
  if (sens == 0)
  {
    empreinte_temps = millis();
    for (int i = 0; i < NUM_LED; i)
    {
      if (millis() - empreinte_temps >= t)
      {
        // Serial.println("dans la couble if");
        leds[i] = couleur_1;
        FastLED.show();
        i++;

        empreinte_temps = millis();
      }
    }
    sens = 1;
  }
  else
  {
    empreinte_temps = millis();
    for (int i = NUM_LED; i != 0; i)
    {
      if (millis() - empreinte_temps > t)
      {
        leds[i] = couleur_2;
        FastLED.show();
        i--;
        empreinte_temps = millis();
      }
    }
    sens = 0;
  }
}

void led_statique(CRGB couleur)
{

  for (u_int8_t i; i < NUM_LED; i++)
  {
    leds[i] = couleur;
  }
  FastLED.show();
}

void couleur_alternance()
{
  FastLED.setBrightness(BRIGNESS);
  static unsigned long empreinte_temps;
  int8_t t = 30;
  if (sens == 0)
  {
    empreinte_temps = millis();
    for (int i = 0; i < NUM_LED; i)
    {

      if (millis() - empreinte_temps > t)
      {
        leds[i] = couleur_1;
        FastLED.show();
        i++;
        empreinte_temps = millis();
      }
    }
    sens = 1;
  }
  else
  {
    Serial.println("boucle");
    empreinte_temps = millis();
    for (int i = 0; i < NUM_LED; i)
    {
      if (millis() - empreinte_temps > t)
      {
        leds[i] = couleur_2;
        FastLED.show();
        i++;
        empreinte_temps = millis();
      }
    }
    sens = 0;
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  FastLED.setBrightness(BRIGNESS);
  for (uint8_t i = 0; i <= NUM_LED; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, BRIGNESS, currentBlending);
    colorIndex -= 3;
  }
}

void pallete_couleur_run(int update_seconde = 100)
{
  static uint8_t startIndex = 0;
  static unsigned long empreinte_temps;
  FillLEDsFromPaletteColors(startIndex);
  startIndex = startIndex + 1; /* motion speed */
  empreinte_temps = millis();
  while (millis() - empreinte_temps <= 1000 / update_seconde)
  {
    FastLED.show();
  }
}

// effet clignotent
void clignotant(int8_t attente = 10)
{
  static unsigned long empreinte_temps;

  if (index_clignotant == 0 || cycle_0 == false)
  {

    for (int i = 0; i < NUM_LED; i++)
    {
      if (couleur_0)
      {
        leds[i] = couleur_1;
        FastLED.setBrightness(0);
        if (i >= NUM_LED)
        {
          couleur_0 = false;
        }
      }
      else
      {
        leds[i] = couleur_2;
        FastLED.setBrightness(0);
        if (i >= NUM_LED)
        {
          couleur_0 = true;
        }
      }
    }
  }
  if (cycle_0)
  {
    empreinte_temps = millis();
    while (millis() - empreinte_temps <= attente * 3 && cycle_0)
    {
      if (millis() - empreinte_temps >= attente)
      {
        FastLED.setBrightness(index_clignotant);
        FastLED.show();
        if (index_clignotant == BRIGNESS)
        {
          cycle_0 = false;
          couleur_0 = false;
          break;
        }
        index_clignotant++;
      }
    }
  }
  else
  {
    empreinte_temps = millis();
    while (millis() - empreinte_temps <= attente * 3 && cycle_0 == false)
    {
      if ((millis() - empreinte_temps) >= attente)
      {
        FastLED.setBrightness(index_clignotant);
        FastLED.show();
        if (index_clignotant == 0)
        {
          cycle_0 = true;
          couleur_0 = true;
          break;
        }
        index_clignotant--;
      }
    }
  }
}

void led_on_off()
{
  if (led_continuer)
  {
    fill_solid(leds, NUM_LED, CRGB::Black);
    FastLED.show();
    led_continuer = false;
    Serial.println("je suis bien off");
  }
  else
  {
    led_continuer = true;
    Serial.println("je suis bien on");
  }
}

void couleur_loop()
{
  couleur_1 = couleur_1_rea;
  couleur_2 = couleur_2_rea;
  fill_gradient_RGB(degrader_which_two_colors, 16, couleur_1, CRGB(couleur_1.r / 2, couleur_1.g / 2, couleur_1.b / 2), couleur_2, couleur_1);
}
/*
void setup()
{
  couleur_1 = couleur_1_rea;
  couleur_2 = couleur_2_rea;
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLT, MAX_AMPS);
  FastLED.setBrightness(BRIGNESS);
  Serial.begin(9600); // modifie le tableaux
  fill_gradient_RGB(degrader_which_two_colors, 16, couleur_1, CRGB(couleur_1.r / 2, couleur_1.g / 2, couleur_1.b / 2), couleur_2, couleur_1);
  currentPalette = degrader_which_two_colors;
  FastLED.show();
  FastLED.clear();
  currentBlending = LINEARBLEND;
  // put your setup code here, to run once:
}



void loop()
{
  couleur_1 = couleur_1_rea;
  couleur_2 = couleur_2_rea;
  fill_gradient_RGB(degrader_which_two_colors, 16, couleur_1, CRGB(couleur_1.r / 2, couleur_1.g / 2, couleur_1.b / 2), couleur_2, couleur_1);

  uint8_t secondHand = (millis() / 1000) % 60;

  if (secondHand >= 0 && secondHand < 10)
  {
    clignotant();
  }
  else if (secondHand >= 10 && secondHand < 20)
  {
    currentPalette = test_rouge_blanc;
    pallete_couleur_run(150);
  }
  else if (secondHand >= 20 && secondHand < 30)
  {
    couleur_alternance();
  }
  else if (secondHand >= 30)
  {
    currentPalette = RainbowColors_p;
    pallete_couleur_run(50);
  }
}
*/