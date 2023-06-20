//===========================================================================
//**********************************������***********************************  
//===========================================================================
/*����textbox�Ķ�λ��
1.ʶ����� -> �ƶ���� -> չʾ��������
2.�����ڵĲ���(���˵��л���) 
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

// ȫ�ֱ���
static double winwidth, winheight;   // ���ڳߴ�
static int    show_more_buttons = 0; // �л�����˵����
static int    flag_for_about = 0;    // �л���Ϣ�� 
static int    copyflag = 0;          // �����б�� 
static int    shearflag = 0;         // �����б�� 
static int    pasteflag = 0;         // ճ���б�� 
static int    row=2;                   // ����� 
static char   memo[200][40];           // ���ڴ洢��������� 
static char   *store_point[50];        // ���ڴ洢ѡ������� 

// ����������provided in libgraphics
void DisplayClear(void); 
// ��ʱ������������provided in libgraphics
void startTimer(int id,int timeinterval);
void backclear(int x,int y,int fx,int fy);
void enterplus(int y,int x);
// �û�����ʾ����
void display(int, int); 
#if defined(DEMO_MENU)
// �˵���ʾ����
void drawMenu()
{   //�˵����� 
	static char * menuListFile[] = {"File",  
		"Open  | Ctrl-O", // ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β
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
	
    //���� 
	double fH = GetFontHeight();
	double x = 0;                                                //fH/8;
	double y = winheight;
	double h = fH*1.5;                                           // �ؼ��߶�
	double w = TextStringWidth(menuListHelp[0])*2;               // �ؼ����
	double wlist = TextStringWidth(menuListTool[3])*1.2;
	double xindent = winheight/20;                               // ����
	int    selection;                                            // �����ѡ����� 
	//�����ѡ��ճ���� 
	pasteflag = 0;
	
	// menu bar
	drawMenuBar(0,y-h,winwidth,h);
	// File �˵�                                                       
	selection = menuList(GenUIID(0), x, y-h, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
	if( selection==3 )
		exit(-1); // choose to exit
	
	// Tool �˵�   
	selection = menuList(GenUIID(0),x+w,  y-h, w, wlist,h, menuListTool,sizeof(menuListTool)/sizeof(menuListTool[0]));
	if( selection==1 )   copyflag = 1;                                                                                        //ʵ��COPY                                                           
    if( selection==2 )   shearflag = 1;                                                                                       //ʵ��SHEAR
	if( selection==3 )   pasteflag = 1;                                                                                       //ʵ��PASTE
		
	// Help �˵�
	menuListHelp[1] = show_more_buttons ? "Close Instructin | Ctrl-M" : "Instructin | Ctrl-M";                          //���ݹ�������л��˵���ʾ 
	menuListHelp[2] = flag_for_about ? "Close About      | Ctrl-A" : "About      | Ctrl-A";                             //���ݹ�������л��˵���ʾ
	selection = menuList(GenUIID(0),x+2*w,y-h, w, wlist, h, menuListHelp,sizeof(menuListHelp)/sizeof(menuListHelp[0]));
	if( selection==1 )                                                                                                  //ʵ�� 
		show_more_buttons = ! show_more_buttons;
	if( selection==2 )
		flag_for_about = ! flag_for_about;

	// ��ʾ�˵����н��                            //����ɼ� 
	if(show_more_buttons)                          //Instruction ��ʾ����   
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
    if(flag_for_about)                             //About ��ʾ���� 
	{
	x=winwidth*0.72;
	y=winheight*0.45;
	SetPenColor("Black");
    drawLabel(x-7*h, y-=h, "**************C Code Editer**************");
    drawLabel(x-7*h, y-=h, "********����һ��ţ�Ƶ��������༭��*******");
    }
}
#endif // #if defined(DEMO_MENU)

#if defined(DEMO_EDITBOX)
// �ı��༭��ʾ����

//������                                                                    //�����ݼ�
void enterplus(int hhy,int hhx)                                            
{                       
	char t[20];
	int i;
	double fH = GetFontHeight();
	double h = fH*1.5; // �ؼ��߶�
	double w = winwidth; // �ؼ����
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
	        drawLabel(0.05,winheight-h*(i+2),s);//�к� 
			textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
		}
//	for (i=hhy;i<=row-2;i++){
//		 sprintf(s, "%d", i+1);
//	    drawLabel(0.05,winheight-h*(i+2),s);
//	   textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
//	}
//	textbox(GenUIID(i), 0, winheight-h*(hhy+1), w, h, memo[hhy-1], sizeof(memo[0]));
} 
void backclear(int x,int y,int fx,int fy)//���øú�������ֱ�Ӱ����з��飨��ѡ�У�����ɾȥ������ȫ�ֵ�memoʵ�� 
{
	int miny=fy,maxy=y,minx=fx,maxx=x;
	double fH = GetFontHeight();
	double h = fH*1.5; // �ؼ��߶�
	double w = winwidth; // �ؼ����
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
		for (i=miny;i<maxy-1;i++) memset(memo[i],'\0',sizeof(memo[i]));//����м��� 
		memset(&(memo[miny-1][minx]),'\0',strlen(memo[miny-1])-minx);//����϶��� 
		for (i=0;i<strlen(memo[maxy-1])-maxx;i++)
			if (memo[maxy-1][i+maxx]!='\0') 
			memo[maxy-1][i]=memo[maxy-1][i+maxx];
			else memo[maxy-1][i]='\0';
		for (i=strlen(memo[maxy-1])-maxx;i<=strlen(memo[maxy-1]);i++)
           	memo[maxy-1][i]='\0';//����������  
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
	        drawLabel(0.05,winheight-h*(i+2),s);//�к� 
			textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
		}//���forѭ��������Ϊ������ģ�飬��Ҫ����ˢ�¹��λ�� 
}
//���ÿ�е�TEXTBOX                                                          //�����ݼ� 
void drawEditText(int key, int event)
{
	static char results[200] = "";
	static char newnum[80] = "";
	double fH = GetFontHeight();
	double h = fH*1.5; // �ؼ��߶�
	double w = winwidth; // �ؼ����
	double x = winwidth/15;
	double y = winheight/2-h;
	int i=0, hhy=0,hhx=0;//���л����п�ͷ��backspaceʱ��y������ʱ��x 
	char s[40];
		int fmx,fmy,mx,my;
		mx=textbox(GenUIID(0),-10, winheight, w, h, memo[i], sizeof(memo[0])); 
		my=textbox(GenUIID(0),-20, winheight, w, h, memo[i], sizeof(memo[0])); 
		fmx=textbox(GenUIID(0),-30, winheight, w, h, memo[i], sizeof(memo[0])); 
		fmy=textbox(GenUIID(0),-40, winheight, w, h, memo[i], sizeof(memo[0]));
		for (i=0;i<=row-2;i++){
			sprintf(s, "%d", i+1);
	        drawLabel(0.05,winheight-h*(i+2),s);//�к� 
			textbox(GenUIID(i), 0.3, winheight-h*(i+2), w, h, memo[i], sizeof(memo[0]));
		}//ˢ�¹��λ�� 
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
					backclear(mx,my,fmx,fmy);//�������ѡ�񰴻��У���ô��ɾ�� 
					if (my<fmy) enterplus(my,mx);
					else enterplus(fmy,fmx);//�ٻ��� 
				}else {
					if (mx<fmx) {
					for (i=mx;i<=strlen(memo[my-1])-fmx-1+mx;i++)
						memo[my-1][i]=memo[my-1][i+fmx-mx];
						memset(&(memo[my-1][mx+strlen(memo[my-1])-fmx]),'\0',fmx-mx);
				    }//������ʾ��ʵ��Ч����һ�£�ʵ��Ч����devC++����һ�� 
					if (mx>fmx) {
					for (i=fmx;i<=strlen(memo[my-1])-mx-1+fmx;i++)
						memo[my-1][i]=memo[my-1][i+mx-fmx];
						memset(&(memo[my-1][fmx+strlen(memo[my-1])-mx]),'\0',mx-fmx);
				    }
					//������ѡ����һ�������У���ô��ɾ�� 
					if (mx<fmx) enterplus(my,mx);
					else enterplus(fmy,fmx);//�ٻ��� 
				}
				break;
		}
	} 		
//	if (row>=3 && key==VK_BACK && event==KEY_DOWN && fmx==-1) {
//		for (i=0;i<=row-2;i++){
//			sprintf(s, "%d", i+1);
//	        drawLabel(0.05,winheight-h*(i+2),s);//�к� 
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
//	 	hhy=i+1;//�ڼ��� 
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
	// ����
	DisplayClear();

#if defined(DEMO_EDITBOX)
	// �༭�ı��ַ���
	drawEditText(x,y);
#endif

#if defined(DEMO_MENU)
	// ���ƺʹ���˵�
	drawMenu();
#endif
}

// �û����ַ��¼���Ӧ����
void CharEventProcess(char ch)
{
	uiGetChar(ch); // GUI�ַ�����
	display(0,-2); //ˢ����ʾ
}

// �û��ļ����¼���Ӧ����
void KeyboardEventProcess(int key, int event)
{
	double fH = GetFontHeight();
	double h = fH*1.5; // �ؼ��߶�
	uiGetKeyboard(key,event); // GUI��ȡ����
	double w = winwidth; // �ؼ����

	display(key,event); // ˢ����ʾ
}

// �û�������¼���Ӧ����
void MouseEventProcess(int x, int y, int button, int event)
{
	uiGetMouse(x,y,button,event); //GUI��ȡ���
	display(0,-2); // ˢ����ʾ
}

// ��ת��ʱ��
#define MY_ROTATE_TIMER  1

//�½����ڲ���ʾ����                                                         //����ɼ� 
void Create_windows( char  *including  )
{
		SetWindowTitle(including);
		InitGraphics();
		winwidth = GetWindowWidth();
        winheight = GetWindowHeight();
        startTimer(MY_ROTATE_TIMER, 50); 
} 

// �û����������
// ����ʼ��ִ��һ��
void Main() 
{
	// ��ʼ�����ں�ͼ��ϵͳ
	SetWindowTitle("Graphics User Interface Demo");
	//SetWindowSize(10, 10); // ��λ - Ӣ��
	//SetWindowSize(15, 10);
	//SetWindowSize(10, 20);  // �����Ļ�ߴ粻�����򰴱�����С
    InitGraphics();

	// ��ô��ڳߴ�
    winwidth = GetWindowWidth();
    winheight = GetWindowHeight();

	// ע��ʱ����Ӧ����
	registerCharEvent(CharEventProcess);        // �ַ�
	registerKeyboardEvent(KeyboardEventProcess);// ����
	registerMouseEvent(MouseEventProcess);      // ���
//	registerTimerEvent(TimerEventProcess);      // ��ʱ��

	// ������ʱ��
	startTimer(MY_ROTATE_TIMER, 50);            

	// �򿪿���̨��������printf/scanf���/�������Ϣ���������
	// InitConsole(); 
}
