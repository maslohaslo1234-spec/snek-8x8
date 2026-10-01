// Game.ino
// Logika gry: waz, jedzenie, wynik, rekord, animacja smierci.

void loadHighScore() {
  EEPROM.get(HIGH_SCORE_ADDRESS, highScore);
  if (highScore < 0 || highScore > 9999) {
    highScore = 0;
  }
}

void saveHighScore() {
  if (score > highScore) {
    highScore = score;
    EEPROM.put(HIGH_SCORE_ADDRESS, highScore);
    isNewHighScore = true;
    oledDirty = true;
  }
}

void spawnFood() {
  bool valid = false;
  while (!valid) {
    foodX = random(0, 8);
    foodY = random(0, 8);
    valid = true;

    // POPRAWKA: zwykle jedzenie nie moze wyladowac na polu, na ktorym
    // aktualnie stoi aktywne jedzenie bonusowe (wczesniej sie moglo
    // nalozyc - wizualnie niegrozne, ale nieladne i niepotrzebne).
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
  bool valid = false;
  while (!valid) {
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
  isBonusFoodActive = true;
  bonusFoodTimer = millis();
}

void resetGame() {
  snakeLen = 3;
  snakeX[0] = 3; snakeY[0] = 3;
  snakeX[1] = 2; snakeY[1] = 3;
  snakeX[2] = 1; snakeY[2] = 3;
  currentDir = 1;
  score = 0;
  foodEatenCount = 0;
  isBonusFoodActive = false;
  isGameOver = false;
  isWinner = false;
  isPaused = false;
  isDeathAnimating = false;
  isNewHighScore = false;

  // Ustawienie predkosci zaleznie od trudnosci
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
  byte newX = snakeX[0];
  byte newY = snakeY[0];

  if (currentDir == 0) newY--;
  else if (currentDir == 1) newX++;
  else if (currentDir == 2) newY++;
  else if (currentDir == 3) newX--;

  // Logika Scian
  if (wallsEnabled) {
    if (newX > 7 || newY > 7) {
      // byte jest bez znaku, wiec zjechanie ponizej zera daje wartosc 255
      triggerDeathAnimation();
      return;
    }
  } else {
    // Wrap-around (przechodzenie przez krawedzie)
    if (newX > 7) newX = (newX > 200) ? 7 : 0;
    if (newY > 7) newY = (newY > 200) ? 7 : 0;
  }

  // Kolizja z wlasnym cialem
  for (byte i = 0; i < snakeLen; i++) {
    if (snakeX[i] == newX && snakeY[i] == newY) {
      triggerDeathAnimation();
      return;
    }
  }

  // Przesuniecie ciala
  for (byte i = snakeLen; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  snakeX[0] = newX;
  snakeY[0] = newY;

  // Zjedzenie zwyklego jedzenia
  if (newX == foodX && newY == foodY) {
    score += (10 * scoreMultiplier);
    foodEatenCount++;
    oledDirty = true;
    soundEat();

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

    // Co 4 zjedzenia pojawia sie bonusowe jedzenie
    if (foodEatenCount % 4 == 0 && !isBonusFoodActive) {
      spawnBonusFood();
    }

    spawnFood();
  }

  // Zjedzenie bonusowego jedzenia
  if (isBonusFoodActive && newX == bonusFoodX && newY == bonusFoodY) {
    score += (30 * scoreMultiplier);
    isBonusFoodActive = false;
    oledDirty = true;
    soundBonus();
  }

  // Wygasanie bonusowego jedzenia
  if (isBonusFoodActive && (millis() - bonusFoodTimer > BONUS_FOOD_DURATION_MS)) {
    isBonusFoodActive = false;
    oledDirty = true;
  }

  saveHighScore();
}

void triggerDeathAnimation() {
  isDeathAnimating = true;
  deathAnimStep = snakeLen;
  lastDeathAnimTime = millis();
  oledDirty = true;
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
