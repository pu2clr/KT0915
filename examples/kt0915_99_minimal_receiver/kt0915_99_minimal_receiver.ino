#include <KT0915.h>
KT0915 r;
void setup() {
    for ( int i = 5; i < 9; i++ ) pinMode(i,INPUT_PULLUP);
    r.setup(-1,OSCILLATOR_32KHZ,REF_CLOCK_DISABLE); // Same r.setup(-1);
    r.setFM(84000,108000,106500,100); // From 84 to 108 MHz; 100KHz step.
    r.setVolume(25); 
}
void loop () {
    if (digitalRead(5) == LOW) r.setAM(520,1710,810,10); // From 520 to 1710KHz; defeult 810KHz; 10kHz step.
    if (digitalRead(6) == LOW) r.setFM(84000, 108000, 106500, 100); 
    if (digitalRead(7) == LOW) r.frequencyUp(); 
    if (digitalRead(8) == LOW) r.frequencyDown();
    delay(150);
} // Finish