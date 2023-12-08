/*
  StripOS by Arsenii Tymoshenko
*/

#include <GyverEncoder.h>
#include <PinChangeInterrupt.h>
#include <microLiquidCrystal_I2C.h>
#include <microWire.h>
#include <FastLED.h>

Encoder br(5, 6, 4);
Encoder sm(8, 9, 7);
Encoder hue(11, 12, 10);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define NUM_LEDS 131
#define LED_PIN 13

#define HUE_START 3  // Starting color of fire -- not used!
#define HUE_GAP 18  // Color coefficient of fire
#define SMOOTH_K 0.15  // Smoothness coefficient of fire -- not used!
#define MIN_BRIGHT 80  // Minimum brightness of fire
#define MAX_BRIGHT 255  // Maximum brightness of fire
#define MIN_SAT 245  // Minimum saturation
#define MAX_SAT 255  // Maximum saturation

#define ZONE_AMOUNT NUM_LEDS
byte zoneValues[ZONE_AMOUNT];
byte zoneRndValues[ZONE_AMOUNT];

CRGB leds[NUM_LEDS];

#define FOR_i(from, to) for (int i = (from); i < (to); i++)
#define FOR_j(from, to) for (int j = (from); j < (to); j++)

float smooth_k_int = 0.25;
byte smooth_rainbow = 2;
byte min_bright = 25;
byte max_bright = 255;
byte fade_brightness;
byte smooth_fade = 40;
byte fill_color = 115;
byte fade_color = 115;
byte brightness_k = 255;
byte hue_start_int = 0;
byte saturation;
byte change = 1;
boolean flag;
boolean lcd_change;
boolean side;
byte cur_color = 0;
int new_comet_id;
int running_speed = 15;
int running_freq = 100;
//unsigned long Ukraine_pro_timer;

byte red = 0;
byte green = 255;

volatile int ads;

int module;
float module_float;

byte color_pos[50][50];

uint32_t start_timer;
uint32_t button_debounce_timer;

unsigned long fade_timer = 0;
uint32_t running_timer;
uint32_t random_timer;

uint32_t loop_timer;

uint32_t change_timer;

void setup()
{
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  br.setType(TYPE2);
  sm.setType(TYPE2);
  hue.setType(TYPE2);

  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  //lcd.print("Hello world!");
  pinMode(13, OUTPUT);

  pinMode(3, INPUT_PULLUP);
  //randomSeed(analogRead(A4));
  //delay(1500);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  // attach the new PinChangeInterrupts and enable event functions below
  attachPCINT(digitalPinToPCINT(4), brightness_tick, CHANGE);
  attachPCINT(digitalPinToPCINT(5), brightness_tick, CHANGE);
  attachPCINT(digitalPinToPCINT(6), brightness_tick, CHANGE);

  attachPCINT(digitalPinToPCINT(7), smooth_tick, CHANGE);
  attachPCINT(digitalPinToPCINT(8), smooth_tick, CHANGE);
  attachPCINT(digitalPinToPCINT(9), smooth_tick, CHANGE);

  attachPCINT(digitalPinToPCINT(10), hue_tick, CHANGE);
  attachPCINT(digitalPinToPCINT(11), hue_tick, CHANGE);
  attachPCINT(digitalPinToPCINT(12), hue_tick, CHANGE);
  lcd_change = 1;
  pinMode(2, OUTPUT);
  start_timer = millis();
}
void loop()
{
  if (millis() - start_timer >= 100)
  {
    digitalWrite(2, 1);
  }
  if (change == 1 && lcd_change == 1)
  {
    lcd_change = 0;
    lcd.clear();
    lcd.home();
    lcd.print("Br=");
    lcd.print(brightness_k);
    lcd.setCursor(0, 1);
    lcd.print("Smooth=");
    lcd.print(smooth_k_int);
    lcd.setCursor(8, 0);
    lcd.print("HUE=");
    lcd.print(hue_start_int);
  }
  else if (change == 2 && lcd_change == 1)
  {
    lcd_change = 0;
    lcd.clear();
    lcd.home();
    lcd.print("Brightness=");
    lcd.print(brightness_k);
    lcd.setCursor(0, 1);
    lcd.print("Smooth=");
    lcd.print(smooth_rainbow);
  }
  else if (change == 3 && lcd_change == 1)
  {
    lcd_change = 0;
    lcd.clear();
    lcd.home();
    lcd.print("Br=");
    lcd.print(max_bright);
    lcd.setCursor(8, 0);
    lcd.print("HUE=");
    lcd.print(fade_color);
    lcd.setCursor(0, 1);
    lcd.print("Smooth=");
    lcd.print(smooth_fade);
  }
  else if (change == 4 && lcd_change == 1)
  {
    lcd_change = 0;
    lcd.clear();
    lcd.home();
    lcd.print("Br=");
    lcd.print(brightness_k);
    lcd.setCursor(8, 0);
    lcd.print("HUE=");
    lcd.print(fill_color);
    lcd.setCursor(0, 1);
  }
  else if (change == 5 && lcd_change == 1)
  {
    lcd_change = 0;
    lcd.clear();
    lcd.home();
    lcd.print("Br=");
    lcd.print(brightness_k);
    lcd.setCursor(0, 1);
    lcd.print("Freq=");
    lcd.print(running_freq);
    lcd.setCursor(7, 0);
    lcd.print("Speed=");
    lcd.print(running_speed);
  }
  if (change != 3)
  {
    FastLED.setBrightness(brightness_k);
  }
  else
  {
    FastLED.setBrightness(fade_brightness);
  }
  if (millis() - button_debounce_timer >= 80 && digitalRead(3) == 0 && flag != 1)
  {
    button_debounce_timer = millis();
    if (change < 5)
    {
      change++;
      lcd_change = 1;
    }
    else
    {
      change = 1;
      lcd_change = 1;
    }
    //change = !change;
    flag = 1;
    //lcd_change = 1;
    if (change == 1 && lcd_change == 1)
    {
      lcd_change = 0;
      lcd.clear();
      lcd.home();
      lcd.print("Br=");
      lcd.print(brightness_k);
      lcd.setCursor(0, 1);
      lcd.print("Smooth=");
      lcd.print(smooth_k_int);
      lcd.setCursor(8, 0);
      lcd.print("HUE=");
      lcd.print(hue_start_int);
    }
    else if (change == 2 && lcd_change == 1)
    {
      lcd_change = 0;
      lcd.clear();
      lcd.home();
      lcd.print("Brightness=");
      lcd.print(brightness_k);
      lcd.setCursor(0, 1);
      lcd.print("Smooth=");
      lcd.print(smooth_rainbow);
    }
    else if (change == 3 && lcd_change == 1)
    {
      lcd_change = 0;
      lcd.clear();
      lcd.home();
      lcd.print("Br=");
      lcd.print(max_bright);
      lcd.setCursor(8, 0);
      lcd.print("HUE=");
      lcd.print(fade_color);
      lcd.setCursor(0, 1);
      lcd.print("Smooth=");
      lcd.print(smooth_fade);
    }
    else if (change == 4 && lcd_change == 1)
    {
      lcd_change = 0;
      lcd.clear();
      lcd.home();
      lcd.print("Br=");
      lcd.print(brightness_k);
      lcd.setCursor(8, 0);
      lcd.print("HUE=");
      lcd.print(fill_color);
      lcd.setCursor(0, 1);
    }
  }
  Serial.println(change);
  if (digitalRead(3) == 1 && flag == 1)
  {
    flag = 0;
  }

  if (change == 1)
  {
    fireTick();
    //running_lights();
  }
  else if (change == 2)
  {
    rainbow();
  }
  else if (change == 3)
  {
    fade_effect();
  }
  else if (change == 4)
  {
    fill_effect();
  }
  else if (change == 5)
  {
    running_lights();
  }
  //Serial.println("loop");

  //brightness_k=255;
  //smooth_k_int=0.08;
  //hue_start_int=2;
}

void fireTick()
{
  static uint32_t prevTime, prevTime2;

  if (millis() - prevTime > 100)
  {
    prevTime = millis();
    for (int i = 0; i < ZONE_AMOUNT; i++)
    {
      zoneRndValues[i] = random(0, 10);
    }
  }

  if (millis() - prevTime2 > 20)
  {
    prevTime2 = millis();
    for (int i = 0; i < ZONE_AMOUNT; i++)
    {
      zoneValues[i] = (float)zoneValues[i] * (1 - SMOOTH_K) + (float)zoneRndValues[i] * 10 * SMOOTH_K;
      leds[i] = getFireColor(zoneValues[i]);
    }
    FastLED.show();
  }
}

void rainbow()
{
  static byte counter = 0;
  counter += smooth_rainbow;

  // Iterate over each LED in the strip
  for (int i = 0; i < NUM_LEDS; i++)
  {
    // Set the LED color to the current hue value
    leds[i] = CHSV(counter + (i * 255 / NUM_LEDS), 255, 255);
  }

  FastLED.show();
}

void fade_effect()
{
  static byte color = 0;
  color = fade_color;

  static bool shift;

  byte minuend;
  byte smooth_correct;
  if (smooth_fade >= 30)
  {
    minuend = 3;
    smooth_correct = 5;
  }
  else
  {
    minuend = 5;
    smooth_correct = smooth_fade;
  }

  if (millis() - fade_timer >= smooth_correct && shift == false)
  {
    fade_timer = millis();
    if (fade_brightness <= max_bright - minuend)
    {
      fade_brightness += minuend;
    }
    else
    {
      shift = true;
    }
  }
  else if (millis() - fade_timer >= smooth_correct && shift == true)
  {
    fade_timer = millis();
    if (fade_brightness >= min_bright + minuend)
    {
      fade_brightness -= minuend;
    }
    else
    {
      shift = false;
    }
  }
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(color, 255, 255);
  }

  FastLED.show();
}

void fill_effect()
{
  static byte color = 0;
  color = fill_color;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(color, 255, 255);
  }

  FastLED.show();
}

void running_lights()
{
  byte comet_brightness_arr[] = {255, 255, 255, 255, 255, 220, 220, 220, 220, 220, 185, 185, 185, 185, 185, 150, 150, 150, 150, 150, 115, 115, 115, 115, 115, 80, 80, 80, 80, 80, 45, 45, 45, 45, 45, 10, 10, 10, 10, 10, 10};
  if (millis() - running_timer >= running_speed)
  {
    running_timer = millis();

    //CRGB color = CHSV(cur_color, 255, 255);
    if (millis() - random_timer >= running_freq)  //1310
    {
      leds[0] = CHSV(cur_color, 255, comet_brightness_arr[new_comet_id]);
      Serial.println(new_comet_id);
      new_comet_id++;
      if (new_comet_id == 41)
      {
        new_comet_id = 0;
        //if (random(0, 10) == 5)
        {
          cur_color += 50;
        }
        random_timer = millis();
      }
    }
    for (int i = NUM_LEDS - 1; i > 0; i--)
    {
      leds[i] = leds[i - 1];
    }
    FastLED.show();
  }
}

void Ukraine()
{
  for (int i = 0; i < 36; i++)
  {
    leds[i] = CRGB(0, 0, 255);
    //leds[i+1] = CRGB(255,120,0);
  }
  for (int i = 37; i < 73; i++)
  {
    //leds[i] = CRGB(0,0,255);
    leds[i] = CRGB(255, 120, 0);
  }
  FastLED.show();
}

void brightness_tick()
{
  br.tick();
  if (change == 1 || change == 2 || change == 4 || change == 5)
  {
    //Serial.println("I'm here!");
    if (br.isLeft())
    {
      Serial.println("br left");
      if (brightness_k > 0)
      {
        brightness_k--;
        lcd_change = 1;
      }
    }
    if (br.isRight())
    {
      Serial.println("br right");
      if (brightness_k < 255)
      {
        brightness_k++;
        lcd_change = 1;
      }
    }

    if (br.isFastL())
    {
      Serial.println("br Fleft");
      if (brightness_k >= 10)
      {
        brightness_k -= 10;
        lcd_change = 1;
      }
    }
    if (br.isFastR())
    {
      Serial.println("br Fright");
      if (brightness_k <= 245)
      {
        brightness_k += 10;
        lcd_change = 1;
      }
    }

    if (br.isPress())
    {
      Serial.println("br Press");
      brightness_k = 255;
      lcd_change = 1;
    }
  }
  else if (change == 3)
  {
    if (br.isLeft())
    {
      Serial.println("br left");
      if (max_bright > min_bright)
      {
        max_bright--;
        lcd_change = 1;
      }
    }
    if (br.isRight())
    {
      Serial.println("br right");
      if (max_bright < 255)
      {
        max_bright++;
        lcd_change = 1;
      }
    }

    if (br.isFastL())
    {
      Serial.println("br Fleft");
      if (max_bright >= (min_bright + 10))
      {
        max_bright -= 10;
        lcd_change = 1;
      }
    }
    if (br.isFastR())
    {
      Serial.println("br Fright");
      if (max_bright <= 245)
      {
        max_bright += 10;
        lcd_change = 1;
      }
    }

    if (br.isPress())
    {
      Serial.println("br Press");
      max_bright = 255;
      lcd_change = 1;
    }
  }
}
void smooth_tick()
{
  sm.tick();
  if (change == 1)
  {
    if (sm.isLeft())
    {
      Serial.println("sm left");
      if (smooth_k_int > 0.05)
      {
        smooth_k_int -= 0.01;
        lcd_change = 1;
      }
    }
    if (sm.isRight())
    {
      Serial.println("sm right");
      if (smooth_k_int < 0.49)
      {
        smooth_k_int += 0.01;
        lcd_change = 1;
      }
    }

    if (sm.isPress())
    {
      Serial.println("sm Double");
      smooth_k_int = 0.20;
      lcd_change = 1;
    }
  }
  else if (change == 2)
  {
    if (sm.isLeft())
    {
      Serial.println("sm left");
      if (smooth_rainbow > 0)
      {
        smooth_rainbow -= 1;
        lcd_change = 1;
      }
    }
    if (sm.isRight())
    {
      Serial.println("sm right");
      if (smooth_rainbow < 9)
      {
        smooth_rainbow += 1;
        lcd_change = 1;
      }
    }

    if (sm.isPress())
    {
      Serial.println("sm Double");
      smooth_rainbow = 3;
      lcd_change = 1;
    }
  }
  else if (change == 3)
  {
    if (sm.isLeft())
    {
      Serial.println("sm left");
      if (smooth_fade > 1)
      {
        smooth_fade -= 1;
        lcd_change = 1;
      }
    }
    if (sm.isRight())
    {
      Serial.println("sm right");
      if (smooth_fade < 30)
      {
        smooth_fade += 1;
        lcd_change = 1;
      }
    }

    if (sm.isPress())
    {
      Serial.println("sm Double");
      smooth_fade = 3;
      lcd_change = 1;
    }
  }
  else if (change == 5)
  {
    if (sm.isLeft())
    {
      Serial.println("sm left");

      running_freq -= 10;
      lcd_change = 1;
    }
    if (sm.isRight())
    {
      Serial.println("sm right");
      running_freq += 10;
      lcd_change = 1;
    }

    if (sm.isFastL())
    {
      Serial.println("sm left");

      running_freq -= 20;
      lcd_change = 1;
    }
    if (sm.isFastR())
    {
      Serial.println("sm right");
      running_freq += 20;
      lcd_change = 1;
    }

    if (sm.isPress())
    {
      Serial.println("sm Double");
      running_freq = 200;
      lcd_change = 1;
    }
    if (running_freq < 0)
    {
      running_freq = 0;
    }
    if (running_freq > 5000)
    {
      running_freq = 5000;
    }
  }
}

void hue_tick()
{
  hue.tick();
  if (change == 1)
  {
    if (hue.isLeft())
    {
      Serial.println("hue left");
      hue_start_int--;
      lcd_change = 1;
      if (hue_start_int < 0)
      {
        hue_start_int = 0;
      }
    }
    if (hue.isRight())
    {
      Serial.println("hue right");

      hue_start_int++;
      lcd_change = 1;
      if (hue_start_int > 255)
      {
        hue_start_int = 255;
      }
    }

    if (hue.isFastL())
    {
      Serial.println("br Fleft");

      hue_start_int -= 5;
      lcd_change = 1;
      if (hue_start_int < 0)
      {
        hue_start_int = 0;
      }
    }
    if (hue.isFastR())
    {
      Serial.println("br Fright");
      hue_start_int += 5;
      lcd_change = 1;
      if (hue_start_int > 255)
      {
        hue_start_int = 255;
      }
    }

    if (hue.isPress())
    {
      Serial.println("hue Double");
      hue_start_int = 0;
      lcd_change = 1;
    }
  }
  else if (change == 3)
  {
    if (hue.isLeft())
    {
      Serial.println("hue left");
      fade_color--;
      lcd_change = 1;
      if (fade_color < 0)
      {
        fade_color = 0;
      }
    }
    if (hue.isRight())
    {
      Serial.println("hue right");
      fade_color++;
      lcd_change = 1;

      if (fade_color > 255)
      {
        fade_color = 255;
      }
    }

    if (hue.isFastL())
    {
      Serial.println("br Fleft");
      fade_color -= 5;
      lcd_change = 1;
      if (fade_color < 0)
      {
        fade_color = 0;
      }
    }
    if (hue.isFastR())
    {
      Serial.println("br Fright");
      fade_color += 5;
      lcd_change = 1;
      if (fade_color > 255)
      {
        fade_color = 255;
      }
    }

    if (hue.isPress())
    {
      Serial.println("hue Double");
      fade_color = 0;
      lcd_change = 1;
    }
  }
  else if (change == 4)
  {
    if (hue.isLeft())
    {
      Serial.println("hue left");

      fill_color--;
      lcd_change = 1;
      if (fill_color < 0)
      {
        fill_color = 0;
      }
    }
    if (hue.isRight())
    {
      Serial.println("hue right");

      fill_color++;
      lcd_change = 1;
      if (fill_color > 255)
      {
        fill_color = 255;
      }
    }

    if (hue.isFastL())
    {
      Serial.println("br Fleft");

      fill_color -= 5;
      lcd_change = 1;
      if (fill_color < 0)
      {
        fill_color = 0;
      }
    }
    if (hue.isFastR())
    {
      Serial.println("br Fright");

      fill_color += 5;
      lcd_change = 1;
      if (fill_color > 255)
      {
        fill_color = 255;
      }
    }

    if (hue.isPress())
    {
      Serial.println("hue Double");
      fill_color = 0;
      lcd_change = 1;
    }
  }
  else if (change == 5)
  {
    if (hue.isLeft())
    {
      Serial.println("hue left");
      running_speed--;
      lcd_change = 1;
    }
    if (hue.isRight())
    {
      Serial.println("hue right");

      running_speed++;
      lcd_change = 1;
    }

    if (hue.isFastL())
    {
      Serial.println("br Fleft");

      running_speed -= 5;
      lcd_change = 1;
    }
    if (hue.isFastR())
    {
      Serial.println("br Fright");
      running_speed += 5;
      lcd_change = 1;
    }
    if (running_speed < 1)
    {
      running_speed = 1;
    }
    if (running_speed > 35)
    {
      running_speed = 35;
    }

    if (hue.isPress())
    {
      Serial.println("hue Double");
      running_speed = 15;
      lcd_change = 1;
    }
  }
}

CRGB getFireColor(int val)
{
  return CHSV(HUE_START + map(val, 20, 60, 0, HUE_GAP),  // H
      constrain(map(val, 20, 60, MIN_SAT, MAX_SAT), 0, 255),  // S
      constrain(map(val, 20, 60, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
  );
}
