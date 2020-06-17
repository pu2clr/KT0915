#include <KT0915.h>
KT0915 r;
void setup()
{
    for (int i = 5; i < 9; i++)
        pinMode(i, INPUT_PULLUP);
    r.setup(-1, OSCILLATOR_32KHZ, REF_CLOCK_DISABLE);
    r.setFM(64000, 108000, 106500, 100); // Sets FM, from 64MHz to 108MHz; default 106.5MHz; step 100Khz
    r.setVolume(20);                     // Adjusts the volume to level 20
}
void loop()
{
    if (digitalRead(5) == LOW)
        r.setAM(520, 1800, 810, 10); // Switch to AM mode, from 520 to 1800; step 10Khz
    if (digitalRead(6) == LOW)
        r.setFM(64000, 108000, 106500, 100);
    if (digitalRead(7) == LOW)
        r.frequencyUp();
    if (digitalRead(8) == LOW)
        r.frequencyDown();
    delay(200);
}