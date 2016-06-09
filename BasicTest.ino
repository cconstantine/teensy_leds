#include <OctoWS2811.h>

const unsigned int show = 1;
struct Packet {
  uint8_t strip_index;
  uint8_t pixel_index;
  uint8_t pixel_count;
  uint8_t flags;
  uint8_t data[60];
};

const int ledsPerStrip = 4*60;

DMAMEM uint8_t displayMemory[ledsPerStrip*8*3];
uint8_t drawingMemory[ledsPerStrip*8*3];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  Serial.begin(9600); // USB is always 12 Mbit/sec

  leds.begin();
  leds.show();
}

void loop() {
  Packet packet;
  int n = RawHID.recv(&packet, 0);
  if (n > 0) {
    for(int i = 0;i < packet.pixel_count*3;i+=3) {
      int idx = i/3 + packet.pixel_index + packet.strip_index*ledsPerStrip;
      leds.setPixel(idx, packet.data[i], packet.data[i+1], packet.data[i+2]);
    }
    leds.show();
  }
}


