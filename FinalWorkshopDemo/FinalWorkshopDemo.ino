// Andrew Combs 2016-12-02
//
// LED scanner with variable pattern / speed and
// RGB LED with variable color / brightness
// 
// Button cycles scanner pattern and RGB LED color
// Knob adjusts scanner speed and RGB LED brightness

const int BUTTON = 2;
const int POT    = A0;
const int LED1   = 8;
const int LED2   = 9;
const int LED3   = 10;
const int LED4   = 11;
const int LED5   = 12;
const int LED_R  = 3;
const int LED_G  = 5;
const int LED_B  = 6;

const byte cylonArraySize = 5;
const byte cylonArray[] = { 0b00100,
                            0b01010,
                            0b10001,
                            0b01010,
                            0b00100 };
const byte trekArraySize = 10;
const byte trekArray[]  = { 0b10000,
                            0b01000,
                            0b00100,
                            0b00010,
                            0b00001,
                            0b00001,
                            0b00010,
                            0b00100,
                            0b01000,
                            0b10000 };
const byte rotateArraySize = 5;
const byte rotateArray[] ={ 0b10000,
                            0b01000,
                            0b00100,
                            0b00010,
                            0b00001 };

// The number of steps
const int numberOfSteps = 10;

// The minimum amount of time between chase steps
const int minDelay = 100;
// The maximum amount of time between chase steps
const int maxDelay = 500;

// The current step in the led chase pattern
int currentStep = 0;

// The available chase types
enum chaseTypes { cylon, trek, rotate };
// The current chase type
chaseTypes currentChaseType = cylon;

// Has the button been pressed since the last time we checked?
volatile bool wasButtonPressed = false;

void setup() {
  // Pot doesn't require setup
  
  pinMode(BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonInterruptHandler, CHANGE);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
}

void loop() {
  int potValue = analogRead(POT);

  int delayTime = updateDelayTime(potValue);
  
  updateLEDOutput();
  updateRGB(potValue);

  if(wasButtonPressed)
  {
    updateChaseType();
    wasButtonPressed = false;
  }

  // Go to the next step, but wrap back to zero
  //   if we pass the last step
  currentStep = (currentStep + 1) % numberOfSteps;
  
  delay(delayTime);
}

// ---------- Functions ----------

// Convert a potentiometer value to a delay time
int updateDelayTime(int potValue)
{
  // Analog read returns a value from 0 to 1023
  // Need to map that to a value from minDelay to maxDelay
  
  float multiplier = (float)(maxDelay - minDelay) / 1023.0;
  float retVal = (potValue * multiplier) + minDelay;
  return (int)retVal;
}

// Update the LED display based on the current
//   chase type and step
void updateLEDOutput()
{
  switch(currentChaseType)
  {
    case cylon:
      updateLEDDisplay(cylonArray[currentStep % cylonArraySize]);
      break;
    case trek:
      updateLEDDisplay(trekArray[currentStep % trekArraySize]);
      break;
    case rotate:
      updateLEDDisplay(rotateArray[currentStep % rotateArraySize]);
      break;
    default: // default to cylon
      updateLEDDisplay(cylonArray[currentStep % cylonArraySize]);
      break;
  }
}

// Update the LED display based on a
//   bitmap from a chase array
void updateLEDDisplay(byte bitmap)
{
  digitalWrite(LED1, (bitmap & 0b00001));
  digitalWrite(LED2, (bitmap & 0b00010));
  digitalWrite(LED3, (bitmap & 0b00100));
  digitalWrite(LED4, (bitmap & 0b01000));
  digitalWrite(LED5, (bitmap & 0b10000));
}

// Update the RGB LED
// The color is based on the current chase pattern
// The brightness is based on the potentiometer
void updateRGB(int potValue)
{
  // Map potValue to analogWrite value
  float multiplier = 255.0 / 1023.0;
  int brightness = (int)(potValue * multiplier);
  
  switch(currentChaseType)
  {
    case cylon: // cylon is red
      analogWrite(LED_R, brightness);
      analogWrite(LED_G, 0);
      analogWrite(LED_B, 0);
      break;
    case trek: // trek is blue
      analogWrite(LED_R, 0);
      analogWrite(LED_G, 0);
      analogWrite(LED_B, brightness);
      break;
    case rotate: // rotate is purple
      analogWrite(LED_R, brightness);
      analogWrite(LED_G, 0);
      analogWrite(LED_B, brightness);
      break;
    default: // default to off
      analogWrite(LED_R, 0);
      analogWrite(LED_G, 0);
      analogWrite(LED_B, 0);
      break;
  }
}

// Move from the current chase type to the next one
void updateChaseType()
{
  switch(currentChaseType)
  {
    case cylon:
      currentChaseType = trek;
      break;
    case trek:
      currentChaseType = rotate;
      break;
    case rotate:
      currentChaseType = cylon;
      break;
    default:
      // If the current chase type isn't one we know about,
      //   go to cylon
      currentChaseType = cylon;
      break;
  }
}

// ---------- Interrupt Service Routines ----------
void buttonInterruptHandler()
{
  wasButtonPressed = true;
  Serial.println("pressed!");
}

