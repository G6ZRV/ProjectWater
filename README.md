# ProjectWater
An automated watering system for outdoor plants using an Arduino Uno and some electronics.

## /AutomaticWatering
Arduino sketch that performs automatic watering (sending digital output) when moisture sensor registers below a specific threshold. Sampling can be customized, but thresholds comparison is made against average of several samples. Each sample is in turn an average of several quick samples.

## /SoilZoneAnalysis
Arduino sketch that can be used to analyse the YL-69 measurements for different zones of an area that should be used for planting stuffz. The sketch can be customized to use a number of different zones, how long to sample each zone and a sampling resolution.

## Components
* Arduino Uno
* YL-69 Moisture sensor
* YL-38 Converter circuit
* AC/DC Adapter 220V -> 12V
* 12V Relay with 5V control
* K-Rui dishwasher water valve

## Moisture measurements made with the SoilZoneAnalysis sketch
Coming soon...

## Todo

* Get suitable water hose diameter and securely attach to water outlet
* Change breadboard to more compact circuit layout
* Waterproof housing for electronics
