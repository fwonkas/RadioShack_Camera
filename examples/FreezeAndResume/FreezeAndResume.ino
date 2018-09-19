#include <RadioShack_Camera.h>

RadioShack_Camera camera;

void setup() {}

void loop() {
  delay(3000);
  camera.freeze();

  delay(3000);
  camera.resume();
}