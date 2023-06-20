//===========================================================================
//**********************************主程序***********************************  
//===========================================================================
/*库中textbox的定位：
1.识别操作 -> 移动光标 -> 展示所有内容
2.单行内的操作(除了单行换行) 
*/ 
#include <windows.h>
#include <winuser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "linkedlist.h"
#include <string.h>
#include "file.h"
#define DEMO_MENU
#define DEMO_BUTTON
#define DEMO_ROTATE
#define DEMO_EDITBOX

// 全局变量
static double winwidth, winheight;   // 窗口尺寸
static int    show_more_buttons = 0; // 切换更多说明栏
static int    flag_for_about = 0;    // 切换信息栏 
static int    copyflag = 0;          // 复制判别号 
static int    shearflag = 0;         // 剪切判别号 
static int    pasteflag = 0;         // 粘贴判别号 
static int    row=2;                   // 行序号 
static char   memo[200][40];           // 用于存储输入的数组 
static char   *store_point[50];        // 用于存储选择的数组 

// 清屏函数，provided in libgraphics
void DisplayClear(void); 
// 计时器启动函数，provided in libgraphics
void startTimer(int id,int timeinterval);
void backclear(int x,int y,int fx,int fy);
void enterplus(int y,int x);
// 用户的显示函数
void display(int, int); 
#if defined(DEMO_MENU)
// 菜单演示程序
void drawMenu()
{   //菜单名称 
	static char * menuListFile[] = {"File",  
		"Open  | Ctrl-O", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"Close",
		"Exit   | Ctrl-E"};
	static char * menuListTool[] = {"Edit",
		"Copy     | Ctrl-C",                                           
		"Cut      | Ctrl-X",                                              
		"Paste    | Ctrl-V"};                                             
	static char * menuListHelp[] = {"Help",
		"Instructin | Ctrl-M",
		"About      | Ctrl-A"};
	static char * selectedLabel = NULL;
	
    //参数 
	double fH = GetFontHeight();
	double x = 0;                                                //fH/8;
	double y = winheight;
	double h = fH*1.5;                                           // 控件高度
	double w = TextStringWidth(menuListHelp[0])*2;               // 控件宽度
	double wlist = TextStringWidth(menuListTool[3])*1.2;
	double xindent = winheight/20;                               // 缩进
	int    selection;                                            // 鼠标点击选项序号 
	//如果不选择粘贴、 
	pasteflag = 0;
	
	// menu bar
	drawMenuBar(0,y-h,winwidth,h);
	// File 菜单                                                       
	selection = menuList(GenUIID(0), x, y-h, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
	if( selection==3 )
		exit(-1); // choose to exit
	
	// Tool 菜单   
	selection = menuList(GenUIID(0),x+w,  y-h, w, wlist,h, menuListTool,sizeof(menuListTool)/sizeof(menuListTool[0]));
	if( selection==1 )   copyflag = 1;                                                                                        //实现COPY                                                           
    if( selection==2 )   shearflag = 1;                                                                                       //实现SHEAR
	if( selection==3 )   pasteflag = 1;                                                                                       //实现PASTE
		
	// Help 菜单
	menuListHelp[1] = show_more_buttons ? "Close Instructin | Ctrl-M" : "Instructin | Ctrl-M";                          //根据功能情况切换菜单显示 
	menuListHelp[2] = flag_for_about ? "Close About      | Ctrl-A" : "About      | Ctrl-A";                             //根据功能情况切换菜单显示
	selection = menuList(GenUIID(0),x+2*w,y-h, w, wlist, h, menuListHelp,sizeof(menuListHelp)/sizeof(menuListHelp[0]));
	if( selection==1 )                                                                                                  //实现 
		show_more_buttons = ! show_more_buttons;
	if( selection==2 )
		flag_for_about = ! flag_for_about;

	// 显示菜单运行结果                            //刘天成加 
	if(show_more_buttons)                          //Instruction 显示内容   
	{
	x=winwidth*0.72;
	y=winheight*0.3;
	SetPenColor("Black");
	drawLabel(x-7*h, y-=h, "***************Instruction**************");
	drawLabel(x-7*h, y-=h, "You can tpye in your codes in the text box.");
	drawLabel(x-5*h, y-=h, "Here are some Keys and Functions");
	drawLabel(x-7*h, y-=h, "Backspace-------------------------Delete");
	drawLabel(x-7*h, y-=h, "Up------------------------------Mouse UP");
    drawLabel(x-7*h, y-=h, "Down--------------------------Mouse Down");
    drawLabel(x-7*h, y-=h, "Left--------------------------Mouse Left");
    drawLabel(x-7*h, y-=h, "Right------------------------Mouse Right");
    drawLabel(x-7*h, y-=h, "Shift+Dirction--------------------Choose");
    drawLabel(x-7*h, y-=h, "************THANKS FOR USING************");
    }
    if(flag_for_about)                             //About 显示内容 
	{
	x=winwidth*0.72;
	y=winheight*0.45;
	SetPenColor("Black");
    drawLabel(x-7*h, y-=h, "**************C Code Editer**************");
    drawLabel(x-7*h, y-=h, "********这是一个牛逼的轻量级编辑器*******");
    }
}
#endif // #if defined(DEMO_MENU)

#if defined(DEMO_EDITBOX)
// 文本编辑演示程序

//增添行                                                                    //汤坤逸加
void enterplus(int hhy,int hhx)                                            
{                       
	char t[20];
	int i;
	double fH = GetFontHeight();
	double h = fH*1.5; // 控件高度
	double w = winwidth; // 控件宽度
	double x = winwidth/15;
	double y = winheight/2-h;
	char s[40];	
	strcpy(t,&(memo[hhy-1][hhx]));
    memset(&(memo[hhy-1][hhx]),'\0',strlen(memo[hhy-1])-hhx);
    for (i=row-1;i>hhy;i--){
    	memset(memo[i],'\0',sizeof(memo[i]));
    	strcpy(memo[i],memo[i-1]);
	}
	memset(memo[i],0,sizeof(memo[i]));
	strcpy(memo[i],t);
	row++;
	for (i=0;i<=row-2;i++){
			sprintf(s, "%d", i+1);
	        drawLabel(0.05,winheight-h*(i+2),s);//行号 
			textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
		}
//	for (i=hhy;i<=row-2;i++){
//		 sprintf(s, "%d", i+1);
//	    drawLabel(0.05,winheight-h*(i+2),s);
//	   textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
//	}
//	textbox(GenUIID(i), 0, winheight-h*(hhy+1), w, h, memo[hhy-1], sizeof(memo[0]));
} 
void backclear(int x,int y,int fx,int fy)//调用该函数可以直接把所有方块（即选中）内容删去，基于全局的memo实现 
{
	int miny=fy,maxy=y,minx=fx,maxx=x;
	double fH = GetFontHeight();
	double h = fH*1.5; // 控件高度
	double w = winwidth; // 控件宽度
	int i; 
	char s[40];	
	if (y<miny) {
	miny=y;maxy=fy;
	minx=x+1;maxx=fx+1;} 
	if (fy==-1 && x==0){
		strcat(memo[maxy-2],memo[maxy-1]);
		memset(memo[y-1],'\0',sizeof(memo[y-1]));
		for (i=maxy-1;i<=row-3;i++){
			memset(memo[i],'\0',sizeof(memo[i]));
			strcpy(memo[i],memo[i+1]);
			}
		row--; 
	}else if (fx!=-1){
		for (i=miny;i<maxy-1;i++) memset(memo[i],'\0',sizeof(memo[i]));//清除中间行 
		memset(&(memo[miny-1][minx]),'\0',strlen(memo[miny-1])-minx);//清除上短行 
		for (i=0;i<strlen(memo[maxy-1])-maxx;i++)
			if (memo[maxy-1][i+maxx]!='\0') 
			memo[maxy-1][i]=memo[maxy-1][i+maxx];
			else memo[maxy-1][i]='\0';
		for (i=strlen(memo[maxy-1])-maxx;i<=strlen(memo[maxy-1]);i++)
           	memo[maxy-1][i]='\0';//清除下面短行  
	strcat(memo[miny-1],memo[maxy-1]);
	memset(memo[maxy-1],'\0',sizeof(memo[maxy-1]));
	for (i=miny;i<=row-2-maxy+miny;i++){
		memset(memo[i],'\0',sizeof(memo[i]));
		strcpy(memo[i],memo[i+maxy-miny]);
	}
	row=row-(maxy-miny); 		
}
		for (i=0;i<=row-2;i++){
			sprintf(s, "%d", i+1);
	        drawLabel(0.05,winheight-h*(i+2),s);//行号 
			textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
		}//这个for循环可以作为单独的模块，主要用于刷新光标位置 
}
//绘出每行的TEXTBOX                                                          //汤坤逸加 
void drawEditText(int key, int event)
{
	static char results[200] = "";
	static char newnum[80] = "";
	double fH = GetFontHeight();
	double h = fH*1.5; // 控件高度
	double w = winwidth; // 控件宽度
	double x = winwidth/15;
	double y = winheight/2-h;
	int i=0, hhy=0,hhx=0;//换行或者行开头按backspace时的y，换行时的x 
	char s[40];
		int fmx,fmy,mx,my;
		mx=textbox(GenUIID(0),-10, winheight, w, h, memo[i], sizeof(memo[0])); 
		my=textbox(GenUIID(0),-20, winheight, w, h, memo[i], sizeof(memo[0])); 
		fmx=textbox(GenUIID(0),-30, winheight, w, h, memo[i], sizeof(memo[0])); 
		fmy=textbox(GenUIID(0),-40, winheight, w, h, memo[i], sizeof(memo[0]));
		for (i=0;i<=row-2;i++){
			sprintf(s, "%d", i+1);
	        drawLabel(0.05,winheight-h*(i+2),s);//行号 
			textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
		}//刷新光标位置 
	if (event==KEY_DOWN){
		switch(key){
			case VK_BACK:
                if ((fmx==-1 && mx==0) || fmx!=-1 && fmy!=my)
				backclear(mx,my,fmx,fmy);
				break;
			case VK_RETURN:		
				if (fmx==-1){
					enterplus(my,mx);
				}else if (fmy!=my){
					backclear(mx,my,fmx,fmy);//如果跨行选择按换行，那么先删除 
					if (my<fmy) enterplus(my,mx);
					else enterplus(fmy,fmx);//再换行 
				}else {
					if (mx<fmx) {
					for (i=mx;i<=strlen(memo[my-1])-fmx-1+mx;i++)
						memo[my-1][i]=memo[my-1][i+fmx-mx];
						memset(&(memo[my-1][mx+strlen(memo[my-1])-fmx]),'\0',fmx-mx);
				    }//这里显示和实际效果不一致，实际效果和devC++保持一致 
					if (mx>fmx) {
					for (i=fmx;i<=strlen(memo[my-1])-mx-1+fmx;i++)
						memo[my-1][i]=memo[my-1][i+mx-fmx];
						memset(&(memo[my-1][fmx+strlen(memo[my-1])-mx]),'\0',mx-fmx);
				    }
					//单行内选择了一串按换行，那么先删除 
					if (mx<fmx) enterplus(my,mx);
					else enterplus(fmy,fmx);//再换行 
				}
				break;
		}
	} 		
//	if (row>=3 && key==VK_BACK && event==KEY_DOWN && fmx==-1) {
//		for (i=0;i<=row-2;i++){
//			sprintf(s, "%d", i+1);
//	        drawLabel(0.05,winheight-h*(i+2),s);//行号 
//			if (textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]))<-1) {
//				hhy=i+1;break;
//			}
//		}
//		if (hhy>0){
//		strcat(memo[hhy-2],memo[hhy-1]);
//		sprintf(s, "%d", hhy-1);
//	    drawLabel(0.05,winheight-h*hhy,s);
//		textbox(GenUIID(0), 0.3, winheight-h*(hhy), w, h, memo[hhy-2], sizeof(memo[0]));
//		for (i=hhy-1;i<=row-3;i++){
//			memset(memo[i],'\0',sizeof(memo[i]));
//			strcpy(memo[i],memo[i+1]);
//		} 
//		memset(memo[i],'\0',sizeof(memo[i]));
//		row--;
//		for (i=hhy-1;i<=row-2;i++){
//			 sprintf(s, "%d", i+1);
//	        drawLabel(0.05,winheight-h*(i+2),s);
//		    textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
//		}
//		}
//	}
//	else
//	{
//    for (i=0;i<=row-2;i++){
//        sprintf(s, "%d", i+1);
//	     drawLabel(0.05,winheight-h*(i+2),s);
//	    if (textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]))>0){
//	 	hhy=i+1;//第几行 
//	  	hhx=textbox(GenUIID(i), -1, 0, 0, 0, memo[i], sizeof(memo[0]));break;
//	 }
//    }
//    if (hhy>0){
//       enterplus(hhy,hhx);
//    }
//   }
}
#endif // #if defined(DEMO_EDITBOX)

void display(int x,int y)
{
	// 清屏
	DisplayClear();

#if defined(DEMO_EDITBOX)
	// 编辑文本字符串
	drawEditText(x,y);
#endif

#if defined(DEMO_MENU)
	// 绘制和处理菜单
	drawMenu();
#endif
}

// 用户的字符事件响应函数
void CharEventProcess(char ch)
{
	uiGetChar(ch); // GUI字符输入
	display(0,-2); //刷新显示
}

// 用户的键盘事件响应函数
void KeyboardEventProcess(int key, int event)
{
	double fH = GetFontHeight();
	double h = fH*1.5; // 控件高度
	uiGetKeyboard(key,event); // GUI获取键盘
	double w = winwidth; // 控件宽度

	display(key,event); // 刷新显示
}

// 用户的鼠标事件响应函数
void MouseEventProcess(int x, int y, int button, int event)
{
	uiGetMouse(x,y,button,event); //GUI获取鼠标
	display(0,-2); // 刷新显示
}

// 旋转计时器
#define MY_ROTATE_TIMER  1

//新建窗口并显示内容                                                         //刘天成加 
void Create_windows( char  *including  )
{
		SetWindowTitle(including);
		InitGraphics();
		winwidth = GetWindowWidth();
        winheight = GetWindowHeight();
        startTimer(MY_ROTATE_TIMER, 50); 
} 

// 用户主程序入口
// 仅初始化执行一次
void Main() 
{
	// 初始化窗口和图形系统
	SetWindowTitle("Graphics User Interface Demo");
	//SetWindowSize(10, 10); // 单位 - 英寸
	//SetWindowSize(15, 10);
	//SetWindowSize(10, 20);  // 如果屏幕尺寸不够，则按比例缩小
    InitGraphics();

	// 获得窗口尺寸
    winwidth = GetWindowWidth();
    winheight = GetWindowHeight();

	// 注册时间响应函数
	registerCharEvent(CharEventProcess);        // 字符
	registerKeyboardEvent(KeyboardEventProcess);// 键盘
	registerMouseEvent(MouseEventProcess);      // 鼠标
//	registerTimerEvent(TimerEventProcess);      // 定时器

	// 开启定时器
	startTimer(MY_ROTATE_TIMER, 50);            

	// 打开控制台，方便用printf/scanf输出/入变量信息，方便调试
	// InitConsole(); 
}
