#ifndef TempleImg_H_
#define TempleImg_H_
#include <cv.h>
#include <highgui.h>

class TempleImg {
public:
	TempleImg(IplImage * img  ,const int tag = 0 ,int _order = 0 );
	
	~TempleImg();
	IplImage  * img;
	void SaveTemple(char  *filePath);
	int Tag;
	int order;
};
bool compTempleImgOrder(TempleImg *a,TempleImg *b);
int CreatDir(char *pDir);
#endif