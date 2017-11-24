#ifndef Coor_H_
#define Coor_H_

#include "mouse.h"
#include "templeImg.h"
class  Coordinate{
public:
	Coordinate(TempleImg *t,CvRect r,double o = 1 );
	~Coordinate();

	TempleImg *te;
	bool Check(const int _v);
	void MouseClick(const int n=1);
	CvRect rect;
	double order;
	void Fill(IplImage * src );
	void Fill(IplImage * src,CvRect rect_old);
}; 
bool compCoordinate(const Coordinate &a,const Coordinate &b);
bool compCoordinateOrder(const Coordinate &a,const Coordinate &b);
#endif