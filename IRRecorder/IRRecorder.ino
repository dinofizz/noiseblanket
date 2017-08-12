/*

   IR Recorder: Modified version of IRrecvDemo by Ken Shirrif:
   
   https://github.com/z3t0/Arduino-IRremote/blob/master/examples/IRrecvDemo/IRrecvDemo.ino
   
   (see original copyright notice below.)

   I have simply modified the output to print the received codes in
   unsigned long format rather than hexadecimal. This makes it easier
   to use the values recorded for comparison later.
  
   ----
 
   IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
   An IR detector/demodulator must be connected to the input RECV_PIN.
   Version 0.1 July, 2009
   Copyright 2009 Ken Shirriff
   http://arcfn.com
*/

#include <IRremote.h>

int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  Serial.println("IR Recorder");  
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    unsigned long ir_result = results.value;
    Serial.println(ir_result);    
    irrecv.resume(); // Receive the next value
  }
  
  delay(100);
}
