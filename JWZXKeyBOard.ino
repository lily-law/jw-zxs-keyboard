#include "Keyboard.h"

#define NUM_ROWS 5
#define NUM_COLS 10

bool test_output = false;
int layout_num = 0;
// JWW Custom ZXSpectrum Layout
const char layout[1][5][10] = {
  {// Layer 0 - Standard layout
    {KEY_DOWN_ARROW, KEY_UP_ARROW, /*Empty*/ NULL, KEY_RIGHT_ARROW, KEY_LEFT_ARROW,/*ESC*/ 177, /*FR1*/ 178, /*FR2*/ 255, /*Fn*/ 134, /*Empty*/ NULL },
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', KEY_RETURN},
    {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', KEY_RIGHT_CTRL, ' '}
  }
};

int p; // current column input pin state
bool state_layer[NUM_ROWS][NUM_COLS] = {
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH},
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}};
bool reprogramed = false;

const int pin_row_config[5] {
  7,
  8,
  9,
  10,
  16
};
const int pin_column_config[10] {
  1,
  0,
  4,
  5,
  6,
  20,
  19,
  18,
  15,
  14
};

void do_reprogram_mode() {
  if (!reprogramed) {
    reprogramed = true;
    Keyboard.releaseAll();
  }
}

void matrix_init() {
  // configure outputs
  for (int o = 0; o < 5; o++) {
    pinMode(pin_row_config[o], OUTPUT);
  }
  
  // configure inputs
  for (int i = 0; i < 10; i++) {
    pinMode(pin_column_config[i], INPUT_PULLUP);
  }
}

void do_matrix_scan() {
  for (int r = 0; r < NUM_ROWS; r++) {
    digitalWrite(pin_row_config[r], LOW);
    for (int c = 0; c < NUM_COLS; c++) {
      p = digitalRead(pin_column_config[c]);  
      if (p != state_layer[r][c]) { // Is the value in the state array different from the value read from the key?  
        uint16_t key = layout[layout_num][r][c]; 
        if (key != NULL) {  
          if (p == LOW) {  // Key Down
              Keyboard.press(key);
              if (test_output) {
                Serial.print("row: ");
                Serial.print(r);
                Serial.print("- column: ");
                Serial.print(c);
                Serial.print("- keyVal: ");
                Serial.print(key);
                Serial.println(); 
              }
           // }
          } else { // Key Up
              Keyboard.release(key);
          }
        }
        else if (test_output && p == LOW) {
          Serial.print("row: ");
          Serial.print(r);
          Serial.print("- column: ");
          Serial.print(c);
          Serial.print(" Key pressed has NULL value!"); 
          Serial.println();
        }
        state_layer[r][c] = p;
      }
    }
    digitalWrite(pin_row_config[r], HIGH);
  }
  delay(50);
}

void setup() {
  if (test_output) {
    Serial.begin(9600);
  }
  // Setup output and input pins
  matrix_init();
  // configure programing switch
  pinMode(21, INPUT_PULLUP);
  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
  while (digitalRead(21) == LOW) {
    do_reprogram_mode();
  }
  reprogramed = false;
  do_matrix_scan();
}
