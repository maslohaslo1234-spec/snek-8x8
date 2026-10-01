void exitPausedGameToMenu() {
  saveHighScore();
  gameStarted = false;
  isPaused = false;
  isDeathAnimating = false;
  isGameOver = false;
  isWinner = false;
  isBonusFoodActive = false;
  pauseExitTriggered = true;
  pauseInputArmed = false;
  menuState = 1;
  mainMenuCursor = 0;
  noTone(BUZZER_PIN);
  oledDirty = true;
  lastButtonTime = millis();
}

void handleInput() {
  bool currentLeft = digitalRead(BTN_LEFT_PIN);
  bool currentRight = digitalRead(BTN_RIGHT_PIN);
  bool currentAction = digitalRead(BTN_ACTION_PIN);

  if (currentLeft == LOW || currentRight == LOW || currentAction == LOW) {
    lastActivityTime = millis();
    if (isScreensaver) {
      isScreensaver = false;
      oledDirty = true;
      lastLeftState = currentLeft;
      lastRightState = currentRight;
      lastActionState = currentAction;
      lastButtonTime = millis();
      return;
    }
  }

  // Resume on a short press; hold ACTION for one second to return to the menu.
  if (gameStarted && isPaused && !isGameOver && !isDeathAnimating) {
    if (!pauseInputArmed) {
      if (currentAction == HIGH) pauseInputArmed = true;
    } else if (currentAction == LOW) {
      if (lastActionState == HIGH) {
        pauseActionPressTime = millis();
        pauseExitTriggered = false;
      } else if (!pauseExitTriggered && (millis() - pauseActionPressTime >= HOLD_EXIT_MS)) {
        exitPausedGameToMenu();
      }
    } else if (lastActionState == LOW && !pauseExitTriggered) {
      if (millis() - pauseActionPressTime < HOLD_EXIT_MS) {
        isPaused = false;
        pauseInputArmed = false;
        soundResume();
        oledDirty = true;
        lastButtonTime = millis();
      }
      pauseExitTriggered = false;
    }

    lastLeftState = currentLeft;
    lastRightState = currentRight;
    lastActionState = currentAction;
    return;
  }

  if (millis() - lastButtonTime > BUTTON_DEBOUNCE_MS) {
    if (!gameStarted && menuState == 0) {
      if (currentAction == LOW && lastActionState == HIGH) {
        menuState = 1;
        mainMenuCursor = 0;
        oledDirty = true;
        lastButtonTime = millis();
      }
    }
    else if (menuState > 0 && !gameStarted) {
      if (currentLeft == LOW && lastLeftState == HIGH) {
        if (menuState == 1) mainMenuCursor = (mainMenuCursor + 3) % 4;
        else if (menuState == 2) settingsCursor = (settingsCursor + 3) % 4;
        else if (menuState == 3) muzykaCursor = (muzykaCursor + 2) % 3;
        oledDirty = true;
        lastButtonTime = millis();
      }
      if (currentRight == LOW && lastRightState == HIGH) {
        if (menuState == 1) mainMenuCursor = (mainMenuCursor + 1) % 4;
        else if (menuState == 2) settingsCursor = (settingsCursor + 1) % 4;
        else if (menuState == 3) muzykaCursor = (muzykaCursor + 1) % 3;
        oledDirty = true;
        lastButtonTime = millis();
      }
      if (currentAction == LOW && lastActionState == HIGH) {
        if (menuState == 1) {
          if (mainMenuCursor == 0) { menuState = 0; resetGame(); }
          else if (mainMenuCursor == 1) { menuState = 0; resetFlappy(); }
          else if (mainMenuCursor == 2) { menuState = 2; settingsCursor = 0; }
          else if (mainMenuCursor == 3) { menuState = 3; muzykaCursor = 0; }
        }
        else if (menuState == 2) {
          if (settingsCursor == 0) {
            difficultySetting = (difficultySetting + 1) % 3;
            saveSettings();
          } else if (settingsCursor == 1) {
            wallsEnabled = !wallsEnabled;
            saveSettings();
          } else if (settingsCursor == 2) {
            languageSetting = (languageSetting + 1) % 2;
            saveSettings();
          } else if (settingsCursor == 3) {
            menuState = 1;
          }
        }
        else if (menuState == 3) {
          if (muzykaCursor == 0) {
            musicMenuTrack = (musicMenuTrack + 1) % 4;
            currentPlayingTrack = 255;
            saveSettings();
          } else if (muzykaCursor == 1) {
            musicGameTrack = (musicGameTrack + 1) % 4;
            saveSettings();
          } else if (muzykaCursor == 2) {
            menuState = 1;
          }
        }
        oledDirty = true;
        lastButtonTime = millis();
      }
    }
    else if (isGameOver) {
      if (currentAction == LOW && lastActionState == HIGH) {
        isPlayingGameOverJingle = false;
        noTone(BUZZER_PIN);
        gameStarted = false;
        menuState = 1;
        oledDirty = true;
        lastButtonTime = millis();
      }
    }
    else if (gameStarted) {
      if (currentGame == 1) {
        if (currentAction == LOW && lastActionState == HIGH && !isDeathAnimating) {
          fbFlapQueued = true;
          soundFlap();
          lastButtonTime = millis();
        }
        if (currentLeft == LOW && lastLeftState == HIGH) {
          isPaused = true;
          pauseExitTriggered = false;
          pauseInputArmed = false;
          soundPause();
          oledDirty = true;
          lastButtonTime = millis();
        }
      } else {
        if (currentAction == LOW && lastActionState == HIGH) {
          isPaused = true;
          pauseExitTriggered = false;
          pauseInputArmed = false;
          soundPause();
          oledDirty = true;
          lastButtonTime = millis();
        }

        if (!isDeathAnimating && !dirChangedThisTick) {
          if (currentLeft == LOW && lastLeftState == HIGH) {
            currentDir = (currentDir + 3) % 4;
            dirChangedThisTick = true;
            lastButtonTime = millis();
          }
          if (currentRight == LOW && lastRightState == HIGH) {
            currentDir = (currentDir + 1) % 4;
            dirChangedThisTick = true;
            lastButtonTime = millis();
          }
        }
      }
    }
  }

  lastLeftState = currentLeft;
  lastRightState = currentRight;
  lastActionState = currentAction;
}
