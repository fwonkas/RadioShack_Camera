#include <RadioShack_Camera.h>

RadioShack_Camera camera(&Serial);

void setup() {}

void loop() {
  delay(3000);
  camera.turnTvOutOff();

  delay(3000);
  camera.turnTvOutOn();
}