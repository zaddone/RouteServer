#ifndef Coor_H_
#define Coor_H_

#include "mouse.h"
#include "templeImg.h"
class  Coordinate{
public:
	Coordinate(TempleImg *t,CvRect r,int o = 0 );
	~Coordinate();

	TempleImg *te;
	bool MouseClick(const int _v,const int n=1);
	CvRect rect;
	int order;
}; 
bool compCoordinate(const Coordinate &a,const Coordinate &b);
bool compCoordinateOrder(const Coordinate &a,const Coordinate &b);
#endif