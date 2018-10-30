#ifndef PIXELMOVEMENT_H_INCLUDED
#define PIXELMOVEMENT_H_INCLUDED

namespace physicsEngine{
bool pixelAverage (int frameRate, double speed, int* numLess, int* numMore,
				   int *numLessOccurrences, int *numMoreOccurrences);
				   
void framesDistribution ( double totalAverage, int numA, int numB,int sizeA,
						  int sizeB,int position, int distributionArray[]);
}
#endif // PIXELMOVEMENT_H_INCLUDED
