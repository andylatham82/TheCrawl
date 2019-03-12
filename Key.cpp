#pragma once
#include "Item.cpp"
#include "Constants.h"
#include "Bitmaps.h"

class Key : public Item
{
public:
	Key (char x, char y) : Item(key, 1, 1, x, y)
	{
		
	}
};
