#include <Servo.h>
Servo boop;
int x;
void setup() {
  pinMode(5, INPUT);
  boop.attach(7);
}

void loop() {
  x = digitalRead(5);
  if (x == HIGH)
  {
    boop.write(60);
    delay(100);
    boop.write(20);
    delay(100);
  }
  else
  {
    boop.write(20);
  }
}
