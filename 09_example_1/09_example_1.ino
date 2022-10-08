#include <CircularQueue.h>

// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.5 // EMA weight of new sample (range: 0 to 1)
                       // Setting EMA to 1 effectively disables EMA filter.

#define DIST_QUEUE_N 10

// global variables
unsigned long last_sampling_time; // unit: msec
float dist_prev = _DIST_MAX;      // Distance last-measured
float dist_ema;                   // EMA distance

CircularQueue *dist_queue;
float *dist_sort_cache;

void setup()
{
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);

  // initialize last sampling time
  last_sampling_time = 0;

  dist_sort_cache = (float *)malloc(sizeof(float) * DIST_QUEUE_N);
  dist_queue = new CircularQueue(DIST_QUEUE_N);

  // initialize
  for (int i = 0; i < DIST_QUEUE_N; i++)
  {
    dist_queue->Enqueue(200);
  }
}

void loop()
{
  float dist_raw;

  // wait until next sampling time.
  // millis() returns the number of milliseconds since the program started.
  // Will overflow after 50 days.
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if (dist_raw < _DIST_MIN)
  {
    dist_raw = dist_prev;
    digitalWrite(PIN_LED, 1); // LED OFF
  }
  else if (dist_raw > _DIST_MAX)
  {
    dist_raw = dist_prev;     // Set Higher Value
    digitalWrite(PIN_LED, 1); // LED OFF
  }
  else
  {                           // In desired Range
    digitalWrite(PIN_LED, 0); // LED ON
  }

  dist_prev = dist_raw;

  for (int i = 0; i < DIST_QUEUE_N; i++)
  {
    float dist = dist_queue->Dequeue();
    dist_sort_cache[i] = dist;
    dist_queue->Enqueue(dist);
  }

  qsort(dist_sort_cache, DIST_QUEUE_N, sizeof(float), compare);

  float dist_median = dist_sort_cache[DIST_QUEUE_N / 2 + 1];

  Serial.print("Min:");
  Serial.print(_DIST_MIN);
  Serial.print(",raw:");
  Serial.print(dist_raw);
  Serial.print(",ema:");
  Serial.print(dist_ema);
  Serial.print(",median:");
  Serial.print(dist_median);
  Serial.print(",Max:");
  Serial.print(_DIST_MAX);
  Serial.println("");

  // update last sampling time
  last_sampling_time += INTERVAL;
}

int compare(const void *a, const void *b)
{
  float fa = *(const float *)a;
  float fb = *(const float *)b;
  return (fa > fb) - (fa < fb);
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - round trip distance: 34.6m
  // - expected pulse duration: 0.1 sec, or 100,000us
  // - pulseIn(ECHO, HIGH, timeout) * 0.001 * 0.5 * SND_VEL
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346 * micro * sec * milli * meter
  //                                        ----------------------------
  //                                         micro * sec
  //        = 100 * 173 milli*meter = 17,300 mm = 17.3m
  // pulseIn() returns microseconds.
}
