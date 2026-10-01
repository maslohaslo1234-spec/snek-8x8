// Matrix.ino
// Obsluga matrycy LED 8x8 (2x rejestr przesuwny 74HC595) oraz logika
// wygaszacza ekranu (screensaver).
//
// ZMIANA ARCHITEKTURY: multipleksowanie wierszy odbywa sie teraz w
// przerwaniu sprzetowego Timer1 (co ok. 1ms - tak samo czesto jak
// wczesniej w blokujacej funkcji renderMatrix()), zamiast bezposrednio
// w loop(). Dzieki temu dlugie operacje typu display.display() na
// OLED-zie (transfer ~1KB po I2C, realnie kilkanascie-dwadziescia ms)
// nie "zamrazaja" juz jednego wiersza matrycy na dluzej niz pozostale -
// a wczesniej dawalo to widoczne miganie/rozjasnienie tego
// wiersza przy kazdej aktualizacji OLED-a.
//
// Dlaczego Timer1, a nie Timer2? Bo funkcja tone() na ATmega328
// (Uno/Nano) korzysta wewnetrznie z Timer2. Gdybysmy multipleksowali
// matryce na Timer2, dzwieki z buzzera zaczelyby konfliktowac z
// odswiezaniem matrycy (i odwrotnie - kazde wywolanie tone()/noTone()
// przeprogramowuje rejestry Timer2). Timer1 nie jest tu przez nikogo
// uzywany (Servo.h go zajmuje, ale nie ma go w tym projekcie), wiec
// jest bezpiecznym wyborem.
//
// UWAGA: ten kod (rejestry TCCR1x, OCR1A, TIMSK1) jest specyficzny dla
// AVR (ATmega328 - Uno/Nano/Mini). Przy ewentualnej migracji na ESP32
// trzeba by to zastapic np. hw_timer_t / esp_timer albo zadaniem FreeRTOS.

#include <avr/interrupt.h>

// NOVUM: DATA_PIN(11)/LATCH_PIN(10)/CLK_PIN(13) siedza wszystkie na PORTB
// (D8-D13 = PB0-PB5 na ATmega328), wiec mozna nimi sterowac bezposrednio
// przez rejestr PORTB zamiast przez shiftOut()/digitalWrite(). Te dwie
// funkcje sa zaskakujaco wolne (przeszukuja tabele pin->port w pamieci
// programu i kazda robi wewnetrzny cli()/sei()) - przy 16 bitach danych
// to okolo 50 wywolan, czyli ISR trwal setki mikrosekund. To on blokowal
// przerwanie Timer2 od tone() na tyle dlugo, ze dzwiek z buzzera "chrapal".
// Ponizsze bity odpowiadaja pinom D10/D11/D13 (PB2/PB3/PB5).
#define MATRIX_LATCH_BIT (1 << PB2) // D10
#define MATRIX_DATA_BIT  (1 << PB3) // D11
#define MATRIX_CLK_BIT   (1 << PB5) // D13

// NOVUM: recznie rozpisany odpowiednik shiftOut(..., MSBFIRST, value),
// ale operujacy tylko na PORTB - bez wywolan funkcji, bez wyszukiwania
// masek w tabelach. To jest teraz jedyny "silnik" wypychania bitow do
// rejestrow 74HC595, wolany z wnetrza ISR-a.
static inline void fastShiftOutMSB(byte value) {
  for (byte i = 0; i < 8; i++) {
    if (value & 0x80) {
      PORTB |= MATRIX_DATA_BIT;
    } else {
      PORTB &= ~MATRIX_DATA_BIT;
    }
    PORTB |= MATRIX_CLK_BIT;   // zbocze narastajace zegara
    PORTB &= ~MATRIX_CLK_BIT;
    value <<= 1;
  }
}

// NOVUM: ta sama kolejnosc bitow i pinow co w oryginalnym writeMatrix()
// (najpierw columns, potem rows), tylko przez PORTB zamiast shiftOut/
// digitalWrite. Caly przebieg trwa teraz pojedyncze mikrosekundy.
void writeMatrix(byte rows, byte columns) {
  PORTB &= ~MATRIX_LATCH_BIT;
  fastShiftOutMSB(columns);
  fastShiftOutMSB(rows);
  PORTB |= MATRIX_LATCH_BIT;
}

void setupMatrixTimer() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 249;                          // ~1ms przy 16MHz / preskaler 64
  TCCR1B |= (1 << WGM12);               // tryb CTC (OCR1A = TOP)
  TCCR1B |= (1 << CS11) | (1 << CS10);  // preskaler 64
  TIMSK1 |= (1 << OCIE1A);              // wlacz przerwanie compare-match

  interrupts();
}

ISR(TIMER1_COMPA_vect) {
  byte rowByte = 0xFF ^ (1 << currentScanRow);
  byte colByte = (byte)~displayBuffer[currentScanRow];

  writeMatrix(rowByte, colByte);

  currentScanRow++;
  if (currentScanRow >= 8) currentScanRow = 0;
}

void clearMatrixBuffer() {
  for (byte i = 0; i < 8; i++) displayBuffer[i] = 0;
}

// Funkcje pomocnicze do animacji w menu (waz chodzacy po krawedzi matrycy)
byte getPerimeterX(byte index) {
  index = index % 28;
  if (index < 8) return index;
  if (index < 15) return 7;
  if (index < 22) return 7 - (index - 14);
  return 0;
}

byte getPerimeterY(byte index) {
  index = index % 28;
  if (index < 8) return 0;
  if (index < 15) return index - 7;
  if (index < 22) return 7;
  return 7 - (index - 21);
}

byte screensaverFrame = 0;
unsigned long lastScreensaverAnim = 0;
const byte screensaverFrames[4][8] PROGMEM = {
  { B00000000, B00000000, B00000000, B00011000, B00011000, B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00111100, B00100100, B00100100, B00111100, B00000000, B00000000 },
  { B00000000, B01111110, B01000010, B01000010, B01000010, B01000010, B01111110, B00000000 },
  { B11111111, B10000001, B10000001, B10000001, B10000001, B10000001, B10000001, B11111111 }
};

void handleScreensaver() {
  if (millis() - lastSsMoveTime > 400) {
    lastSsMoveTime = millis();
    ssX += ssDirX * 3;
    ssY += ssDirY * 2;

    if (ssX <= 2 || ssX >= 70) ssDirX *= -1;
    if (ssY <= 2 || ssY >= 35) ssDirY *= -1;
    oledDirty = true;
  }

  if (millis() - lastScreensaverAnim > 250) {
    lastScreensaverAnim = millis();
    screensaverFrame = (screensaverFrame + 1) % 4;
    oledDirty = true;
  }
}

// NOVUM: cala funkcja rysuje teraz klatke w lokalnej tablicy newBuffer[]
// (zwyklej, nie-volatile, widocznej tylko tutaj), a nie bezposrednio w
// dzielonym displayBuffer[]. Wczesniej ISR (co ~1ms) mogl odczytac
// displayBuffer dokladnie w trakcie jego przebudowy - np. juz wyczyszczony,
// ale jeszcze nie wypelniony na nowo wiersz - co dawalo widoczne miganie.
// Logika rysowania (kolejnosc warunkow, ksztalty, wszystkie animacje)
// jest identyczna jak wczesniej - zmienia sie tylko to, DO CZEGO pisza te
// instrukcje.
void updateDisplayBuffer() {
  byte newBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // NOVUM: lokalna klatka robocza

  if (isScreensaver) {
    for (byte i = 0; i < 8; i++) {
      newBuffer[i] = pgm_read_byte(&screensaverFrames[screensaverFrame][i]); // NOVUM
    }
  } else if (!gameStarted) {
    // Animacja pelzajacego weza po krawedzi matrycy w menu
    unsigned long menuAnimTime = millis() / 80; // Szybkosc (im mniej, tym szybciej)
    byte headPos = menuAnimTime % 28;
    byte menuSnakeLen = 8; // Dlugosc pokazowego weza

    for (byte i = 0; i < menuSnakeLen; i++) {
      byte pos = (headPos + 28 - i) % 28;
      byte px = getPerimeterX(pos);
      byte py = getPerimeterY(pos);
      newBuffer[py] |= (1 << (7 - px)); // NOVUM
    }

    // Pulsujace "jedzenie" na srodku matrycy
    if (blinkState) {
      newBuffer[4] |= (1 << (7 - 4)); // NOVUM
    }
  } else if (isDeathAnimating) {
    for (byte i = 0; i < deathAnimStep; i++) {
      newBuffer[snakeY[i]] |= (1 << (7 - snakeX[i])); // NOVUM
    }
  } else if (isGameOver) {
    if (blinkState) {
      for (byte i = 0; i < 8; i++) {
        newBuffer[i] = (1 << i) | (1 << (7 - i)); // NOVUM
      }
    }
  } else {
    // Rysowanie normalnego jedzenia
    newBuffer[foodY] |= (1 << (7 - foodX)); // NOVUM

    // Rysowanie bonusowego jedzenia (szybkie miganie)
    if (isBonusFoodActive && blinkState) {
      newBuffer[bonusFoodY] |= (1 << (7 - bonusFoodX)); // NOVUM
    }

    // Rysowanie weza
    for (byte i = 0; i < snakeLen; i++) {
      newBuffer[snakeY[i]] |= (1 << (7 - snakeX[i])); // NOVUM
    }
  }

  // NOVUM: jedyne miejsce, ktore dotyka dzielonego displayBuffer[] - krotka
  // sekcja krytyczna (kilka mikrosekund) tak, ze ISR widzi zawsze albo
  // cala POPRZEDNIA klatke, albo cala NOWA - nigdy stan posredni.
  noInterrupts();
  for (byte i = 0; i < 8; i++) {
    displayBuffer[i] = newBuffer[i];
  }
  interrupts();
}
