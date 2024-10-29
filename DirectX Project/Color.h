// ELEFTERIOS LAZARIDIS
#pragma once
typedef unsigned char BYTE;

class Color
{
private:
	union
	{
		BYTE rgba[4];
		unsigned int color;
	};
public:
	Color();
	Color(BYTE r, BYTE g, BYTE b);
	Color(BYTE r, BYTE g, BYTE b, BYTE a);
};