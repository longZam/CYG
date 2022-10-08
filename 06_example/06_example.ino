#include <Arduino.h>

unsigned long _period, _duty, accumulator;
bool inc = true;

void setup()
{
    set_period(10000);
    set_duty(0);
    pinMode(PIN7, OUTPUT);
    accumulator = 0;

    Serial.begin(115200);
}

void loop()
{
    tick();
    accumulator += _period;
    set_duty(_duty + (accumulator / 5000) * (inc ? 1 : -1));
    accumulator %= 5000;
}

void tick()
{
    int delay = _period * _duty / 100;
    digitalWrite(PIN7, HIGH);
    delayMicroseconds(delay);

    digitalWrite(PIN7, LOW);
    delayMicroseconds(_period - delay);
}

void set_period(int period)
{
    _period = period;
}

void set_duty(int duty)
{
    _duty = constrain(duty, 0, 100);

    // 100 초과 혹은 0 미만
    if (duty > 100 || duty < 0)
    {
        inc = !inc;
    }
}