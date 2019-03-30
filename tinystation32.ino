/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 * Copyright (c) 2016 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#define USE_CO2       1
#define USE_PMS       1

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "SH1106Wire.h"`

// Include the UI lib
#include "OLEDDisplayUi.h"

// Include custom images
#include "images.h"

// Initialize the OLED display using Wire library
//SSD1306 display(0x3c, 5, 4);
SH1106Wire display(0x3c, 5, 4);

OLEDDisplayUi ui(&display);

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y

  display->drawXbm(x + 34, y + 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 10 + y, "Arial 10");

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 20 + y, "Arial 16");

  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, 34 + y, "Arial 24");
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 11 + y, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 33 + y, "Right aligned (128,33)");
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demo for drawStringMaxWidth:
  // with the third parameter you can define the width after which words will be wrapped.
  // Currently only spaces and "-" are allowed for wrapping
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 10 + y, 128, "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore.");
}

void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}


#if USE_CO2

#include "MHZ19.h"

#define RX0 3
#define TX0 1
MHZ19 mhz19;

// MH-Z19B CO2 Sensor
void drawCO2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  static int initialized = 0;
  static unsigned long warmup_millis = 0;
  static unsigned long last_millis = 0;
  static char s[2][64];
  
  if (!initialized) {
    Serial.begin(9600);
    mhz19.begin(Serial);
    mhz19.autoCalibration();
    warmup_millis = millis() + 30 * 1000;
    initialized = 1;
  }

  if (millis() < warmup_millis) {
    last_millis = millis();
    int rm = warmup_millis - last_millis;
    sprintf(s[0], "CO2 Starting...");
    sprintf(s[1], "remains %d secs", rm / 1000);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_16);
    display->drawString(64 + x, 14 + y, s[0]);
    display->setFont(ArialMT_Plain_10);
    display->drawString(64 + x, 36 + y, s[1]);
    return;
  }

  if (millis() >= last_millis + 2000) {
    int co2 = mhz19.getCO2();
    int8_t temp = mhz19.getTemperature();
    sprintf(s[0], "CO2: %d", co2);
    sprintf(s[1], "TEMP: %d C", temp);
    last_millis = millis();
  }

  display->setFont(ArialMT_Plain_16);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 14 + y, s[0]);
  display->drawString(64 + x, 36 + y, s[1]);
}
#endif


#if USE_PMS

#include "PMS.h"

#define PMS_RESET 14
#define PMS_SET   12
#define TX1 13
#define RX1 15
HardwareSerial pmsSerial1(1);
PMS pms(pmsSerial1);

// PLANTOWER PM2.5 PMS7003 / G7 PMS Sensor
void drawPMS(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  static int initialized = 0;
  static unsigned long warmup_millis = 0;
  static unsigned long last_millis = 0;
  static char s[3][100];
  PMS::DATA data;

  if (!initialized) {
    pinMode(PMS_RESET, OUTPUT);
    pinMode(PMS_SET, OUTPUT);
    digitalWrite(PMS_RESET, HIGH);
    digitalWrite(PMS_SET, HIGH);
    pmsSerial1.begin(9600, SERIAL_8N1, RX1, TX1);
    pms.passiveMode();
    pms.wakeUp();
    warmup_millis = millis() + 30 * 1000;
    initialized = 1;
  }

  if (millis() < warmup_millis) {
    last_millis = millis();
    int rm = warmup_millis - last_millis;
    sprintf(s[0], "PMS Starting...");
    sprintf(s[1], "remains %d secs", rm / 1000);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_16);
    display->drawString(64 + x, 14 + y, s[0]);
    display->setFont(ArialMT_Plain_10);
    display->drawString(64 + x, 36 + y, s[1]);
    return;
  }
  
  if (millis() >= last_millis + 1000) {
    pms.requestRead();
    if (pms.readUntil(data)) {
      // Unit: ug/m3
      sprintf(s[0], "PM1.0: %d", data.PM_AE_UG_1_0);
      sprintf(s[1], "PM2.5: %d", data.PM_AE_UG_2_5);
      sprintf(s[2], "PM10: %d", data.PM_AE_UG_10_0);
    }
    last_millis = millis();
  }
  display->setFont(ArialMT_Plain_16);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 11 + y, s[0]);
  display->drawString(64 + x, 25 + y, s[1]);
  display->drawString(64 + x, 39 + y, s[2]);
 
}

#endif


// This array keeps function pointers to all frames
// frames are the single views that slide in
//FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4, drawFrame5 };
FrameCallback frames[] = { drawFrame1, drawCO2, drawPMS };
int frameCount = 3;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 0;


void setup() {

#if PMS
  // init as disabled status for startup power short
  pinMode(PMS_RESET, OUTPUT);
  pinMode(PMS_SET, OUTPUT);
  digitalWrite(PMS_RESET, LOW);
  digitalWrite(PMS_SET, LOW);
#endif
    
	// The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);

	// Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();

}

void loop() {
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }
}

