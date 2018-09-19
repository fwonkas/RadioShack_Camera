#include <RadioShack_Camera.h>

RadioShack_Camera camera;

void setup() {}

void loop() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("tv out off");
  camera.turn_tv_out_off();

  delay(3000);
  Serial.println("tv out on");
  camera.turn_tv_out_on();
}