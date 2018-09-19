# RadioShack_Camera

This is an Arduino library for the RadioShack Camera Board model 2760248. It is based on [code provided by RadioShack](https://github.com/RadioShackCorp/2760248-Camera-Board).

Connect 5v to 5v, Gnd to Gnd, RX to TX and TX to RX. Note that TX and RX will need to be disconnected when you're uploading a sketch to your Arduino.

```
#include <RadioShack_Camera.h>
#include <SD.h>

File photoFile;

#define SD_SELECT_PIN 10

RadioShack_Camera camera;

void setup() {
  SD.begin(SD_SELECT_PIN);
  char filename[] = "photo.jpg";
  if (SD.exists(filename)) SD.remove(filename);
  photoFile = SD.open(filename, FILE_WRITE);
  camera.capture_photo(photoFile);
}

void loop() {}
```
