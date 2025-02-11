const int TIP102 = 2;
const int IRLZ34N = 3;
const int FEMTOBUCK = 4;
const int POT = A7;

#define POT_SPEED 0
#define POT_BRIGHTNESS 1
#define POT_CONTROL 2
#define OUTPUT_TIP102 0
#define OUTPUT_IRLZ34N 1
#define OUTPUT_FEMTOBUCK 2

// DEFINE MODES HERE!!!!!
#define POT_MODE POT_CONTROL
#define OUTPUT_MODE OUTPUT_FEMTOBUCK

const int steps = 256;
int change = 1;
int currentLevel = 1;
byte levelTable[steps];

void setup() {
  Serial.begin(115200);
  fillLevelTable();
  Serial.print("Pot Mode: ");
  if (POT_MODE == POT_SPEED) Serial.println("SPEED");
  else if (POT_MODE == POT_BRIGHTNESS) Serial.println("BRIGHTNESS");
  else if (POT_MODE == POT_CONTROL) Serial.println("CONTROL");
  Serial.print("Output Mode: ");
  if (OUTPUT_MODE == OUTPUT_TIP102) Serial.println("TIP102");
  else if (OUTPUT_MODE == OUTPUT_IRLZ34N) Serial.println("IRLZ34N");
  else if (OUTPUT_MODE == OUTPUT_FEMTOBUCK) Serial.println("FEMTOBUCK");
}

void loop() {
  int potValue = analogRead(POT);
  int fadeSpeed = 10;
  int maxBrightness = steps - 1;
  float targetLevel = 0;

  if (POT_MODE == POT_BRIGHTNESS) {
    maxBrightness = map(potValue, 0, 4095, 0, steps - 1);
  } else if (POT_MODE == POT_SPEED) {
    fadeSpeed = map(potValue, 0, 4095, 2, 100);
  } else if (POT_MODE == POT_CONTROL) {
    targetLevel = map(potValue, 0, 4095, 0, steps - 1);
  }

  if (POT_MODE != POT_CONTROL) {
    if (currentLevel <= 0 || currentLevel >= maxBrightness) {
      change = -change;
    }
    currentLevel += change;
  } else {
    currentLevel += (targetLevel - currentLevel) * 0.1;
  }

  int outputPin;
  if (OUTPUT_MODE == OUTPUT_TIP102) outputPin = TIP102;
  else if (OUTPUT_MODE == OUTPUT_IRLZ34N) outputPin = IRLZ34N;
  else if (OUTPUT_MODE == OUTPUT_FEMTOBUCK) outputPin = FEMTOBUCK;

  analogWrite(outputPin, levelTable[currentLevel]);

  Serial.print("POT MODE: ");
  if (POT_MODE == POT_SPEED) Serial.print("SPEED | ");
  else if (POT_MODE == POT_BRIGHTNESS) Serial.print("BRIGHTNESS | ");
  else if (POT_MODE == POT_CONTROL) Serial.print("CONTROL | ");

  Serial.print("OUTPUT MODE: ");
  if (OUTPUT_MODE == OUTPUT_TIP102) Serial.print("TIP102 | ");
  else if (OUTPUT_MODE == OUTPUT_IRLZ34N) Serial.print("IRLZ34N | ");
  else if (OUTPUT_MODE == OUTPUT_FEMTOBUCK) Serial.print("FEMTOBUCK | ");

  Serial.print("POT VALUE: ");
  Serial.print(potValue);
  Serial.print(" | FADE SPEED: ");
  Serial.print(fadeSpeed);
  Serial.print(" | MAX BRIGHTNESS: ");
  Serial.print(maxBrightness);
  Serial.print(" | CURRENT BRIGHTNESS: ");
  Serial.println((int)currentLevel);

  delay(fadeSpeed);
}

void fillLevelTable() {
  /*
    For CIE1931, the following formulas have  Y as luminance, and
    Yn is the luminance of a white reference (basically, max luminance).
    This assumes a perceived lightness value L between 0 and 100,
    and  a luminance value (Y) of 0-1.0.
    if L > 8:  Y = ((L + 16) / 116)^3 * Yn
    if L <= 8: Y = L * 903.3 * Yn
  */
  // scaling factor to convert from 0-100 to 0-steps:
  float scalingFactor = 100.0 / float(steps);
  // luminance value:
  float luminance = 0.0;

  // iterate over the array and calculate the right value for it:
  for (int l = 0; l < steps; l++) {
    // you need to scale lightness from a 0-steps range to a 0-100 range:
    float lightness = float(l) * scalingFactor;
    if (lightness <= 8) {
      luminance = (lightness / 903.3);
    } else {
      luminance = (lightness + 16) / 116.0;
      luminance = pow(luminance, 3);
    }
    // multiply to get 0 to steps, and fill in the table:
    levelTable[l] = byte(luminance * steps);
  }
}
