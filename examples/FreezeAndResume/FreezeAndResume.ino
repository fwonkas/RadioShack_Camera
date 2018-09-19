#include <RadioShack_Camera.h>

RadioShack_Camera camera;

void setup() {}

void loop() {
  delay(3000);
  Serial.println("freeze");
  camera.freeze();

  delay(3000);
  Serial.println("resume");
  camera.resume();
}