#include <windows.h>
#include "io.h"
#include "mystruct.h"

extern VOID MyWait(int); //int broi micro sec.
extern int iAnt,iBand,iTune; //v asdf.c


VOID SetAllTune(void);

VOID SetAnt(int);//0 do 5
VOID SetC1(int);  //0 do 255
VOID SetC2(int);  //0 do 255
VOID SetL(int);  // 0 do 255

VOID SetThrough(BOOL); //slaga na Through
VOID TunePa(BOOL);  //puskam 50Ohm tovar za Tune PA


	
VOID SetAllTune(void)
{
  SetAnt(iAnt);
  SetC1(Ant[iAnt].Band[iBand].Tune[iTune].c1);
  SetC2(Ant[iAnt].Band[iBand].Tune[iTune].c2);
  SetL(Ant[iAnt].Band[iBand].Tune[iTune].l);
  
  return;
}
   
VOID SetAnt(int iNum)//Prevkliuchva antenite
{
	switch(iNum)
	{
	case 0:
        PortOut(0x37A, 0);//nuliram
	    PortOut(0x378, 0);//nuliram
		   MyWait(100);//izchakva 100us
		PortOut(0x378,~3 );//data
		PortOut(0x37A,3|4);//init
	    MyWait(100);//izchakva 100us
	    PortOut(0x37A,3);//init
		MyWait(2000);//chaka 20ms sled vkliuchvane na rele
		
		break;
    case 1:
		PortOut(0x37A, 0);//nuliram
	 	PortOut(0x378, 0);//nuliram
		 MyWait(100);//izchakva 100us
		PortOut(0x378,~12);//data
		PortOut(0x37A,3|4);//init
	    MyWait(100);
	    PortOut(0x37A,3);//init
		MyWait(2000);//chaka 20ms sled vkliuchvane na rele
		break;
    case 2:
		PortOut(0x37A, 0);//nuliram
		PortOut(0x378, 0);//nuliram
		   MyWait(100);//izchakva 100us
		PortOut(0x378,~48);//data
		PortOut(0x37A, 3|4);//init
	    MyWait(100);
		PortOut(0x37A,3);//init
		MyWait(2000);//chaka 20ms sled vkliuchvane na rele
		break;
    case 3:
		PortOut(0x37A, 0);//nuliram
		PortOut(0x378, 0);//nuliram
		   MyWait(100);//izchakva 100us
		PortOut(0x378,(char) ~192);//data
		PortOut(0x37A,3|4);//init
	    MyWait(100);
	    PortOut(0x37A,3);//init
		MyWait(2000);//chaka 20ms sled vkliuchvane na rele
		break;
	}
	
	return;
}


VOID SetC1(int iNum)
{
	
	PortOut(0x37A, 0);//nuliram
	PortOut(0x378, 0);//nuliram
        MyWait(100);//izchakva 100us
	PortOut(0x378,(char)~iNum);//data
	PortOut(0x37A,4);//init
	MyWait(100);
	PortOut(0x37A,0);//init
    MyWait(2000);//chaka 20ms sled vkliuchvane na rele
	return;

}

VOID SetC2(int iNum)
{
	PortOut(0x378, 0);//nuliram
	PortOut(0x37A, 0);//nuliram    
	   MyWait(100);//izchakva 100us
	PortOut(0x378,(char)~iNum);//data
	PortOut(0x37A,2|4);//init
	MyWait(100);
	PortOut(0x37A,2);//init
    MyWait(2000);//chaka 20ms sled vkliuchvane na rele
	return;

}

VOID SetL(int iNum)
{
	PortOut(0x37A, 0);//nuliram
	PortOut(0x378, 0);//nuliram
		MyWait(100);//izchakva 100us
	PortOut(0x378,(char)(iNum));//data
	PortOut(0x37A,1|4);//init
	MyWait(100);
	PortOut(0x37A,1);//init
    MyWait(2000);//chaka 20ms sled vkliuchvane na rele
	return;

}


VOID SetThrough(BOOL on)
{
    PortOut(0x37A, 0);//nuliram
	PortOut(0x378, 0);//nuliram
		MyWait(100);//izchakva 100us
	PortOut(0x378, (char)(128*on));//&(PortIn(0x378)));//data:6bit da e ON
	//PortOut(0x37A,8);//LATCH
	PortOut(0x37A,8|4);//LATCH + init
	MyWait(100);
	PortOut(0x37A,8);//init
    
	return;
}

VOID TunePa(BOOL on)
{
  
	PortOut(0x37A,0);//nuliram
	PortOut(0x378, 0);//nuliram
	PortOut(0x378, (char)(64*on));	
	   MyWait(100);//izchakva 100usPortOut(0x378, (char)(64*on));//data:6bit da e ON
   //PortOut(0x37A,8);//LATCH
    PortOut(0x37A,8|4);//LATCH + init
    MyWait(100);
    PortOut(0x37A,8);//init
}