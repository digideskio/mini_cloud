
#include "SPI.h"
#include "WS2801.h"

/**
 * Streams of rain drops from a portable cloud.
 */

boolean debug_mode = false;

// System configs.
int pins[] = {
  2, // data  #1
  3, // clock #1
  4, // data  #2
  5  // clock #2
  6, // data  #3
  7, // clock #3
  8, // data  #4
  9  // clock #4
};
const unsigned int num_cols = 4;
const unsigned int num_rows = 7;

// Light control object.
WS2801 strip1 = WS2801(num_pins_per_strip, pins[0], pins[1]);
WS2801 strip2 = WS2801(num_pins_per_strip, pins[2], pins[3]);
WS2801 strip3 = WS2801(num_pins_per_strip, pins[4], pins[5]);
WS2801 strip4 = WS2801(num_pins_per_strip, pins[6], pins[7]);

// Rain holder (4x7).
int rain_matrix[num_cols][num_rows] = {0};
unsigned int chance_of_rain = 2; // Starting value.
unsigned int chance_of_rain_min = 2;
static unsigned int wait = 25;

// Colors.
uint32_t bg_color = Color(0, 0, 0);
uint32_t rain_color = Color(79, 104, 239);

// REQUIRED...

void setup() {
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  randomSeed(analogRead(0));
  
  // Debug.
  if(debug_mode) {
    Serial.begin(115200);
    Serial.println("Ready to send data.");
  }
}

void loop() {
  make_it_rain();
  delay(wait);
}


// WORK HORSES...

// Master function.
void make_it_rain() {
  int r, c;

  make_drops();

  // Display drops.
  for (c=0; c<num_cols; c++) {
    for (r=0; r<num_rows; r++) {
      if (rain_matrix[c][r] > 0) {
        draw_drop(c, r, true);
        move_drop(c, r);
      }
      else {
        draw_drop(c, r, false);
      }
    } 
  }

  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
}

// Generate new rain drops.
void make_drops() {
  
  // Change the weather?
  switch (random(0,3)) {
    case 1:
      chance_of_rain += 1;
      break;
      
    case 2:
      chance_of_rain -= 1;
      break;
      
    case 3:
    default:
      break;
  }
  
  // Protect boundaries.
  if (chance_of_rain <= 1) {
   chance_of_rain = 2;
  }
  else if (chance_of_rain > chance_of_rain_min) {
    chance_of_rain = chance_of_rain_min;
  }

  // Add a new drop?
  if( random(0, chance_of_rain) == 1) {
    // Pick a column
    int c = random(0, num_cols);
     // Prevent neighbors.
     if (rain_matrix[c][num_rows-2] == 0) {
       rain_matrix[c][num_rows-1] = 1;
     }
  }

}

// Shift known drops to new position.
void move_drop(int c, int r) {  
  int s = rain_matrix[c][r];
  // Out with the old.
  rain_matrix[c][r] = 0;
  // Can't move it on the ground.
  if (r > 0) {
    rain_matrix[c][r-1] = s;
  }
}

// Translate drop positions into data output.
void draw_drop(int c, int r, boolean rain) {
  // Draw or delete.
  switch (c) {
    case 1:
      if (rain) {
        strip1.setPixelColor(r, rain_color);
      }
      else {
        strip1.setPixelColor(r, bg_color);
      }
      break;
    case 2:
      if (rain) {
        strip2.setPixelColor(r, rain_color);
      }
      else {
        strip2.setPixelColor(r, bg_color);
      }
      break;
    case 3:
      if (rain) {
        strip3.setPixelColor(r, rain_color);
      }
      else {
        strip3.setPixelColor(r, bg_color);
      }
      break;
    case 4:
      if (rain) {
        strip4.setPixelColor(r, rain_color);
      }
      else {
        strip4.setPixelColor(r, bg_color);
      }
      break;
  }
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

