/*
  send & receive OSC - button with wait function
  by Jonas Fehr - Vertigo.dk
*/
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <Bounce.h>

#define inputPin 5

EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(10, 0, 0, 10);
//destination IP
IPAddress outIp(10, 0, 0, 1);
const unsigned int outPort = 9002;
const unsigned int inPort = 9001;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; // you can find this written on the board of some Arduino Ethernets or shields

Bounce button = Bounce(inputPin, 5);

bool isPlaying = false;


void releaseIsPlaying(OSCMessage &msg) {
  if (msg.isFloat(0)) {
    if ((boolean) msg.getFloat(0) ) {
      isPlaying = false;

      OSCMessage msg("/released");
      msg.add((float) 1.0);
      Udp.beginPacket(outIp, outPort);
      msg.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      msg.empty();
    }
  }
}

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(inPort);

  pinMode(inputPin, INPUT_PULLUP);
}

void loop() {
  // Receiving OSC
  OSCBundle bundleIN;
  int size;

  if ( (size = Udp.parsePacket()) > 0)
  {
    while (size--)
      bundleIN.fill(Udp.read());

    if (!bundleIN.hasError())
      bundleIN.dispatch("/release", releaseIsPlaying);
  }


  // Send OSC when button

  button.update();
  isPlaying = false;
  if (button.risingEdge() && !isPlaying) {
    OSCMessage msg("/play");
    msg.add((float) 1.0);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msg.empty();
    delay(20);

    isPlaying = true;

  }
}
