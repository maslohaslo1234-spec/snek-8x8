// OLED.ino
// Rysowanie ekranu OLED (SSD1306 128x64) - ekran startowy, menu,
// HUD w trakcie gry, wygaszacz oraz nakladki pauzy/konca gry.

void updateOLED() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (isScreensaver) {
    display.setTextSize(2);
    display.setCursor(ssX, ssY);
    display.print(F("SNAKE"));

    display.setTextSize(1);
    display.setCursor(ssX - 4, ssY + 18);
    display.print(F("[ DEMO MODE ]"));

    if (blinkState) {
      display.setCursor(16, 52);
      display.print(F("Nacisnij przycisk"));
    }

  } else if (!gameStarted && menuState == 0) {
    // Ekran przed wejsciem do glownego menu
    display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(34, 8);
    display.print(F("SNAKE"));

    display.setTextSize(1);
    display.setCursor(20, 28);
    display.print(F("REKORD: "));
    display.print(highScore);

    if (blinkState) {
      display.fillRoundRect(14, 42, 100, 15, 3, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(22, 46);
      display.print(F("NACISNIJ START"));
    }
  } else if (menuState > 0) {
    // Ekran Menu Glownego oraz Podmenu (Zasady i Muzyka)
    display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);
    display.setTextSize(1);

    if (menuState == 1) {
      // Glowne Menu
      display.setCursor(30, 4);
      display.print(F("MENU GLOWNE"));
      display.drawLine(15, 14, 113, 14, SSD1306_WHITE);

      display.setCursor(10, 18);
      display.print(mainMenuCursor == 0 ? F("> ") : F("  "));
      display.print(F("GRAJ!"));

      display.setCursor(10, 30);
      display.print(mainMenuCursor == 1 ? F("> ") : F("  "));
      display.print(F("ZASADY GRY"));

      display.setCursor(10, 42);
      display.print(mainMenuCursor == 2 ? F("> ") : F("  "));
      display.print(F("MUZYKA"));
    } 
    else if (menuState == 2) {
      // Podmenu: Zasady (Poziom + Sciany)
      display.setCursor(45, 4);
      display.print(F("ZASADY"));
      display.drawLine(15, 14, 113, 14, SSD1306_WHITE);

      display.setCursor(10, 18);
      display.print(zasadyCursor == 0 ? F("> ") : F("  "));
      display.print(F("POZIOM: "));
      if (difficultySetting == 0) display.print(F("LATWY"));
      else if (difficultySetting == 1) display.print(F("SREDNI"));
      else display.print(F("TRUDNY"));

      display.setCursor(10, 30);
      display.print(zasadyCursor == 1 ? F("> ") : F("  "));
      display.print(F("SCIANY: "));
      display.print(wallsEnabled ? F("TAK") : F("NIE (WRAP)"));

      display.setCursor(10, 42);
      display.print(zasadyCursor == 2 ? F("> ") : F("  "));
      display.print(F("[ WSTECZ ]"));
    } 
    else if (menuState == 3) {
      // Podmenu: Muzyka (W menu + W grze) - krotkie nazwymieszace sie na ekranie
      display.setCursor(45, 4);
      display.print(F("MUZYKA"));
      display.drawLine(15, 14, 113, 14, SSD1306_WHITE);

      const __FlashStringHelper* trackNames[] = { F("MEGA"), F("TETR"), F("MARIO"), F("OFF") };

      display.setCursor(10, 18);
      display.print(muzykaCursor == 0 ? F("> ") : F("  "));
      display.print(F("W MENU: "));
      display.print(trackNames[musicMenuTrack]);

      display.setCursor(10, 30);
      display.print(muzykaCursor == 1 ? F("> ") : F("  "));
      display.print(F("W GRZE: "));
      display.print(trackNames[musicGameTrack]);

      display.setCursor(10, 42);
      display.print(muzykaCursor == 2 ? F("> ") : F("  "));
      display.print(F("[ WSTECZ ]"));
    }

  } else {
    // Ekran Glownej Gry (HUD)
    display.drawLine(0, 38, 128, 38, SSD1306_WHITE);

    // Wynik i Rekord
    display.setTextSize(1);
    display.setCursor(5, 3);
    display.print(F("WYNIK"));
    display.setTextSize(2);
    display.setCursor(5, 14);
    display.print(score);

    display.setTextSize(1);
    display.setCursor(75, 3);
    display.print(F("REKORD"));
    display.setTextSize(2);
    display.setCursor(75, 14);
    display.print(highScore);

    display.fillRect(0, 39, 128, 15, SSD1306_BLACK);

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
      display.print(F("DLUGOSC: "));

      if (snakeLen < 10) {
        display.print('0');
      }

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
        display.setCursor(boxX + 22, boxY + 12);
        display.print(F("PAUZA"));
      } else if (isGameOver) {
        display.setTextSize(1);

        if (isWinner) {
          display.setCursor(boxX + 8, boxY + 6);
          display.print(F("WYGRANA! 64/64"));
        } else {
          display.setCursor(boxX + 18, boxY + 6);
          display.print(F("KONIEC GRY!"));
        }

        unsigned long totalSec = (gameEndTime - gameStartTime) / 1000;
        display.setCursor(boxX + 10, boxY + 18);
        display.print(F("CZAS: "));
        display.print(totalSec);
        display.print(F(" sek."));

        if (isNewHighScore) {
          display.setCursor(boxX + 10, boxY + 28);
          display.print(F("* NOWY REKORD! *"));
        }
      }
    }
  }

  display.display();
}