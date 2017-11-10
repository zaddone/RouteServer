#include "stdafx.h"
#include "mouse.h"


void MouseMove(int x, int y)//鼠标移动到指定位置  
    {  
        double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;//获取屏幕分辨率宽度  
        double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;//获取屏幕分辨率高度  
        double fx = x*(65535.0f / fScreenWidth);  
        double fy = y*(65535.0f / fScreenHeight);  
		
        INPUT  Input = { 0 };  
        Input.type = INPUT_MOUSE;  
        Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;  
        Input.mi.dx = (long)fx;  
        Input.mi.dy = (long)fy;  
        SendInput(1, &Input, sizeof(INPUT));  
		
		//mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, fx, fy , 0, 0);
	//Sleep(100);
	//mouse_event (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
    }  
      
    void MouseLeftDown()//鼠标左键按下  
    {  
        INPUT  Input = { 0 };  
        Input.type = INPUT_MOUSE;  
        Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;  
        SendInput(1, &Input, sizeof(INPUT));  
		//ZeroMemory()
    }  
      
    void MouseLeftUp()//鼠标左键放开  
    {  
        INPUT  Input = { 0 };  
        Input.type = INPUT_MOUSE;  
        Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;  
        SendInput(1, &Input, sizeof(INPUT));  
          
    }  
      
    void MouseRightDown()//鼠标右键按下  
    {  
        INPUT  Input = { 0 };  
        Input.type = INPUT_MOUSE;  
        Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;  
        SendInput(1, &Input, sizeof(INPUT));  
    }  
      
    void MouseRightUp()//鼠标右键放开  
    {  
        INPUT  Input = { 0 };  
        Input.type = INPUT_MOUSE;  
        Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;  
        SendInput(1, &Input, sizeof(INPUT));  
    }  
void LeftClick ()
{  
	mouse_event (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
	/**
  INPUT    Input={0};
  // left down 
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
  ::SendInput(1,&Input,sizeof(INPUT));

  // left up
  ::ZeroMemory(&Input,sizeof(INPUT));
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
  ::SendInput(1,&Input,sizeof(INPUT));
  **/
}
	
/**
//模拟鼠标拖动框选  
ShowWindow(SW_SHOWMINIMIZED);//窗体最小化  
POINT mypoint;  
GetCursorPos(&mypoint);//获取鼠标当前所在位置  
MouseMove(800, 1000);//鼠标移动到指定位置  
MouseLeftDown();//鼠标左键点下  
MouseMove(10, 10);//鼠标拖动到指定位置  
Sleep(10);//这里需要等待一下，不然拖动会没有效果  
MouseLeftUp();//鼠标释放  
MouseMove(mypoint.x, mypoint.y);//将鼠标放回所在的位置


    //模拟鼠标右键按下并释放  
    ShowWindow(SW_SHOWMINIMIZED);//窗体最小化  
    MouseRightDown();  
    Sleep(10);  
    MouseRightUp();  


	POINT currentPoint;
GetCursorPos(&currentPoint);
mouse_event(MOUSEEVENTF_LEFTDOWN,currentPoint.x,currentPoint.x,0,0);
mouse_event(MOUSEEVENTF_LEFTUP,currentPoint.x,currentPoint.y,0,0);
mouse_event (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 )
**/