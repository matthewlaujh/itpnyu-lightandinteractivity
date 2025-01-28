// Based off of Tom's CIE1931 Fade Curve https://github.com/tigoe/LightProjects/tree/main/FadeCurves/CIE1931Fade
// Had to use some llm to help understand the math get the RGB values to work with the white LED, but after that I played with the values to get them to match the white LED as best as possible.

const int steps = 256;       
const int whitePin = 9;      
const int redPin = 3;        
const int greenPin = 5;      
const int bluePin = 6;       
const int buttonPin = 2;     
bool paused = false;
int change = 1;              
int currentLevel = 1;        
byte levelTable[steps];      

const float redValue = 0.175; 
const float greenValue = 0.08;
const float blueValue = 0.06; 
const float whiteMaxBrightness = 0.4;

void setup() {
  Serial.begin(9600);
  fillLevelTable(); // Pre-calculate the brightness levels
  pinMode(whitePin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), togglePause, FALLING);
}

void loop() {
  if (paused) {
    analogWrite(whitePin, 0);
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
    return;
  }

  if (currentLevel <= 0 || currentLevel >= steps - 1) {
    change = -change;
  }
  currentLevel += change;

  int whiteLevel = levelTable[currentLevel] * whiteMaxBrightness;
  float remainingLuminance = 1.0 - (float(whiteLevel) / 255.0);
  int redLevel = remainingLuminance * redValue * 255;
  int greenLevel = remainingLuminance * greenValue * 255;
  int blueLevel = remainingLuminance * blueValue * 255;
  float totalValue = redValue + greenValue + blueValue;
  redLevel = constrain(redLevel / totalValue, 0, 255);
  greenLevel = constrain(greenLevel / totalValue, 0, 255);
  blueLevel = constrain(blueLevel / totalValue, 0, 255);

  analogWrite(whitePin, whiteLevel);
  analogWrite(redPin, redLevel);
  analogWrite(greenPin, greenLevel);
  analogWrite(bluePin, blueLevel);

  Serial.print("White: ");
  Serial.print(whiteLevel);
  Serial.print(" | Red: ");
  Serial.print(redLevel);
  Serial.print(" | Green: ");
  Serial.print(greenLevel);
  Serial.print(" | Blue: ");
  Serial.println(blueLevel);

  delay(20);
}

void togglePause() {
  paused = !paused;
}

void fillLevelTable() {
  float scalingFactor = 100.0 / float(steps);
  float luminance = 0.0;

  for (int l = 0; l < steps; l++) {
    float lightness = float(l) * scalingFactor;
    if (lightness <= 8) {
      luminance = (lightness / 903.3);
    } else {
      luminance = (lightness + 16) / 116.0;
      luminance = pow(luminance, 3);
    }
    levelTable[l] = byte(luminance * 255); // Scale to 0-255
  }
}