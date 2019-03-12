#include "Sprite.cpp"
#include "Constants.h"
#include "Tile.cpp"
#include "Level.cpp"

class Player : public Sprite {
  public:
    Level *pLevel;
    int posX = playerStartingPositionX * 256;  // 16-bit value.
    int posY = playerStartingPositionY * 256;
    int state = 0;      // 0 = idleRight
                        // 1 = idleLeft
                        // 2 = runRight
                        // 3 = runLeft
                        // 4 = fallRight
                        // 5 = fallLeft
    
    int velocityX = 0;
    int velocityY = 0;
    char velocityStep = 16;
    char gravity = 32;
    int velocityXMax = 256;
    int velocityYMax = 256;
    char directionX = 1;
    int keysHeld = 0;
    bool failed = 0;
    bool won = 0;
    int pathCount = 1;

    Player(Level *pLev) {
      pLevel = pLev;
    }

  void Update() {
    char stateOld = state;
    if (velocityY == 0) {
      frameRate = 60;
      animationLength = 6;
      if (velocityX > 0)
        state = 2;
      else if (velocityX < 0)
        state = 3;
      else {
        frameRate = 15;
        animationLength = 4;
        if (directionX > 0)
          state = 0;
        if (directionX < 0)
          state = 1;
      }
    }
    else {
      frameRate = 45;
      animationLength = 2;
      if (directionX > 0)
        state = 4;
      if (directionX < 0)
        state = 5;
    }
    Sprite::Update(stateOld == state);
    
    if (velocityY < (4 * 256))
      velocityY += gravity;
      
    if (velocityX >= 12)
      velocityX -= 12;
    else if (velocityX <= -12)
      velocityX += 12;
    else
      velocityX = 0;

    CheckForCollisionsAndUpdatePosition();

    //if (posY < (-8 * 256))
    //  velocityY = 0;
    
    if (posY > (66 * 256))
    {
      if ((pLevel->floorNumber == 2 and pLevel->roomNumber == 3))
      {
        GoDown();
        keysHeld++;
      }
      else
        failed = 1;
    }
  }
  void Draw() {
    switch (state) {
      case 0: // Idle Right
        Sprite::Draw(posX, posY, playerIdleRight, playerIdleRightMask);
        break;
      case 1: // Idle Left
        Sprite::Draw(posX, posY, playerIdleLeft, playerIdleLeftMask);
        break;
      case 2: // Run Right
        Sprite::Draw(posX, posY, playerRunRight, playerRunRightMask);
        break;
      case 3: // Run Left
        Sprite::Draw(posX, posY, playerRunLeft, playerRunLeftMask);
        break;
      case 4: // Fall Right
        Sprite::Draw(posX, posY, playerFallRight, playerFallRightMask);
        break;
      case 5: // Fall Left
        Sprite::Draw(posX, posY, playerFallLeft, playerFallLeftMask);
        break;
    }
  }

  void CheckForCollisionsAndUpdatePosition() {
    int tempX = posX + velocityX;
    int tempY = posY + velocityY;

    // Right collision
    for (int i = 0; i < (playerHeight >> 3); i++) {
      int testTileX = ((tempX >> 8) + playerRightBound) >> 3;
      int testTileY = ((posY >> 8) + (i * tileHeight)) >> 3;
      char testTileValue = 0;
      if (testTileY >= 0) // If not off the top of the screen (allows jumping in undefined tiles).
        testTileValue = pgm_read_byte(&(tileMap[pLevel->currentFloor][pLevel->currentRoom][testTileY][testTileX]));
      switch (testTileValue) {
        case 1:
          posX += (testTileX * tileWidth * 256) - (posX + (playerRightBound * 256));
          velocityX = 0;
          break;
      }
    }

    // Left collision
    for (int i = 0; i < (playerHeight >> 3); i++) {
      int testTileX = ((tempX >> 8) + playerLeftBound) >> 3;
      int testTileY = ((posY >> 8) + (i * tileHeight)) >> 3;
      char testTileValue = 0;
      if (testTileY >= 0)
        testTileValue = pgm_read_byte(&(tileMap[pLevel->currentFloor][pLevel->currentRoom][testTileY][testTileX]));
      switch (testTileValue) {
        case 1:   // Wall/Floor
          posX -= ( (posX + (playerLeftBound * 256) ) - ( ((testTileX * tileWidth) + 8) * 256) );
          velocityX = 0;
          break;
        case 2:   // Fire (nothing)
          break;
        case 3:   // Wall (nothing)
          break;
        case 4:   // Door Right
          if (pLevel->doorLocks[pLevel->currentFloor][pLevel->currentRoom][0] == 0)
          {
            if (keysHeld > 0) // AND DOOR LOCKED
            {
              PlayDoorTone();
              GoRight();
              return;
            }
          }
          else
          {
            GoRight();
            return;
          }
          break;
        case 5:   // Door Left
          if (pLevel->doorLocks[pLevel->currentFloor][pLevel->currentRoom][1] == 0)
          {
            if (keysHeld > 0) // AND DOOR LOCKED
            {
              PlayDoorTone();
              GoLeft();
              return;
            }
          }
          else
          {
            GoLeft();
            return;
          }
        case 6:   // Door Up
          if (pLevel->doorLocks[pLevel->currentFloor][pLevel->currentRoom][2] == 0)
          {
            if (keysHeld > 0) // AND DOOR LOCKED
            {
              PlayDoorTone();
              GoUp();
              return;
            }
          }
          else
          {
            GoUp();
            return;
          }
          break;
        case 7:   // Door Down
          if (pLevel->doorLocks[pLevel->currentFloor][pLevel->currentRoom][3] == 0)
          {
            if (keysHeld > 0) // AND DOOR LOCKED
            {
              PlayDoorTone();
              GoDown();
              return;
            }
          }
          else
          {
            GoDown();
            return;
          }
          break;
          case 8:   // Exit
            won = 1; // WIN CONDITION
            return;
          break;
      }
    }

    posX += velocityX;

    // Top collision
    if (velocityY < 0) {
      for (int i = 0; i < (playerWidth >> 3); i++) {
        int testTileX = ((posX >> 8) + 5 - (i*5) + (i * (tileWidth))) >> 3;
        int testTileY = (tempY >> 8) >> 3;
        char testTileValue = 0;
        if (testTileY >= 0)
          testTileValue = pgm_read_byte(&(tileMap[pLevel->currentFloor][pLevel->currentRoom][testTileY][testTileX]));
        switch (testTileValue) {
          case 1:
            posY -= posY - ( ((testTileY * tileHeight) + tileHeight) * 256);
            velocityY = 0;
            break;
        }
      }
    }

    // Bottom collision
    if (velocityY > 0) {
      for (int i = 0; i < (playerWidth >> 3); i++) {
        int testTileX = ((posX >> 8) + 5 - (i*5) + (i * (tileWidth))) >> 3;
        int testTileY = ((tempY >> 8) + playerHeight) >> 3;
        char testTileValue = 0;
        if (testTileY >= 0)
          testTileValue = pgm_read_byte(&(tileMap[pLevel->currentFloor][pLevel->currentRoom][testTileY][testTileX]));
        switch (testTileValue) {
          case 1:
            posY += (testTileY * tileHeight * 256) - (posY + (playerHeight * 256));
            velocityY = 0;
            break;
        }
      }
    }
    
    posY += velocityY;

    // Check for collisions with level objects
    for (int i = 0; i < sizeof(pLevel->items)/sizeof(pLevel->items[0]); i++)
    {
      if (pLevel->items[i].level == pLevel->roomNumber and pLevel->items[i].flr == pLevel->floorNumber)
      {
        if ( (posX + (playerRightBound * 256)) > pLevel->items[i].posX  and posX < (pLevel->items[i].posX + (tileWidth * 256))
              and (posY + (playerHeight * 256)) > pLevel->items[i].posY and posY < (pLevel->items[i].posY + (tileHeight * 256))
              and !pLevel->items[i].collected)
          {
            pLevel->items[i].collected = true;
            pLevel->keysCollected[pLevel->floorNumber - 1][pLevel->roomNumber - 1] = 1;
            //pLevel->items[i].level = -1;
            
            int keyValue = 0;
            keyValue = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0]));
            
            keysHeld += keyValue;

            tone(5, 600, 100);
          }
      }
    }
  }

  void Reset() {
    //keysHeld--;
    //posX = playerStartingPositionX * 256;
    //posY = playerStartingPositionY * 256;
    velocityX = 0;
    //velocityY = 0;
    //directionX = 1;
  }

  void GameReset() {
    keysHeld = 0;
    velocityX = 0;
    velocityY = 0;
    directionX = 1;
    EnterDungeon();
    pathCount = 1;
  }

  void EnterDungeon()
  {
    pLevel->roomNumber = 1;
    pLevel->floorNumber = 1;
    posX = pLevel->playerEnterLeft[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] * 8 * 256;
    posY = pLevel->playerEnterLeft[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] * 8 * 256;
    int testKeyValue = 0;
    int testKeyPosX = 0;
    int testKeyPosY = 0;
    testKeyValue = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0]));
    testKeyPosX = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1]));
    testKeyPosY = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][2]));
    if (testKeyValue == 1)
    {
      pLevel->items[0].flr = pLevel->floorNumber;
      pLevel->items[0].level = pLevel->roomNumber;
      pLevel->items[0].ConvertPosition(testKeyPosX, testKeyPosY);
      pLevel->items[0].collected = false;
    }
    if (testKeyValue == 2)
    {
      pLevel->items[1].flr = pLevel->floorNumber;
      pLevel->items[1].level = pLevel->roomNumber;
      pLevel->items[1].ConvertPosition(testKeyPosX, testKeyPosY);
      pLevel->items[1].collected = false;      
    }
    if (testKeyValue == 3)
    {
      pLevel->items[2].flr = pLevel->floorNumber;
      pLevel->items[2].level = pLevel->roomNumber;
      pLevel->items[2].ConvertPosition(testKeyPosX, testKeyPosY);
      pLevel->items[2].collected = false;      
    }
    Reset();
  }

  void GoRight()
  {
    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] == 0)
    {
      keysHeld--;
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] = 1;
    }
    
    if (pLevel->roomNumber > 4){
      pLevel->roomNumber = 1;
    }

    pLevel->roomNumber++;

    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] == 0)
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] = 1;

    CheckKeys();
    
    Reset();

    posX = pLevel->playerEnterLeft[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] * 8 * 256;
    posY = pLevel->playerEnterLeft[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] * 8 * 256;
  }

  
  void GoLeft()
  {
    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] == 0)
    {
      keysHeld--;
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] = 1;
    }

    pLevel->roomNumber--;

    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] == 0)
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] = 1;

    CheckKeys();
    
    Reset();

    posX = pLevel->playerEnterRight[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] * 8 * 256;
    posY = pLevel->playerEnterRight[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] * 8 * 256;
  }

  void GoUp()
  {
    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][2] == 0)
    {
      keysHeld--;
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][2] = 1;
    }

    pLevel->floorNumber--;

    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][3] == 0)
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][3] = 1;

    CheckKeys();
    
    Reset();

    posX = pLevel->playerEnterBottom[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] * 8 * 256;
    posY = pLevel->playerEnterBottom[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] * 8 * 256;
  }

  void GoDown()
  {
    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][3] == 0)
    {
      keysHeld--;
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][3] = 1;
    }

    pLevel->floorNumber++;

    if (pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][2] == 0)
      pLevel->doorLocks[pLevel->floorNumber - 1][pLevel->roomNumber - 1][2] = 1;

    CheckKeys();
    
    Reset();

    posX = pLevel->playerEnterTop[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0] * 8 * 256;
    posY = pLevel->playerEnterTop[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1] * 8 * 256;
  }

  void PlayDoorTone()
  {
    tone(5, 300, 100);
  }

  void CheckKeys()
  {
    pathCount++;
    int testKeyValue = 0;
    int testKeyPosX = 0;
    int testKeyPosY = 0;
    testKeyValue = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][0]));
    testKeyPosX = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][1]));
    testKeyPosY = pgm_read_byte(&(keyMap[pLevel->floorNumber - 1][pLevel->roomNumber - 1][2]));
    if (testKeyValue == 1)
    {
      pLevel->items[0].flr = pLevel->floorNumber;
      pLevel->items[0].level = pLevel->roomNumber;
      pLevel->items[0].ConvertPosition(testKeyPosX, testKeyPosY);
      pLevel->items[0].collected = pLevel->keysCollected[pLevel->floorNumber - 1][pLevel->roomNumber - 1];
    }
    if (testKeyValue == 2)
    {
      pLevel->items[1].flr = pLevel->floorNumber;
      pLevel->items[1].level = pLevel->roomNumber;
      pLevel->items[1].ConvertPosition(testKeyPosX, testKeyPosY);
      pLevel->items[1].collected = pLevel->keysCollected[pLevel->floorNumber - 1][pLevel->roomNumber - 1];
    }
    if (testKeyValue == 3)
    {
      pLevel->items[2].flr = pLevel->floorNumber;
      pLevel->items[2].level = pLevel->roomNumber;
      pLevel->items[2].ConvertPosition(testKeyPosX, testKeyPosY);
      pLevel->items[2].collected = pLevel->keysCollected[pLevel->floorNumber - 1][pLevel->roomNumber - 1];
    }
  }
};


// Fixed Point 8.8
// ---------------
// 8 bits in a byte.
// In 8 bits (1 byte) you can store 0 - 255 (enough for a coordinate
// on the small screen.
// In 16 bits (2 bytes) you can store 0 - 65536. Use 1st byte for integer
// and 2nd byte for fraction.
// Increment the bottom 8 bits until it wraps around 256, and the upper
// 8 bits will increment by 1.
// Acts like a decimal: 0.0 to 255.255
// Use whole 16 bits to calculate velocities but only use the upper 8
// bits to render the object to the screen, discarding the lower 8 with
// the >> operator.
// 256 is 1 pixel per frame. 128 is 0.5 pixels per frame.
