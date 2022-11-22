#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10

// configurable parameters
#define _DUTY_MIN 530  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1480 // servo neutral position (90 degree)
#define _DUTY_MAX 2430 // servo full counter-clockwise position (180 degree)

#define _POS_START (_DUTY_MIN) // servo start position
#define _POS_END (_DUTY_MAX)   // servo end position

#define _SERVO_SPEED 500 // servo speed limit (unit: degree/second)
#define INTERVAL 20      // servo update interval (unit: msec)

// global variables
unsigned long last_sampling_time; // unit: msec

Servo myservo;

int duty_change_per_interval; // maximum duty difference per interval
int duty_target;              // Target duty time
int duty_curr;                // Current duty time
int servo_prev;

int toggle_interval, toggle_interval_cnt;

void setup()
{
  // initialize GPIO pins
  myservo.attach(PIN_SERVO);

  duty_target = duty_curr = _POS_START;
  myservo.writeMicroseconds(duty_curr);

  // initialize serial port
  Serial.begin(57600);

  // convert angular speed into duty change per interval.
  // Next two lines are WRONG. you have to modify.
  duty_change_per_interval =
      (_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED / 180.f) * (INTERVAL / 1000.f);
  
  // (2^31 - 1) / (duty max - duty min) * 9000 <  servospeed

  Serial.print("duty_change_per_interval:");
  Serial.println(duty_change_per_interval);

  // initialize variables for servo update.
  toggle_interval = (180.f / _SERVO_SPEED) * 1000.f / INTERVAL;
  toggle_interval_cnt = toggle_interval;

  // initialize last sampling time
  last_sampling_time = 0;
}

void loop()
{
  // wait until next sampling time.
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  // adjust duty_curr toward duty_target by duty_change_per_interval
  if (duty_target > duty_curr)
  {
    duty_curr += duty_change_per_interval;
    if (duty_curr > duty_target)
      duty_curr = duty_target;
  }
  else
  {
    duty_curr -= duty_change_per_interval;
    if (duty_curr < duty_target)
      duty_curr = duty_target;
  }

  // update servo position
  myservo.writeMicroseconds(duty_curr);

  // output the read value to the serial port
  Serial.print("Min:1000");
  Serial.print(",duty_target:");
  Serial.print(duty_target);
  Serial.print(",duty_curr:");
  Serial.print(duty_curr);
  Serial.print(",Speed:");
  Serial.print((myservo.readMicroseconds() - servo_prev) / (INTERVAL / 1000.f));
  Serial.println(",Max:2000");

  // toggle duty_target between _DUTY_MIN and _DUTY_MAX.
  if (toggle_interval_cnt >= toggle_interval)
  {
    toggle_interval_cnt = 0;
    if (duty_target == _POS_START)
      duty_target = _POS_END;
    else
      duty_target = _POS_START;
  }
  else
  {
    toggle_interval_cnt++;
  }

  // update last sampling time
  last_sampling_time += INTERVAL;

  servo_prev = myservo.readMicroseconds();
}
