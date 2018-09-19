#include <RadioShack_Camera.h>

RadioShack_Camera camera;

void setup() {}

void loop() {
  delay(3000);
  camera.turn_tv_out_off();

  delay(3000);
  camera.turn_tv_out_on();
}