#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

// Melodies store alternating note and duration-divider values.
const int tempoMega = 100;
const int melodyMega[] = {
  NOTE_D3,16, NOTE_D3,16, NOTE_D4,8, NOTE_A3,6, REST,32, NOTE_GS3,8, NOTE_G3,8, NOTE_F3,8, NOTE_D3,16, NOTE_F3,16, NOTE_G3,16,
  NOTE_C3,16, NOTE_C3,16, NOTE_D4,8, NOTE_A3,6, REST,32, NOTE_GS3,8, NOTE_G3,8, NOTE_F3,8, NOTE_D3,16, NOTE_F3,16, NOTE_G3,16,
  NOTE_B2,16, NOTE_B2,16, NOTE_D4,8, NOTE_A3,6, REST,32, NOTE_GS3,8, NOTE_G3,8, NOTE_F3,8, NOTE_D3,16, NOTE_F3,16, NOTE_G3,16,
  NOTE_AS2,16, NOTE_AS2,16, NOTE_D4,8, NOTE_A3,6, REST,32, NOTE_GS3,8, NOTE_G3,8, NOTE_F3,8, NOTE_D3,16, NOTE_F3,16, NOTE_G3,16,
  NOTE_D4,16, NOTE_D4,16, NOTE_D5,8, NOTE_A4,6, REST,32, NOTE_GS4,8, NOTE_G4,8, NOTE_F4,8, NOTE_D4,16, NOTE_F4,16, NOTE_G4,16,
  NOTE_C4,16, NOTE_C4,16, NOTE_D5,8, NOTE_A4,6, REST,32, NOTE_GS4,8, NOTE_G4,8, NOTE_F4,8, NOTE_D4,16, NOTE_F4,16, NOTE_G4,16,
  NOTE_B3,16, NOTE_B3,16, NOTE_D5,8, NOTE_A4,6, REST,32, NOTE_GS4,8, NOTE_G4,8, NOTE_F4,8, NOTE_D4,16, NOTE_F4,16, NOTE_G4,16,
  NOTE_AS3,16, NOTE_AS3,16, NOTE_D5,8, NOTE_A4,6, REST,32, NOTE_GS4,8, NOTE_G4,8, NOTE_F4,8, NOTE_D4,16, NOTE_F4,16, NOTE_G4,16,
  NOTE_F4,16, NOTE_F4,16, NOTE_F4,16, NOTE_F4,16, NOTE_F4,16, NOTE_D4,8, NOTE_D4,8, NOTE_D4,16, NOTE_F4,8, NOTE_F4,8, NOTE_F4,8,
  NOTE_G4,8, NOTE_GS4,8, NOTE_G4,8, NOTE_F4,8, NOTE_D4,16, NOTE_F4,16, NOTE_G4,16, REST,2,
  NOTE_F4,8, NOTE_F4,16, NOTE_F4,8, NOTE_G4,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C5,4, NOTE_A4,16, NOTE_D5,8, NOTE_D5,16, NOTE_D5,8,
  NOTE_A4,8, NOTE_D5,8, NOTE_C5,16, NOTE_F4,16, NOTE_F4,16, NOTE_F4,16, NOTE_F4,16, NOTE_D4,8, NOTE_D4,8, NOTE_D4,16, NOTE_F4,8,
  NOTE_F4,8, NOTE_F4,8, NOTE_F4,8, NOTE_D4,8, NOTE_F4,8, NOTE_E4,8, NOTE_D4,8, NOTE_C4,16, REST,16, NOTE_G4,15, NOTE_E4,8, NOTE_D4,8,
  NOTE_D4,2, NOTE_D4,3, NOTE_F3,16, NOTE_G3,16, NOTE_AS3,16, NOTE_C4,16, NOTE_D4,16, NOTE_F4,16, NOTE_C5,16, REST,8
};
const int melodyMegaSize = sizeof(melodyMega) / sizeof(melodyMega[0]);

const int tempoTetris = 144;
const int melodyTetris[] = {
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST,4,
  REST,8, NOTE_D5, 4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4
};
const int melodyTetrisSize = sizeof(melodyTetris) / sizeof(melodyTetris[0]);

const int tempoMario = 200;
const int melodyMario[] = {
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8,
  NOTE_G5,4, REST,4, NOTE_G4,8, REST,4,
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4,
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4
};
const int melodyMarioSize = sizeof(melodyMario) / sizeof(melodyMario[0]);

void playSFX(unsigned int frequency, unsigned long duration) {
  tone(BUZZER_PIN, frequency, duration);
  sfxEndTime = millis() + duration + 20;
}

void soundEat()      { playSFX(1100, 80); }
void soundBonus()    { playSFX(1600, 150); }
void soundStart()    { playSFX(850, 100); }
void soundPause()    { playSFX(350, 100); }
void soundResume()   { playSFX(650, 100); }
void soundWin()      { playSFX(1200, 400); }

const int gameOverMelody[] = {
  NOTE_E5, 12, NOTE_DS5, 12, NOTE_D5, 12, NOTE_CS5, 12,
  NOTE_C5, 8,  NOTE_B4, 8,  NOTE_A4, 8,  NOTE_G4, 8,
  NOTE_E4, 4,  NOTE_C4, 4,  NOTE_G3, 3,  REST, 8,
  NOTE_G2, 2
};
const int gameOverMelodySize = sizeof(gameOverMelody) / sizeof(gameOverMelody[0]);
const int gameOverTempo = 100;

int gameOverNoteIndex = 0;
unsigned long gameOverNoteStart = 0;
unsigned long gameOverNoteDur = 0;

void soundGameOver() {
  noTone(BUZZER_PIN);
  isPlayingGameOverJingle = true;
  gameOverNoteIndex = 0;
  gameOverNoteStart = 0;
  gameOverNoteDur = 0;
  // Prevent background music from interrupting the jingle.
  sfxEndTime = millis() + 4000;
  currentPlayingTrack = 255;
}

void playGameOverJingle() {
  if (!isPlayingGameOverJingle) return;

  if (gameOverNoteStart == 0 || millis() >= gameOverNoteStart + gameOverNoteDur) {
    if (gameOverNoteIndex >= gameOverMelodySize) {
      isPlayingGameOverJingle = false;
      noTone(BUZZER_PIN);
      sfxEndTime = millis() + 50;
      return;
    }

    int note = gameOverMelody[gameOverNoteIndex];
    int divider = gameOverMelody[gameOverNoteIndex + 1];
    int wholenote = (60000 * 4) / gameOverTempo;
    int noteDuration = 0;

    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = wholenote / (-divider);
      noteDuration += noteDuration / 2;
    }

    if (note != REST) {
      tone(BUZZER_PIN, note, noteDuration - (noteDuration / 12));
    } else {
      noTone(BUZZER_PIN);
    }

    gameOverNoteDur = noteDuration;
    gameOverNoteStart = millis();
    gameOverNoteIndex += 2;
    sfxEndTime = millis() + noteDuration + 30;
  }
}

void playAsyncMusic() {
  byte targetTrack = (menuState > 0) ? musicMenuTrack : musicGameTrack;

  if (targetTrack == 3) {
    noTone(BUZZER_PIN);
    return;
  }

  if (currentPlayingTrack != targetTrack) {
    currentPlayingTrack = targetTrack;
    currentMusicNoteIndex = 0;
    noTone(BUZZER_PIN);
  }

  if (millis() < sfxEndTime) return;

  if (millis() >= lastMusicNoteTime + currentNoteDuration) {
    const int* currentMelody;
    int totalNotes;
    int currentTempo;

    if (targetTrack == 0) {
      currentMelody = melodyMega; totalNotes = melodyMegaSize; currentTempo = tempoMega;
    } else if (targetTrack == 1) {
      currentMelody = melodyTetris; totalNotes = melodyTetrisSize; currentTempo = tempoTetris;
    } else {
      currentMelody = melodyMario; totalNotes = melodyMarioSize; currentTempo = tempoMario;
    }

    if (currentMusicNoteIndex >= totalNotes) currentMusicNoteIndex = 0;

    int note = currentMelody[currentMusicNoteIndex];
    int divider = currentMelody[currentMusicNoteIndex + 1];

    int wholenote = (60000 * 4) / currentTempo;
    int noteDuration = 0;

    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = wholenote / (-divider);
      noteDuration += noteDuration / 2;
    }

    if (note != REST) {
      tone(BUZZER_PIN, note, noteDuration - (noteDuration / 10));
    } else {
      noTone(BUZZER_PIN);
    }

    currentNoteDuration = noteDuration;
    lastMusicNoteTime = millis();
    currentMusicNoteIndex += 2;
  }
}
