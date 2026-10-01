#include <Arduino.h>

extern const byte DATA_PIN;
extern const byte LATCH_PIN;
extern const byte CLK_PIN;
extern volatile byte displayBuffer[8];
extern volatile byte currentScanRow;
extern bool isScreensaver;
extern bool gameStarted;
extern bool isDeathAnimating;
extern bool isGameOver;
extern bool isBonusFoodActive;
extern bool blinkState;
extern byte deathAnimStep;
extern byte foodX, foodY;
extern byte bonusFoodX, bonusFoodY;
extern byte snakeX[], snakeY[];
extern byte snakeLen;
extern bool oledDirty;
extern int ssX, ssY, ssDirX, ssDirY;
extern unsigned long lastSsMoveTime;

// This matrix wiring uses active-low rows and columns.
const bool INVERT_ROWS = true;
const bool INVERT_COLS = true;

static inline void IRAM_ATTR fastShiftOutMSB(byte value) {
  for (byte i = 0; i < 8; i++) {
    digitalWrite(DATA_PIN, (value & 0x80) ? HIGH : LOW);
    digitalWrite(CLK_PIN, HIGH);
    digitalWrite(CLK_PIN, LOW);
    value <<= 1;
  }
}

void IRAM_ATTR writeMatrix(byte rows, byte columns) {
  digitalWrite(LATCH_PIN, LOW);
  fastShiftOutMSB(columns);
  fastShiftOutMSB(rows);
  digitalWrite(LATCH_PIN, HIGH);
}

hw_timer_t *matrixTimer = NULL;
portMUX_TYPE matrixBufferMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onMatrixTimer() {
  byte rowByte = (1 << currentScanRow);
  if (INVERT_ROWS) {
    rowByte = 0xFF ^ rowByte;
  }

  byte colByte = displayBuffer[currentScanRow];
  if (INVERT_COLS) {
    colByte = 0xFF ^ colByte;
  }

  writeMatrix(rowByte, colByte);

  currentScanRow++;
  if (currentScanRow >= 8) currentScanRow = 0;
}

void setupMatrixTimer() {
  matrixTimer = timerBegin(1000000);
  timerAttachInterrupt(matrixTimer, &onMatrixTimer);
  timerAlarm(matrixTimer, 1000, true, 0);
}

void clearMatrixBuffer() {
  for (byte i = 0; i < 8; i++) displayBuffer[i] = 0;
}

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

static void spawnScreensaverFoodOled() {
  const int margin = 8;
  for (int attempt = 0; attempt < 40; attempt++) {
    int nx = random(margin, SCREEN_WIDTH - margin);
    int ny = random(margin, SCREEN_HEIGHT - margin);
    int dx = nx - ssX;
    int dy = ny - ssY;
    if (dx * dx + dy * dy < 400) continue;

    bool onBody = false;
    for (byte i = 0; i < SS_TRAIL_LEN; i++) {
      int bx = nx - ssTrailX[i];
      int by = ny - ssTrailY[i];
      if (bx * bx + by * by < 64) {
        onBody = true;
        break;
      }
    }
    if (onBody) continue;

    ssFoodX = nx;
    ssFoodY = ny;
    ssFoodDirX = random(0, 2) ? 1 : -1;
    ssFoodDirY = random(0, 2) ? 1 : -1;
    return;
  }
  ssFoodX = (ssX < 64) ? 100 : 28;
  ssFoodY = (ssY < 32) ? 48 : 16;
  ssFoodDirX = -ssFoodDirX;
  ssFoodDirY = -ssFoodDirY;
}

static void spawnScreensaverFoodMatrix(byte headPos) {
  for (int attempt = 0; attempt < 40; attempt++) {
    byte foodPos = random(0, 28);
    byte nx = getPerimeterX(foodPos);
    byte ny = getPerimeterY(foodPos);

    bool onSnake = false;
    for (byte i = 0; i < 8; i++) {
      byte pos = (headPos + 28 - i) % 28;
      if (getPerimeterX(pos) == nx && getPerimeterY(pos) == ny) {
        onSnake = true;
        break;
      }
    }
    if (foodPos == headPos || foodPos == (headPos + 1) % 28) onSnake = true;

    if (!onSnake) {
      ssMatFoodPos = foodPos;
      ssMatFoodX = nx;
      ssMatFoodY = ny;
      ssMatFoodDir = random(0, 2) ? 1 : -1;
      return;
    }
  }
  ssMatFoodPos = (headPos + 14) % 28;
  ssMatFoodX = getPerimeterX(ssMatFoodPos);
  ssMatFoodY = getPerimeterY(ssMatFoodPos);
}

void handleScreensaver() {
  if (millis() - lastSsMoveTime >= 28) {
    lastSsMoveTime = millis();

    const int margin = 5;
    int nextX = ssX + ssDirX;
    int nextY = ssY + ssDirY;

    if (nextX < margin || nextX > (SCREEN_WIDTH - 1 - margin)) {
      ssDirX = -ssDirX;
      nextX = ssX + ssDirX;
    }
    if (nextY < margin || nextY > (SCREEN_HEIGHT - 1 - margin)) {
      ssDirY = -ssDirY;
      nextY = ssY + ssDirY;
    }

    for (byte i = SS_TRAIL_LEN - 1; i > 0; i--) {
      ssTrailX[i] = ssTrailX[i - 1];
      ssTrailY[i] = ssTrailY[i - 1];
    }
    ssTrailX[0] = nextX;
    ssTrailY[0] = nextY;
    ssX = nextX;
    ssY = nextY;

    ssFoodX += ssFoodDirX;
    ssFoodY += ssFoodDirY;
    if (ssFoodX < margin || ssFoodX > SCREEN_WIDTH - 1 - margin) ssFoodDirX = -ssFoodDirX;
    if (ssFoodY < margin || ssFoodY > SCREEN_HEIGHT - 1 - margin) ssFoodDirY = -ssFoodDirY;
    ssFoodX = constrain(ssFoodX, margin, SCREEN_WIDTH - 1 - margin);
    ssFoodY = constrain(ssFoodY, margin, SCREEN_HEIGHT - 1 - margin);

    int fdx = ssX - ssFoodX;
    int fdy = ssY - ssFoodY;
    if (fdx * fdx + fdy * fdy <= 49) {
      spawnScreensaverFoodOled();
    }

    static byte matFoodTick = 0;
    matFoodTick++;
    if (matFoodTick >= 2) {
      matFoodTick = 0;
      ssMatFoodPos = (ssMatFoodPos + ssMatFoodDir + 28) % 28;
      ssMatFoodX = getPerimeterX(ssMatFoodPos);
      ssMatFoodY = getPerimeterY(ssMatFoodPos);
    }

    oledDirty = true;
  }
}

void updateDisplayBuffer() {
  byte newBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  if (isScreensaver) {
    unsigned long t = millis() / 55;
    byte headPos = t % 28;
    const byte ssLen = 8;
    for (byte i = 0; i < ssLen; i++) {
      byte pos = (headPos + 28 - i) % 28;
      byte px = getPerimeterX(pos);
      byte py = getPerimeterY(pos);
      newBuffer[py] |= (1 << (7 - px));
    }

    byte hx = getPerimeterX(headPos);
    byte hy = getPerimeterY(headPos);
    if (hx == ssMatFoodX && hy == ssMatFoodY) {
      spawnScreensaverFoodMatrix(headPos);
    }
    newBuffer[ssMatFoodY] |= (1 << (7 - ssMatFoodX));
  } else if (!gameStarted) {
    unsigned long menuAnimTime = millis() / 80;
    byte headPos = menuAnimTime % 28;
    byte menuSnakeLen = 8;
    for (byte i = 0; i < menuSnakeLen; i++) {
      byte pos = (headPos + 28 - i) % 28;
      byte px = getPerimeterX(pos);
      byte py = getPerimeterY(pos);
      newBuffer[py] |= (1 << (7 - px));
    }
    if (blinkState) newBuffer[4] |= (1 << (7 - 4));
  } else if (isDeathAnimating) {
    for (byte i = 0; i < deathAnimStep; i++) newBuffer[snakeY[i]] |= (1 << (7 - snakeX[i]));
  } else if (isGameOver) {
    if (blinkState) {
      for (byte i = 0; i < 8; i++) newBuffer[i] = (1 << i) | (1 << (7 - i));
    }
  } else if (currentGame == 1) {
    byte by = (byte)constrain((int)(fbBirdY * 8.0f / SCREEN_HEIGHT), 0, 7);
    newBuffer[by] |= (1 << (7 - 2));

    for (byte i = 0; i < FB_PIPE_COUNT; i++) {
      int col = fbPipeX[i] * 8 / SCREEN_WIDTH;
      if (col < 0 || col > 7) continue;
      int gapTop = fbGapY[i] - fbGapSize / 2;
      int gapBot = fbGapY[i] + fbGapSize / 2;
      byte gt = (byte)constrain(gapTop * 8 / SCREEN_HEIGHT, 0, 7);
      byte gb = (byte)constrain(gapBot * 8 / SCREEN_HEIGHT, 0, 7);
      for (byte r = 0; r < 8; r++) {
        if (r < gt || r > gb) {
          newBuffer[r] |= (1 << (7 - col));
        }
      }
    }
  } else {
    newBuffer[foodY] |= (1 << (7 - foodX));
    if (isBonusFoodActive && blinkState) newBuffer[bonusFoodY] |= (1 << (7 - bonusFoodX));
    for (byte i = 0; i < snakeLen; i++) newBuffer[snakeY[i]] |= (1 << (7 - snakeX[i]));
  }

  portENTER_CRITICAL(&matrixBufferMux);
  for (byte i = 0; i < 8; i++) displayBuffer[i] = newBuffer[i];
  portEXIT_CRITICAL(&matrixBufferMux);
}