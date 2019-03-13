#pragma once
#include "Sprite.cpp"
#include "Bitmaps.h"

class Tile : public Sprite {
  public:
    const unsigned char *pTileTexture;

    Tile(const unsigned char *pTexture) : pTileTexture(pTexture), Sprite(1, 60) {
      // Using the colon is the only way of changing the value of a const variable.
    }

    Tile(const unsigned char *pTexture, int animLength, int frameRate) : pTileTexture(pTexture), Sprite(animLength, frameRate) {
      // Using the colon is the only way of changing the value of a const variable.
      animationLength = animLength;
      frameRate = frameRate;
    }

    void Update() {
      active = true;
      Sprite::Update(true);
    }
    
    void Draw(int posX, int posY) {
      Sprite::Draw(posX, posY, pTileTexture, pTileTexture);
    }
    void Draw(int posX, int posY, int frame) {
      Sprite::Draw(posX, posY, pTileTexture, pTileTexture, frame);
    }
};
