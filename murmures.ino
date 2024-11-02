/*
  WS2812ESPFast

   Gestion d'un bandeau WS2812 specific Charlotte Jules Mumures

   Copyright 20201 Pierre HENRY net23@frdev.com

   WS2812ESPFast is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   WS2812ESPFast is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with betaEvents.  If not, see <https://www.gnu.org/licenses/lglp.txt>.


  History
   
   V1.0 (29/102024)
  - rebuild from Bandeau (for nano)

*************************************************/


#include <Arduino.h>
#include "ESP8266.h"
#include "WS2812ESPFast.h"

#define APP_NAME "BandeauESPfast V1.0"


uint8_t div10Hz = 10;
uint8_t div1Hz = 10;
//uint16_t divAnime = 250;

// varibale modifiables
const int ledsMAX = 3120;  // nombre de led sur le bandeau
// varibale modifiables (fin)

WS2812rvbf_t ledOff;
WS2812rvbf_t ledOn;
WS2812rvbf_t ledSkip;

int modeTest = false;


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  Serial.println("\r\n\r\n" APP_NAME);
  Serial.println("Bonjour");
  pinMode(LED_LIFE, OUTPUT);
  pinMode(BP0_PIN, INPUT_PULLUP);
  pinMode(WS2812_PIN, OUTPUT);
  //for (int N = 0; N < ledsMAX; N++) {
  //  leds[N].color = (e_rvb)((rvb_white + N) % MAX_e_rvb);
  ledOff.color = rvb_black;
  ledOn.color = rvb_white;
  ledSkip.color = rvb_black;
}


uint32_t milliCent = millis();  // heure du dernier 100Hz obtenus
bool ledLifeStat = true;
bool bp0Stat = false;


const int tabLedLarg = 10;  // fréquence
const int tabLedLong = 6;   // temps

uint8_t tabLeds[tabLedLarg][tabLedLong];  // Correction de la déclaration du tableau


// the loop function runs over and over again forever
void loop() {

  // detection de 10milliseconde pour btenir du 100Hz
  uint16_t delta = millis() - milliCent;
  if (delta >= 10) {
    milliCent += 10;

    // 100 Hz rafraichissement bandeau
    //jobRefreshLeds(10);

    // diviseur pour avoir du 10Hz
    if (--div10Hz == 0) {
      // 10 Hz
      div10Hz = 10;
      //  10HZ sample
      sample();
      // Impression du tableau pour vérifier les valeurs
      if (modeTest)
        for (int i = 0; i < tabLedLarg; i++) {
          Serial.print(i);
          Serial.print("=>");
          for (int j = 0; j < tabLedLong; j++) {
            Serial.print(tabLeds[i][j]);
            Serial.print("\t");  // Ajoute une tabulation pour espacer les valeurs
          }
          Serial.println();  // Nouvelle ligne à la fin de chaque rangée
        }

      // 10 Hz rafraichissement bandeau
      jobRefreshLeds();

      //10HZ test poussoir
      jobPoussoir();

      // diviseur pour avoir du 1HZ
      if (--div1Hz == 0) {
        div1Hz = 10;
        digitalWrite(LED_LIFE, not digitalRead(LED_LIFE));


      }  // 1Hz
    }    // 10Hz
  }      // 100Hz

  //delay(1);
}



void jobPoussoir() {
  if ((digitalRead(BP0_PIN) == LOW) != bp0Stat) {
    bp0Stat = not bp0Stat;
    if (bp0Stat) {

      Serial.println("bpdown");
      digitalWrite(WS2812_PIN, LOW);
      modeTest++;
      if (modeTest > 2) modeTest = 0;
      if (modeTest == 2) ledSkip.color = rvb_pink;
      else ledSkip.color = rvb_black;
    } else {
      Serial.println("bpUp");
      digitalWrite(WS2812_PIN, HIGH);
    }
  }
}

/*
const int tabLedLarg = 10;  // fréquence
const int tabLedLong = 50;  // temps

uint8_t tabLeds[tabLedLarg][tabLedLong];  // Correction de la déclaration du tableau
*/


// 100 HZ  Remplissage des bandeaux a partir de tabLeds
void jobRefreshLeds() {
  if (modeTest == 1) {
    //full affichage blanc
    for (int N = 0; N < 3210; N++) {
      ledOn.write();
    }
    ledOff.reset();  // obligatoire
    return;
  }

#define adj(x) (T % 2) ? tabLedLarg - x - 1 : x
#define adjc(x) (e_rvb)((T % 2) ? rvb_fr0 + rvb_fr9 - x  : x)
  for (int T = 0; T < tabLedLong; T++) {

    if (T == 0) ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 4) {
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
    }
    // premiere ligne aigue T0  endroit
    WS2812rvbf_t ledTemp;

    ledTemp.color = adjc(rvb_fr9);
    for (int N = 1; N < 49; N++) {
      if (tabLeds[adj(0)][T] < N * 100 / 49) ledTemp.color = rvb_black;
      ledTemp.write();
    }

    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 3) ledSkip.write();
    // deuxieme ligne aigue T0  envers
    ledTemp.color = rvb_black;
    for (int N = 48; N > 0; N--) {
      if (tabLeds[adj(1)][T] > N * 100 / 49) ledTemp.color = adjc(rvb_fr8);
      ledTemp.write();
    }
    ledSkip.write();
    ledSkip.write();
    ledSkip.write();


    //troisieme ligne
    ledTemp.color = adjc(rvb_fr7);
    for (int N = 1; N < 49; N++) {
      if (tabLeds[adj(2)][T] < N * 100 / 49) ledTemp.color = rvb_black;
      ledTemp.write();
    }

    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 3 or T == 5) ledSkip.write();
    // quatrieme ligne aigue T0  envers
    ledTemp.color = rvb_black;
    for (int N = 48; N > 0; N--) {
      if (tabLeds[adj(3)][T] > N * 100 / 49) ledTemp.color = adjc(rvb_fr6);
      ledTemp.write();
    }
    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if ((T == 1) or (T == 2) or T == 4) ledSkip.write();
    //cinquieme
    ledTemp.color = adjc(rvb_fr5);
    for (int N = 1; N < 49; N++) {
      if (tabLeds[adj(4)][T] < N * 100 / 49) ledTemp.color = rvb_black;
      ledTemp.write();
    }

    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 3) ledSkip.write();

    // sixieme ligne aigue T0  envers
    ledTemp.color = rvb_black;
    for (int N = 48; N > 0; N--) {
      if (tabLeds[adj(5)][T] > N * 100 / 49) ledTemp.color = adjc(rvb_fr4);
      ledTemp.write();
    }
    ledSkip.write();
    ledSkip.write();
    ledSkip.write();

    //sept
    ledTemp.color = adjc(rvb_fr3);
    for (int N = 1; N < 49; N++) {
      if (tabLeds[adj(6)][T] < N * 100 / 49) ledTemp.color = rvb_black;
      ledTemp.write();
    }

    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 3 or T == 5) ledSkip.write();

    // huit ligne aigue T0  envers
    ledTemp.color = rvb_black;
    for (int N = 48; N > 0; N--) {
      if (tabLeds[adj(7)][T] > N * 100 / 49) ledTemp.color = adjc(rvb_fr2);
      ledTemp.write();
    }
    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 1 or T == 2) ledSkip.write();
    //neuf
    ledTemp.color = adjc(rvb_fr1);
    for (int N = 1; N < 49; N++) {
      if (tabLeds[adj(8)][T] < N * 100 / 49) ledTemp.color = rvb_black;
      ledTemp.write();
    }

    ledSkip.write();
    ledSkip.write();
    ledSkip.write();
    if (T == 3) ledSkip.write();


    // 10 ligne aigue T0  envers
    ledTemp.color = rvb_black;
    for (int N = 48; N > 0; N--) {
      if (tabLeds[adj(9)][T] > N * 100 / 49) ledTemp.color = adjc(rvb_fr0);
      ledTemp.write();
    }
    if (T == 1) {
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
      ledSkip.write();
    }
  }
  ledOn.write();




  //  for (int N = 0; N < 2210; N++) {
  //    ledOff.write();
  //  }

  //construction des bandeaux de led a partir de tabLed
  /*
  for (int N = 0; N < ledsMAX; N++) {
    leds[N].write();
  }
  //  option mode mirroir
  //  for (int8_t N = ledsMAX - 1; N > 0; N--) {
  //    leds[N].write();
  //  }
  */
  ledOff.reset();  // obligatoire
}
