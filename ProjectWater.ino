#define DEBUG true

// Humidity values
#define FLOOR_NOT_IN_SOIL 1000
#define FLOOR_DRY_SOIL 600
#define FLOOR_HUMID_SOIL 370

// Pins
#define PIN_CONTROL 2
#define PIN_SAMPLE A0

// Confguration
const int NR_OF_SAMPLES = 10;
int SECS_BETWEEN_SAMPLINGS = 60;
// Default to 1 hour
int PAUSE_SECS_AFTER_WATER = 60 * 60;
// Default to 15 min
int SECS_TO_WATER = 15 * 60;

// Globals
int SAMPLES[NR_OF_SAMPLES];
int LAST_SAMPLE = 0;
int SAMPLE_INDEX = 0;
int SAMPLES_GATHERED = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SAMPLE, INPUT);
  pinMode(PIN_CONTROL, OUTPUT);
  if (DEBUG) {
    setupDebugValues();
  }
}

void setupDebugValues() {
  SECS_BETWEEN_SAMPLINGS = 5;
  PAUSE_SECS_AFTER_WATER = 10;
  SECS_TO_WATER = 10;
}

void openWaterFlow() {
  digitalWrite(PIN_CONTROL, HIGH);
}

void closeWaterFlow() {
  digitalWrite(PIN_CONTROL, LOW);
}

unsigned long minuteInMs() {
  return 60UL * 1000UL;
}

void doWatering() {
  openWaterFlow();
  for (int i = 0; i < (SECS_TO_WATER / 60); i += 60) {
    delay(minuteInMs());
    printWateringMsg(i);
  }
  closeWaterFlow();
  doWaitAfterWatering();
}

void printWateringMsg(int i) {
  Serial.print("Status: WATERING. ");
  Serial.print(i/60);
  Serial.print(" out of ");
  Serial.print(SECS_TO_WATER/60);
  Serial.print(" minutes elapsed");
}

void doWaitAfterWatering() {
  for (int i = 0; i < (PAUSE_SECS_AFTER_WATER / 60); i += 60) {
    delay(minuteInMs());
    printAfterWateringMsg(i);
  }
  resetSampling();
}

void printAfterWateringMsg(int i) {
  Serial.print("Status: WAITING AFTER WATERING. ");
  Serial.print(i/60);
  Serial.print(" out of ");
  Serial.print(PAUSE_SECS_AFTER_WATER/60);
  Serial.print(" minutes elapsed");
}

void resetSampling() {
  LAST_SAMPLE = 0;
  SAMPLE_INDEX = 0;
  SAMPLES_GATHERED = 0;
}

bool valueIsDry(int value) {
  return value < FLOOR_NOT_IN_SOIL && value >= FLOOR_DRY_SOIL;
}

int getAverageOfSamples() {
  int total = 0;
  for (int i = 0; i < NR_OF_SAMPLES; i++) {
    total += SAMPLES[i];
  }
  int avg = (int)(total / NR_OF_SAMPLES);
  Serial.print("Average of last ");
  Serial.print(NR_OF_SAMPLES);
  Serial.print(" is ");
  Serial.print(avg);
  return avg;
}

void sampleSoil() {
  int sample = analogRead(PIN_SAMPLE);
  SAMPLES[SAMPLE_INDEX % NR_OF_SAMPLES] = sample;
  LAST_SAMPLE = sample;
  SAMPLES_GATHERED++;
  Serial.print("Sampled value: " + sample);
}

void loop() {
  sampleSoil();

  if (SAMPLES_GATHERED >= NR_OF_SAMPLES) {
    int avgValue = getAverageOfSamples();

    if (valueIsDry(avgValue)) {
      doWatering();
    }
  }

  delay(SECS_BETWEEN_SAMPLINGS * 1000UL);
}

