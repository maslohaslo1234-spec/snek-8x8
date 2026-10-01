// sketch_sep5a.ino
// Snake na matrycy LED 8x8 + wyswietlacz OLED + muzyka
//
// Struktura szkicu (zakladki widoczne w Arduino IDE):
//   sketch_sep5a.ino  - piny, stale, zmienne globalne, setup()/loop()
//   Matrix.ino        - obsluga matrycy LED (multipleksowanie w przerwaniu Timer1)
//   OLED.ino          - rysowanie ekranu OLED
//   Music.ino         - nuty, melodie, dzwieki
//   Game.ino          - logika weza, jedzenia, wyniku
//   Input.ino         - obsluga przyciskow
//
// Wszystkie te pliki .ino sa przez Arduino IDE sklejane w jedno
// tlumaczenie, wiec funkcje z jednej zakladki spokojnie wywoluja
// funkcje/zmienne zdefiniowane w innej - nie trzeba deklaracji extern.

#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

// ================= USTAWIENIA EKRANU =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= PINY =================
// Piny Rejestru Przesuwnego (Matryca LED)
const byte DATA_PIN = 11;
const byte LATCH_PIN = 10;
const byte CLK_PIN = 13;

// Piny Przyciskow i Buzzera
const byte BTN_LEFT_PIN = 2;
const byte BTN_RIGHT_PIN = 3;
const byte BTN_ACTION_PIN = 4;
const byte BUZZER_PIN = 9;

// ================= STALE GRY =================
const unsigned long BUTTON_DEBOUNCE_MS = 120;
const unsigned long IDLE_TIMEOUT_MS = 15000;
const int HIGH_SCORE_ADDRESS = 0;

// ================= ZMIENNE GRY =================
// Logika Weza
byte snakeX[64];
byte snakeY[64];
byte snakeLen = 3;
byte foodX = 0;
byte foodY = 0;

// Logika Bonusowego Jedzenia
byte bonusFoodX = 0;
byte bonusFoodY = 0;
bool isBonusFoodActive = false;
unsigned long bonusFoodTimer = 0;
const unsigned long BONUS_FOOD_DURATION_MS = 6000;
byte foodEatenCount = 0;

// Wynik i Rozgrywka
int score = 0;
int highScore = 0;
int currentDir = 1; // 0: Gora, 1: Prawo, 2: Dol, 3: Lewo
int gameSpeed = 350;
int scoreMultiplier = 1;
unsigned long gameStartTime = 0;
unsigned long gameEndTime = 0;
bool isNewHighScore = false;

// Stany Gry
bool gameStarted = false;
bool isPaused = false;
bool isGameOver = false;
bool isWinner = false;
bool isScreensaver = false;
bool oledDirty = true;

// Ustawienia Menu i Podmenu
byte menuState = 0;              // 0: Gra/Ekran Startowy, 1: Main Menu, 2: Zasady, 3: Muzyka
byte mainMenuCursor = 0;         // 0: Start, 1: Zasady, 2: Muzyka
byte zasadyCursor = 0;           // 0: Poziom, 1: Sciany, 2: Wstecz
byte muzykaCursor = 0;           // 0: W menu, 1: W grze, 2: Wstecz

byte difficultySetting = 1;      
bool wallsEnabled = true;        

// 0: Megalovania, 1: Tetris, 2: Mario, 3: OFF
byte musicMenuTrack = 0;         
byte musicGameTrack = 1;         
byte currentPlayingTrack = 255;  // Zmienna pomocnicza do resetowania melodii przy zmianie

// Wygaszacz ekranu (Screensaver)
int ssX = 24, ssY = 15;
int ssDirX = 1, ssDirY = 1;
unsigned long lastSsMoveTime = 0;

// Animacja Smierci
bool isDeathAnimating = false;
byte deathAnimStep = 0;
unsigned long lastDeathAnimTime = 0;

// Przycisk i Odliczanie Czasu
bool lastLeftState = HIGH;
bool lastRightState = HIGH;
bool lastActionState = HIGH;
unsigned long lastActivityTime = 0;

unsigned long lastMoveTime = 0;
unsigned long lastButtonTime = 0;
unsigned long lastBlinkTime = 0;
bool blinkState = true;

// Dzwiek i Asynchroniczna Muzyka
unsigned long sfxEndTime = 0;
unsigned long lastMusicNoteTime = 0;
int currentMusicNoteIndex = 0;
unsigned long currentNoteDuration = 0;
unsigned long lastOledUpdateTime = 0;
const unsigned long OLED_UPDATE_MS = 150;

// ================= MATRYCA (bufor dzielony z przerwaniem Timer1) =================
// volatile: te bajty sa odczytywane wewnatrz ISR (patrz Matrix.ino),
// wiec kompilator nie moze ich sobie "podebrac" do rejestru i pominac
// odczytu z pamieci przy kolejnych probach.
volatile byte displayBuffer[8];
volatile byte currentScanRow = 0;

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);

  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_ACTION_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  clearMatrixBuffer();
  writeMatrix(0xFF, 0xFF); // zgas matryce przed startem przerwania

  randomSeed(analogRead(A0));

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  Wire.setClock(400000L);
  display.clearDisplay();
  loadHighScore();
  lastActivityTime = millis();

  setupMatrixTimer(); // od teraz matryca odswieza sie sama w tle (Timer1)
}

void loop() {
  handleInput();

if (millis() - lastActivityTime > IDLE_TIMEOUT_MS && !gameStarted && menuState == 0 && !isScreensaver) {
    isScreensaver = true;
    oledDirty = true;
  }

  if (isScreensaver) {
    handleScreensaver();
  } else if (gameStarted && !isPaused) {
    if (isDeathAnimating) {
      processDeathAnimation();
    } else if (!isGameOver) {
      if (millis() - lastMoveTime > gameSpeed) {
        lastMoveTime = millis();
        updateSnake();
      }
    }
  }

  if (millis() - lastBlinkTime > 300) {
    lastBlinkTime = millis();
    blinkState = !blinkState;
  }

// Odtwarzaj muzyke jesli gramy (i nie ma pauzy/konca) LUB jesli jestesmy w menu
  if ((gameStarted && !isPaused && !isGameOver) || (!gameStarted && menuState > 0)) {
    playAsyncMusic();
  }
  // Uwaga: matryca LED NIE jest juz odswiezana blokujaco tutaj -
  // robi to w tle przerwanie Timer1 (patrz Matrix.ino). Tutaj tylko
  // wypelniamy bufor tresciasa tej klatki.
  updateDisplayBuffer();

  if (oledDirty && millis() - lastOledUpdateTime >= OLED_UPDATE_MS) {
    lastOledUpdateTime = millis();
    updateOLED();
    oledDirty = false;
  }
}
