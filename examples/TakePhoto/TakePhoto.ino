#include <RadioShack_Camera.h>
#include <SD.h>

File photoFile;

#define SD_SELECT_PIN 10

RadioShack_Camera camera(&Serial);

void setup() {
  SD.begin(SD_SELECT_PIN);
  char filename[] = "photo.jpg";
  if (SD.exists(filename)) SD.remove(filename);
  photoFile = SD.open(filename, FILE_WRITE);
  camera.capture_photo(photoFile);
}

void loop() {}