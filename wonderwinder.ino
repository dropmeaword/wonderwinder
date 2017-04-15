#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <Metro.h>
#include <AccelStepper.h>

#include <AccelStepper.h>
#include "linereader.h"

#include "wifisettings.h"

Metro alive = Metro(5000);
int device_id = -1;

LineReader reader;
char *linein;

const int STEPS_PER_WINDING = 200;
const int MAX_SPEED = 4000;

// Define a stepper and the pins it will use
//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
// driver, step, direction
//AccelStepper stepper(AccelStepper::DRIVER, D5, D6);
AccelStepper stepper(AccelStepper::DRIVER, D3, D4);

void init_wifi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  thisip = WiFi.localIP();
  Serial.println( thisip );

  Udp.begin(rxport);

  Serial.println("Starting UDP");
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

  device_id = thisip[3];  // 10.0.0.24 [10,0,0,3] [0] = 10, [1] = 0, [2] = 0, [3] = 24
}


void on_rotate(OSCMessage &msg, int addrOffset) {
  Serial.println(">> CMD: ROTATE");
  if( msg.isInt(0) && msg.isInt(1) ) {
    int dir = msg.getInt(0);
    int steps = msg.getInt(1);
    int rate = msg.getInt(2);

    steps = steps * dir; // apply direction sign

    Serial.print("steps -> ");
    Serial.print(steps);
    Serial.print(" speed -> ");
    Serial.println(rate);
    stepper.setAcceleration( rate );
    //stepper.computeNewSpeed();
    stepper.move(steps);
  }
}

void on_stop(OSCMessage &msg, int addrOffset) {
  Serial.println(">> CMD: STOP");
  stepper.stop();
}

void setup()
{
  Serial.begin(115200);

  init_wifi();

  stepper.setMaxSpeed(MAX_SPEED);
  //stepper.setSpeed(MAX_SPEED);
  //stepper.setAcceleration(2000);
}


void state_loop() {
  // send alive ACK message to show-control
  if(alive.check()) {
    OSCMessage out("/mirror/ack");
    out.add( device_id );
    Udp.beginPacket(dest, txport);
    out.send(Udp);
    Udp.endPacket();
    out.empty();
  }
}

void osc_message_pump() {
  OSCMessage in;
  int size;

  if( (size = Udp.parsePacket()) > 0)
  {
    Serial.println("processing OSC package");
    // parse incoming OSC message
    while(size--) {
      in.fill( Udp.read() );
    }

    if(!in.hasError()) {
      in.route("/mirror/rot", on_rotate);
      in.route("/mirror/stop", on_stop);
    }
  } // if
}

void serial_pump() {
  char *line;
  if ((line = reader.readln(Serial)) != NULL) {
    on_process_command( line );
  }
}

void loop() {
  serial_pump();
  osc_message_pump();
  state_loop();
  stepper.run();
}


void stepper_drive(int windings) {
    Serial.print("Number of windings: ");
    Serial.print(windings);
    Serial.println();

    stepper.move(windings*STEPS_PER_WINDING);
}

void on_process_command(char *cmd) {
  int windings;
  int ms;
  Serial.print("windwings: ");
  Serial.println(cmd);
  stepper_drive( 20 );
}

