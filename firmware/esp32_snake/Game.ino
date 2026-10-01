static int highScoreAddressFor(byte difficulty) {
  if (difficulty == 0) return EEPROM_ADDR_HIGHSCORE_EASY;
  if (difficulty == 1) return EEPROM_ADDR_HIGHSCORE_MED;
  return EEPROM_ADDR_HIGHSCORE_HARD;
}

int currentHighScore() {
  if (currentGame == 1) {
    if (gameStarted && score > highScoreFlappy) return score;
    return highScoreFlappy;
  }
  if (difficultySetting > 2) return highScores[1];
  int best = highScores[difficultySetting];
  if (gameStarted && score > best) return score;
  return best;
}

void loadSettings() {
  byte magic = EEPROM.read(EEPROM_ADDR_MAGIC);
  if (magic != EEPROM_MAGIC) {
    highScores[0] = 0;
    highScores[1] = 0;
    highScores[2] = 0;
    highScoreFlappy = 0;
    difficultySetting = 1;
    wallsEnabled = true;
    musicMenuTrack = 0;
    musicGameTrack = 1;
    saveSettings();
    return;
  }

  difficultySetting = EEPROM.read(EEPROM_ADDR_DIFFICULTY);
  if (difficultySetting > 2) difficultySetting = 1;

  wallsEnabled = EEPROM.read(EEPROM_ADDR_WALLS) != 0;

  musicMenuTrack = EEPROM.read(EEPROM_ADDR_MUSIC_MENU);
  if (musicMenuTrack > 3) musicMenuTrack = 0;

  musicGameTrack = EEPROM.read(EEPROM_ADDR_MUSIC_GAME);
  if (musicGameTrack > 3) musicGameTrack = 1;

  for (byte i = 0; i < 3; i++) {
    EEPROM.get(highScoreAddressFor(i), highScores[i]);
    if (highScores[i] < 0 || highScores[i] > 9999) {
      highScores[i] = 0;
    }
  }

  EEPROM.get(EEPROM_ADDR_HIGHSCORE_FLAPPY, highScoreFlappy);
  if (highScoreFlappy < 0 || highScoreFlappy > 9999) highScoreFlappy = 0;
}

void saveSettings() {
  EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
  EEPROM.write(EEPROM_ADDR_DIFFICULTY, difficultySetting);
  EEPROM.write(EEPROM_ADDR_WALLS, wallsEnabled ? 1 : 0);
  EEPROM.write(EEPROM_ADDR_MUSIC_MENU, musicMenuTrack);
  EEPROM.write(EEPROM_ADDR_MUSIC_GAME, musicGameTrack);

  for (byte i = 0; i < 3; i++) {
    EEPROM.put(highScoreAddressFor(i), highScores[i]);
  }
  EEPROM.put(EEPROM_ADDR_HIGHSCORE_FLAPPY, highScoreFlappy);

  EEPROM.commit();
}

void saveHighScore() {
  if (currentGame == 1) {
    if (score > highScoreFlappy) {
      highScoreFlappy = score;
      EEPROM.put(EEPROM_ADDR_HIGHSCORE_FLAPPY, highScoreFlappy);
      EEPROM.commit();
      isNewHighScore = true;
      oledDirty = true;
    }
    return;
  }

  if (difficultySetting > 2) return;

  if (score > highScores[difficultySetting]) {
    highScores[difficultySetting] = score;
    EEPROM.put(highScoreAddressFor(difficultySetting), highScores[difficultySetting]);
    EEPROM.commit();
    isNewHighScore = true;
    oledDirty = true;
  }
}

void spawnFood() {
  if (snakeLen >= 64) return;

  bool valid = false;
  int attempts = 0;

  while (!valid && attempts < 100) {
    attempts++;
    foodX = random(0, 8);
    foodY = random(0, 8);
    valid = true;

    if (isBonusFoodActive && foodX == bonusFoodX && foodY == bonusFoodY) {
      valid = false;
      continue;
    }

    for (byte i = 0; i < snakeLen; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        valid = false;
        break;
      }
    }
  }
}

void spawnBonusFood() {
  if (snakeLen >= 63) return;

  bool valid = false;
  int attempts = 0;

  while (!valid && attempts < 100) {
    attempts++;
    bonusFoodX = random(0, 8);
    bonusFoodY = random(0, 8);
    valid = true;

    if (bonusFoodX == foodX && bonusFoodY == foodY) {
      valid = false;
      continue;
    }

    for (byte i = 0; i < snakeLen; i++) {
      if (snakeX[i] == bonusFoodX && snakeY[i] == bonusFoodY) {
        valid = false;
        break;
      }
    }
  }

  if (valid) {
    isBonusFoodActive = true;
    bonusFoodTimer = millis();
  }
}

void resetGame() {
  currentGame = 0;
  snakeLen = 3;
  snakeX[0] = 3; snakeY[0] = 3;
  snakeX[1] = 2; snakeY[1] = 3;
  snakeX[2] = 1; snakeY[2] = 3;
  currentDir = 1;
  dirChangedThisTick = false;
  score = 0;
  foodEatenCount = 0;
  isBonusFoodActive = false;
  isGameOver = false;
  isWinner = false;
  isPaused = false;
  isDeathAnimating = false;
  isNewHighScore = false;
  pauseExitTriggered = false;
  pauseInputArmed = false;
  isPlayingGameOverJingle = false;

  if (difficultySetting == 0) {
    gameSpeed = 450;
    scoreMultiplier = 1;
  } else if (difficultySetting == 1) {
    gameSpeed = 300;
    scoreMultiplier = 2;
  } else {
    gameSpeed = 180;
    scoreMultiplier = 3;
  }

  spawnFood();
  gameStarted = true;
  gameStartTime = millis();
  lastMoveTime = millis();
  currentMusicNoteIndex = 0;
  currentNoteDuration = 0;
  oledDirty = true;
  soundStart();
}

void updateSnake() {
  // Allow one direction change before the next movement tick.
  dirChangedThisTick = false;

  byte newX = snakeX[0];
  byte newY = snakeY[0];

  if (currentDir == 0) newY--;
  else if (currentDir == 1) newX++;
  else if (currentDir == 2) newY++;
  else if (currentDir == 3) newX--;

  if (wallsEnabled) {
    if (newX > 7 || newY > 7) {
      triggerDeathAnimation();
      return;
    }
  } else {
    if (newX > 7) newX = (newX > 200) ? 7 : 0;
    if (newY > 7) newY = (newY > 200) ? 7 : 0;
  }

  for (byte i = 0; i < snakeLen; i++) {
    if (snakeX[i] == newX && snakeY[i] == newY) {
      triggerDeathAnimation();
      return;
    }
  }

  for (byte i = snakeLen; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  snakeX[0] = newX;
  snakeY[0] = newY;

  if (newX == foodX && newY == foodY) {
    score += (10 * scoreMultiplier);
    foodEatenCount++;
    oledDirty = true;
    soundEat();

    if (score > highScores[difficultySetting]) {
      isNewHighScore = true;
    }

    if (foodEatenCount % SPEED_UP_EVERY_FOODS == 0 && gameSpeed > MIN_GAME_SPEED_MS) {
      gameSpeed -= SPEED_UP_STEP_MS;
      if (gameSpeed < MIN_GAME_SPEED_MS) gameSpeed = MIN_GAME_SPEED_MS;
    }

    if (snakeLen < 64) {
      snakeLen++;

      if (snakeLen == 64) {
        isWinner = true;
        isGameOver = true;
        gameEndTime = millis();
        saveHighScore();
        soundWin();
        return;
      }
    } else {
      isWinner = true;
      isGameOver = true;
      gameEndTime = millis();
      saveHighScore();
      soundWin();
      return;
    }

    if (foodEatenCount % 4 == 0 && !isBonusFoodActive) {
      spawnBonusFood();
    }

    spawnFood();
  }

  if (isBonusFoodActive && newX == bonusFoodX && newY == bonusFoodY) {
    score += (30 * scoreMultiplier);
    isBonusFoodActive = false;
    oledDirty = true;
    soundBonus();

    if (score > highScores[difficultySetting]) {
      isNewHighScore = true;
    }
  }

  if (isBonusFoodActive && (millis() - bonusFoodTimer > BONUS_FOOD_DURATION_MS)) {
    isBonusFoodActive = false;
    oledDirty = true;
  }
}

void triggerDeathAnimation() {
  isDeathAnimating = true;
  deathAnimStep = snakeLen;
  lastDeathAnimTime = millis();
  oledDirty = true;
  noTone(BUZZER_PIN);
  soundGameOver();
}

void processDeathAnimation() {
  if (millis() - lastDeathAnimTime > 120) {
    lastDeathAnimTime = millis();
    if (deathAnimStep > 0) {
      deathAnimStep--;
    } else {
      isDeathAnimating = false;
      isGameOver = true;
      gameEndTime = millis();
      saveHighScore();
      oledDirty = true;
    }
  }
}
