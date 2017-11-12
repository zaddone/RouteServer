#ifndef ReadNumber_H_
#define ReadNumber_H_

#include <cv.h>
#include <highgui.h>
//void ReadDirImg(const char * path,Callback func,void * obj);
void MergeIplImage(IplImage * dst,const IplImage * src);
bool MatchingIplImage(const IplImage * dst,const IplImage * src);
void splitIplImage(IplImage * dst,const IplImage * src ,int &x,bool &isFind,int src_x);
class Number
{
public:
	Number(int i,char *path,IplImage * temp = NULL);
	~Number();
	IplImage * temple;
	int Num;
	//void Train();
	void Merge(IplImage * img);
	bool Matching(IplImage * img);
	char *TPath;
private:
	void SaveImg();

};
IplImage * ShowNumsListImg(Number **likeNum ,const int Len,IplImage * img,int x);
class NumberList {
public:	
	NumberList(char * TemplePath,char * bak="templebak",int sep=150);
	~NumberList();
	int Know(IplImage * img,int sep=150);
	//friend void TrainImg(void *handle,IplImage * img);
	
	void ReadDirImg(const char * path);
	void ReadListImg();
	void SetNums(int i,IplImage * src);
private:
	Number * Nums[10];
	void TrainImg(IplImage * img);
	int Sep;
	char * templebak;
	void cutIplImage( IplImage * src);
	int splitTemple(Number ** likeNum,int & Len,IplImage * src );
	Number * CheckImg( IplImage * src);
	
};

#endif