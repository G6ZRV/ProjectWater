/*
 * Debug flag (sets shorter timings)
 */
 
#define DEBUG false

/*
 * Humidity value breakpoints for YL-69
 */
 
#define FLOOR_NOT_IN_SOIL 1000
#define FLOOR_DRY_SOIL 400
#define FLOOR_HUMID_SOIL 370

/*
 * Pin defines
 */
 
#define PIN_CONTROL 2
#define PIN_SAMPLE A0

/*
 * Default configuration
 */
 
const int NR_OF_SAMPLES = 10;
int SECS_BETWEEN_SAMPLINGS = 60;
int PAUSE_SECS_AFTER_WATER = 60 * 60;
int SECS_TO_WATER = 15 * 60;

/*
 * Globals
 */
 
int SAMPLES[NR_OF_SAMPLES];
int SAMPLES_GATHERED = 0;

/*
 * Print functions
 */

void printConfiguration() {
  Serial.print("Started program with configuration:\n");
  Serial.print("Nr of samples to average: ");
  Serial.print(NR_OF_SAMPLES);
  Serial.print("\n");
  Serial.print("Seconds between samplings: ");
  Serial.print(SECS_BETWEEN_SAMPLINGS);
  Serial.print("\n");
  Serial.print("Pause in seconds after watering: ");
  Serial.print(PAUSE_SECS_AFTER_WATER);
  Serial.print("\n");
  Serial.print("Watering time in seconds: ");
  Serial.print(SECS_TO_WATER);
  Serial.print("\n\n\n");
}
 
void printSampleMessage(int sample) {
  Serial.print("Sampled value (avg): ");
  Serial.print(sample);
  Serial.print("\n");
}

void printWateringMsg(int i) {
  Serial.print("Status: WATERING. ");
  Serial.print(secsToMins(i));
  Serial.print(" out of ");
  Serial.print(secsToMins(SECS_TO_WATER));
  Serial.print(" minutes elapsed\n");
}

void printAfterWateringMsg(int i) {
  Serial.print("Status: WAITING AFTER WATERING. ");
  Serial.print(secsToMins(i));
  Serial.print(" out of ");
  Serial.print(secsToMins(PAUSE_SECS_AFTER_WATER));
  Serial.print(" minutes elapsed\n");
}

void printAvgMessage(int avg) {
  Serial.print("Average of last ");
  Serial.print(NR_OF_SAMPLES);
  Serial.print(" is ");
  Serial.print(avg);
  Serial.print("\n");
}

/*
 * Performs setup of the Arduino pins and debug mode
 */
 
void setup() {
  Serial.begin(9600);
  pinMode(PIN_SAMPLE, INPUT);
  pinMode(PIN_CONTROL, OUTPUT);
  if (DEBUG) {
    setupDebugValues();
  }
  printConfiguration();
}

/*
 * Change to lower debug timings (if #debug is true)
 */
 
void setupDebugValues() {
  SECS_BETWEEN_SAMPLINGS = 1;
  PAUSE_SECS_AFTER_WATER = 10;
  SECS_TO_WATER = 15;
}

/*
 * Opens/closes the water valve
 */

void openWaterValve() {
  Serial.print("\nStarted watering.\n");
  digitalWrite(PIN_CONTROL, HIGH);
}

void closeWaterValve() {
  Serial.print("\nStopped watering.\n");
  digitalWrite(PIN_CONTROL, LOW);
}

/*
 * Time conversion functions (to avoid integer overlows)
 */

unsigned long minuteInMs() {
  return 60UL * 1000UL;
}

unsigned long secInMs() {
  return 1000UL;
}

int secsToMins(int secs) {
  return (int)(secs / 60);
}

/*
 * Runs watering program
 */

void doWatering() {
  openWaterValve();
  for (int i = 0; i < SECS_TO_WATER; i++) {
    delay(secInMs());
    if (i == 0 || i % 60 == 0) {
      printWateringMsg(i);
    }   
  }
  closeWaterValve();
  doWaitAfterWatering();
}

/*
 * Runs wait cycle after watering has completed
 */

void doWaitAfterWatering() {
  for (int i = 0; i < PAUSE_SECS_AFTER_WATER; i++) {
    delay(secInMs());  
    if (i == 0 || i % 60 == 0) {
      printAfterWateringMsg(i);
    } 
  }
  resetSampling();
}

/*
 * Reset number of samples gathered after each watering
 */

void resetSampling() {
  SAMPLES_GATHERED = 0;
}

/*
 * Checks if a value is considered to be in the range of dry soil
 */

bool valueIsDry(int value) {
  return value < FLOOR_NOT_IN_SOIL && value >= FLOOR_DRY_SOIL;
}

/*
 * Calculates average of samples currently in array
 */

int getAverageOfSamples() {
  int total = 0;
  for (int i = 0; i < NR_OF_SAMPLES; i++) {
    total += SAMPLES[i];
  }
  int avg = (int)(total / NR_OF_SAMPLES);
  printAvgMessage(avg);
  return avg;
}

/*
 * Gets a soil sample and saves it to array
 */

void sampleSoil() {
  int sample = 0, sampleTotal = 0, nrOfSamples = 10;
  Serial.print("Started sampling: ");
  for (int i = 0; i < nrOfSamples; i++) {
    sample = analogRead(PIN_SAMPLE);
    sampleTotal += sample;
    Serial.print(sample);
    Serial.print(" ");
    delay(200);    
  }
  sample = (int)(sampleTotal / nrOfSamples);
  SAMPLES[SAMPLES_GATHERED % NR_OF_SAMPLES] = sample;
  SAMPLES_GATHERED++;
  printSampleMessage(sample);
}

/*
 * Main loop
 */

void loop() {
  sampleSoil();

  if (SAMPLES_GATHERED >= NR_OF_SAMPLES) {
    int avgValue = getAverageOfSamples();

    if (valueIsDry(avgValue)) {
      doWatering();
    }
  } else {
    Serial.print("Not enough samples gathered for average. Need ");
    Serial.print(NR_OF_SAMPLES - SAMPLES_GATHERED);
    Serial.print(" more\n");
  }

  delay(SECS_BETWEEN_SAMPLINGS * 1000UL);
}


