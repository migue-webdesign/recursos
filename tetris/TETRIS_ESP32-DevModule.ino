#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R1, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/ 21);

// Pines ESP32
const int BTN_LEFT = 14;
const int BTN_RIGHT = 27;
const int BTN_ROTATE = 26;
const int BTN_DOWN = 25;
const int buzzerPin = 4;

// Juego
const byte rows = 18;
const byte cols = 10;
const int cellSize = 6;
const int topMargin = 15;
byte board[rows][cols];
unsigned long score = 0;
int fallDelay = 500;
int blockX, blockY, rotation, blockType, nextBlockType;
unsigned long lastFallTime = 0;
unsigned long lastInputTime = 0;
const int debounceDelay = 120;
bool gameStarted = false;

const byte tetrominoes[7][4][4][4] = {
  {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}},
  {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
  {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},{{1,1,1,0},{0,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
  {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},{{1,1,1,0},{1,0,0,0},{0,0,0,0},{0,0,0,0}},{{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}},
  {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},{{1,1,1,0},{0,0,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}},
  {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}},
  {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}}
};

void setup() {
  u8g2.begin();
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_ROTATE, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  nextBlockType = random(0, 7);
}

void loop() {
  if (!gameStarted) {
    drawStartScreen();
    if (digitalRead(BTN_ROTATE) == LOW) {
      playStartSound();
      gameStarted = true;
      spawnBlock();
      delay(200);
    }
  } else {
    handleButtons();
    if (millis() - lastFallTime > fallDelay) {
      if (!moveBlock(0, 1)) {
        mergeBlock();
        clearLines();
        spawnBlock();
      }
      lastFallTime = millis();
    }
    drawGame();
  }
}

void drawStartScreen() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(5, 20, "ESP32 TETRIS");
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(5, 50, "Presiona ROTAR para jugar");
  } while (u8g2.nextPage());
}

void drawGame() {
  u8g2.firstPage();
  do {
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        if (board[i][j])
          u8g2.drawBox(j * cellSize + 1, topMargin + i * cellSize + 1, 5, 5);

    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        if (tetrominoes[blockType][rotation][i][j])
          u8g2.drawBox((blockX + j) * cellSize + 1, topMargin + (blockY + i) * cellSize + 1, 5, 5);

    u8g2.setCursor(38, 6);
    u8g2.print("SCORE:");
    u8g2.setCursor(90, 6);
    u8g2.print(score);
  } while (u8g2.nextPage());
}

void handleButtons() {
  if (millis() - lastInputTime > debounceDelay) {
    if (digitalRead(BTN_LEFT) == LOW) {
      if (moveBlock(-1, 0)) playMoveSound();
      lastInputTime = millis();
    } else if (digitalRead(BTN_RIGHT) == LOW) {
      if (moveBlock(1, 0)) playMoveSound();
      lastInputTime = millis();
    } else if (digitalRead(BTN_ROTATE) == LOW) {
      rotateBlock();
      playRotateSound();
      lastInputTime = millis();
    } else if (digitalRead(BTN_DOWN) == LOW) {
      moveBlock(0, 1);
      lastInputTime = millis();
    }
  }
}

bool checkCollision(int x, int y, int r) {
  for (byte i = 0; i < 4; i++)
    for (byte j = 0; j < 4; j++)
      if (tetrominoes[blockType][r][i][j])
        if (x + j < 0 || x + j >= cols || y + i >= rows || board[y + i][x + j])
          return true;
  return false;
}

bool moveBlock(int dx, int dy) {
  if (!checkCollision(blockX + dx, blockY + dy, rotation)) {
    blockX += dx;
    blockY += dy;
    return true;
  }
  return false;
}

void rotateBlock() {
  int newRot = (rotation + 1) % 4;
  if (!checkCollision(blockX, blockY, newRot))
    rotation = newRot;
}

void mergeBlock() {
  for (byte i = 0; i < 4; i++)
    for (byte j = 0; j < 4; j++)
      if (tetrominoes[blockType][rotation][i][j])
        board[blockY + i][blockX + j] = 1;
}

void spawnBlock() {
  blockX = 3;
  blockY = 0;
  rotation = 0;
  blockType = nextBlockType;
  nextBlockType = random(0, 7);
  if (checkCollision(blockX, blockY, rotation)) {
    playGameOverSound();
    memset(board, 0, sizeof(board));
    score = 0;
  }
}

void clearLines() {
  int linesCleared = 0;
  for (int i = rows - 1; i >= 0; i--) {
    bool full = true;
    for (int j = 0; j < cols; j++)
      if (!board[i][j]) full = false;
    if (full) {
      linesCleared++;
      playLineClearSound();
      for (int k = i; k > 0; k--)
        memcpy(board[k], board[k - 1], cols);
      memset(board[0], 0, cols);
      i++;
    }
  }

  switch (linesCleared) {
    case 1: score += 40; break;
    case 2: score += 100; break;
    case 3: score += 300; break;
    case 4: score += 1200; break;
  }
}

// 🎵 Sonidos usando tone()
void playTone(int freq, int duration) {
  tone(buzzerPin, freq, duration);
  delay(duration);
  noTone(buzzerPin);
}

void playMoveSound()      { playTone(300, 50); }
void playRotateSound()    { playTone(500, 60); }
void playLineClearSound() {
  playTone(440, 100); delay(120);
  playTone(660, 100); delay(120);
  playTone(880, 150); delay(150);
}
void playGameOverSound()  {
  playTone(400, 150); delay(200);
  playTone(300, 300); delay(300);
}
void playStartSound()     {
  playTone(440, 100); delay(120);
  playTone(660, 100); delay(120);
  playTone(880, 150); delay(150);
}