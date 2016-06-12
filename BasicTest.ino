#include <OctoWS2811.h>

const unsigned int show = 1;
struct Packet {
  uint8_t strip_index;
  uint8_t pixel_index;
  uint8_t pixel_count;
  uint8_t flags;
  uint8_t data[60];
};

const int ledsPerStrip = 4 * 60;

DMAMEM uint8_t displayMemory[ledsPerStrip * 8 * 3];
uint8_t drawingMemory[ledsPerStrip * 8 * 3];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  Serial.begin(9600); // USB is always 12 Mbit/sec
  Serial.setTimeout(0);
  leds.begin();

  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, 0x0f000F);
  }
  leds.show();
}

void loop() {
  static unsigned long bytes = 0;
  static unsigned long lastMillis = millis();
  if (lastMillis + 1000 < millis()) {
    lastMillis += 1000;

    Serial.print("Transfered: ");Serial.println(bytes);
    bytes = 0;
  }
  Packet packet;
  if (Serial.available() < sizeof(packet)) {
    return;
  }
  int n = Serial.readBytes((char*)&packet, sizeof(packet));// RawHID.recv(&packet, 0);
  if (n > 0 ) {
    bytes += n;

    for (int i = 0; i < packet.pixel_count; i++) {
      int idx = i + packet.pixel_index + packet.strip_index * ledsPerStrip;
      leds.setPixel(idx, packet.data[i * 3], packet.data[i * 3 + 1], packet.data[i * 3 + 2]);
    }

    if (packet.flags & 0x01 && !leds.busy()) {
      leds.show();
    }
  }
}


