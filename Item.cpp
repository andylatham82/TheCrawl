#pragma once
#include "Sprite.cpp"
#include "Constants.h"

class Item : public Sprite {
  public:
    const unsigned char *pItemTexture;
    int level = 0;
    int flr = 0;
    int posX = 0;
    int posY = 0;
    bool collected = true;
    
    Item(const unsigned char *pTexture, char f, char l, char x, char y) : pItemTexture(pTexture), Sprite(1, 60) {
      flr = f;
      level = l;
      posX = x * tileWidth * bitConversion;
      posY = y * tileHeight * bitConversion;
    }

    Item(const unsigned char *pTexture, char f, char l,char x, char y, int animLength, int frameRate) : pItemTexture(pTexture), Sprite(animLength, frameRate) {
      flr = f;
      level = l;
      posX = x * tileWidth * bitConversion;
      posY = y * tileHeight * bitConversion;
      animationLength = animLength;
      frameRate = frameRate;
    }

    void ConvertPosition(int x, int y)
    {
      posX = x * tileWidth * bitConversion;
      posY = y * tileHeight * bitConversion;
    }

    void Draw() {
      Sprite::Draw(posX, posY, pItemTexture, pItemTexture);
    }

    void Draw(int x, int y) {
      Sprite::Draw(x, y, pItemTexture, pItemTexture);
    }

    void Reset() {
      collected = false;
      posX = -8 * 256 * 8;
      posY = -8 * 256 * 8;
      //level = 0;
      //flr = 0;
    }
};
