#include <Arduino.h>
#include <FastLED.h>
#define NUM_LED 30
#define DATA_PIN 32
#define COLOR_ORDER RGB
#define CHIPSET WS2812B
#define BRIGNESS 150 // elle vont entre 0 et 255 comme beaucoup de paramétre de fastled
#define VOLT 12
#define MAX_AMPS 500 // milliamps
#define UPDATES_PER_SECOND 100
int sens = 0;
bool led_continuer = true;
int led_mode = 3;
int long attente =0;
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

void couleur_alternance_ale_retour()
{
  if (sens == 0)
  {
    for (int i = 0; i < NUM_LED; i++)
    {
      leds[i] = couleur_1;
      FastLED.show();
      delay(25);
    }
    sens = 1;
  }
  else
  {
    Serial.println("boucle");
    for (int i = NUM_LED; i != 0; i--)
    {
      leds[i] = couleur_2;
      FastLED.show();
      delay(25);
    }
    sens = 0;
  }
}

void couleur_alternance()
{
  if (sens == 0)
  {
    for (int i = 0; i < NUM_LED; i++)
    {
      leds[i] = couleur_1;
      FastLED.show();
      delay(25);
    }
    sens = 1;
  }
  else
  {
    Serial.println("boucle");
    for (int i = 0; i < NUM_LED; i++)
    {
      leds[i] = couleur_2;
      FastLED.show();
      delay(25);
    }
    sens = 0;
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  FastLED.setBrightness(BRIGNESS);

  for (int i = NUM_LED; i >= 0; i--)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, BRIGNESS, currentBlending);
    colorIndex += 3;
  }
}

void pallete_couleur_run(int update_seconde = 100)
{
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */

  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(1000 / update_seconde);
}

// effet clignotent
void clignotant(int attente = 5)
{

  for (int i = 0; i < NUM_LED; i++)
  {
    leds[i] = couleur_1;
  }
  for (int i = 0; i <= BRIGNESS; i++)
  {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(attente);
  }

  for (int i = BRIGNESS; i >= 0; i--)
  {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(attente);
  }
  for (int i = 0; i < NUM_LED; i++)
  {
    leds[i] = couleur_2;
  }
  for (int i = 0; i <= BRIGNESS; i++)
  {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(attente);
  }

  for (int i = BRIGNESS; i >= 0; i--)
  {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(attente);
  }
}

void led_on_off()
{
  if (led_continuer)
  {
   fill_solid(leds,NUM_LED,CRGB::Black);
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