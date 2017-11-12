#ifndef Block_H_
#define Block_H_
#include "Coordinate.h"
#include "readNumber.h"
#include <vector>
using namespace std;

bool inSameOne(IplImage *img,CvRect rect  ,IplImage * num,CvPoint *loc);
double inSameOneT(IplImage *img ,IplImage * num,CvPoint *loc);

class Block
{
public:
	Block(CvRect r,Block * _Par);
	Block(char *RectData,Block *_Par ,int _id,char * GName );
	~Block();

	int FindOne(IplImage * src);
	int FindOneT(IplImage * src);
	int FindNL(IplImage * src );
	int FindArr(IplImage * src,bool isUpdate = false);
	Block * GetChildOne(IplImage * src,int id=0);
	bool ClickCoordinate(int _v,int num);
	bool ClickCoordinateCheck(int _v,int num);
	vector <Coordinate>  Coord;
	CvRect  rect;
	void ShowRectImg(IplImage * src);
	
private:
	void LoadToTempleList(char * Filepath);
	//void SaveTemple(IplImage * src);
	void GetChildBlock();
	void GetChildFileName(char  *PathID );
	void GetTemplePath(char *TemplePatho);
	void ClearCoord();
	void LoadTempleForDir(char * Filepath);
	void Fill(IplImage * src);
	vector <Block *> Child;	
	vector <TempleImg*> TempleList ;	
	vector <TempleImg*>  GetTempleList();
	int ID;
	Block *Par;
	int Sep;
	int Coll;
	char * tag;
	NumberList *NL;
	NumberList * GetNumblerListClass();
	
};
bool compBlockX(Block *a,Block *b);
bool compBlockY(Block *a,Block *b);
#endif