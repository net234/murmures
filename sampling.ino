#include "arduinoFFT.h"

/*
These values can be changed in order to evaluate the functions
*/
#define CHANNEL A0
const uint16_t samples = 32;           //This value MUST ALWAYS be a power of 2
const float samplingFrequency = 4000;  //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
unsigned long microseconds;

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
float vReal[samples];
float vImag[samples];

/* Create FFT object with weighing factor storage */
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, samples, samplingFrequency, true);

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03


void sample() {

  /*SAMPLING*/

  microseconds = micros();
  for (int i = 0; i < samples; i++) {
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us) {
      //empty loop
    }
    microseconds += sampling_period_us;
  }
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); /* Weigh data */
  FFT.compute(FFTDirection::Forward);                       /* Compute FFT */
  FFT.complexToMagnitude();                                 /* Compute magnitudes */
  //digitalWrite(2,HIGH);
  tranfersled(vReal, samples);
}



void tranfersled(float *vData, uint16_t bufferSize) {

  // Décalage des colonnes du tableau vers la droite
  for (int i = tabLedLong - 1; i > 0; i--) {
    for (int j = 0; j < tabLedLarg; j++) {
      tabLeds[j][i] = tabLeds[j][i - 1];  // Utilisation correcte des crochets
    }
  }

  // Mise à jour de la première colonne avec les nouvelles valeurs
  for (uint16_t i = 0; i+2 < bufferSize && i < tabLedLarg; i++) {
    int valeur;  // Déclaration correcte de la variable 'valeur'

    // Assignation de la valeur selon la plage

    valeur = constrain(vData[i+2], 0, 1000);
    valeur = map(valeur, 0, 1000, -10, 100);  // Mise à jour de la première colonne
    valeur = constrain(valeur, 0, 100);
    tabLeds[i][0] = valeur;
  }
}




void PrintVector(float *vData, uint16_t bufferSize, uint8_t scaleType) {
  for (uint16_t i = 0; i < bufferSize; i++) {
    float abscissa;
    /* Print abscissa value */
    switch (scaleType) {
      case SCL_INDEX:
        abscissa = (i * 1.0);
        break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
        break;
    }
    Serial.print(abscissa, 6);
    if (scaleType == SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
