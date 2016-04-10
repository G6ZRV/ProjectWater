#define PIN_SAMPLE A0

const short ZONES_TO_ANALYSE = 3;
const short ANALYSE_FOR_MINS = 1;
const short SAMPLES_PER_MIN = 60;
const short TOTAL_SAMPLES_PER_ZONE = ANALYSE_FOR_MINS * SAMPLES_PER_MIN;

short SAMPLES[ZONES_TO_ANALYSE][TOTAL_SAMPLES_PER_ZONE];

void printConfiguration() {
  Serial.print("Running analysis with the following settings:\n");
  Serial.print("Zones to analyse:\t");
  Serial.print(ZONES_TO_ANALYSE);
  Serial.print("\nMinutes to analyse per zone:\t");
  Serial.print(ANALYSE_FOR_MINS);
  Serial.print("\nSamples/minute:\t");
  Serial.print(SAMPLES_PER_MIN);
  Serial.print("\nTotal nr of samples per zone:\t");
  Serial.print(TOTAL_SAMPLES_PER_ZONE);
  Serial.print("\n\n");  
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SAMPLE, INPUT);
  printConfiguration();
}

short getAvg(short zone) {
  long total = 0;
  for (int i = 0; i < TOTAL_SAMPLES_PER_ZONE; i++) {
    total += SAMPLES[zone][i];    
  }
  return (int)(total/TOTAL_SAMPLES_PER_ZONE);
}

short getMedian(short zone) {
  return SAMPLES[zone][(int)(TOTAL_SAMPLES_PER_ZONE / 2)];
}

short getMax(short zone) {
  short maxValue = -1;
  for (int i = 0; i < TOTAL_SAMPLES_PER_ZONE; i++) {
    if (SAMPLES[zone][i] > maxValue) {
      maxValue = SAMPLES[zone][i];    
    }
  }
  return maxValue;  
}

short getMin(short zone) {
  short minValue = 5000;
  for (int i = 0; i < TOTAL_SAMPLES_PER_ZONE; i++) {
    if (SAMPLES[zone][i] < minValue) {
      minValue = SAMPLES[zone][i];    
    }
  }
  return minValue;  
}

void prepareForZone(int zone) {
  Serial.print("\n\nMove sensor to zone ");
  Serial.print(zone);
  Serial.print("\nAnalysis starts in ");
  for (short i = 10; i > 0; i--) {
    Serial.print(i);
    Serial.print(".. ");
    delay(1000);
  }
}

void runAnalysis(short zone) {
  Serial.print("\nRunning analysis for zone ");
  Serial.print(zone);
  Serial.print(" ...\n");
  unsigned short sampleDelay = (unsigned short)(60000 / SAMPLES_PER_MIN);
  for (int i = 0; i < TOTAL_SAMPLES_PER_ZONE; i++) {
    SAMPLES[zone][i] = analogRead(PIN_SAMPLE);
    delay(sampleDelay);
    Serial.print("*");
  }
}

void printGraph() {
  Serial.print("\n\nGraph:\n");
  short blockValue = 20, zoneAvg = 0;
  for (short zone = 0; zone < ZONES_TO_ANALYSE; zone++) {
    zoneAvg = getAvg(zone);
    Serial.print("\nZone ");
    Serial.print(zone);
    Serial.print("\tAvg:");
    Serial.print(zoneAvg);
    Serial.print(" ->\t");
    for (int u = 0; u < (short)(zoneAvg / blockValue); u++) {
      Serial.print("=");    
    }
  }    
}

void printStats(short zone) {
  Serial.print("\n\n");
  Serial.print("Results for zone ");
  Serial.print(zone);
  Serial.print(":");
  Serial.print("\nAverage value:\t");
  Serial.print(getAvg(zone));
  Serial.print("\nMedian value:\t");
  Serial.print(getMedian(zone));
  Serial.print("\nMax value:\t");
  Serial.print(getMax(zone));
  Serial.print("\nMin value:\t");
  Serial.print(getMin(zone));  
}

void waitBetweenAnalysis() {
  Serial.print("\nWaiting 60 seconds before next analysis...");
  delay(60000UL);
}

void loop() {
  for (short zone = 0; zone < ZONES_TO_ANALYSE; zone++) {
    prepareForZone(zone);
    runAnalysis(zone);      
  }
  for (short zone = 0; zone < ZONES_TO_ANALYSE; zone++) {
    printStats(zone);    
  }
  printGraph();
  waitBetweenAnalysis();
}
