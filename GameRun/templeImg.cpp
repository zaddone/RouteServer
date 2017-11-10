#include "stdafx.h"
#include "templeImg.h"

#include <direct.h>  
#include <io.h> 
#include <sys/stat.h>  
#define ACCESS _access  
#define MKDIR(a) _mkdir((a))

TempleImg::TempleImg(IplImage * img ,const int tag,int _order ){
	this->img = img;
	Tag = tag;
	this->order = _order;
}

TempleImg::~TempleImg(){
	cvReleaseImage(&this->img);
	//this->img = cvLoadImage(path,0);
}
void TempleImg::SaveTemple(char  *filePath){	
	char file[1024] = {0};		
	//this->GetTemplePath(file);
	CreatDir(filePath);
	sprintf(file,"%s\\%d.bmp\0",filePath,this->Tag);
	int result;
	struct _stat st;
	result = _stat(file, &st);
	clock_t begin = clock();
	while(result==0){
		begin++;
		sprintf(file,"%s\\%d_%d.bmp\0",filePath,this->Tag,begin);
		result = _stat(file, &st);		
		
	}	
	cvSaveImage(file,this->img);

}
bool compTempleImgOrder(TempleImg *a,TempleImg *b)
{
	return a->order > b->order;
}
int CreatDir(char *pDir){  
    int i = 0;  
    int iRet;  
    int iLen;  
    char* pszDir;  
  
    if(NULL == pDir)  
    {  
        return 0;  
    }  
      
    pszDir = _strdup(pDir);  
    iLen = int(strlen(pszDir));  
    for (i = 0;i < iLen;i ++)  
    {  
        if (pszDir[i] == '\\' || pszDir[i] == '/')  
        {   
            pszDir[i] = '\0';   
            iRet = ACCESS(pszDir,0);  
            if (iRet != 0)  
            {  
                iRet = MKDIR(pszDir);  
                if (iRet != 0)  
                {  
                    return -1;  
                }   
            }
            pszDir[i] = '/';  
        }   
    }  
  
    iRet = MKDIR(pszDir);  
    //free(pszDir);  
	delete []pszDir;
    return iRet;  
}