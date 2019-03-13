#include "Player.cpp"
#include "Constants.h"
#include "Level.cpp"
#include "Tile.cpp"
#include "Item.cpp"
#include "pitches.h"


Arduboy2 arduboy;
Level level;
Level *pLevel = &level;
Player player1(pLevel);
int gameState = 0;
bool buttonA = false;
bool buttonB = false;
bool buttonLeft = false;
bool buttonRight = false;
// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_C3, NOTE_DS4, NOTE_D4, NOTE_DS4, NOTE_D4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 4, 4, 4, 4
};
int melodyCount = 0;
int noteNumber = 0;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.clear();
  player1.Reset();
  player1.EnterDungeon();
  Serial.begin(9600);
}

void loop() {
  if (!arduboy.nextFrame()) // Check time for next frame.
    return;
    
  switch (gameState) {
    case 0: // Title screen.
      level.winScreen = false;
      DrawTitleScreen();
      if (!buttonA)
      PlayTune();
      if (arduboy.pressed(A_BUTTON) and !buttonA) {
        gameState = 1;
        buttonA = true;
      }
      break;
    case 1: // Gameplay.
      level.winScreen = false;
      if (arduboy.pressed(RIGHT_BUTTON) and player1.velocityX < player1.velocityXMax and !level.inventory) {
        player1.velocityX += player1.velocityStep;
        player1.directionX = 1;
      }
      if (arduboy.pressed(LEFT_BUTTON) and player1.velocityX > -player1.velocityXMax and !level.inventory) {
        player1.velocityX += -player1.velocityStep;
        player1.directionX = -1;
      }

      // Jump:
      if (arduboy.pressed(A_BUTTON) and !buttonA and player1.velocityY == 0 and !level.inventory) {
        player1.velocityY = -3 * 214;
        buttonA = true;
      }

      UpdateGameObjects();
      
      DrawGameObjects();
      
      // Show/Hide inventory:
      if (arduboy.pressed(B_BUTTON) and !buttonB) {
        level.inventory = !level.inventory;
        buttonB = true;
      }
      if (player1.failed)
      {
        gameState = 2;
      }
        
      if (arduboy.pressed(A_BUTTON) and arduboy.pressed(B_BUTTON)) {
        level.Reset();
        player1.GameReset();
        gameState = 0;
        player1.failed = 0;
      }
      if (player1.won)
        gameState = 3;
      break;

    case 2: // Game Over screen
      DrawGameOverScreen();
      if ((arduboy.pressed(A_BUTTON) and !buttonA) or (arduboy.pressed(B_BUTTON) and !buttonB)) {
        level.Reset();
        player1.GameReset();
        gameState = 0;
        player1.won = 0;
        player1.failed = 0;
        buttonA = true;
        buttonB = true;
      }
    break;

    case 3: // Win screen
      DrawWinScreen();
      if ((arduboy.pressed(A_BUTTON) and !buttonA) or (arduboy.pressed(B_BUTTON) and !buttonB)) {
        level.Reset();
        player1.GameReset();
        gameState = 0;
        player1.won = 0;
        buttonA = true;
        buttonB = true;
      }
    break;
  }
  
  if (arduboy.notPressed(A_BUTTON))
    buttonA = false;
  if (arduboy.notPressed(B_BUTTON))
    buttonB = false;

  Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
  arduboy.display();
}

void UpdateGameObjects() {
  level.Update();
  player1.Update();   // Update player.
}

void DrawGameObjects() {
  arduboy.clear();    // Clear the screen.

  DrawHUD();

  level.Draw();

  if (!level.inventory)
    player1.Draw();     // Draw player.
  
  arduboy.display();  // Copy bitmap from frame buffer to display.
}

void DrawHUD() {
  if (level.inventory) {
    arduboy.setCursor(46, 0);
    arduboy.print("Paused");
    arduboy.setCursor(12, 8);
    arduboy.print(" = ");
    arduboy.setCursor(26, 8);
    arduboy.print(player1.keysHeld);
    arduboy.setCursor(8, 20);
    arduboy.print("Rooms visited = ");
    arduboy.setCursor(104, 20);
    arduboy.print(player1.pathCount);
    
    arduboy.setCursor(10, 40);
    arduboy.print("Press both buttons");
    arduboy.setCursor(39 , 48);
    arduboy.print("to reset");

  }
  else {
    arduboy.setCursor(40, 0);
    //arduboy.print("Level ");
    //arduboy.print(level.roomNumber);
  }
}

void DrawWinScreen()
{
  arduboy.clear();    // Clear the screen.
  level.winScreen = true;
  level.Draw();
  arduboy.setCursor(39, 0);
  arduboy.print("Game Over");
  arduboy.setCursor(18, 10);
  arduboy.print("You escaped the");
  arduboy.setCursor(35, 18);
  arduboy.print(" dungeon!");
  arduboy.setCursor(8, 34);
  arduboy.print("Rooms visited = ");
  arduboy.setCursor(104, 34);
  arduboy.print(player1.pathCount);
  
  arduboy.setCursor(28, 48);
  arduboy.print("Press button");
  arduboy.setCursor(39, 56);
  arduboy.print("to reset");
  arduboy.display();  // Copy bitmap from frame buffer to display.
}

void DrawGameOverScreen()
{
  arduboy.clear();    // Clear the screen.
  level.winScreen = true;
  level.Draw();
  arduboy.setCursor(39, 0);
  arduboy.print("Game Over");
  arduboy.setCursor(30, 16);
  arduboy.print("You failed!");
  arduboy.setCursor(28, 40);
  arduboy.print("Press button");
  arduboy.setCursor(39, 48);
  arduboy.print("to reset");
  arduboy.display();  // Copy bitmap from frame buffer to display.
}

void DrawTitleScreen() {
  Tile titleScreenTile = Tile(titleScreen);
  arduboy.clear();    // Clear the screen.
  arduboy.setCursor(0, 56);
  arduboy.print("LatheSoft");
  titleScreenTile.Draw(0, 0, 0);
  arduboy.display();  // Copy bitmap from frame buffer to display.
}

void PlayTune() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 6; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 500 / noteDurations[thisNote];
    tone(5, melody[thisNote], noteDuration);
    if (arduboy.pressed(A_BUTTON) and !buttonA) {
        gameState = 1;
        buttonA = true;
        return;
    }
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(5);
  }

//  if (melodyCount % 2 == 0)
//  {
//    // to calculate the note duration, take one second divided by the note type.
//    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//    int noteDuration = 1000 / noteDurations[noteNumber];
//    tone(5, melody[noteNumber], noteDuration);
//
//    // to distinguish the notes, set a minimum time between them.
//    // the note's duration + 30% seems to work well:
//    int pauseBetweenNotes = noteDuration * 1.30;
//    delay(pauseBetweenNotes);
//    // stop the tone playing:
//    noTone(5);
//    melodyCount++;
//    noteNumber++;
//    if (noteNumber >= 8)
//      noteNumber = 0;
//  }
}
