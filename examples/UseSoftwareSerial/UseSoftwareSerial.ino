#include <RadioShack_Camera.h>
#include <SoftwareSerial.h>

SoftwareSerial serial(2, 3);

RadioShack_Camera camera(&serial);

void setup() {
    Serial.begin(115200);
    Serial.println(camera.getVersion());
}

void loop() {}