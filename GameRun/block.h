#ifndef Block_H_
#define Block_H_
#include "Coordinate.h"
#include "readNumber.h"
#include <vector>
using namespace std;

bool inSameOne(IplImage *img,CvRect rect  ,IplImage * num,CvPoint *loc);
double inSameOneT(IplImage *img ,IplImage * num,CvPoint *loc);
typedef void (*CallBackTask)(LPVOID lpParamter);
bool compImg(IplImage * a,IplImage * b,CvRect rect);

class TrainTemple{
public:
	TrainTemple(char * _path,char * _toPath=NULL);
	~TrainTemple();

	IplImage *img;
	
	void MoveTo(char * _path,char * name);
	bool IsSame(TrainTemple * t);
	bool Show();
	void Remove();
	char  toPath[1024];
	char  Path[1024];
	//char * toPath;
private:

};

class Block
{
public:
	Block(CvRect r,Block * _Par);
	Block(char *RectData,Block *_Par ,int _id,char * GName );
	~Block();

	int FindOne(IplImage * src);
	bool ClickFindOne(IplImage * src,int tag);
	int FindNL(IplImage * src );
	int FindArr(IplImage * src,bool isUpdate = false);
	char* FindArr(IplImage * src,int & size);
	Block * GetChildOne(IplImage * src,int id=0);
	bool ClickCoordinate(int _v,int num,bool isDel = false);
	bool ClickCoordinateCheck(int _v,int num,IplImage *Img);
	vector <Coordinate>  Coord;

	CvRect  rect;
	void ShowRectImg(IplImage * src);
	vector <Block *> Child;	
	void ErgodicBlock(CallBackTask _f=NULL);
	friend void trainNL(LPVOID lpParamter);
	friend void train(LPVOID lpParamter);
	bool Merge(Block *bl);
	bool MergeWithCoordinate(Coordinate coor);

	vector <TrainTemple*> TrainList;

	TempleImg *Nothing;
	NumberList * GetNumblerListClass();
private:
	void AddTrainTemple(char *path);
	void ReadDirImg(const char * path );
	void LoadToTempleList(char * Filepath);
	//void SaveTemple(IplImage * src);
	void GetChildBlock();
	void GetChildFileName(char  *PathID );
	void GetTemplePath(char *TemplePath);
	void ClearCoord();
	void LoadTempleForDir(char * Filepath);
	void Fill(IplImage * src);
	
	vector <TempleImg*> TempleList ;	
	vector <TempleImg*>  GetTempleList();
	int ID;
	Block *Par;
	double MaxVal;
	//int Sep;
	int Coll;
	char * tag;
	NumberList *NL;
	
	//void GetNLForAll();
	

	char * CollPath;
	
};
bool compBlockX(Block *a,Block *b);
bool compBlockY(Block *a,Block *b);
#endif