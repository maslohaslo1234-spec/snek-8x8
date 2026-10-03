#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_system.h>
#include "pins.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned long BUTTON_DEBOUNCE_MS = 120;
const unsigned long IDLE_TIMEOUT_MS = 30000;
const unsigned long HOLD_EXIT_MS = 1000;
const int SPEED_UP_EVERY_FOODS = 4;
const int SPEED_UP_STEP_MS = 15;
const int MIN_GAME_SPEED_MS = 90;

// EEPROM address map.
const size_t EEPROM_SIZE = 36;
const byte EEPROM_MAGIC = 0xA8;
const int EEPROM_ADDR_MAGIC = 0;
const int EEPROM_ADDR_DIFFICULTY = 1;
const int EEPROM_ADDR_WALLS = 2;
const int EEPROM_ADDR_MUSIC_MENU = 3;
const int EEPROM_ADDR_MUSIC_GAME = 4;
const int EEPROM_ADDR_LANGUAGE = 5;
const int EEPROM_ADDR_HIGHSCORE_EASY = 8;
const int EEPROM_ADDR_HIGHSCORE_MED = 12;
const int EEPROM_ADDR_HIGHSCORE_HARD = 16;
const int EEPROM_ADDR_HIGHSCORE_FLAPPY = 20;

byte currentGame = 0; // 0: Snake, 1: Flappy Bird

byte snakeX[64];
byte snakeY[64];
byte snakeLen = 3;
byte foodX = 0;
byte foodY = 0;

byte bonusFoodX = 0;
byte bonusFoodY = 0;
bool isBonusFoodActive = false;
unsigned long bonusFoodTimer = 0;
const unsigned long BONUS_FOOD_DURATION_MS = 6000;
byte foodEatenCount = 0;

int score = 0;
int highScores[3] = {0, 0, 0}; // Snake: easy, medium, hard.
int highScoreFlappy = 0;
int currentDir = 1; // 0: up, 1: right, 2: down, 3: left.
bool dirChangedThisTick = false;
int gameSpeed = 350;
int scoreMultiplier = 1;
unsigned long gameStartTime = 0;
unsigned long gameEndTime = 0;
bool isNewHighScore = false;

bool gameStarted = false;
bool isPaused = false;
bool isGameOver = false;
bool isWinner = false;
bool isScreensaver = false;
bool oledDirty = true;

byte menuState = 0;              
byte mainMenuCursor = 0;         
byte settingsCursor = 0;
byte muzykaCursor = 0;           

byte difficultySetting = 1;
bool wallsEnabled = true;
byte languageSetting = 0; // 0: English, 1: Polish.

byte musicMenuTrack = 0;
byte musicGameTrack = 1;
byte currentPlayingTrack = 255;

const byte FB_PIPE_COUNT = 3;
float fbBirdY = 32;
float fbBirdVel = 0;
bool fbFlapQueued = false;
int fbPipeX[3];
int fbGapY[3]; // Vertical center of each gap.
int fbGapSize = 22;
int fbPipeW = 16;
int fbScrollSpeed = 2;
int fbBirdX = 28;
unsigned long lastFlappyFrame = 0;  

int ssX = 40, ssY = 28;
int ssDirX = 1, ssDirY = 1;
unsigned long lastSsMoveTime = 0;
const byte SS_TRAIL_LEN = 12;
int ssTrailX[12];
int ssTrailY[12];
int ssFoodX = 90, ssFoodY = 40;
int ssFoodDirX = -1, ssFoodDirY = 1;
byte ssMatFoodX = 7, ssMatFoodY = 3;
int ssMatFoodDir = 1;
byte ssMatFoodPos = 10;

bool isDeathAnimating = false;
byte deathAnimStep = 0;
unsigned long lastDeathAnimTime = 0;

bool lastLeftState = HIGH;
bool lastRightState = HIGH;
bool lastActionState = HIGH;
bool lastUpState = HIGH;
bool lastDownState = HIGH;
unsigned long lastActivityTime = 0;
unsigned long pauseActionPressTime = 0;
bool pauseExitTriggered = false;
bool pauseInputArmed = false; // Wait for ACTION to be released after entering pause.

unsigned long lastMoveTime = 0;
unsigned long lastButtonTime = 0;
unsigned long lastBlinkTime = 0;
bool blinkState = true;

unsigned long sfxEndTime = 0;
unsigned long lastMusicNoteTime = 0;
int currentMusicNoteIndex = 0;
unsigned long currentNoteDuration = 0;
bool isPlayingGameOverJingle = false;
unsigned long lastOledUpdateTime = 0;
const unsigned long OLED_UPDATE_MS = 35;

volatile byte displayBuffer[8];
volatile byte currentScanRow = 0;

void IRAM_ATTR writeMatrix(byte rows, byte columns);
void clearMatrixBuffer();
void setupMatrixTimer();

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);

  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_UP_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_ACTION_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  clearMatrixBuffer();
  writeMatrix(0xFF, 0xFF);

  randomSeed(esp_random()); 

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  Wire.setClock(400000L);
  display.clearDisplay();

  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  lastActivityTime = millis();

  setupMatrixTimer(); 
}

void loop() {
  handleInput();

  if (millis() - lastActivityTime > IDLE_TIMEOUT_MS && !gameStarted && !isScreensaver) {
    isScreensaver = true;
    ssX = 40;
    ssY = 28;
    ssDirX = 1;
    ssDirY = 1;
    ssFoodX = 96;
    ssFoodY = 20;
    ssFoodDirX = -1;
    ssFoodDirY = 1;
    ssMatFoodPos = 10;
    ssMatFoodDir = 1;
    ssMatFoodX = getPerimeterX(ssMatFoodPos);
    ssMatFoodY = getPerimeterY(ssMatFoodPos);
    for (byte i = 0; i < SS_TRAIL_LEN; i++) {
      ssTrailX[i] = ssX;
      ssTrailY[i] = ssY;
    }
    lastSsMoveTime = millis();
    noTone(BUZZER_PIN);
    oledDirty = true;
  }

  if (isScreensaver) {
    handleScreensaver();
  } else if (gameStarted && !isPaused) {
    if (currentGame == 1) {
      if (!isGameOver) updateFlappy();
    } else if (isDeathAnimating) {
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

  // Give the game-over jingle priority over background music.
  if (isPlayingGameOverJingle || isDeathAnimating) {
    playGameOverJingle();
  } else if (!isScreensaver && ((gameStarted && !isPaused && !isGameOver) || (!gameStarted && menuState > 0))) {
    playAsyncMusic();
  }
  
  updateDisplayBuffer();

  if (oledDirty && millis() - lastOledUpdateTime >= OLED_UPDATE_MS) {
    lastOledUpdateTime = millis();
    updateOLED();
    oledDirty = false;
  }
}