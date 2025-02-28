#define ESP32DEBUGGING
#include <ESP32Logger.h>

#include "Arduino.h"
#include "melody.h"

// Define melodies
Note melodyWiFiConnecting[] = {
    {NOTE_B0, 250}, {NOTE_B1, 250}};

Note melodyWiFiConnected[] = {
    {NOTE_C4, 250}, {NOTE_E4, 250}, {NOTE_G4, 500}, {NOTE_G4, 500}};

Note melodyWiFiDisconnected[] = {
    {NOTE_D4, 250},
    {NOTE_E4, 250},
    {NOTE_C4, 750},
};

Note melodyAPStarted[] = {{NOTE_A4, 100},
                          {NOTE_A4, 200},
                          {NOTE_A4, 300},
                          {NOTE_A4, 400},
                          {NOTE_A4, 500}};

Note melodySystemRestart[] = {{NOTE_G4, 500}, {NOTE_E4, 500}, {NOTE_C4, 500}};

Note melodyDNSSelectionMode[] = {
    {NOTE_C4, 250}, {NOTE_E4, 250}, {NOTE_G4, 500}};

Note melodyDNSNextSelected[] = {{NOTE_C4, 250}, {NOTE_E4, 250}};

Note melodyPing[] = {{NOTE_C4, 100}};

Note melodyPingTimeout[] = {{NOTE_B0, 1000}};

Note melodyPingAlert[] = {
    {NOTE_D4, 250},
    {NOTE_E4, 250},
    {NOTE_C4, 750},
};

Note melodySettingOn[] = {{NOTE_B5, 50}, {NOTE_B5, 50}, {NOTE_B5, 50}};

Note melodySettingOff[] = {{NOTE_B0, 100}, {NOTE_B0, 100}, {NOTE_B0, 100}};

// Store melodies in an array
Note *melodies[NUM_OF_MELODIES] = {
    melodyWiFiConnecting,
    melodyWiFiConnected, melodyWiFiDisconnected, melodyAPStarted,
    melodySystemRestart, melodyDNSSelectionMode, melodyDNSNextSelected,
    melodyPing,          melodyPingTimeout,      melodyPingAlert,
    melodySettingOn,     melodySettingOff};

// Define the length of each melody
int melodyLengths[NUM_OF_MELODIES] = {
    sizeof(melodyWiFiConnecting) / sizeof(melodyWiFiConnecting[0]),
    sizeof(melodyWiFiConnected) / sizeof(melodyWiFiConnected[0]),
    sizeof(melodyWiFiDisconnected) / sizeof(melodyWiFiDisconnected[0]),
    sizeof(melodyAPStarted) / sizeof(melodyAPStarted[0]),
    sizeof(melodySystemRestart) / sizeof(melodySystemRestart[0]),
    sizeof(melodyDNSSelectionMode) / sizeof(melodyDNSSelectionMode[0]),
    sizeof(melodyDNSNextSelected) / sizeof(melodyDNSNextSelected[0]),
    sizeof(melodyPing) / sizeof(melodyPing[0]),
    sizeof(melodyPingTimeout) / sizeof(melodyPingTimeout[0]),
    sizeof(melodyPingAlert) / sizeof(melodyPingAlert[0]),
    sizeof(melodySettingOn) / sizeof(melodySettingOn[0]),
    sizeof(melodySettingOff) / sizeof(melodySettingOff[0]),
};

void playMelody(int gpio, MelodyName melodyName) {

  Note *melody = melodies[melodyName];
  int length = melodyLengths[melodyName];

  for (int i = 0; i < length; i++) {
    int noteDuration = melody[i].duration;
    tone(gpio, melody[i].note, noteDuration);

    delay(noteDuration * 1.30); // Pause between notes
    // stop the tone playing:
    noTone(gpio);
  }
}

// Structure to hold melody playback information
struct MelodyPlayback {
  int gpio;
  MelodyName melodyName;
  int noteIndex;
  unsigned long startTime;
};

// Global variable to store the currently playing melody
volatile MelodyPlayback currentMelody;
volatile bool isPlaying = false;

// Function to start playing a melody asynchronously
void playMelodyAsync(int gpio, MelodyName melodyName) {
  if (isPlaying) {
    // Optionally queue the new melody or ignore it
    DBGLOG(Error, "A melody is already playing. Ignoring new request.");
    return;
  }
  DBGLOG(Debug, "Melody=%d", melodyName);
  currentMelody.gpio = gpio;
  currentMelody.melodyName = melodyName;
  currentMelody.noteIndex = 0;
  currentMelody.startTime = millis();
  isPlaying = true;
}

void melody_loop() {
  if (isPlaying) {
    Note *melody = melodies[currentMelody.melodyName];
    int length = melodyLengths[currentMelody.melodyName];

    if (currentMelody.noteIndex < length) {
      int noteDuration = melody[currentMelody.noteIndex].duration;
      unsigned long elapsedTime = millis() - currentMelody.startTime;

      if (elapsedTime >=
          noteDuration * 1.3) { // Check if the note duration + pause has passed
        tone(currentMelody.gpio, melody[currentMelody.noteIndex].note,
             noteDuration); // Play the note
        currentMelody.startTime = millis();
        currentMelody.noteIndex = currentMelody.noteIndex +1;
        if (currentMelody.noteIndex >= length) {
          noTone(currentMelody.gpio);
          isPlaying = false;
        }
      }
    }
  }
}
