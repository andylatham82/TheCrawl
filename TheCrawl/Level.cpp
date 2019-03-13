#pragma once
#include "Tile.cpp"
#include "Bitmaps.h"
#include "Constants.h"
#include "Item.cpp"
#include "Key.cpp"

class Level {
  public:
    Tile tiles[9] = {NULL, Tile(floorTile), Tile(candleTile, 2, 15), Tile(wallTile), Tile(doorRight), Tile(doorLeft), Tile(doorUp), Tile(doorDown), Tile(door)};
    Tile inventoryTiles[7] = {NULL,
                              Tile(inventoryBorderVertical),
                              Tile(inventoryBorderHorizontal),
                              Tile(inventoryBorderTL),
                              Tile(inventoryBorderTR),
                              Tile(inventoryBorderBL),
                              Tile(inventoryBorderBR)};
    
    
    Item items[3] = {Item(key, 1, 1, 7, 4), Item(key2, 1, 1, 12, 4), Item(key3, 1, 1, 12, 4)};

    //Right,Left,Up,Down
    int doorLocks[4][4][4] = {
      { {0,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} },
      { {0,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} },
      { {0,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} },
      { {0,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} }
    };

    int keysCollected [4][4] = {
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 0, 0 }
    };

    int playerEnterLeft[4][4][2] = {
      { {5, 5}, {3, 5}, {3, 5}, {3, 3}, },
      { {5, 5}, {5, 5}, {9, 5}, {11, 4}, },
      { {5, 5}, {3, 5}, {3, 3}, {3, 5}, },
      { {5, 5}, {3, 1}, {3, 3}, {3, 5}, },
    };
    int playerEnterRight[4][4][2] = {
      { {11, 2}, {10, 1}, {2, 1}, {5, 5}, },
      { {10, 0}, {13, 5}, {10, 1}, {5, 5}, },
      { {11, 2}, {4, 1}, {11, 5}, {5, 5}, },
      { {7, 3}, {11, 1}, {11, 0}, {5, 5}, },
    };
    int playerEnterTop[4][4][2] = {
      { {5, 5}, {5, 5}, {5, 5}, {5, 5}, },
      { {1, 2}, {5, 5}, {5, 5}, {5, 5}, },
      { {8, 0}, {5, 5}, {5, 0}, {5, 5}, },
      { {5, 2}, {5, 5}, {5, 5}, {5, 5}, },
    };
    int playerEnterBottom[4][4][2] = {
      { {4, 5}, {5, 5}, {5, 5}, {5, 5}, },
      { {9, 5}, {5, 5}, {7, 6}, {5, 5}, },
      { {6, 5}, {5, 5}, {5, 5}, {5, 5}, },
      { {5, 5}, {5, 5}, {5, 5}, {5, 5}, },
    };
    
    Item *inventoryArray[10];
    // https://stackoverflow.com/questions/7877993/c-create-array-of-objects-from-different-classes

    int currentFloor = 0;
    int currentRoom = -1;
    int floorNumber = 1;
    int roomNumber = 0;
    bool inventory = false;
    bool winScreen = false;

    Update() {
      currentFloor = floorNumber - 1;
      currentRoom = roomNumber - 1;
      for (int i = 0; i < (sizeof(tiles) / sizeof(tiles[0])); i++) {
        tiles[i].Update();
      }
    }

    Draw() {
      // Inventory screen
      if (inventory or winScreen) {
        // Draw inventory background.
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 16; j++) {
            inventoryTiles[ pgm_read_byte(&(inventoryTileMap[i][j])) ].Draw(j * 8 * 256, i * 8 * 256, 0);
          }
        }

        // Draw inventory contents.
//        for (int i = 0; i < sizeof(items)/sizeof(items[0]); i++) {
//          if (items[i].collected == true)
//            items[i].Draw( (i + 1) * 8 * 256, 8 * 256);
//        }
        if (!winScreen)
        {
          Item invKey(key, 0, 0, 1, 1);
          invKey.Draw();
        }
      }

      // Gameplay screen
      else {
        // Tiles
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 16; j++) {
            if ( pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) == 4 )
              tiles[ pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) ].Draw(j * 8 * 256, i * 8 * 256, doorLocks[currentFloor][currentRoom][0]);
            else if ( pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) == 5 )
              tiles[ pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) ].Draw(j * 8 * 256, i * 8 * 256, doorLocks[currentFloor][currentRoom][1]);
            else if ( pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) == 6 )
              tiles[ pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) ].Draw(j * 8 * 256, i * 8 * 256, doorLocks[currentFloor][currentRoom][2]);
            else if ( pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) == 7 )
              tiles[ pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) ].Draw(j * 8 * 256, i * 8 * 256, doorLocks[currentFloor][currentRoom][3]);
            else
              tiles[ pgm_read_byte(&(tileMap[currentFloor][currentRoom][i][j])) ].Draw(j * 8 * 256, i * 8 * 256);
          }
        }
        // Items
        for (int i = 0; i < (sizeof(items) / sizeof(items[0])); i++) {
          if ( (items[i].level == roomNumber) and (items[i].flr == floorNumber) and !items[i].collected)
            items[i].Draw();
        }
      }
    }

    void Reset() {
      for (int i = 0; i < (sizeof(items) / sizeof(items[0])); i++) {
            items[i].Reset();
      }
      roomNumber = 1;
      inventory = false;
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          for (int k = 0; k < 4; k++)
          {
            doorLocks[i][j][k] = 0;
          }
        }
      }
      
      for (int l = 1; l < 4; l++)
      {
        doorLocks[0][0][l] = 1;
        doorLocks[1][0][l] = 1;
        doorLocks[2][0][l] = 1;
        doorLocks[3][0][l] = 1;
      }

      items[0].Reset();
      items[1].Reset();
      items[2].Reset();

      for (int m = 0; m < 4; m++)
      {
        for (int n = 0; n < 4; n++)
        {
          keysCollected[m][n] = 0;
        }
      }
    }
};
