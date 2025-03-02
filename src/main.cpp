#include <TFT_eSPI.h>  // Include the TFT_eSPI library

TFT_eSPI tft = TFT_eSPI(); // Create TFT object


void setup() {
  tft.begin(); // Initialize TFT
  tft.setRotation(3); // Set rotation if needed
  tft.fillScreen(TFT_BLACK); // Clear screen
}

void loop() {
  tft.setTextSize(2);           // Set text size
  tft.setTextColor(TFT_WHITE);  // Set text color
  tft.setCursor(50, 100);      // Set text position
  tft.print("Hello, world!");  // Print text on screen
  delay(2000); // Wait 2 seconds before repeating
}
