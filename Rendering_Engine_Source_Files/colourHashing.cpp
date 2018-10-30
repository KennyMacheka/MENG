#include "colourHashing.h"
unsigned int getColourHash (unsigned int r, unsigned int  g, unsigned int b, unsigned int  a){

	/**
		Every RGBA value is composed of four numbers,
		from 0-255
		
		Thus, there are 256*256*256*256
		unique values. As such, one should be able to create
		a hashing algorithm to convert every RGBA value
		to a unique number from 0 to 256^4
		
		I was able to figure one about. It works in a similar way
		to how one would convert from a number base to another,
		like hexadeimal to denary, except that every R G B A 
		value uses only numbers from the denary system.
		
		For every RGBA values, we do
		A + B*256 + G*(256^2) + R*(256^3)
		
		One can think of this as converting from
		base-256 to denary, although each R G B A value is in denary
	**/
    unsigned int base = 256;

    return base*b + base*base*g + base*base*base*r + a;

}

void convertHash (unsigned int colourNo, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a){

    //This function does the reverse opposite of colour hash via...
	//...via the use of remainders and quotients 
    unsigned int base = 256;
    *r = colourNo / (base*base*base);


    colourNo -= (base*base*base*(*r));

    *g = colourNo / (base*base);

    colourNo -= (base*base* (*g) );

    *b = colourNo / base;
    colourNo -= (base*(*b));

    *a = colourNo;
}

