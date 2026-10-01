const __FlashStringHelper* localizedText(
  const __FlashStringHelper* english,
  const __FlashStringHelper* polish
) {
  return languageSetting == 0 ? english : polish;
}

void updateOLED() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (isScreensaver) {
    for (int i = SS_TRAIL_LEN - 1; i >= 0; i--) {
      byte r = (i == 0) ? 4 : (i < 4 ? 3 : 2);
      display.fillCircle(ssTrailX[i], ssTrailY[i], r, SSD1306_WHITE);
    }
    display.fillCircle(ssTrailX[0] + ssDirX * 2, ssTrailY[0] - ssDirY, 1, SSD1306_BLACK);
    display.fillCircle(ssFoodX, ssFoodY, 2, SSD1306_WHITE);

  } else if (!gameStarted && menuState == 0) {
    display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(22, 8);
    display.print(F("ARCADE"));

    display.setTextSize(1);
    display.setCursor(18, 30);
    display.print(F("Snake + Flappy"));

    if (blinkState) {
      display.fillRoundRect(14, 42, 100, 15, 3, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(22, 46);
      display.print(localizedText(F("PRESS TO START"), F("NACISNIJ START")));
    }
  } else if (menuState > 0 && !gameStarted) {
    display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);
    display.setTextSize(1);

    if (menuState == 1) {
      display.setCursor(languageSetting == 0 ? 37 : 30, 3);
      display.print(localizedText(F("MAIN MENU"), F("MENU GLOWNE")));
      display.drawLine(15, 12, 113, 12, SSD1306_WHITE);

      display.setCursor(8, 16);
      display.print(mainMenuCursor == 0 ? F("> ") : F("  "));
      display.print(F("SNAKE"));

      display.setCursor(8, 28);
      display.print(mainMenuCursor == 1 ? F("> ") : F("  "));
      display.print(F("FLAPPY BIRD"));

      display.setCursor(8, 40);
      display.print(mainMenuCursor == 2 ? F("> ") : F("  "));
      display.print(localizedText(F("SETTINGS"), F("USTAWIENIA")));

      display.setCursor(8, 52);
      display.print(mainMenuCursor == 3 ? F("> ") : F("  "));
      display.print(localizedText(F("MUSIC"), F("MUZYKA")));
    }
    else if (menuState == 2) {
      display.setCursor(languageSetting == 0 ? 40 : 34, 3);
      display.print(localizedText(F("SETTINGS"), F("USTAWIENIA")));
      display.drawLine(15, 12, 113, 12, SSD1306_WHITE);

      display.setCursor(8, 16);
      display.print(settingsCursor == 0 ? F("> ") : F("  "));
      display.print(localizedText(F("LEVEL: "), F("POZIOM: ")));
      if (difficultySetting == 0) display.print(localizedText(F("EASY"), F("LATWY")));
      else if (difficultySetting == 1) display.print(localizedText(F("MEDIUM"), F("SREDNI")));
      else display.print(localizedText(F("HARD"), F("TRUDNY")));

      display.setCursor(8, 28);
      display.print(settingsCursor == 1 ? F("> ") : F("  "));
      display.print(localizedText(F("WALLS: "), F("SCIANY: ")));
      display.print(wallsEnabled ? localizedText(F("ON"), F("TAK")) : localizedText(F("OFF"), F("NIE")));

      display.setCursor(8, 40);
      display.print(settingsCursor == 2 ? F("> ") : F("  "));
      display.print(localizedText(F("LANGUAGE: "), F("JEZYK: ")));
      display.print(languageSetting == 0 ? F("ENGLISH") : F("POLSKI"));

      display.setCursor(8, 52);
      display.print(settingsCursor == 3 ? F("> ") : F("  "));
      display.print(localizedText(F("[ BACK ]"), F("[ WSTECZ ]")));
    }
    else if (menuState == 3) {
      display.setCursor(languageSetting == 0 ? 49 : 45, 4);
      display.print(localizedText(F("MUSIC"), F("MUZYKA")));
      display.drawLine(15, 14, 113, 14, SSD1306_WHITE);

      const __FlashStringHelper* trackNames[] = {
        F("MEGA"), F("TETR"), F("MARIO"), localizedText(F("OFF"), F("WYLACZ"))
      };

      display.setCursor(10, 18);
      display.print(muzykaCursor == 0 ? F("> ") : F("  "));
      display.print(localizedText(F("IN MENU: "), F("W MENU: ")));
      display.print(trackNames[musicMenuTrack]);

      display.setCursor(10, 30);
      display.print(muzykaCursor == 1 ? F("> ") : F("  "));
      display.print(localizedText(F("IN GAME: "), F("W GRZE: ")));
      display.print(trackNames[musicGameTrack]);

      display.setCursor(10, 42);
      display.print(muzykaCursor == 2 ? F("> ") : F("  "));
      display.print(localizedText(F("[ BACK ]"), F("[ WSTECZ ]")));
    }

  } else if (gameStarted && currentGame == 1) {
    drawFlappyOLED();

  } else {
    display.drawLine(0, 38, 128, 38, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(5, 3);
    display.print(localizedText(F("SCORE"), F("WYNIK")));
    display.setTextSize(2);
    display.setCursor(5, 14);
    display.print(score);

    display.setTextSize(1);
    display.setCursor(75, 3);
    display.print(localizedText(F("BEST"), F("REKORD")));
    display.setTextSize(2);
    display.setCursor(75, 14);
    display.print(currentHighScore());

    display.fillRect(0, 39, 128, 15, SSD1306_BLACK);
    display.setTextSize(1);

    if (isBonusFoodActive) {
      unsigned long elapsed = millis() - bonusFoodTimer;
      unsigned long remainingMs = 0;
      if (elapsed < BONUS_FOOD_DURATION_MS) {
        remainingMs = BONUS_FOOD_DURATION_MS - elapsed;
      }
      int remainingSec = (remainingMs + 999) / 1000;
      display.setCursor(5, 42);
      display.print(F("BONUS: "));
      display.print(remainingSec);
      display.print(F("s!"));
    } else {
      display.setCursor(5, 42);
      display.print(localizedText(F("LENGTH: "), F("DLUGOSC: ")));
      if (snakeLen < 10) display.print('0');
      display.print(snakeLen);
      display.print(F("/64"));
    }

    int progressWidth = map(snakeLen, 3, 64, 0, 116);
    progressWidth = constrain(progressWidth, 0, 116);
    display.drawRect(4, 54, 120, 7, SSD1306_WHITE);
    display.fillRect(6, 56, progressWidth, 3, SSD1306_WHITE);

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
        display.print(localizedText(F("PAUSED"), F("PAUZA")));
        display.setTextSize(1);
        display.setCursor(boxX + 8, boxY + 28);
        display.print(localizedText(F("HOLD = MENU"), F("PRZYTRZ = MENU")));
      } else if (isGameOver) {
        display.setTextSize(1);
        if (isWinner) {
          display.setCursor(boxX + 8, boxY + 6);
          display.print(localizedText(F("YOU WIN! 64/64"), F("WYGRANA! 64/64")));
        } else {
          display.setCursor(languageSetting == 0 ? boxX + 22 : boxX + 18, boxY + 6);
          display.print(localizedText(F("GAME OVER!"), F("KONIEC GRY!")));
        }
        unsigned long totalSec = (gameEndTime - gameStartTime) / 1000;
        display.setCursor(boxX + 10, boxY + 18);
        display.print(localizedText(F("TIME: "), F("CZAS: ")));
        display.print(totalSec);
        display.print(localizedText(F(" sec."), F(" sek.")));
        if (isNewHighScore) {
          display.setCursor(boxX + 10, boxY + 28);
          display.print(localizedText(F("* NEW RECORD! *"), F("* NOWY REKORD! *")));
        }
      }
    }
  }

  display.display();
}
