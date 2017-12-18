#include <Servo.h>
Servo boop;
int x;
void setup() {
  boop.attach(7);
  boop.write(90);

}

void loop() {
  
boop.write(280);
delay(1000);
boop.write(250);
delay(1000);

}





//140, 190
