void soundFlap() {
  playSFX(980, 35);
}

void fbPlacePipe(byte idx, int x) {
  int margin = fbGapSize / 2 + 6;
  fbPipeX[idx] = x;
  fbGapY[idx] = random(margin, SCREEN_HEIGHT - margin);
}

void resetFlappy() {
  currentGame = 1;
  score = 0;
  isGameOver = false;
  isWinner = false;
  isPaused = false;
  isDeathAnimating = false;
  isNewHighScore = false;
  isBonusFoodActive = false;
  pauseExitTriggered = false;
  pauseInputArmed = false;
  isPlayingGameOverJingle = false;
  fbFlapQueued = false;
  fbBirdY = SCREEN_HEIGHT / 2.0f;
  fbBirdVel = 0;

  if (difficultySetting == 0) {
    fbScrollSpeed = 1;
    fbGapSize = 28;
  } else if (difficultySetting == 1) {
    fbScrollSpeed = 2;
    fbGapSize = 22;
  } else {
    fbScrollSpeed = 3;
    fbGapSize = 18;
  }

  int spacing = 52;
  for (byte i = 0; i < FB_PIPE_COUNT; i++) {
    fbPlacePipe(i, SCREEN_WIDTH + 20 + i * spacing);
  }

  gameStarted = true;
  gameStartTime = millis();
  lastFlappyFrame = millis();
  currentMusicNoteIndex = 0;
  currentNoteDuration = 0;
  oledDirty = true;
  soundStart();
}

void triggerFlappyGameOver() {
  if (isGameOver) return;
  isGameOver = true;
  gameEndTime = millis();
  noTone(BUZZER_PIN);
  soundGameOver();
  saveHighScore();
  oledDirty = true;
}

void updateFlappy() {
  if (millis() - lastFlappyFrame < 33) return;
  lastFlappyFrame = millis();

  if (fbFlapQueued) {
    fbBirdVel = -2.6f;
    fbFlapQueued = false;
  }

  float gravity = 0.22f;
  if (difficultySetting == 0) gravity = 0.18f;
  else if (difficultySetting == 2) gravity = 0.26f;

  fbBirdVel += gravity;
  if (fbBirdVel > 4.5f) fbBirdVel = 4.5f;
  fbBirdY += fbBirdVel;

  const int birdR = 4;
  if (fbBirdY - birdR < 0) {
    fbBirdY = birdR;
    fbBirdVel = 0;
  }
  if (fbBirdY + birdR >= SCREEN_HEIGHT) {
    triggerFlappyGameOver();
    return;
  }

  for (byte i = 0; i < FB_PIPE_COUNT; i++) {
    int prevX = fbPipeX[i];
    fbPipeX[i] -= fbScrollSpeed;

    if (prevX + fbPipeW >= fbBirdX && fbPipeX[i] + fbPipeW < fbBirdX) {
      score++;
      if (score > highScoreFlappy) isNewHighScore = true;
      soundEat();
    }

    if (fbPipeX[i] + fbPipeW < -4) {
      int maxX = fbPipeX[0];
      for (byte j = 1; j < FB_PIPE_COUNT; j++) {
        if (fbPipeX[j] > maxX) maxX = fbPipeX[j];
      }
      fbPlacePipe(i, maxX + 52);
    }

    int gapTop = fbGapY[i] - fbGapSize / 2;
    int gapBot = fbGapY[i] + fbGapSize / 2;
    int birdLeft = fbBirdX - birdR;
    int birdRight = fbBirdX + birdR;
    int birdTop = (int)fbBirdY - birdR;
    int birdBot = (int)fbBirdY + birdR;

    bool overlapX = birdRight > fbPipeX[i] && birdLeft < fbPipeX[i] + fbPipeW;
    if (overlapX && (birdTop < gapTop || birdBot > gapBot)) {
      triggerFlappyGameOver();
      return;
    }
  }

  oledDirty = true;
}

void drawFlappyOLED() {
  display.drawLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, SSD1306_WHITE);

  for (byte i = 0; i < FB_PIPE_COUNT; i++) {
    int x = fbPipeX[i];
    if (x > SCREEN_WIDTH || x + fbPipeW < 0) continue;
    int gapTop = fbGapY[i] - fbGapSize / 2;
    int gapBot = fbGapY[i] + fbGapSize / 2;

    display.fillRect(x, 0, fbPipeW, gapTop, SSD1306_WHITE);
    display.fillRect(x, gapBot, fbPipeW, SCREEN_HEIGHT - gapBot - 1, SSD1306_WHITE);
    display.fillRect(x - 2, gapTop - 4, fbPipeW + 4, 4, SSD1306_WHITE);
    display.fillRect(x - 2, gapBot, fbPipeW + 4, 4, SSD1306_WHITE);
  }

  int by = (int)fbBirdY;
  display.fillCircle(fbBirdX, by, 4, SSD1306_WHITE);
  display.fillCircle(fbBirdX + 2, by - 1, 1, SSD1306_BLACK);
  display.fillTriangle(fbBirdX + 4, by, fbBirdX + 8, by - 1, fbBirdX + 8, by + 2, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print(F("W:"));
  display.print(score);
  display.setCursor(70, 2);
  display.print(F("R:"));
  display.print(currentHighScore());

  if (isPaused || isGameOver) {
    int boxW = 110;
    int boxH = 40;
    int boxX = (SCREEN_WIDTH - boxW) / 2;
    int boxY = (SCREEN_HEIGHT - boxH) / 2 - 2;

    display.fillRoundRect(boxX, boxY, boxW, boxH, 4, SSD1306_BLACK);
    display.drawRoundRect(boxX, boxY, boxW, boxH, 4, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);

    if (isPaused) {
      display.setTextSize(2);
      display.setCursor(boxX + 22, boxY + 6);
      display.print(F("PAUZA"));
      display.setTextSize(1);
      display.setCursor(boxX + 8, boxY + 28);
      display.print(F("TRZYMAJ = MENU"));
    } else {
      display.setTextSize(1);
      display.setCursor(boxX + 18, boxY + 6);
      display.print(F("KONIEC GRY!"));
      display.setCursor(boxX + 10, boxY + 18);
      display.print(F("WYNIK: "));
      display.print(score);
      if (isNewHighScore) {
        display.setCursor(boxX + 10, boxY + 28);
        display.print(F("* NOWY REKORD! *"));
      }
    }
  }
}
