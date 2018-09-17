#include <RadioShack_Camera.h>

#define SD_SELECT_PIN 10

RadioShack_Camera camera(SD_SELECT_PIN);

void setup() {
  camera.capture_photo("photo.jpg");
}

void loop() {}
