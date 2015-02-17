#include <windows.h>
#include "resource.h" 
#include <string.h>
#include "mystruct.h"
#include <stdio.h>


static TCHAR* tcVersion = "ATU v1.2";


//-------------------------------------------------------
// Extern functions
//-------------------------------------------------------
extern VOID MyWait(inty64);
extern inty64 us100; //broi taktove za 100us
extern inty64 Takt(void);
extern VOID SetAllTune(void);
extern VOID SetAnt(int);//0 do 5
extern VOID SetC1(int);  //0 do 255
extern VOID SetC2(int);  //0 do 255 
extern VOID SetL(int);  // 0 do 255
extern VOID SetThrough(BOOL);
extern long double GetSwr(void); //Vrushta SWR, 
extern VOID ChangeMode(HWND, char);//smqna wida na rabota
extern VOID SwitchOffTrc(void);
extern BOOL RadioCheck(HWND);//vrushta TRUE ako ima radio i FALSE ako niama,
extern inty64 Period();

extern VOID ChangeBand(HWND, char, char); //Funkciq za smqna na banda
extern VOID BaudRate(HWND, HINSTANCE, int); //naglasqm Baude rate-a
extern char FmOn(HWND, int); //icom.c--->, vrushta wida na rabota
extern VOID FmOff(HWND, char); //icom.c(hwnd, wid na rabota)
extern VOID SwrOn(void);//swr.c
extern VOID SwrOff(void);//swr.c
extern VOID CloseCom(void);//icom.c
extern VOID TunePa(BOOL);
extern int  TunerTest(void);
extern int  TestGetSwr(void);


extern char szComPort[20] = "Com1";

// Access to LPT
extern int LoadIODLL();



WNDPROC OldScroll[3];//polzva se za ScrollButonite
WNDPROC OldScrollAnt[5];//polzva se za Edit butonite v  AntBox
WNDPROC OldScrollTune[10];//polzva se za ManualBox da raboti TAB-a



//-------------------------------------------------------
// Functions
//-------------------------------------------------------

//Funkcii za Save Open i dr..........
VOID  MyFileInitialize(HWND);
BOOL  MyFileOpen (HWND , PTSTR , PTSTR);
BOOL  MyFileSave (HWND , PTSTR , PTSTR);

void DisableButtons(HWND hwnd);//tova e funkciqta za disable/enable buttons 
//pri startirane na programata kato chete ot bCheckBox

VOID AutoTuneNow(HWND); //(handle, iTune) prashtam mu iTune i vruashta noviq iTune
void ResetBandButtons(void);
void ResetAntButtons(void);

void vSaveSettings(HANDLE hFile); 
void vLoadSettings(HANDLE hFile);

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

//Proc za dialogboxes
BOOL CALLBACK AboutSaveChanges(HWND, UINT, WPARAM, LPARAM) ;//za SAve changes: YES/NO?
BOOL CALLBACK AboutHelpProc(HWND, UINT, WPARAM, LPARAM);//za Helpa
BOOL CALLBACK AboutAntNames(HWND, UINT, WPARAM, LPARAM);//za BOX-a s ant names
BOOL CALLBACK AboutManualBoxProc (HWND , UINT, WPARAM , LPARAM); //Za About
BOOL CALLBACK AboutAutoTuneBox(HWND, UINT, WPARAM, LPARAM); //Za Tune Box
BOOL CALLBACK AboutPwr(HWND, UINT, WPARAM, LPARAM);//za PWR box-a
BOOL CALLBACK AboutCommPortName(HWND, UINT, WPARAM, LPARAM);//dialog v koito se vpisva imeto na COM1 ako ne moje da go 
BOOL CALLBACK AboutCommPortName1(HWND, UINT, WPARAM, LPARAM);//bliznak na gornoto
BOOL CALLBACK AboutSwrDialog(HWND, UINT, WPARAM, LPARAM);//dialog koito pokazva stulba s SWR
BOOL CALLBACK AboutButtonDisable(HWND, UINT, WPARAM, LPARAM);//DialogBox v koito mojesh da deactivirash kopcheta.

//Proc za TAB klavisha
LRESULT CALLBACK ScrollProc (HWND, UINT, WPARAM, LPARAM) ;// Procedura za TAB na Scroll controlite
LRESULT CALLBACK ScrollProcAnt (HWND, UINT, WPARAM, LPARAM) ;// Procedura za TAB na Ant BOX
LRESULT CALLBACK ScrollProcManual (HWND, UINT, WPARAM, LPARAM) ;// Procedura za TAB na ManualBox


//-------------------------------------------------------
// Variables
//-------------------------------------------------------
BOOL    bRadio;//ako TRUE ima radio ako FALSE niama radio; taka che da moga da mahna FmOn kudeto chakam Info ot radioto

char    szBuffy1[4];//szBuffy1- za mnogo neshta

OPENFILENAME ofn;
char    charMode;//priema ot FmOn wida na rabota i vrushta v FmOff;
HANDLE  hFile;
DWORD   uli;
BOOL    bMenuTuneBox=1;//kogato se otvori ManualTuneBOx ot menuto da ne  minava na predavane;
BOOL    bCw;//za da razbera kakvi nastroiki da puska na icoma(za udobstvo), 1-ON, 0-OFF
BOOL    bTuner=0;// ako e TRue ima Tuner ako e False-niama
int     iCw;
static double dSWR=17;//ksw-to

static TCHAR  sztesting[50];
static TCHAR szTest[50];

BOOL    bCheckBox[23];// Tova e masiva koito pokazva koi kopcheta da sa Disable/Enable; //1-disable; 0-enable

int iZelena; //tfa e za zelenata tochka v manual boxa

   
//za butonite i displeia
HWND    hwndMy[31],hwndBand, hwndSwr, hwndAnt, hwndC1,hwndC2,hwndL;   
HWND    hwndAntBack, hwndBandBack, hwndSwrBack;

//za AboutTuneBox, za da ne miga ekrana
RECT    rectTochka;
RECT    rectStulb;//Tova e za stulbcheto v Dialog Box hSwr


RECT    rectTx;
BOOL    bTransmit=FALSE;//Za da sveti TX dokato e otvoren MAnualBox
BOOL    bSave=0;//ako e TRUE pita dali da savne promenite
BOOL    bTuneBox=FALSE;//polzva se za da ne moje da se otvarqt
// poveche ot edin prozorec( ManualTune)
int     iBand=0,iAnt=0, iTune=0;
BOOL    bCommQuit=0;//v AboutCommPortName za da moje da zatvorq napulno ATU
int     iPower = 5;
int     iPaPower = 81;  // if(iPaPower>80) nastroiva sys 80w vij icom>

static  TCHAR ant[6][25];   //Texts for antenna buttons
int     iLastBand = -1;     // Show which was the last selected band
int     iLastAnt = -1;      // Show which was the last selected ant


static HWND hwndTuneBox;
static int  idFocus, idFocusAnt, idFocusManual ;//polzva sa za Scroll butonite i za AntBox(edit butonite)





int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     

	 HACCEL       hAccel; 
	 static TCHAR szAppName[] = TEXT ("Shablon") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;
     

     wndclass.style         =/* CS_HREDRAW |CS_VREDRAW |*/CS_OWNDC;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, TEXT("atu")) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (COLOR_BACKGROUND + 1) ;
     wndclass.lpszMenuName  = TEXT("menu") ;
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindow (szAppName, tcVersion,
                          WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
                          100, 20,
                          402, 200,
                          NULL, NULL, hInstance, NULL) ;
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     us100=Takt();//us100 stava ravno na broi taktove za 100 micro sec
     hAccel = LoadAccelerators (hInstance,(LPCTSTR) IDR_ACCELERATOR1) ;
	 
	 while (GetMessage (&msg, NULL, 0, 0))
     {
         //Quick buttons are ON
         if( bCheckBox[22] == 0 )
         {
             if (!TranslateAccelerator (hwnd, hAccel, &msg))
             {
                 TranslateMessage (&msg) ;
                 DispatchMessage (&msg) ;
             }
         }
         //Quick buttons are OFF
         else
         {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
         }
          
     }
     return msg.wParam ;

}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     
     //BITMAP things
	 static HBITMAP hBitmap;
	 static BITMAP         bitmap;
     HDC            hdcMem;
 
	
	//DC stuff
	 HDC			hdc ;	 
	 static HBRUSH         hBrushStatic,hBrushStatic1;
	 
	 
	 //Different handles
	 //static HWND		 	hwndMy[29];
	 HMENU					hMenu;
	 //-------------


	 //Some Structures
	 PAINTSTRUCT		ps;	 
	 //My integers
     static int			    i;   
	
	 //za OpenFile i savefile 
	 static TCHAR     szFileName[MAX_PATH], szTitleName[MAX_PATH] ;
	 
	 
	 //My Buffers
	 static TCHAR			szBuffy[50]; //ZA mnogo neshta
	 
	 
	 //Antennas
	 static TCHAR szAnt[20]=TEXT("NULL");
	 //MHZ
	 static TCHAR szMhz[10]=TEXT("NULL");


	 //HANDLE				   hAccel ;
	 static HINSTANCE      hInstance ;
	 
	  
	 //TX rectangle
	 rectTx.left=260;
	 rectTx.top=52;
	 rectTx.right=290;
	 rectTx.bottom=70;
	 //----------------------------------------------------
     	
	//rect za stulbcheto v Dilog BOx hSWr
	 rectStulb.left=46;
	 rectStulb.right=130;
	 rectStulb.top=0;
	 rectStulb.bottom=211;

     
	

	 switch (message)
     {
     
     
	 case WM_CREATE:
		 ///RegisterClassEx(&myclass);
		 hBrushStatic=CreateSolidBrush(RGB(19,165,226));
         hBrushStatic1=CreateSolidBrush(RGB(125,125,125));
		  
		 LoadIODLL();//Loadvam IOdll
		 
		 hInstance=(HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE); //za create window go polzvam
         //-----------------------------------
           
        //----------TEMP
		hBitmap=LoadBitmap(hInstance, (LPCTSTR) IDB_BITMAP1 );
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);


        //------
		

          //suzdavam prozorcite
          //AUTO        
		  hwndMy[1]=CreateWindow(TEXT("button"), TEXT("AUTO"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 250, 17 , 50, 20, hwnd,
			  (HMENU) 1, hInstance, NULL);
		  
		  //Manual Tune, Tune PA, Carrier Through
		  hwndMy[2]=CreateWindow(TEXT("button"), TEXT("Manual Tune"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 305, 1 , 90, 17, hwnd,
			  (HMENU) 2, hInstance, NULL);

		  hwndMy[3]=CreateWindow(TEXT("button"), TEXT("Tune PA"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 305, 18 , 90, 17, hwnd,
			  (HMENU) 3, hInstance, NULL);

		  hwndMy[4]=CreateWindow(TEXT("button"), TEXT("Carrier"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 305, 35 , 90, 17, hwnd,
			  (HMENU) 4, hInstance, NULL);

		  //hwndMy[5]=CreateWindow(TEXT("button"), TEXT("Through"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 305, 52 , 90, 17, hwnd,
			//  (HMENU) 5, hInstance, NULL);

		  //Band Buttons
		  hwndMy[6]=CreateWindow(TEXT("button"), TEXT("1,8"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 3, 90 , 35, 20, hwnd,
			  (HMENU) 6, hInstance, NULL);
		  
		  hwndMy[7]=CreateWindow(TEXT("button"), TEXT("3,5"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 38, 90 , 35, 20, hwnd,
			  (HMENU) 7, hInstance, NULL);

		  hwndMy[8]=CreateWindow(TEXT("button"), TEXT("7"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 73, 90 , 35, 20, hwnd,
			  (HMENU) 8, hInstance, NULL);

		  hwndMy[9]=CreateWindow(TEXT("button"), TEXT("10"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 3, 110 , 35, 20, hwnd,
			  (HMENU) 9, hInstance, NULL);

		  hwndMy[10]=CreateWindow(TEXT("button"), TEXT("14"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 38, 110 , 35, 20, hwnd,
			  (HMENU) 10, hInstance, NULL);

		  hwndMy[11]=CreateWindow(TEXT("button"), TEXT("18"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 73, 110, 35, 20, hwnd,
			  (HMENU) 11, hInstance, NULL);

		  hwndMy[12]=CreateWindow(TEXT("button"), TEXT("21"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 3, 130 , 35, 20, hwnd,
			  (HMENU) 12, hInstance, NULL);

		  hwndMy[13]=CreateWindow(TEXT("button"), TEXT("24"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 38, 130 , 35, 20, hwnd,
			  (HMENU) 13, hInstance, NULL);

		  hwndMy[14]=CreateWindow(TEXT("button"), TEXT("28"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 73, 130 , 35, 20, hwnd,
			  (HMENU) 14, hInstance, NULL);
		  
		  //Antenna Buttons
		  hwndMy[15]=CreateWindow(TEXT("button"), ant[0], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 125, 90 , 70, 20, hwnd,
			  (HMENU) 15, hInstance, NULL);

		  hwndMy[16]=CreateWindow(TEXT("button"), ant[1], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 125, 110 , 70, 20, hwnd,
			  (HMENU) 16, hInstance, NULL);

		  hwndMy[17]=CreateWindow(TEXT("button"), ant[2], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 125, 130 , 70, 20, hwnd,
			  (HMENU) 17, hInstance, NULL);

		  hwndMy[18]=CreateWindow(TEXT("button"), ant[3], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 195, 90 , 70, 20, hwnd,
			  (HMENU) 18, hInstance, NULL);

		  hwndMy[19]=CreateWindow(TEXT("button"), ant[4], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 195, 110 , 70, 20, hwnd,
			  (HMENU) 19, hInstance, NULL);

		  hwndMy[20]=CreateWindow(TEXT("button"), ant[5], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 195, 130 , 70, 20, hwnd,
			  (HMENU) 20, hInstance, NULL);
		  
		  //SCROLLBAR   Tunning controls
		  
		  
					//C1
		  hwndMy[21]=CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD| WS_VISIBLE | WS_TABSTOP  | SBS_HORZ, 275, 90 , 115, 10, hwnd,
			  (HMENU) 21, hInstance, NULL);

		  SetScrollRange(hwndMy[21], SB_CTL, 1, 255, TRUE);
		  
		  
				//L
		  hwndMy[22]=CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_TABSTOP | WS_VISIBLE  | SBS_HORZ, 275, 115 , 115, 10, hwnd,
			  (HMENU) 22, hInstance, NULL);

		  SetScrollRange(hwndMy[22], SB_CTL, 1, 255, TRUE);
		  
					//C2
		  hwndMy[23]=CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_TABSTOP | WS_VISIBLE   | SBS_HORZ, 275, 140 , 115, 10, hwnd,
			  (HMENU) 23, hInstance, NULL);
		  //CW mode
		  hwndMy[24]=CreateWindow(TEXT("button"), TEXT("CW"), WS_CHILD  | WS_VISIBLE | BS_AUTORADIOBUTTON  , 199, 70 , 45, 15, hwnd,
			  (HMENU) 24, hInstance, NULL);

		  //SSB mode
		  hwndMy[25]=CreateWindow(TEXT("button"), TEXT("SSB"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT , 144, 70 , 45, 15, hwnd,
			  (HMENU) 25, hInstance, NULL);

		  SetScrollRange(hwndMy[23], SB_CTL, 1, 255, TRUE);
		 

//Adresa na starata winproc    //zamenqm WinProc za scrollbar-ovete
		  OldScroll[0] = (WNDPROC) SetWindowLong (hwndMy[21], 
                 /*nov address za winproc*/  GWL_WNDPROC, (LONG) ScrollProc);/*novata winproc*/
		  OldScroll[1] = (WNDPROC) SetWindowLong (hwndMy[22], 
                                             GWL_WNDPROC, (LONG) ScrollProc);
		  OldScroll[2] = (WNDPROC) SetWindowLong (hwndMy[23], 
                                            GWL_WNDPROC, (LONG) ScrollProc);
		  //-------------


		  //risuva prozorcite za SWR, ANT, BAND-----------------------------
		  hwndBandBack=CreateWindow (TEXT ("static"), NULL,//statichen prozorec samo za backgraounda
                                     WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER ,
                                    5, 5, 70, 50, 
                                    hwnd, (HMENU) 201, 
                                    hInstance, NULL) ;
		  
		  hwndBand=CreateWindow (TEXT ("static"), NULL,//statichen prozorec(maluk) samo za ispisvane na banda
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    27, 20, 20, 20, 
                                    hwnd, (HMENU) 202, 
                                    hInstance, NULL) ;
		  

		  hwndAntBack=CreateWindow (TEXT ("static"), NULL,//statichen prozorec samo za backgraounda
                                    WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
                                    80, 5, 100, 50, 
                                    hwnd, (HMENU) 203, 
                                    hInstance, NULL) ;
		  
		  hwndAnt=CreateWindow (TEXT ("static"), NULL,//statichen prozorec(maluk) samo za ispisvane na Ant
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    82, 20, 96, 20, 
                                    hwnd, (HMENU) 204, 
                                    hInstance, NULL) ;

		   hwndSwrBack=CreateWindow (TEXT ("static"), NULL,//statichen prozorec samo za backgraounda
                                    WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
                                    184, 5, 60, 50, 
                                    hwnd, (HMENU) 205, 
                                    hInstance, NULL) ;
		  
		  hwndSwr=CreateWindow (TEXT ("static"), NULL,//statichen prozorec(maluk) samo za ispisvane na SWr
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    204, 20, 20, 20, 
                                    hwnd, (HMENU) 206, 
                                    hInstance, NULL) ;
		  //-------------------------------------
		  
         //SetClassLong(hwndBand, GCL_HBRBACKGROUND, GetStockObject(WHITE_BRUSH));
		  //suzadava STATIC prozorcite za c1, l i c2
		  hwndC1=CreateWindow (TEXT ("static"), NULL,//statichen prozorec(maluk) samo za C1
                                    WS_CHILD | WS_VISIBLE | SS_CENTER ,
                                    305, 75, 60, 15, 
                                    hwnd, (HMENU) 207, 
                                    hInstance, NULL) ;
		  hwndL=CreateWindow (TEXT ("static"), NULL,//statichen prozorec(maluk) samo za C1
                                    WS_CHILD | WS_VISIBLE | SS_CENTER ,
                                    295, 100, 80, 15, 
                                    hwnd, (HMENU) 208, 
                                    hInstance, NULL) ;
		  hwndC2=CreateWindow (TEXT ("static"), NULL,//statichen prozorec(maluk) samo za C1
                                    WS_CHILD | WS_VISIBLE | SS_CENTER ,
                                    305, 125, 60, 15, 
                                    hwnd, (HMENU) 209, 
                                    hInstance, NULL) ;
		  

		  //-------------LOADING "CURRENT.ATU"------------	       
          hFile=CreateFile(TEXT("current.atu"), 
                           GENERIC_READ, 0,
			               NULL, OPEN_EXISTING, 0, NULL);
          if(hFile == INVALID_HANDLE_VALUE)
          {
              MessageBox(hwnd, TEXT("Error opening file with settings: \"current.atu\""), TEXT("Error"), MB_OK);
          }
          else
          {
              vLoadSettings(hFile); 
              CloseHandle(hFile);
          }
         
		//-----------------------------------------
         
	
		  //-----------Initialization--------------------
		     BaudRate(hwnd, hInstance, CBR_9600); //Slagam na 9600baud  rate za Icoma
		     if(bCommQuit) SendMessage(hwnd, WM_DESTROY, 0,0);//idva ot ABoutCOmmNAmes, sled kato izbera QUIT bCommQuit=>1 i prashtam oshte edin msg WM_DESTROY;
			 bRadio=RadioCheck(hwnd);//proverqva za vkliuchen transceiver
			 if(!bRadio)//ako niama radiokakvo da napravi
				 SendMessage(hwnd, WM_COMMAND, IDM_SETUP_RADIO, 0);//maham Radio On
	
			 SendMessage(hwnd, WM_COMMAND, ID_ANT1, 0);//slagam na Ant1
			 SendMessage(hwnd, WM_COMMAND, 10,0);//natiskam da e na 14mhz
			 SendMessage(hwnd, WM_COMMAND, 25,0);//slagab na SSB
			 SendMessage(hwndMy[25], BM_SETCHECK, 1,0);//slagam da e natisnato kopcheto na SSB mode
			 SendMessage(hwnd, WM_HSCROLL, 0,0);
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
             
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 
			 bTuner=TunerTest();
			 if(bTuner) MessageBox(hwnd, TEXT("Tuner initialized"), TEXT("Tuner initialized"), MB_OK);
             else MessageBox(hwnd, TEXT("Tuner NOT initialized!"), TEXT("Tuner NOT initialized!"), MB_OK);

			  //Slagam texta na ant buttonite:
			 SetWindowText(hwndMy[15], ant[0]);
		     SetWindowText(hwndMy[16], ant[1]);
		     SetWindowText(hwndMy[17], ant[2]);
		     SetWindowText(hwndMy[18], ant[3]);
		     SetWindowText(hwndMy[19], ant[4]);
		     SetWindowText(hwndMy[20], ant[5]);
			 
			 //Ako ima disable/enable buttoni ot tuk se izkliuchvat kato cheta masiva bCheckBox[];
			 DisableButtons(hwnd);
			 
			 //----End Initialization-------------
			 return 0;

	
	//za static prozorcite
	case WM_CTLCOLORSTATIC ://obrabotvam go za da se smeni cveta na prozorcite Band, Ant, Swr
		
		if((HWND)lParam==hwndAnt){
               SetTextColor ((HDC) wParam, RGB(0,0,240)) ;
               SetBkColor ((HDC) wParam, RGB(19,165,226));
			   return  (LRESULT) hBrushStatic;
		}
		else if((HWND)lParam==hwndBand){
               SetTextColor ((HDC) wParam, RGB(0,0,240)) ;
               SetBkColor ((HDC) wParam, RGB(19,165,226));
			   return (LRESULT)  hBrushStatic;
		}
		else if((HWND)lParam==hwndSwr){
               SetTextColor ((HDC) wParam, RGB(0,0,240)) ;
               SetBkColor ((HDC) wParam, RGB(19,165,226));           
			   return (LRESULT)  hBrushStatic;
		}
		
		else if((HWND)lParam==hwndBandBack){
               SetTextColor ((HDC) wParam, RGB(0,0,240)) ;
               SetBkColor ((HDC) wParam, RGB(19,165,226));   
			   return (LRESULT)  hBrushStatic;
		}
		else if((HWND)lParam==hwndAntBack){
               SetTextColor ((HDC) wParam, RGB(0,0,240)) ;
               SetBkColor ((HDC) wParam, RGB(19,165,226));             
			   return (LRESULT)  hBrushStatic;
		}
		else if((HWND)lParam==hwndSwrBack){
               SetTextColor ((HDC) wParam, RGB(0,0,240)) ;
               SetBkColor ((HDC) wParam, RGB(19,165,226));              
			   return (LRESULT)  hBrushStatic;
		}
		
		else if((HWND)lParam==hwndC1){ //Za C1
               SetTextColor ((HDC) wParam, RGB(255,160,255)) ;
               SetBkColor ((HDC) wParam, RGB(125,125,125));               
			   return (LRESULT)  hBrushStatic1;
		}
		else if((HWND)lParam==hwndL){//za L
               SetTextColor ((HDC) wParam, RGB(255,160,255)) ;
               SetBkColor ((HDC) wParam, RGB(125,125,125));             
			   return (LRESULT)  hBrushStatic1;
		}
		else if((HWND)lParam==hwndC2){//Za C2
               SetTextColor ((HDC) wParam, RGB(255,160,255)) ;
               SetBkColor ((HDC) wParam, RGB(125,125,125));     
			   return (LRESULT)  hBrushStatic1;
		}
		
		break;

     


    //Tuning
	case WM_HSCROLL : 
		i = GetWindowLong ((HWND) lParam, GWL_ID) ;
		
		switch (LOWORD (wParam))
		{

        

        
		case SB_LINELEFT:
			
			switch(i)
			{
			case 21: 
				bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
				if(Ant[iAnt].Band[iBand].Tune[iTune].c1>0)
					Ant[iAnt].Band[iBand].Tune[iTune].c1-=1;
				SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
				break;	
			case 22: 
				bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
				if(Ant[iAnt].Band[iBand].Tune[iTune].l>0)
					Ant[iAnt].Band[iBand].Tune[iTune].l-=1;
				SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
				break;
			case 23:
				bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
				if(Ant[iAnt].Band[iBand].Tune[iTune].c2>0)
					Ant[iAnt].Band[iBand].Tune[iTune].c2-=1;
				SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
				
				break;
			default:
				
				break;
			}
			
			
			break;
        
		case SB_LINERIGHT:
			bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
			switch(i)
			{
			case 21: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].c1<255)
					Ant[iAnt].Band[iBand].Tune[iTune].c1+=1;
				SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
				break;	
			case 22: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].l<255)
					Ant[iAnt].Band[iBand].Tune[iTune].l+=1;
				SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
				break;
			case 23: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].c2<255)
					Ant[iAnt].Band[iBand].Tune[iTune].c2+=1;
				SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
				break;
			default:
				break;
			}
			break;

		case SB_PAGELEFT:
		    bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
			switch(i)
			{
			case 21: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].c1>5)
					Ant[iAnt].Band[iBand].Tune[iTune].c1-=5;
				SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
				break;	
			case 22: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].l>5)
					Ant[iAnt].Band[iBand].Tune[iTune].l-=5;
				SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
				break;
			case 23: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].c2>5)
					Ant[iAnt].Band[iBand].Tune[iTune].c2-=5;
				SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
				break;
			default:
				break;
			}
			break;
		
		case SB_PAGERIGHT:
		    bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
			switch(i)
			{
			case 21: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].c1<251)
					Ant[iAnt].Band[iBand].Tune[iTune].c1+=5;
				
				SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
				break;	
			case 22: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].l<251)
					Ant[iAnt].Band[iBand].Tune[iTune].l+=5;
				SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
				break;
			case 23: 
				if(Ant[iAnt].Band[iBand].Tune[iTune].c2<251)
					Ant[iAnt].Band[iBand].Tune[iTune].c2+=5;
				SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
				break;
			default:
				break;
			}
			break;
		
		case SB_THUMBTRACK:
		    bSave=1;//imam smenena nastroika i izliza prozorec Save Changes
			switch(i)
			{
			case 21:
				Ant[iAnt].Band[iBand].Tune[iTune].c1=HIWORD (wParam);
				SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
				break;	
			case 22: Ant[iAnt].Band[iBand].Tune[iTune].l=HIWORD (wParam);
				SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
				break;
			case 23: Ant[iAnt].Band[iBand].Tune[iTune].c2=HIWORD (wParam);
				SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
				break;
			default:
				break;
			}
			break;
		
		default :
			break;
		}
		
		
	    
	
		//Izprashta na TuneBox novite danni
		PostMessage(hwndTuneBox, WM_PAINT, 0, 0); 
		//-----------
		SetScrollPos  (hwndMy[21], SB_CTL, Ant[iAnt].Band[iBand].Tune[iTune].c1, TRUE);
		wsprintf(szBuffy, TEXT("C1=%4d"), Ant[iAnt].Band[iBand].Tune[iTune].c1*5);
		SetWindowText(hwndC1, szBuffy);
		 
		SetScrollPos  (hwndMy[22], SB_CTL, Ant[iAnt].Band[iBand].Tune[iTune].l, TRUE);
		wsprintf(szBuffy, TEXT("L=%5d"), Ant[iAnt].Band[iBand].Tune[iTune].l*100);
		SetWindowText(hwndL, szBuffy);
		
		SetScrollPos  (hwndMy[23], SB_CTL, Ant[iAnt].Band[iBand].Tune[iTune].c2, TRUE);
		wsprintf(szBuffy, TEXT("C2=%4d"), Ant[iAnt].Band[iBand].Tune[iTune].c2*5);
		SetWindowText(hwndC2, szBuffy);
        
		return 0;
		

		

	case WM_PAINT:
		  hdc=BeginPaint (hwnd, &ps) ;
          //temp
		  hdcMem=CreateCompatibleDC(hdc);
		  SelectObject(hdcMem, hBitmap);
		  StretchBlt(hdc, 6, 60,  bitmap.bmWidth, bitmap.bmHeight,
			  hdcMem, 0,0,bitmap.bmWidth,bitmap.bmHeight, SRCCOPY);
		  
		  //ACTIVATInG TRANSMIT dislpay  
		  if(bTransmit==TRUE){
			  SaveDC(hdc);
			  SetBkColor(hdc, RGB(218, 0, 0));
		      SetTextColor(hdc, 0x00);
		      TextOut(hdc, 260, 52, TEXT(" TX  "), 5);
		      RestoreDC(hdc, -1);
		  }  
		  
          DeleteDC(hdc);
		  EndPaint (hwnd, &ps) ;
          DeleteDC(hdcMem);//temp BITMAP stuff
		  return 0;
	 
	 
               
     case WM_COMMAND:
		  //hdc=GetDC(hwnd);
		  hMenu=GetMenu(hwnd);
          //proverqvam ako e natisnato nqkoe disable-nato kopche da vrushta, bez da pravi nishto
		  if(LOWORD(wParam)==ID_BAND1 && bCheckBox[0]==1) break;	
		  else if(LOWORD(wParam)==ID_BAND2 && bCheckBox[1]==1) break;
		  else if(LOWORD(wParam)==ID_BAND3 && bCheckBox[2]==1) break;
		  else if(LOWORD(wParam)==ID_BAND4 && bCheckBox[3]==1) break;
		  else if(LOWORD(wParam)==ID_BAND5 && bCheckBox[4]==1) break;
		  else if(LOWORD(wParam)==ID_BAND6 && bCheckBox[5]==1) break;
		  else if(LOWORD(wParam)==ID_BAND7 && bCheckBox[6]==1) break;
		  else if(LOWORD(wParam)==ID_BAND8 && bCheckBox[7]==1) break;
		  else if(LOWORD(wParam)==ID_BAND9 && bCheckBox[8]==1) break;
		  else if(LOWORD(wParam)==ID_ANT1 && bCheckBox[9]==1) break;
		  else if(LOWORD(wParam)==ID_ANT2 && bCheckBox[10]==1) break;
		  else if(LOWORD(wParam)==ID_ANT3 && bCheckBox[11]==1) break;
		  else if(LOWORD(wParam)==ID_ANT4 && bCheckBox[12]==1) break;
		  else if(LOWORD(wParam)==ID_ANT5 && bCheckBox[13]==1) break;
		  else if(LOWORD(wParam)==ID_ANT6 && bCheckBox[14]==1) break;
		  //----------------------

        
		  switch(LOWORD (wParam))
		  {
			 
		  
		  
		  //Menu
		  case IDM_HELP_INDEX: //INDEX
			 
			 DialogBox (hInstance, (LPCTSTR)IDD_DIALOG1, hwnd, AboutHelpProc);
			  break;

		  
		  
		  case IDM_SETUP_ANT:     //Ant dialog box
			  
	          
	          
			  
			  DialogBox(hInstance,(LPCTSTR) 109, hwnd, AboutAntNames);	
	          
			  break;
		  
		  
		  
          case ID_SETUP_PATUNEPWR://Tune PWR  dialog box za PA(sushtiq dialog kakto i TunePwr)
              bSave=1; //ako se otvori menuto, posle pita za Save?
			  DialogBoxParam(hInstance,(LPCTSTR)IDD_PWR, hwnd, AboutPwr,1);
          //5-ia parametyr(1) pokazva che iskam da se otvori PaPower box-a
			  break;

		  case IDM_SETUP_TUNE:    //Tune PWR dialog box
              bSave=1; //ako se otvori menuto, posle pita za Save?
			  DialogBoxParam(hInstance,(LPCTSTR)IDD_PWR, hwnd, AboutPwr,2);
              //5-ia parametyr(0) pokazva che  iskam da se otvori TunePower box-a
			  break;

		  
		  case IDM_COMM_PORT://izbirash si ako iskash nov addres za Commport-a

			 DialogBox(hInstance, (LPSTR)IDD_COMM1, hwnd, AboutCommPortName1);
			 CloseCom();
			 BaudRate(hwnd, hInstance, CBR_9600); //Slagam na 9600baud  rate za Icoma
		     if(bCommQuit) 
                 SendMessage(hwnd, WM_DESTROY, 0,0);//idva ot ABoutCOmmNAmes, sled kato izbera QUIT bCommQuit=>1 i prashtam oshte edin msg WM_DESTROY;
			 bRadio=RadioCheck(hwnd);//proverqva za vkliuchen transceiver
			 if(!bRadio)//ako niama radiokakvo da napravi
				 SendMessage(hwnd, WM_COMMAND, IDM_SETUP_RADIO, 0);//maham Rad
			  break;
		  
		  
		  case IDM_SETUP_BOX:   
              
			  bMenuTuneBox=0;//kogato se otvori ManualTuneBOx ot menuto da ne  minava na predavane;
			  hwndTuneBox=CreateDialog(hInstance, (LPCTSTR)IDD_TUNEBOX, hwnd, AboutManualBoxProc);
		       
			  break;
		  
		  
		  case IDM_HELP_ABOUT:  //about
			 
			  DialogBox (hInstance, (LPCTSTR) IDD_DIALOG, hwnd, AboutHelpProc);
				  break;
			  
		  

		  case IDM_HELP:   //help
			  DialogBox (hInstance, (LPCTSTR) IDD_DIALOG1, hwnd, AboutHelpProc);
			  break;
			  
		  	  
		  case IDM_FILE_SAVEAS:  //Save as dialog box
              
			  MyFileInitialize(hwnd);
			  if(MyFileSave(hwnd, szFileName,szTitleName))
              {
                  hFile=CreateFile(szFileName, GENERIC_WRITE, 0,
				                   NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			      if(hFile == INVALID_HANDLE_VALUE)
                  {
                      MessageBox(hwnd, TEXT("Error Saving File"), TEXT("Error"), MB_OK);
                      CloseHandle(hFile);
				      return 0;
				  }
			 
                 vSaveSettings(hFile); // save program settings to file
			   
				 CloseHandle(hFile);
			  }
			  

			  
			  
              
			 
			  break;
		  
		  case IDM_FILE_SAVE: //Save
			  
		      bSave = 0;//da ne pita za DO YOU want TO save sled kato veche sum savnal.

              hFile = CreateFile(TEXT("current.atu"), GENERIC_WRITE, 0,
				                 NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			  if(hFile ==INVALID_HANDLE_VALUE)
              {
				  MessageBox(hwnd, TEXT("Error Saving File"), TEXT("Error"), MB_OK);
                  CloseHandle(hFile);
				  break;		      
			  }
              
			  vSaveSettings(hFile); // Write program settings to file
			  CloseHandle(hFile);


              break;
		 
		  
		  case IDM_FILE_OPEN: //OPen
			  
			  MyFileInitialize(hwnd);
              if(MyFileOpen(hwnd, szFileName, szTitleName))
              {
                  hFile = CreateFile(szFileName, GENERIC_READ, 0,
				                   NULL, OPEN_EXISTING, 0, NULL);
			      if(hFile == INVALID_HANDLE_VALUE)
                  {
				      MessageBox(hwnd, TEXT("Error Opening File"), TEXT("Error"), MB_OK);
                      CloseHandle(hFile);
				      break;
				  }
                  vLoadSettings(hFile);
			      CloseHandle(hFile);
			  }			   
			  break;
			  
		  case IDM_DISABLE://Menu BUTTON DISABLE
			  
			  DialogBox (hInstance, (LPCTSTR) IDD_DISABLE, hwnd, AboutButtonDisable);
			  
			  break;


			  
			  
			  //End Menu commands--------------------------------

		  
		  //MHZ
		  case ID_BAND1:
		  case 6:

             if( iLastBand != 160)
             {
                ResetBandButtons();
                iLastBand = 160;
             }
			 strcpy(szMhz, TEXT("1,8"));
			 iBand=0;
			 SetWindowText(hwndBand, TEXT("1,8"));	
			 SendMessage(hwndMy[6],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x1,(char)0x81);// 
			 else{
				 ChangeBand(hwnd,0x1,(char)0x85);//hwnd, iMhz, iKhz
			     if(!bTuneBox) ChangeMode(hwnd, 0);
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND2:
		  case 7:

             if( iLastBand != 80)
             {
                ResetBandButtons();
                iLastBand = 80;
             }

			 strcpy(szMhz, TEXT("3,5"));
			 iBand=1;
			 SetWindowText(hwndBand, TEXT("3,5"));	
			 SendMessage(hwndMy[7],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x3,0x52);//spored izbraniqt wid na rabota, se naglasqva i chestotata
			 else{
				 ChangeBand(hwnd,0x3,0x75);//ssb chestota
			     if(!bTuneBox) ChangeMode(hwnd, 0);//da ne smenq mode-a kogato TuneBOx-a e vkliuchen, zashtoto minava 
				 //na USB po vreme na predavane i ne otchita KSW-to vqrno
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND3:
		  case 8:

             if( iLastBand != 40)
             {
                ResetBandButtons();
                iLastBand = 40;
             }

             strcpy(szMhz, TEXT("7,0"));
			 iBand=2;
			 SetWindowText(hwndBand, TEXT("7,0"));	
			 SendMessage(hwndMy[8],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x7, 0x02);//cw
			 else{
				 ChangeBand(hwnd,0x7, 0x07);//ssb
				 if(!bTuneBox) ChangeMode(hwnd, 0);
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND4:
		  case 9:

             if( iLastBand != 30)
             {
                ResetBandButtons();
                iLastBand = 30;
             }

             strcpy(szMhz, TEXT("10"));
			 iBand=3;
			 SetWindowText(hwndBand, TEXT("10"));	
			 SendMessage(hwndMy[9],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x10,0x10);
			 else    ChangeBand(hwnd,0x10,0x10);
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND5:
		  case 10:

             if( iLastBand != 20)
             {
                ResetBandButtons();
                iLastBand = 20;
             }
             strcpy(szMhz, TEXT("14"));
			 iBand=4;
			 SetWindowText(hwndBand, TEXT("14"));	
             SendMessage(hwndMy[10],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x014,0x02);
			 else{
				 ChangeBand(hwnd,0x014,0x20);
				 if(!bTuneBox) ChangeMode(hwnd, 1);
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND6:
		  case 11:

             if( iLastBand != 17)
             {
                ResetBandButtons();
                iLastBand = 17;
             }

             strcpy(szMhz, TEXT("18"));
			 iBand=5;
			 SetWindowText(hwndBand, TEXT("18"));	
			 SendMessage(hwndMy[11],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x18,0x08);
			 else{
				 ChangeBand(hwnd,0x18,0x14);
				 if(!bTuneBox) ChangeMode(hwnd, 1);
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND7:
		  case 12:

             if( iLastBand != 15)
             {
                ResetBandButtons();
                iLastBand = 15;
             }

             strcpy(szMhz, TEXT("21"));
			 iBand=6;
			 SetWindowText(hwndBand, TEXT("21"));	
			 SendMessage(hwndMy[12],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x21,0x03);
			 else{
				 ChangeBand(hwnd,0x21,0x25);
				 if(!bTuneBox) ChangeMode(hwnd, 1);
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_BAND8:
		  case 13: //24mhz

             if( iLastBand != 12)
             {
                ResetBandButtons();
                iLastBand = 12;
             }
			 strcpy(szMhz, TEXT("24"));
			 iBand=7;
			 SetWindowText(hwndBand, TEXT("24"));	
			 SendMessage(hwndMy[13],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x24,(char) 0x90);
			 else{
				 ChangeBand(hwnd,0x24,(char) 0x96);
				 if(!bTuneBox) ChangeMode(hwnd, 1);
			 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
          
		  case ID_BAND9:
		  case 14:

             if( iLastBand != 10)
             {
                ResetBandButtons();
                iLastBand = 10;
             }

             strcpy(szMhz, TEXT("28"));
			 iBand=8;
			 SetWindowText(hwndBand, TEXT("28"));	
			 SendMessage(hwndMy[14],BM_SETSTATE,1,0);//Pravi go da izglejda natisnat buttona
			 if(bCw) ChangeBand(hwnd,0x28,0x03);
			 else{
				 ChangeBand(hwnd,0x28,0x45);
				 if(!bTuneBox) ChangeMode(hwnd, 1);
				 }
			 SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		//--------------------------------------------------
		  
		 
		  
		  
		//ANt BUTTONS  
		  
		  case ID_ANT1:  
		  case 15:

             if( iLastBand != 1)
             {
                ResetAntButtons();
                iLastBand = 1;
             }

			 strcpy(szAnt, ant[0]);
			 iAnt=0;
			 SendMessage(hwndMy[15],BM_SETSTATE,1,0);//pravi se za da stoi natisnat klavisha 
			 SetWindowText(hwndAnt, szAnt);
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 //--Comandi kum Tunera
			 break;
		  
		  case ID_ANT2:
		  case 16:

             if( iLastBand != 2)
             {
                ResetAntButtons();
                iLastBand = 2;
             }
			 strcpy(szAnt, ant[1]);
			 iAnt=1;
			 SendMessage(hwndMy[16],BM_SETSTATE,1,0);
			 SetWindowText(hwndAnt, szAnt);
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak 
			 break;
		  
		  case ID_ANT3:
		  case 17:
             if( iLastBand != 3)
             {
                ResetAntButtons();
                iLastBand = 3;
             }

			 strcpy(szAnt, ant[2]);
			 iAnt=2;
			 SendMessage(hwndMy[17],BM_SETSTATE,1,0);
			 SetWindowText(hwndAnt, szAnt);
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak 
			 break;
		  
		  case ID_ANT4:
		  case 18:

             if( iLastBand != 4)
             {
                ResetAntButtons();
                iLastBand = 4;
             }

			 strcpy(szAnt, ant[3]);
			 iAnt=3;
			 SendMessage(hwndMy[18],BM_SETSTATE,1,0);
			 SetWindowText(hwndAnt, szAnt);
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_ANT5:
		  case 19:

             if( iLastBand != 5)
             {
                ResetAntButtons();
                iLastBand = 5;
             }
			 strcpy(szAnt, ant[4]);
			 iAnt=4;
			 SendMessage(hwndMy[19],BM_SETSTATE,1,0);
			 SetWindowText(hwndAnt, szAnt);
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
		  
		  case ID_ANT6:
		  case 20:

             if( iLastBand != 6)
             {
                ResetAntButtons();
                iLastBand = 6;
             }
			 strcpy(szAnt, ant[5]);
			 iAnt=5;
			 SendMessage(hwndMy[20],BM_SETSTATE,1,0);
			 SetWindowText(hwndAnt, szAnt);
		     SetFocus(hwnd);//pravi se za da ne se vrushta butona kogato se natisne pak
			 break;
			  
		//--------------------------------------			   	   
		  case 24://CW mode
              ChangeMode(hwnd, 3);
			  bCw=1;//vkliuchvam na CW(prashta na Icoma chestoti za CW i mode-a  e CW);
			  iCw=1;
			  //iTune=lastAnt[iAnt].lastBand[iBand].lastCw[iCw];//Chete ot polsednite nastroiki za Band,Ant,mode			 
			  SendMessage(hwnd, WM_COMMAND, iBand+ID_BAND1, 0);// ??????
			  //;//polzvam go za zapomnqne na posledna nastroika sprqmo Ant->Band->mode
			  break;
		  case 25://SSB mode
              if(iBand>3) ChangeMode(hwnd, 1);
			  else ChangeMode(hwnd, 0);
			  bCw=0;//analogichno na gornoto
			  iCw=0;//polzvam go za zapomnqne na posledna nastroika sprqmo Ant->Band->mode
		      //iTune=lastAnt[iAnt].lastBand[iBand].lastCw[iCw];//Chete ot polsednite nastroiki za Band,Ant,mode
			  SendMessage(hwnd, WM_COMMAND, iBand+ID_BAND1, 0);////???????
			  break;
		//////AUTO, PA TUNE, CARRIER and more..........	  
		  case 1:  //Auto  
			  
             AutoTuneNow(hwnd);

 

              break;

		  case 2: //Manual
			  bMenuTuneBox=1;//kogato se otvori ManualTuneBOx ot menuto da ne  minava na predavane;
			  bTransmit=TRUE;             
              InvalidateRect(hwnd, &rectTx, TRUE);
			  
			  
			  if(bTuneBox==TRUE){
				  SendMessage(hwndTuneBox, WM_CLOSE, 0,0);
				  //FmOff(hwnd, charMode);
				  break;
			  }
			  bTuneBox=TRUE;
			  hwndTuneBox=CreateDialog(hInstance, (LPCTSTR) IDD_TUNEBOX, hwnd, AboutManualBoxProc);
              
				  
			      
			  

			  
              break;

		  case 3://PaTune
			
				 bTransmit=TRUE;
                 InvalidateRect(hwnd, &rectTx, TRUE);
				 SendMessage(hwndTuneBox, WM_CLOSE, 0,0);//maham Manual Tune prozoreca
				 TunePa(1);//puskam na 50Ohm 
				 if(bRadio) 
                     charMode=FmOn(hwnd, iPaPower);//transceiver na predavane
				 MessageBox(hwnd, TEXT("PA Tuning....press OK to Cancel"), TEXT("PA Tune"), MB_OK);	  
			     FmOff(hwnd,charMode);//na priemane
				 TunePa(0);//vkl na Ant
				 bTransmit=FALSE;
				 InvalidateRect(hwnd, &rectTx, TRUE);
				 
				 break;
			  
        
          case 4://Carrier
			  
                bTransmit=TRUE;
				InvalidateRect(hwnd, &rectTx, TRUE);
				SendMessage(hwndTuneBox, WM_CLOSE, 0,0);//maham Manual Tune prozoreca
				//SetThrough(1);//izkliuchvam PA
				if(bRadio) charMode=FmOn(hwnd,80);//puskam da predava
				MessageBox(hwnd, TEXT("Carrier Mode....press OK to Cancel"), TEXT("Carrier mode"), MB_OK);
				FmOff(hwnd,charMode);//spiram da predava
				//SetThrough(0);//vkl PA
				bTransmit=FALSE;
				InvalidateRect(hwnd, &rectTx, TRUE);	
				break;
			  
	    /*  case 5: //Through
			  if(SendMessage(hwndMy[5], BM_GETCHECK,0, 0)==BST_UNCHECKED){
                 SendMessage(hwndMy[5], BM_SETCHECK, 1, 0);
			    
				 SetThrough(1);//puskam through(namira se v realy.c)
                
                 
				 break;
			  }

			  else if(SendMessage(hwndMy[5], BM_GETCHECK,0, 0)==BST_CHECKED){
                 SendMessage(hwndMy[5], BM_SETCHECK, BST_UNCHECKED, 0);
                 SendMessage(hwndTuneBox, WM_CLOSE, 0,0);
				 SetThrough(0); //izkliuchvam go
				 
				 break;
			  }*/
           /////// End AUTO , CARRIER, THROUGH, and more......


          
		  
		  
		  
		  
		  
		  //------------UP Down Hot keys---------------------
		  
		  
		           //----SLow
		  case ID_C1UP:
              if(Ant[iAnt].Band[iBand].Tune[iTune].c1<255)
			   Ant[iAnt].Band[iBand].Tune[iTune].c1+=1;
			  SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
			  bSave=1;
			  break;
		  case ID_C1DOWN:
			  if(Ant[iAnt].Band[iBand].Tune[iTune].c1>0)
			   Ant[iAnt].Band[iBand].Tune[iTune].c1-=1;
			  SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
			  bSave=1;
			  break;

		  case ID_C2UP:
              if(Ant[iAnt].Band[iBand].Tune[iTune].c2<255)
			   Ant[iAnt].Band[iBand].Tune[iTune].c2+=1;
			  SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
			  bSave=1;
			  break;
		  case ID_C2DOWN:
			  if(Ant[iAnt].Band[iBand].Tune[iTune].c2>0)
			   Ant[iAnt].Band[iBand].Tune[iTune].c2-=1;
			  SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
			  bSave=1;
			  break;

		  case ID_LUP:
              if(Ant[iAnt].Band[iBand].Tune[iTune].l<255)
			   Ant[iAnt].Band[iBand].Tune[iTune].l+=1;
			  SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
			  bSave=1;
			  break;
		  case ID_LDOWN:
			  if(Ant[iAnt].Band[iBand].Tune[iTune].l>0)
			   Ant[iAnt].Band[iBand].Tune[iTune].l-=1;
			  SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
			  bSave=1;
			  break;
           
			  //----------FAst--------
		  case ID_C1FASTUP:
              if(Ant[iAnt].Band[iBand].Tune[iTune].c1<251)
			   Ant[iAnt].Band[iBand].Tune[iTune].c1+=5;
			  SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
			  bSave=1;
			  break;
		  case ID_C1FASTDOWN:
			  if(Ant[iAnt].Band[iBand].Tune[iTune].c1>5)
			   Ant[iAnt].Band[iBand].Tune[iTune].c1-=5;
			  SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
			  bSave=1;
			  break;

		  case ID_C2FASTUP:
              if(Ant[iAnt].Band[iBand].Tune[iTune].c2<251)
			   Ant[iAnt].Band[iBand].Tune[iTune].c2+=5;
			  SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
			  bSave=1;
			  break;
		  case ID_C2FASTDOWN:
			  if(Ant[iAnt].Band[iBand].Tune[iTune].c2>5)
			   Ant[iAnt].Band[iBand].Tune[iTune].c2-=5;
			  SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
			  bSave=1;
			  break;

		  case ID_LFASTUP:
              if(Ant[iAnt].Band[iBand].Tune[iTune].l<251)
			   Ant[iAnt].Band[iBand].Tune[iTune].l+=5;
			  SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
			  bSave=1;
			  break;
		  case ID_LFASTDOWN:
			  if(Ant[iAnt].Band[iBand].Tune[iTune].l>5)
			   Ant[iAnt].Band[iBand].Tune[iTune].l-=5;
			  SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
			  bSave=1;
			  break;			  
			  //---------------------------------------------------
     }  
		  
	 
	     //za da moje tochkata da otgovarq na nastoiashtata nastroika
	     iZelena=29*lastAnt[iAnt].lastBand[iBand].lastCw[iCw];//chete ot poslednata nastroika i pravi iZelena da suvpada s iTune
	     iTune=lastAnt[iAnt].lastBand[iBand].lastCw[iCw];//Chete ot polsednite nastroiki za Band,Ant,mode	     
		 SetAllTune();//shtrakva reletata na suotvetnata poziciq
		 SendMessage(hwnd, WM_HSCROLL, 0, 0);//Obnovqva SCROLL controlite
		 
		 SendMessage(hwndTuneBox, WM_COMMAND, 0, 0);//za da moje da se mrudne 
		 //zelenata tochka sprqmo tekushtata nastroika 
		 
	     
		  return 0;	
	 
	
  
           
     case WM_DESTROY:
		  if(bSave) 
              DialogBox(hInstance, (LPCTSTR) IDD_CHANGES , hwnd, AboutSaveChanges); //Creates dialog box: Save changes: yes/no
		  
          
		  FmOff(hwnd,charMode);  
		  SwrOff();
		  DeleteObject(hBrushStatic);
		  DeleteObject(hBrushStatic1);
          DeleteObject(hBitmap);//temp

		  CloseCom();
		  KillTimer(hwnd, 111);
		  KillTimer(hwndTuneBox, 511);//Timer-a ot ManualTuneBox-hwndTuneBox
		  PostQuitMessage (0) ;
		  return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

LRESULT CALLBACK ScrollProc (HWND hwnd, UINT message, 
                             WPARAM wParam, LPARAM lParam)
{
     int id = GetWindowLong (hwnd, GWL_ID) ; //Wzimam ID-to na prozoreca
          
     switch (message)
     {
     case WM_KEYDOWN :
		 if (wParam == VK_TAB)
				   SetFocus (GetDlgItem (GetParent (hwnd), 
				   (id+1 >23 ? 21 : id+1)) ) ;
		
		  break ;
               
     case WM_SETFOCUS : //KOgat daden prozorec e na fokus poluchava WM_SETFOCUS
          idFocus = id ;
          break ;
     }
     return CallWindowProc (OldScroll[id-21], hwnd, message, wParam, lParam) ;
}

BOOL CALLBACK AboutManualBoxProc(HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
     
    
    //LOGBRUSH lBrush;
    static HWND hSwr;
    static int j;
	static TCHAR szBuffy[50];
	static HBRUSH hBrushy;
	static int iFlagStulb=0, iFlagTimeOut=0;
	
    PAINTSTRUCT  ps;
    HDC    hdc;
    //DWORD dwSec;
    
	
	rectTochka.left=16;
    rectTochka.top=2;
    rectTochka.right=35;
    rectTochka.bottom=290;  

    

    //lBrush.lbColor=RGB(0, 168, 192);
	
	 
	
	
	switch (message)
     {
     case WM_INITDIALOG :
         hBrushy=CreateSolidBrush(RGB(0,255,0));
		 
		 if(bMenuTuneBox){//Ako e pusnato ot Menuto nqma da se aktivira
			SwrOn();//ne se vkliuchva SWR ako e otvoreno ot MENU-top
		    MyWait(100);
		    if(bRadio) charMode=FmOn(hDlg, iPower);//Chete na kakuv Mode e i zapisva v CharMode
		    SetTimer(hDlg, 511, 100, NULL);//polzvam go za merene na SWR-a
		 }
		 for(j=0; j<10;j++)
		 {
			 OldScrollTune[j] = (WNDPROC) SetWindowLong (GetDlgItem(hDlg, IDC_BUTTON1+j),
			          GWL_WNDPROC, (LONG) ScrollProcManual);/*novata winproc*/
		  //Za Tune Box
		  }
 
		  //--------------------------------------------------
		  //Za Through  
        //SendMessage(GetDlgItem(GetParent(hDlg),5), BM_SETCHECK, 1, 0);
	    //PostMessage(hDlg, WM_COMMAND, lastAnt[iAnt].lastBand[iBand].lastCw[iCw]+IDC_BUTTON1,0);
		if(bMenuTuneBox) hSwr=CreateDialog((HINSTANCE) GetWindowLong (hDlg, GWL_HINSTANCE), (LPCTSTR)IDD_DIALOGSWR, hDlg, AboutSwrDialog);
		 
		 return TRUE ;
     
	
	 case  WM_TIMER:	 		 
		 	
		if(bTuner) dSWR=GetSwr();//chete SWR samo ako ima tuner i e pusnato ot glavniq prozorec(ne ot menuto)
		sprintf(szBuffy1, "%.1f", dSWR);//slagam v buffer
		SetWindowText(GetDlgItem( (HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 206), szBuffy1); 
		
		
		//iFlagStulb++;//polzva se ako iskam SWR-metyra da izmerva samo vednuj pri smenqne na nastroika
	//	if(iFlagStulb>100);//pokazva samo vednuj SWR-a osven ako nqma suobshtenie WM_PAINT i WM_COMMAND koito resetvat flag-a 
		//izpisva SWR-a 
	    InvalidateRect(hSwr, &rectStulb, 0);
		iFlagTimeOut++;//slaga se timeout na MAnualTuneBox-a 60sec.
		if(iFlagTimeOut>600) SendMessage(hDlg, WM_CLOSE, 0,0);//kogato stanat 60sec zatvarq prozoreca
		return 0;
      
	 case WM_SETFOCUS:
		 SetFocus((HWND)idFocusManual);
		 break;
	  
	  
	 case WM_PAINT: 
		  hdc=BeginPaint(hDlg, &ps);//Pravi se za da se murda zelenata tochka
		  SelectObject(hdc, hBrushy);
		  RoundRect(hdc, 17, 6+iZelena , 32, 24+iZelena, 15, 15);
		  EndPaint(hDlg, &ps);
		  DeleteDC(hdc);
		  //-----
		  
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[0].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[0].l*100,
			  Ant[iAnt].Band[iBand].Tune[0].c2*5);
		  
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON1), szBuffy);
		  
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[1].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[1].l*100,
			  Ant[iAnt].Band[iBand].Tune[1].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON2), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[2].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[2].l*100,
			  Ant[iAnt].Band[iBand].Tune[2].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON3), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[3].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[3].l*100,
			  Ant[iAnt].Band[iBand].Tune[3].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON4), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[4].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[4].l,
			  Ant[iAnt].Band[iBand].Tune[4].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON5), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[5].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[5].l*100,
			  Ant[iAnt].Band[iBand].Tune[5].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON6), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[6].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[6].l*100,
			  Ant[iAnt].Band[iBand].Tune[6].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON7), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[7].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[7].l*100,
			  Ant[iAnt].Band[iBand].Tune[7].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON8), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[8].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[8].l*100,
			  Ant[iAnt].Band[iBand].Tune[8].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON9), szBuffy);
		  wsprintf(szBuffy, TEXT("c1=%d  l=%d  c2=%d"), 
			  Ant[iAnt].Band[iBand].Tune[9].c1*5, 
			  Ant[iAnt].Band[iBand].Tune[9].l*100,
			  Ant[iAnt].Band[iBand].Tune[9].c2*5);
		  SetWindowText(GetDlgItem(hDlg, IDC_BUTTON10), szBuffy);
          
		  iFlagStulb=0;
		 
		  return 0;


		 
    

     case WM_COMMAND :
          
		  
                		  
		  switch (LOWORD (wParam))
          {
          
		  
		  //KOmandi za Ant Dialog box
		  case IDC_BUTTON14:
		  case IDOK :
          case IDCANCEL :
			   FmOff(hDlg,charMode); 
			   SwrOff();
			   bTuneBox=0;//za da ne izlizat poveche ot edin Manual Box
			   bTransmit=0; //Pravi se za da zagasne TX
               InvalidateRect((HWND)GetWindowLong(hDlg,GWL_HWNDPARENT), &rectTx, TRUE);
			   KillTimer(hDlg, 511);
			   DeleteObject(hBrushy);
			   DestroyWindow (hDlg) ;
			   return TRUE ;

		  
		  //tfa 156 idva WndProc ot WM_COMMAND za da mrudna
			//zelenata tochka sprqmo tekushtata nastroika
		  //case 156:
            //  InvalidateRect(hDlg, &rectTochka, TRUE);
			//  break;
		//------
			  
			  
		//OKmandi za TUNE BOX
			   case IDC_BUTTON1:
				   iTune=0;
				   iZelena=0;  //i e za zelenata tochka
				   
				   break;
			   case IDC_BUTTON2:
				   iTune=1;
				   iZelena=29; //i e za Zelenata tochka
				   
				   break;
			   case IDC_BUTTON3:
				   iTune=2;
				   iZelena=29*2;
				   break;
			   case IDC_BUTTON4:
				   iTune=3;
				   iZelena=29*3;
				   break;
			   case IDC_BUTTON5:
				   iTune=4;
				   iZelena=29*4;
				   break;
			   case IDC_BUTTON6: //button6
				   iTune=5;
				   iZelena=29*5;
				   break;
			   case IDC_BUTTON7: //button7
				   iTune=6;
				   iZelena=29*6;
				   break;
			   case IDC_BUTTON8:  //button8
				   iTune=7;
				   iZelena=29*7;
				   break;
			   case IDC_BUTTON9: //button9
 				   iTune=8;
				   iZelena=29*8;
				   break;
			   case IDC_BUTTON10:  //button 10
				   iTune=9;
				   iZelena=29*9;
				   break;
			 // case IDC_BUTTON14:
			//	   bTuneBox=0;//za da ne izlizat poveche ot edin Manual Box
			//	   bTransmit=0; //Pravi se za da zagasne TX
              //     InvalidateRect((HWND)GetWindowLong(hDlg,GWL_HWNDPARENT), &rectTx, TRUE);
				//   DestroyWindow (hDlg) ;

		  }
          InvalidateRect(hDlg, &rectTochka, TRUE);
		  SetAllTune();//Pravi se za da prevkluchvat reletata sled smqna na nastroika
		  lastAnt[iAnt].lastBand[iBand].lastCw[iCw]=iTune;//zapisva daden iTune za opredelena Ant, Band i mode.
		  
		  //Pravi se za da murdat scrolovete kogato smeniam iTune
		  PostMessage((HWND)GetWindowLong(hDlg,GWL_HWNDPARENT), WM_HSCROLL, 0, 0);
		  iFlagStulb=0;//stava 0 i izpisva SWR pri sledvashtiq WM_TIMER
		  iFlagTimeOut=0; //nulira FLAG-a i zapochva da broi oshte vednuj do 60sec
		  //dwSec=GetTickCount();
		 //while(dwSec+10000<GetTickCount());
		  break ;
    
	 case WM_CLOSE :
		 if(bMenuTuneBox){//kogato e otvoreno ot Menuto tezi ne sa aktivni
			 FmOff(hDlg,charMode);
			 SwrOff();
			 KillTimer(hDlg, 511);
		 }
		DeleteObject(hBrushy);//ot wm-paint-a
		bTuneBox=0;//za da ne izlizat poveche ot edin Manual Box
		bTransmit=0; //Pravi se za da zagasne TX
        InvalidateRect((HWND)GetWindowLong(hDlg,GWL_HWNDPARENT), &rectTx, TRUE);
		DeleteObject(hBrushy);
		DestroyWindow (hDlg) ;
		break ;

     
	 
	 }
     return FALSE ;
}




BOOL CALLBACK AboutAntNames(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	
	static TCHAR szBuffy[50];
	static int j;
	//int iBuf; //za da vidia kolko bukvi ima v edit butonite

	

	 
	 switch (message)
     {
     case WM_INITDIALOG :
		  
          bSave=1;//za da pita posle: save as
		  //Za Ant Dialog Box
	      SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), ant[0]);
		  SetWindowText(GetDlgItem(hDlg, IDC_EDIT2), ant[1]);
		  SetWindowText(GetDlgItem(hDlg, IDC_EDIT3), ant[2]);
		  SetWindowText(GetDlgItem(hDlg, IDC_EDIT4), ant[3]);
		  SetWindowText(GetDlgItem(hDlg, IDC_EDIT5), ant[4]);
		  SetWindowText(GetDlgItem(hDlg, IDC_EDIT6), ant[5]);
		  for(j=IDC_EDIT1;j<IDC_EDIT6+1;j++)
		  {
			  SendDlgItemMessage(hDlg, j, EM_LIMITTEXT, 24, 0);//ogranichavam text-a za ANT da ne e poveche ot 24znaka
		  }   
  	  //------------------------------------------------
		  return TRUE ;
     
	


     case WM_SETFOCUS:
		 SetFocus((HWND)idFocusAnt);
		 break;

     case WM_COMMAND :
      
		 switch (LOWORD (wParam))
          
		 {
		  //KOmandi za Ant Dialog box
		  case IDOK :
			   EndDialog (hDlg, 0) ;
               return TRUE ;

		  case IDC_EDIT1:
			   switch (HIWORD(wParam))
			   {	   

			   case EN_KILLFOCUS:
                  
				   GetWindowText((HWND)lParam, ant[0], 50);//reading the text from the button, just after focus is gone
				   SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),15), ant[0]);//sends message to main window with the text for the button
				   
				   return TRUE;
			   }
			   
			   break;
		  case IDC_EDIT2:
			   switch (HIWORD(wParam))
			   {
			   case EN_KILLFOCUS:

				   GetWindowText((HWND)lParam, ant[1], 50);
				   SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),16), ant[1]);
				   
				   return TRUE;
			   }
			   
			   break;
		   case IDC_EDIT3:
			   switch (HIWORD(wParam))
			   {
			   case EN_KILLFOCUS:

				   GetWindowText((HWND)lParam, ant[2], 50);
				   SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),17), ant[2]);
				   
				   return TRUE;
			   }
			   
			   break;	   
           case IDC_EDIT4:
			   switch (HIWORD(wParam))
			   {
			   case EN_KILLFOCUS:

				   GetWindowText((HWND)lParam, ant[3], 50);
				   SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),18), ant[3]);
				   
				   return TRUE;
			   }
			   
			   break;
		    case IDC_EDIT5:
			   switch (HIWORD(wParam))
			   {
			   case EN_KILLFOCUS:

				   GetWindowText((HWND)lParam, ant[4], 50);
				   SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),19), ant[4]);
				   
				   return TRUE;
			   }
			   
			   break;
			case IDC_EDIT6:
			   switch (HIWORD(wParam))
			   {
			   case EN_KILLFOCUS:

				   GetWindowText((HWND)lParam, ant[5], 50);
				   SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),20), ant[5]);
				   
				   return TRUE;
			   }
		    //Krai vutreshen switch za WM_COMMAND -----  
		     break;
		  }
          
		 
		  return 0;
    //End WM_COMMAND---------------------
	
		 
	case WM_CLOSE :
            /*     SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),15), ant[0]);
				 SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),16), ant[1]);
			     SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),17), ant[2]);
				 SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),18), ant[3]);
				 SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),19), ant[4]);
				 SetWindowText((HWND) GetDlgItem((HWND) GetWindowLong(hDlg,GWL_HWNDPARENT),20), ant[5]);
		   */
		EndDialog(hDlg,0);
		break ;

     
	 
	 }
     return FALSE ;
}
BOOL CALLBACK AboutHelpProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{


	switch (message)
	{
	
	case WM_INITDIALOG :
        SetFocus(hDlg);
		return TRUE;



	case WM_COMMAND:
        switch(wParam)
		{
		case IDOK :  
		case IDCANCEL :
               EndDialog (hDlg, 0) ;
               return TRUE ;
		}
		
	

    case WM_CLOSE :
         
		EndDialog (hDlg, 0);
	    
		return TRUE;
	
	
	}
	return FALSE;	
}


/*LRESULT CALLBACK ScrollProcAnt (HWND hwnd, UINT message, 
                             WPARAM wParam, LPARAM lParam)
{
     int id = GetWindowLong (hwnd, GWL_ID) ; //Wzimam ID-to na prozoreca
          
     switch (message)
     { 
     case WM_KEYDOWN :
		 if (wParam == VK_TAB )
				   SetFocus (GetDlgItem (GetParent (hwnd), 
				   (id+1 >IDC_EDIT6 ? IDC_EDIT1 : id+1)) ) ;
				  
		 else if(wParam==VK_RETURN)
			 SendMessage((HWND)GetWindowLong(hwnd, GWL_HWNDPARENT),
			 WM_COMMAND, IDOK, 0);
		  
		 break;
               
     case WM_SETFOCUS : //KOgat daden prozorec e na fokus poluchava WM_SETFOCUS
          idFocusAnt=id;
          break ;
	

 }        
     return  CallWindowProc (OldScrollAnt[id-IDC_EDIT1], hwnd, message, wParam, lParam) ;
}*/


LRESULT CALLBACK ScrollProcManual(HWND hwnd, UINT message, 
                             WPARAM wParam, LPARAM lParam)
{
     static int id;
	 id= GetWindowLong (hwnd, GWL_ID) ; //Wzimam ID-to na prozoreca
        
     switch (message)
     {
     case WM_KEYDOWN :
		 
		 switch (wParam)
		 {
		 
		 case VK_RETURN:
			 SendMessage((HWND)GetWindowLong(hwnd, GWL_HWNDPARENT), WM_COMMAND, IDOK, 0);
             break;
         case VK_DOWN:
				   SetFocus (GetDlgItem (GetParent (hwnd), 
				   ((id+1) >IDC_BUTTON10 ? IDC_BUTTON1 : id+1)) ) ;
				   break;
		 case VK_UP:
				   SetFocus (GetDlgItem (GetParent (hwnd), 
				   ((id-1) <IDC_BUTTON1 ? IDC_BUTTON10 : id-1)) ) ;
				   break;
		 }
	 
		
		  
		 
		 
		  SendMessage((HWND)GetWindowLong(hwnd, GWL_HWNDPARENT), WM_COMMAND, id, 0); 
		  break ;
               
     case WM_SETFOCUS : //KOgat daden prozorec e na fokus poluchava WM_SETFOCUS
          
		  idFocusManual=id;
          break ;
	

	 }        
     return CallWindowProc (OldScrollTune[id-IDC_BUTTON1], hwnd, message, wParam, lParam) ;
}




//ffff
VOID  MyFileInitialize(HWND hwnd)
{
	

	static TCHAR szFilter[]=TEXT("ATU files only(*.ATU)\0*.atu\0")  ;

	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hwnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
    ofn.lpstrFile         = NULL;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = TEXT("E:");
	ofn.lpstrTitle        = NULL ;
	//tfa e za da moje da se otvarqt samo sushtstvuvashti filove
	ofn.Flags             = OFN_FILEMUSTEXIST ; 
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("atu");
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
    ofn.lpTemplateName    = NULL ;
}


///Funkciite za open i save as


BOOL  MyFileOpen (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
 	 ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
    
     
     return GetOpenFileName (&ofn) ;
}

BOOL MyFileSave (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_OVERWRITEPROMPT ;
     
     return GetSaveFileName (&ofn) ;
}

	

 

void ResetBandButtons(void)
{
    int l;

	for(l=6;l<15;l++){
		SendMessage(hwndMy[l], BM_SETSTATE, 0, 0);//resetva Vsichki band buttoni
	}
}

void ResetAntButtons(void)
{
    int l;
    
	for(l=15;l<21;l++){
		SendMessage(hwndMy[l], BM_SETSTATE, 0, 0);//resetva Vsichki ant buttoni
	}
}



BOOL CALLBACK AboutPwr(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szPower[5];
	int static iFlg;


	BOOL bEdikvosi=0;
	switch (message)
	{
	
	case WM_INITDIALOG :
		SendDlgItemMessage(hDlg,IDC_MENU_PWR , EM_LIMITTEXT, 2, 0);
		if(lParam==1){//lParam v WM_INITDIALOG e 5-ia parametyr ot DialogBoxParam
			iFlg=1;//za WM_COMMAND da moje da razbere za koe se otnasq, dali za PA ili za TunePwr
			wsprintf(szPower, TEXT("%d"), iPaPower);
			SetWindowText(GetDlgItem(hDlg, IDC_MENU_PWR), szPower);
		}
		else if(lParam==2){
			iFlg=2;
			wsprintf(szPower, TEXT("%d"), iPower);
			SetWindowText(GetDlgItem(hDlg, IDC_MENU_PWR), szPower);
		}
		SetFocus(GetDlgItem(hDlg,IDC_MENU_PWR));
		return FALSE;//koagto vrushta TRue, 
		//Windows sets the input focus to the first child window control in the dialog box that has a WS_TABSTOP style



	case WM_COMMAND://vrushta FALSE ako ne e obrabotilo suobshtenie
                    //vrushta TRUE ako e obrabotilo suobshtenieto
		switch(LOWORD (wParam))
		{
		case IDOK :  
		case IDCANCEL :
               EndDialog (hDlg, 0) ;
               return TRUE ;
		
		 case IDC_MENU_PWR:
			   switch (HIWORD(wParam))
			   {	   

			   case EN_KILLFOCUS:
                   if(iFlg==1) iPaPower=(int) GetDlgItemInt(hDlg, IDC_MENU_PWR, &bEdikvosi , 0);
				   else if(iFlg==2) iPower=(int)GetDlgItemInt(hDlg, IDC_MENU_PWR, &bEdikvosi , 0);
				   //wsprintf(sztesting, TEXT("%d"), iPower);
				   //MessageBox(hDlg, sztesting, sztesting, MB_OK); 
				   break;
				   
			   }//switch
		      break;
		}//end wm_command
		
	    return TRUE;

    case WM_CLOSE :
         
		EndDialog (hDlg, 0);
	    
		return TRUE;
	
	
	}
	return FALSE;	
}


BOOL CALLBACK AboutSaveChanges(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
     {
     case WM_INITDIALOG :
          return TRUE ;
          
     case WM_COMMAND :
          switch (LOWORD (wParam))
          {
          case ID_YESS://Saves on current.at


              hFile = CreateFile(TEXT("current.atu"), GENERIC_WRITE, 0,
				                 NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
              if(hFile == INVALID_HANDLE_VALUE)
              {
				  MessageBox(hDlg, TEXT("Error Saving File"), TEXT("Error"), MB_OK);
                  CloseHandle(hFile);
				  break; 
			  }
              
			  vSaveSettings(hFile); // write settings to file
			  CloseHandle(hFile);

			  EndDialog (hDlg, 0) ;
		      return TRUE;

		  case ID_NOO:
              EndDialog (hDlg, 0) ;
              return TRUE ;
          
		  }
          break ;
     }
     return FALSE ;
}


BOOL CALLBACK AboutCommPortName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	
	case WM_INITDIALOG :
        SetFocus(hDlg);
		SendDlgItemMessage(hDlg,IDC_COMMPORT , EM_LIMITTEXT, 6, 0);
		SetWindowText(GetDlgItem(hDlg, IDC_COMMPORT), szComPort);
		return TRUE;



	case WM_COMMAND:
        switch(LOWORD (wParam))
		{
		case IDOK :  
               EndDialog (hDlg, 0) ;
               return TRUE ;
		
		case IDC_QUITT:
			   bCommQuit = 1;//izklichvam progrmata s tazi stoinost;
			   EndDialog (hDlg, 0);
			   return FALSE;
		case IDC_COMMPORT:
			   switch (HIWORD(wParam))
			   {	   

			   case EN_KILLFOCUS:
                  
				   GetWindowText(GetDlgItem(hDlg, IDC_COMMPORT), szComPort, 6);
				   //wsprintf(sztesting, TEXT("%d"), iPower);
				   //MessageBox(hDlg, sztesting, sztesting, MB_OK); 
				   break;
				   
			   }//switch
		      break;
		}//end wm_command
		
	    return TRUE;

    case WM_CLOSE :
         
		EndDialog (hDlg, 0);
	    
		return TRUE;
	
	
	}
	return FALSE;
}


BOOL CALLBACK AboutCommPortName1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	
	case WM_INITDIALOG :
        SetFocus(hDlg);
		SendDlgItemMessage(hDlg,IDC_COMMPORT , EM_LIMITTEXT, 6, 0);
		SetWindowText(GetDlgItem(hDlg, IDC_COMMPORT), szComPort);
		return TRUE;



	case WM_COMMAND:
        switch(LOWORD (wParam))
		{
		case IDOK :  
               EndDialog (hDlg, 0) ;
               return TRUE ;
		
		case IDC_QUITT:
			   bCommQuit = 1;//izklichvam progrmata s tazi stoinost;
			   EndDialog (hDlg, 0);
			   return FALSE;
		case IDC_COMMPORT:
			   switch (HIWORD(wParam))
			   {	   

			   case EN_KILLFOCUS:
				   GetWindowText(GetDlgItem(hDlg, IDC_COMMPORT), szComPort, 6);
				   break;
				   
			   }//switch
		      break;
		}//end wm_command
		
	    return TRUE;

    case WM_CLOSE :
         
		EndDialog (hDlg, 0);
	    
		return TRUE;
	
	
	}
	return FALSE;
}

VOID AutoTuneNow(HWND hwnd)
{
   
   
   long double  iSwrAuto[11], lTemp;
   int k=0,i=0;
   //char szStr[20];
   DWORD dwCount;
   
   if(!bTuneBox)//ako ima veche  vkliuchen ManualTuneBox da ne puska na predavane i tn
	{
		  SwrOn();
		  if(bRadio) charMode=FmOn(hwnd, iPower);//kogato se otvori ManualTuneBOx ot menuto da ne  minava na predavane;
	}
		  
   do{
	   iTune=i;//naglasqm iTune=0,1,2.....9
	   if(!Ant[iAnt].Band[iBand].Tune[iTune].c1 && !Ant[iAnt].Band[iBand].Tune[iTune].c2
		   && !Ant[iAnt].Band[iBand].Tune[iTune].l){//ako tova e TRUE znachi za nastoiashtata nastroika
       //c1,c2 i l sa 0 i spestqvam shtrakaneto na reletata(podminava nastroikata i ne chete SWR)
			iSwrAuto[i]=100;
			SetWindowText(GetDlgItem(hwnd, 206), "NO");
	   }
	   else{
	   SetAllTune();//shtrakvam reletata
	   dwCount=GetTickCount();
	   while(GetTickCount()<dwCount+300);//chakam 100ms
	   if(bTuner) iSwrAuto[i]=GetSwr();//cheta SWR
	   if(bTuner) sprintf(szBuffy1, "%.1f", GetSwr());
	   SetWindowText(GetDlgItem(hwnd, 206), szBuffy1); 
	   }
	   i++;
   }while(i<10);
	 
    	 
   lTemp=iSwrAuto[0];//slagam da sravnqvam s purva nastroika
   for(i=1; i<10; i++)
	{
	  if(iSwrAuto[i]<lTemp)//proverqvam dali nastoiashtoto KSW e po malko
	  {
		 lTemp=iSwrAuto[i];//ako da zapametqvam go za nova proverka
		 k=i;//i zapametqvam nomera na nastroikata s po nisko KSW
	  }//if
	}//for
    
   iTune=k;//izbiram naj dobroto swr		 
   lastAnt[iAnt].lastBand[iBand].lastCw[iCw]=iTune;// za da zapomnq nastroikite
   SetAllTune();
   dwCount=GetTickCount();
   while(GetTickCount()<dwCount+150);//chakam 150ms za nastroika
   if(bTuner) sprintf(szBuffy1, "%.1f", GetSwr());
   SetWindowText(GetDlgItem(hwnd, 206), szBuffy1);
   SendMessage(hwndTuneBox, WM_COMMAND, iTune+IDC_BUTTON1,0); 
   //prashtam MSG na ManualBox za da natisne butona s naj dobrata nastroika
   //SendMessage(hwnd, WM_COMMAND, 0,0); 
  			
   if(!bTuneBox){//izkliuchvam predavaneto ako nqma ManualBox
	FmOff(hwnd,charMode);
	SwrOff();	
   };
			   
	return;
}



BOOL CALLBACK AboutSwrDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	
	HDC hdc;
    static HBRUSH hBrush1, hBrush2;
    PAINTSTRUCT ps;
	static int ii=0;
	char szKsw[2];
	


	switch (message)

		
	{
     case WM_INITDIALOG :
         hBrush1=CreateSolidBrush(RGB(134,145,0));
		 return TRUE ;
          
     case WM_PAINT:
		 hdc=BeginPaint(hDlg, &ps); 
		 //if(bTuner) test111=TestGetSwr();
		 Rectangle(hdc, 47,0, 73, (int)(240-dSWR*30));	
		 SelectObject(hdc, hBrush1);
		// sprintf(szBuffy1, TEXT("%d"), test111);
		 //
	//	 MessageBox(hDlg,szBuffy1, TEXT("eto na"), MB_OK);
		 Rectangle(hdc, 47,210, 73, (int)(240-dSWR*30));	
		 SetBkColor (hdc, RGB(190,190,190));
		 TextOut(hdc, 76, 194, szKsw, sprintf(szKsw, TEXT("%.1f  "), dSWR)); //izpisva KSW(vzeto ot ManualTuenBox) do stulbcheto 
		 EndPaint(hDlg, &ps);
		 DeleteDC(hdc);
		 break;
	 
	 case WM_CLOSE :	  
	  	 DeleteObject(hBrush1);
		 EndDialog (hDlg, 0);
		 return 0;
     }
     return FALSE ;
}



BOOL CALLBACK AboutButtonDisable(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    
	
	switch (message)
	{
	
	case WM_INITDIALOG :
        /* Check/not Checked v zavisimots ot tova kakvo pishe v masiva*/
		SendMessage(GetDlgItem(hDlg, IDC_CHECK11), BM_SETCHECK, bCheckBox[0], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK12), BM_SETCHECK, bCheckBox[1], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK13), BM_SETCHECK, bCheckBox[2], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK14), BM_SETCHECK, bCheckBox[3], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK15), BM_SETCHECK, bCheckBox[4], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK16), BM_SETCHECK, bCheckBox[5], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK17), BM_SETCHECK, bCheckBox[6], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK18), BM_SETCHECK, bCheckBox[7], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK19), BM_SETCHECK, bCheckBox[8], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK20), BM_SETCHECK, bCheckBox[9], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK21), BM_SETCHECK, bCheckBox[10], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK22), BM_SETCHECK, bCheckBox[11], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK23), BM_SETCHECK, bCheckBox[12], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK24), BM_SETCHECK, bCheckBox[13], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK25), BM_SETCHECK, bCheckBox[14], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK26), BM_SETCHECK, bCheckBox[15], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK27), BM_SETCHECK, bCheckBox[16], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK28), BM_SETCHECK, bCheckBox[17], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK29), BM_SETCHECK, bCheckBox[18], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK30), BM_SETCHECK, bCheckBox[19], 0);
        SendMessage(GetDlgItem(hDlg, IDC_CHECK31), BM_SETCHECK, bCheckBox[20], 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK32), BM_SETCHECK, bCheckBox[21], 0);
        SendMessage(GetDlgItem(hDlg, IDC_CHECKDISABLE), BM_SETCHECK, bCheckBox[22], 0); // Quick buttons disable

		SetFocus(hDlg);
		
		bSave=1;//ako se otvori menuto posle she pita dali da Save
		return TRUE;



	case WM_COMMAND:
        switch(wParam)
		{
		
		
		case IDOK :  
               EndDialog (hDlg, 0) ;
               break ;
		
		case IDC_CHECK11://1.8
			
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 6),  0);
                bCheckBox[0]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){
				bCheckBox[0]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 6),  1);
			}
            break;

		case IDC_CHECK12://3.5
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 7),  0);
                bCheckBox[1]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){
				bCheckBox[1]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 7),  1);
			}
            break;

		case IDC_CHECK13://7
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 8),  0);
                bCheckBox[2]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){
				bCheckBox[2]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 8),  1);
			}
            break;

        case IDC_CHECK14://10
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 9),  0);
                bCheckBox[3]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){
				bCheckBox[3]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 9),  1);
			}
            break;

        case IDC_CHECK15://14
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 10),  0);
                bCheckBox[4]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){
				bCheckBox[4]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 10),  1);
			}
            break;

		case IDC_CHECK16://18
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 11),  0);
                bCheckBox[5]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[5]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 11),  1);
			}
            break;

		case IDC_CHECK17://21
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 12),  0);
                bCheckBox[6]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[6]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 12),  1);
			}
            break;

		case IDC_CHECK18://24
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 13),  0);
                bCheckBox[7]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[7]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 13),  1);
			}
            break;

		case IDC_CHECK19://28mhz
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 14),  0);
                bCheckBox[8]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[8]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 14),  1);
			}
            break;

        case IDC_CHECK20://ant1
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 15),  0);
                bCheckBox[9]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[9]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 15),  1);
			}
            break;
			
        case IDC_CHECK21://ant2
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 16),  0);
                bCheckBox[10]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[10]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 16),  1);
			}
            break;

		case IDC_CHECK22://ant3
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 17),  0);
                bCheckBox[11]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[11]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 17),  1);
			}
			break;

		case IDC_CHECK23://ant4
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 18),  0);
                bCheckBox[12]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[12]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 18),  1);
			}
			break;

		case IDC_CHECK24://ant5
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 19),  0);
                bCheckBox[13]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[13]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 19),  1);
			}
			break;

		case IDC_CHECK25://ant6
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 20),  0);
                bCheckBox[14]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[14]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 20),  1);
			}
            break;

		case IDC_CHECK26://Manual
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 2),  0);
                bCheckBox[15]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[15]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 2),  1);
			}
			break;

		case IDC_CHECK27://TunePA
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 3),  0);
                bCheckBox[16]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[16]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 3),  1);
			}
			break;

		case IDC_CHECK28://Through
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 5),  0);
                bCheckBox[17]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[17]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 5),  1);
			}
			break;

		case IDC_CHECK29://Carrier
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 4),  0);
                bCheckBox[18]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[18]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 4),  1);
			}
			break;

		case IDC_CHECK30://AUTO
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 1),  0);
                bCheckBox[19]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[19]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 1),  1);
			}
			break;

		case IDC_CHECK31://SSB
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 25),  0);
                bCheckBox[20]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[20]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 25),  1);
			}
			break;

		case IDC_CHECK32://CW
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){//Aktivirano-Button is disabled; bCheckBox[0]=1;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 24),  0);
                bCheckBox[21]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0)){

				bCheckBox[21]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), 24),  1);
			}
            break;

         case IDC_CHECKDISABLE://CW
/*check*/	if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0))
            {
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), IDC_CHECKDISABLE),  0);
                bCheckBox[22]=1;
			}
/*uncheck*/	else if(!SendMessage((HWND)lParam, BM_GETCHECK, 0, 0))
            {

				bCheckBox[22]=0;
				EnableWindow((HWND)GetDlgItem((HWND)GetWindowLong(hDlg, GWL_HWNDPARENT), IDC_CHECKDISABLE),  1);
			}
            break;

		default:
			   break;;

		
		}
		
	    return 0;

    case WM_CLOSE :
         
		EndDialog (hDlg, 0);
	    
		return TRUE;
	
	
	}
	return FALSE;	
}



void DisableButtons(HWND hwnd){//funkciq koqto se vika ot WNDPROC v WM_CREAT za da napravi koito buttoni trqbwa enable ili disable
	EnableWindow(GetDlgItem(hwnd, 6), !bCheckBox[0]);//1,8mhz
    EnableWindow(GetDlgItem(hwnd, 7), !bCheckBox[1]);
	EnableWindow(GetDlgItem(hwnd, 8), !bCheckBox[2]);
	EnableWindow(GetDlgItem(hwnd, 9), !bCheckBox[3]);
	EnableWindow(GetDlgItem(hwnd, 10), !bCheckBox[4]);
	EnableWindow(GetDlgItem(hwnd, 11), !bCheckBox[5]);
	EnableWindow(GetDlgItem(hwnd, 12), !bCheckBox[6]);
	EnableWindow(GetDlgItem(hwnd, 13), !bCheckBox[7]);
	EnableWindow(GetDlgItem(hwnd, 14), !bCheckBox[8]);//28mhz
	EnableWindow(GetDlgItem(hwnd, 15), !bCheckBox[9]);//ant1
	EnableWindow(GetDlgItem(hwnd, 16), !bCheckBox[10]);
	EnableWindow(GetDlgItem(hwnd, 17), !bCheckBox[11]);
	EnableWindow(GetDlgItem(hwnd, 18), !bCheckBox[12]);
	EnableWindow(GetDlgItem(hwnd, 19), !bCheckBox[13]);
	EnableWindow(GetDlgItem(hwnd, 20), !bCheckBox[14]);//ant6
	EnableWindow(GetDlgItem(hwnd, 2), !bCheckBox[15]);//Manual
	EnableWindow(GetDlgItem(hwnd, 3), !bCheckBox[16]);//TunePA
	EnableWindow(GetDlgItem(hwnd, 5), !bCheckBox[17]);//Through
	EnableWindow(GetDlgItem(hwnd, 4), !bCheckBox[18]);//Carrier
	EnableWindow(GetDlgItem(hwnd, 1), !bCheckBox[19]);//AUTO
	EnableWindow(GetDlgItem(hwnd, 25), !bCheckBox[20]);//ssb
	EnableWindow(GetDlgItem(hwnd, 24), !bCheckBox[21]);//cw
    return;
}

//-------------------------------------------------------------------
// Save the program settings in a File
//-------------------------------------------------------------------
void vSaveSettings(HANDLE hFile)
{
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    WriteFile(hFile, &ant, sizeof ant, &uli, NULL);//pishe imenata
	WriteFile(hFile, &Ant, sizeof Ant, &uli, NULL);// pishe masiva s nastroikite
	WriteFile(hFile, &iPower, sizeof iPower, &uli, NULL);// pishe s kolko wata da se nastroiwa
	WriteFile(hFile, &szComPort, sizeof szComPort, &uli, NULL);// pishe iPower
	WriteFile(hFile, &bCheckBox, sizeof bCheckBox, &uli, NULL);
	WriteFile(hFile, &lastAnt, sizeof lastAnt, &uli, NULL);//pishe posledni nastroiki sprqmo band ant i mode
    WriteFile(hFile, &iPaPower, sizeof iPaPower, &uli, NULL); 
}

//-------------------------------------------------------------------
// Load the program settings from a File
//-------------------------------------------------------------------
void vLoadSettings(HANDLE hFile)
{
    BOOL bResult;

    if(hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    bResult = ReadFile(hFile, &ant, sizeof(ant), &uli, NULL);//chete imenata
    if(bResult==FALSE || uli!=sizeof ant)
    {
        MessageBox(NULL, TEXT("Couldn't load the antenna names"), TEXT("Settings file error..."), MB_OK);
        return;
    }

    bResult = ReadFile(hFile, &Ant, sizeof(Ant), &uli, NULL);//chete masiva s nastroikite
	if(bResult==FALSE || uli!=sizeof(Ant))
    {
        MessageBox(NULL, TEXT("Couldn't load the tune settings"), TEXT("Settings file error..."), MB_OK);
        return;
    }

    bResult = ReadFile(hFile, &iPower, sizeof(iPower), &uli, NULL);//chete iPower - s klko vata se nastroiva
	if(bResult==FALSE || uli!=sizeof(iPower))
    {
        MessageBox(NULL, TEXT("Couldn't load the tune power"), TEXT("Settings file error..."), MB_OK);
        return;
    }

    bResult = ReadFile(hFile, &szComPort, sizeof(szComPort), &uli, NULL);// chete Commport settings
	if(bResult==FALSE || uli!=sizeof(szComPort))
    {
        MessageBox(NULL, TEXT("Couldn't load the ComPort name"), TEXT("Settings file error..."), MB_OK);
        return;
    }
    
    bResult = ReadFile(hFile, &bCheckBox, sizeof(bCheckBox), &uli, NULL);//chete masiva za Disable/enable button Menuto
	if(bResult==FALSE || uli!=sizeof(bCheckBox))
    {
        MessageBox(NULL, TEXT("Couldn't load the disabled buttons settings"), TEXT("Settings file error..."), MB_OK);
        return;
    }
    
    bResult = ReadFile(hFile, &lastAnt, sizeof(lastAnt), &uli, NULL);//chete posledni nastroiki sprqmo, band ant i mode
    if(bResult==FALSE || uli!=sizeof(lastAnt))
    {
        MessageBox(NULL, TEXT("Couldn't load the last Ant settings"), TEXT("Settings file error..."), MB_OK);
        return;
    }
    
    bResult = ReadFile(hFile, &iPaPower, sizeof(iPaPower), &uli, NULL); 
    if(bResult==FALSE || uli!=sizeof(iPaPower))
    {
        MessageBox(NULL, TEXT("Couldn't load the PA tune power"), TEXT("Settings file error..."), MB_OK);
        return;
    }
}