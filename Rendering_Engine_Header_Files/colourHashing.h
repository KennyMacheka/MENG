#ifndef COLOURHASHING_H_INCLUDED
#define COLOURHASHING_H_INCLUDED

#include <cstdint>

unsigned int getColourHash (unsigned int r, unsigned int  g, unsigned int b, unsigned int  a);
void convertHash (unsigned int colourNo, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
#endif // COLOURHASHING_H_INCLUDED
