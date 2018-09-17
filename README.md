# RadioShack_Camera

This is an Arduino library for the RadioShack Camera Board model 2760248. It is based [code provided by RadioShack](https://github.com/RadioShackCorp/2760248-Camera-Board).

Connect 5v to 5v, Gnd to Gnd, RX to TX and TX to RX. Note that TX and RX will need to be disconnected when you're uploading a sketch to your Arduino.

Currently this library only allows for one thing: capturing an image to an SD card. Initialize RadioShack_Camera with the SD Select pin on your SD shield. Then call capture_photo with the file name for the image.

```
#include <RadioShack_Camera.h>

#define SD_SELECT_PIN 10

RadioShack_Camera camera(SD_SELECT_PIN);

void setup() {
  camera.capture_photo("photo.jpg");
}

void loop() {}
```