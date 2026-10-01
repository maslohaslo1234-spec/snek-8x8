// Input.ino
// Obsluga przyciskow: nawigacja w menu, sterowanie wezem, pauza.

void handleInput() {
  bool currentLeft = digitalRead(BTN_LEFT_PIN);
  bool currentRight = digitalRead(BTN_RIGHT_PIN);
  bool currentAction = digitalRead(BTN_ACTION_PIN);

  if (currentLeft == LOW || currentRight == LOW || currentAction == LOW) {
    lastActivityTime = millis();
    if (isScreensaver) {
      isScreensaver = false;
      oledDirty = true;
      return;
    }
  }

  if (millis() - lastButtonTime > BUTTON_DEBOUNCE_MS) {
    // 1. Ekran Powitalny -> Przejscie do Menu
    if (!gameStarted && menuState == 0) {
      if (currentAction == LOW && lastActionState == HIGH) {
        menuState = 1; // Wejscie do glownego menu
        mainMenuCursor = 0;
        oledDirty = true;
        lastButtonTime = millis();
      }
    }
    // 2. Obsluga Menu (Glowne i Podmenu)
    else if (menuState > 0 && !gameStarted) {
      // Nawigacja GORA (Lewo)
      if (currentLeft == LOW && lastLeftState == HIGH) {
        if (menuState == 1) mainMenuCursor = (mainMenuCursor + 2) % 3;
        else if (menuState == 2) zasadyCursor = (zasadyCursor + 2) % 3;
        else if (menuState == 3) muzykaCursor = (muzykaCursor + 2) % 3;
        oledDirty = true;
        lastButtonTime = millis();
      }
      // Nawigacja DOL (Prawo)
      if (currentRight == LOW && lastRightState == HIGH) {
        if (menuState == 1) mainMenuCursor = (mainMenuCursor + 1) % 3;
        else if (menuState == 2) zasadyCursor = (zasadyCursor + 1) % 3;
        else if (menuState == 3) muzykaCursor = (muzykaCursor + 1) % 3;
        oledDirty = true;
        lastButtonTime = millis();
      }
      // Akcja (Zatwierdz)
      if (currentAction == LOW && lastActionState == HIGH) {
        if (menuState == 1) {
          if (mainMenuCursor == 0) { menuState = 0; resetGame(); }
          else if (mainMenuCursor == 1) { menuState = 2; zasadyCursor = 0; }
          else if (mainMenuCursor == 2) { menuState = 3; muzykaCursor = 0; }
        } 
        else if (menuState == 2) {
          if (zasadyCursor == 0) difficultySetting = (difficultySetting + 1) % 3;
          else if (zasadyCursor == 1) wallsEnabled = !wallsEnabled;
          else if (zasadyCursor == 2) menuState = 1; // Wstecz
        } 
        else if (menuState == 3) {
          if (muzykaCursor == 0) musicMenuTrack = (musicMenuTrack + 1) % 4;
          else if (muzykaCursor == 1) musicGameTrack = (musicGameTrack + 1) % 4;
          else if (muzykaCursor == 2) menuState = 1; // Wstecz
        }
        oledDirty = true;
        lastButtonTime = millis();
      }
    }
    // 3. Koniec Gry -> Powrot do Menu
    else if (isGameOver) {
      if (currentAction == LOW && lastActionState == HIGH) {
        gameStarted = false;
        menuState = 1;
        oledDirty = true;
        lastButtonTime = millis();
      }
    }
    // 4. Rozgrywka Gry
    else if (gameStarted) {
      if (currentAction == LOW && lastActionState == HIGH) {
        isPaused = !isPaused;
        if (isPaused) soundPause();
        else soundResume();
        oledDirty = true;
        lastButtonTime = millis();
      }

      if (!isPaused && !isDeathAnimating) {
        if (currentLeft == LOW && lastLeftState == HIGH) {
          currentDir = (currentDir + 3) % 4; // Skret w lewo
          lastButtonTime = millis();
        }
        if (currentRight == LOW && lastRightState == HIGH) {
          currentDir = (currentDir + 1) % 4; // Skret w prawo
          lastButtonTime = millis();
        }
      }
    }
  }

  lastLeftState = currentLeft;
  lastRightState = currentRight;
  lastActionState = currentAction;
}