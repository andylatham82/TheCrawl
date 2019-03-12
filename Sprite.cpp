#include<Arduboy2.h>
#include "Constants.h"

#pragma once
class Sprite {
  public:
    Sprites sprites;

    // Sprite animation variables.
    unsigned int framesSinceAnimUpdate = 0; // Number of updates since last animation frame advance.
    unsigned int currentFrame = 0;          // Displayed frame.
    unsigned int animationLength = 6;       // Number of frames.
    unsigned int frameRate = 60;            // Sprite frame rate.
    unsigned int framesBetweenUpdates;      // Number of frames between sprite animation updates.
    bool active = true;
    
    Sprite() {
      // Calculate number of frames between animation updates.
      framesBetweenUpdates = round((1.0 / frameRate) * 100);
    }

    Sprite(int animLength, int fRate) {
      // Calculate number of frames between animation updates.
      animationLength = animLength;
      frameRate = fRate;
      framesBetweenUpdates = round((1.0 / frameRate) * 100);
    }

    void Update(bool sameState) {
      if (active) {
        
        framesBetweenUpdates = round((1.0 / frameRate) * 100);
        // Advance the sprite animation frame.
        framesSinceAnimUpdate++;
        if (framesSinceAnimUpdate > framesBetweenUpdates){
          currentFrame = (currentFrame + 1) % animationLength;
          framesSinceAnimUpdate = 0;
        }
        if (!sameState)
          currentFrame = 0;
      }
    }

    void Draw(int posX, int posY, const unsigned char bitmap[], const unsigned char mask[]) {
      sprites.drawExternalMask(posX >> 8, posY >> 8, bitmap, mask, currentFrame, currentFrame);
    }
    void Draw(int posX, int posY, const unsigned char bitmap[], const unsigned char mask[], int frame) {
      sprites.drawExternalMask(posX >> 8, posY >> 8, bitmap, mask, frame, currentFrame);
    }
};
