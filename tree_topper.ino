#include <Adafruit_NeoPixel.h>

#define BRIGHTNESS 100
#define PIN 0
#define PIXELS 8
#define LOOP_DELAY 10000
#define FADE_DELAY 75
#define STEPS 100
#define NUM_COLORS 3
#define CHASE_COUNT 25
#define CHASE_DELAY 500

typedef struct color {
  int red;
  int green;
  int blue;
} COLOR;

typedef struct COLOR_DELTA {
  double red;
  double green;
  double blue;
} COLOR_DELTA;

COLOR RED = { 255, 0, 0 };
COLOR GREEN = {0, 255, 0};
COLOR WHITE = { 255, 255, 255};
COLOR OFF = {0, 0, 0};
int loop_index = 0;
COLOR last = OFF;

COLOR colors[] =
{
  RED,
  WHITE,
  GREEN
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(BRIGHTNESS);
}

void setColor(COLOR color)
{
  uint16_t j = 0;
  for(j = 0; j < strip.numPixels(); j++) {
    strip.setPixelColor(j, color.red, color.green, color.blue);
  }
  strip.show();
}

double color_delta(int current, int next)
{
  return (double)(next - current) / STEPS;
}

int apply_delta(int current, int target, double delta)
{
  // With rounding errors, it's possible for us to overshoot what we are going for, so add some checks so we don't overshoot
  if (delta < 0 && current <= target)
  {
    return target;
  }
  else if (delta > 0 && current >= target)
  {
    return target;
  }
  else
  {
    return current + delta;
  }
}

void transition(COLOR last, COLOR next)
{
  bool transitioned = false;
  COLOR current = last;
  COLOR_DELTA delta = { 0, 0, 0 };
  int i = 0;

  delta.red = color_delta(current.red, next.red);
  delta.blue = color_delta(current.blue, next.blue);
  delta.green = color_delta(current.green, next.green);

  for (; i < STEPS; i++)
  {
    current.red = apply_delta(current.red, next.red, delta.red);
    current.blue = apply_delta(current.blue, next.blue, delta.blue);
    current.green = apply_delta(current.green, next.green, delta.green);
    setColor(current);
    delay(FADE_DELAY);
  }

  // ensure we are actually at the intended color
  setColor(next);
  delay(FADE_DELAY);
}

void chase()
{
  COLOR current_color = OFF;

  uint16_t i = 0;
  uint16_t j = 0;
  uint16_t color = 0;

  setColor(OFF);

  for(j = 0; j < CHASE_COUNT; j++)
  {
    for(i = 0; i < strip.numPixels(); i++, color++)
    {
      current_color = colors[color % NUM_COLORS];
      strip.setPixelColor(i, current_color.red, current_color.green, current_color.blue);
      if (j == 0)
      {
        strip.show();
        delay(CHASE_DELAY / 2);
      }
    }

    if (j > 0)
    {
      strip.show();
    }
    delay(CHASE_DELAY);
  }
}

void loop()
{
  COLOR color = OFF;

  if ((loop_index == 0) ||
    (loop_index % 42) == 0)
  {
    chase();
    color = WHITE;
  }
  else if ((loop_index % NUM_COLORS) == 0)
  {
    color = RED;
  }
  else if ((loop_index % NUM_COLORS) == 1)
  {
    color = WHITE;
  }
  else if ((loop_index % NUM_COLORS) == 2)
  {
    color = GREEN;
  }
  else
  {
    color = WHITE;
  }
  transition(last, color);

  loop_index++;
  last = color;
  delay(LOOP_DELAY);
}
