#include <windows.h>
#include <stdio.h>
#include "io.h"
#include "mystruct.h"
char   szSwr[15];
extern inty64 Rdtsc(void); //ot timer.c , vrushta broi taktove otkakto e pusnato PC-to
extern VOID MyWait(int iWait);//ot timer.c
extern VOID SetThrough(BOOL);


inty64 Period(void);
VOID SwrOn(void);
VOID SwrOff(void);
long double  GetSwr(void); 
int  TestGetSwr(void);
int TunerTest(void);

long double  GetSwr(void)
{
		
        long double lReverse,lForward,lKsw; 
 		inty64 lPeriodR[10], lPeriodS[10],lTemp;
		BOOL k;		
		
		//Reverse or Straight: 1 or 0(bin)
		//Reverse-------------------------------------------------------
		PortOut(0x378, 1 | 2 | (char)128);//data 128 e za da e Through
	    PortOut(0x37A,8|4);//init
	    MyWait(100);//izchakva 100us
	    PortOut(0x37A,8);//init
        MyWait(10000);
		//Izmerva se 2x3puti kato se vzima medianata na dvata puti
		//i se pravi sredno aritmetichno
	 	lPeriodR[0]=Period();
		lPeriodR[1]=Period();
        lPeriodR[2]=Period();
        lPeriodR[3]=Period();
		lPeriodR[4]=Period();
        lPeriodR[5]=Period();
		lPeriodR[6]=Period();
		lPeriodR[7]=Period();
        lPeriodR[8]=Period();
         //namiram medianata ot trite perioda lPeriod[4]
		
		do
		{
			k=0;
			if(lPeriodR[1]>lPeriodR[0]){
			lTemp=lPeriodR[0];
			lPeriodR[0]=lPeriodR[1];
			lPeriodR[1]=lTemp;
			k=1;
			}

			if(lPeriodR[2]>lPeriodR[1]){
			lTemp=lPeriodR[1];
			lPeriodR[1]=lPeriodR[2];
			lPeriodR[2]=lTemp;
			k=1;
			}
			
			if(lPeriodR[3]>lPeriodR[2]){
			lTemp=lPeriodR[2];
			lPeriodR[2]=lPeriodR[3];
			lPeriodR[3]=lTemp;
			k=1;
			}		   

			if(lPeriodR[4]>lPeriodR[3]){
			lTemp=lPeriodR[3];
			lPeriodR[3]=lPeriodR[4];
			lPeriodR[4]=lTemp;
			k=1;
			}		   

			if(lPeriodR[5]>lPeriodR[4]){
			lTemp=lPeriodR[4];
			lPeriodR[4]=lPeriodR[5];
			lPeriodR[5]=lTemp;
			k=1;
			}		   

			if(lPeriodR[6]>lPeriodR[5]){
			lTemp=lPeriodR[5];
			lPeriodR[5]=lPeriodR[6];
			lPeriodR[6]=lTemp;
			k=1;
			}
			
			if(lPeriodR[7]>lPeriodR[6]){
			lTemp=lPeriodR[6];
			lPeriodR[6]=lPeriodR[7];
			lPeriodR[7]=lTemp;
			k=1;
			}	
			
			if(lPeriodR[8]>lPeriodR[7]){
			lTemp=lPeriodR[7];
			lPeriodR[7]=lPeriodR[8];
			lPeriodR[8]=lTemp;
			k=1;
			}		   	
		}while(k);

	
		lReverse=lPeriodR[4];
		//kraina soinost na perioda na reverse vulna
        
        
		
		//------------------------------------------------------------
		//Stright wave
		PortOut(0x378,2 | (char)128);//data
	    PortOut(0x37A,8|4);//init
	    MyWait(100);//izchakva 100us
	    PortOut(0x37A,8|0);//init
        MyWait(10000);
		//Izmerva se 2x3 kato se vzima medianata na dvata puti
		//i se tyrsi sredno aritmetichnoto na dvete stoinosti
	 	  lPeriodS[0]=Period();
	      lPeriodS[1]=Period();
          lPeriodS[2]=Period();
          lPeriodS[3]=Period();
		  lPeriodS[4]=Period();
          lPeriodS[5]=Period();
		  lPeriodS[6]=Period();
		  lPeriodS[7]=Period();
          lPeriodS[8]=Period();
		  //namiram medianata ot trite periodam lPeriodS[1]
		  do{
		    k=0;
			if(lPeriodS[1]>lPeriodS[0]){
			lTemp=lPeriodS[0];
			lPeriodS[0]=lPeriodS[1];
			lPeriodS[1]=lTemp;
			k=1;
			}

			if(lPeriodS[2]>lPeriodS[1]){
			lTemp=lPeriodS[1];
			lPeriodS[1]=lPeriodS[2];
			lPeriodS[2]=lTemp;
			k=1;
			}
			
			if(lPeriodS[3]>lPeriodS[2]){
			lTemp=lPeriodS[2];
			lPeriodS[2]=lPeriodS[3];
			lPeriodS[3]=lTemp;
			k=1;
			}		   

			if(lPeriodS[4]>lPeriodS[3]){
			lTemp=lPeriodS[3];
			lPeriodS[3]=lPeriodS[4];
			lPeriodS[4]=lTemp;
			k=1;
			}		   

			if(lPeriodS[5]>lPeriodS[4]){
			lTemp=lPeriodS[4];
			lPeriodS[4]=lPeriodS[5];
			lPeriodS[5]=lTemp;
			k=1;
			}		   

			if(lPeriodS[6]>lPeriodS[5]){
			lTemp=lPeriodS[5];
			lPeriodS[5]=lPeriodS[6];
			lPeriodS[6]=lTemp;
			k=1;
			}
			
			if(lPeriodS[7]>lPeriodS[6]){
			lTemp=lPeriodS[6];
			lPeriodS[6]=lPeriodS[7];
			lPeriodS[7]=lTemp;
			k=1;
			}	
			
			if(lPeriodS[8]>lPeriodS[7]){
			lTemp=lPeriodS[7];
			lPeriodS[7]=lPeriodS[8];
			lPeriodS[8]=lTemp;
			k=1;
			}		   	
		}while(k);
       
		lForward=lPeriodS[4];   
		//mashtabiram prava i obratna vulna
	    lForward=10000000/lForward;
		//lForward=lForward-15;
		if(lForward<=0) lForward=1;
	
		lReverse=10000000/lReverse;
        //lReverse=lReverse-15;
        if(lReverse<0) lReverse=1;
		if(lReverse>lForward) return 30;//greshka e
        if((lForward-lReverse)==0) return 40;//da ne deli na 0;
	    //Slagam SWR w char[]
		lKsw=(lReverse+lForward)/(lForward-lReverse);//SWR predi zapetaq
		//------------------------	
		//umnojavam po 12/15(0,8) zashtoto pri KSW 1 pokazva 1,3. I 0,8*1,3 pravi nqkude 1;
		if(lKsw>50) lKsw=50;
		return lKsw*0.81;          	         
}



inty64 Period(void)
{
    	inty64 lPeriodche, lTime1,lTime2;
        ;
		//Reading from port(0x379) bit5(out of paper)	
			while(PortIn(0x379) & 32);
				//dizchakvam da stane 0
            while(!(PortIn(0x379) & 32));//izchakvam da stane 1
			  lTime1=Rdtsc(); //start count
			while(PortIn(0x379) & 32); //izchakvam da stane 0
			 lTime2=Rdtsc(); //end count
		
           
		//namiram duljinata na impulsa v taktove na procesora
		return lPeriodche=lTime2-lTime1;
}


VOID SwrOn(void)
{
	
;
	
	//SWR meter on:  10(bin)
	// PortOut(0x378, 0);//nuliram
	 PortOut(0x37A, 0);//nuliram    
	    MyWait(100);//izchakva 100us
     PortOut(0x378,2 | (char)128);//data 
	 PortOut(0x37A,8|4);//init
	 MyWait(100);//izchakva 100us
	 PortOut(0x37A, 8);//init
        
    return; 
}

VOID SwrOff(void)
{
	   SetThrough(FALSE);
	//Izkliuchva SWR-a
        //PortOut(0x378,0);//data
	    PortOut(0x37A, 0);//nuliram
	     MyWait(100);//izchakva 100us
		PortOut(0x37A,8|4);//init
	    MyWait(100);//izchakva 100us
	    PortOut(0x37A,8);//init
    return;
}



int  TestGetSwr(void)
{
		
        
 		inty64 lPeriodR[10], lPeriodS[10],lTemp, iRev, iFor;
		BOOL k;		
		
		//Reverse or Straight: 1 or 0(bin)
		//Reverse-------------------------------------------------------
		PortOut(0x378, 1 | 2 | (char)128);//data 128 e za da e Through
		PortOut(0x37A,0);
		  MyWait(200);
		PortOut(0x37A,8|4);//init
	    MyWait(100);//izchakva 100us
	    PortOut(0x37A,8);//init
        MyWait(10000);
		//Izmerva se 2x3puti kato se vzima medianata na dvata puti
		//i se pravi sredno aritmetichno
	 	lPeriodR[0]=Period();
		lPeriodR[1]=Period();
		lPeriodR[2]=Period();
		lPeriodR[3]=Period();
		lPeriodR[4]=Period();
        lPeriodR[5]=Period();
		lPeriodR[6]=Period();
		lPeriodR[7]=Period();
        lPeriodR[8]=Period();
         //namiram medianata ot trite perioda lPeriod[4]
		
		do
		{
			k=0;
			if(lPeriodR[1]>lPeriodR[0]){
			lTemp=lPeriodR[0];
			lPeriodR[0]=lPeriodR[1];
			lPeriodR[1]=lTemp;
			k=1;
			}

			if(lPeriodR[2]>lPeriodR[1]){
			lTemp=lPeriodR[1];
			lPeriodR[1]=lPeriodR[2];
			lPeriodR[2]=lTemp;
			k=1;
			}
			
			if(lPeriodR[3]>lPeriodR[2]){
			lTemp=lPeriodR[2];
			lPeriodR[2]=lPeriodR[3];
			lPeriodR[3]=lTemp;
			k=1;
			}		   

			if(lPeriodR[4]>lPeriodR[3]){
			lTemp=lPeriodR[3];
			lPeriodR[3]=lPeriodR[4];
			lPeriodR[4]=lTemp;
			k=1;
			}		   

			if(lPeriodR[5]>lPeriodR[4]){
			lTemp=lPeriodR[4];
			lPeriodR[4]=lPeriodR[5];
			lPeriodR[5]=lTemp;
			k=1;
			}		   

			if(lPeriodR[6]>lPeriodR[5]){
			lTemp=lPeriodR[5];
			lPeriodR[5]=lPeriodR[6];
			lPeriodR[6]=lTemp;
			k=1;
			}
			
			if(lPeriodR[7]>lPeriodR[6]){
			lTemp=lPeriodR[6];
			lPeriodR[6]=lPeriodR[7];
			lPeriodR[7]=lTemp;
			k=1;
			}	
			
			if(lPeriodR[8]>lPeriodR[7]){
			lTemp=lPeriodR[7];
			lPeriodR[7]=lPeriodR[8];
			lPeriodR[8]=lTemp;
			k=1;
			}		   	
		}while(k);

	
		iRev=lPeriodR[4];
		//kraina soinost na perioda na reverse vulna
        
        
		
		//------------------------------------------------------------
		//Stright wave
		PortOut(0x378,2 | (char)128);//data
		PortOut(0x37A,0);//izchistvam 0x37A 
		MyWait(200);
		PortOut(0x37A,8|4);//init
	    MyWait(100);//izchakva 100us
	    PortOut(0x37A,8);//init
        MyWait(10000);
		//Izmerva se 2x3 kato se vzima medianata na dvata puti
		//i se tyrsi sredno aritmetichnoto na dvete stoinosti
	 	  lPeriodS[0]=Period();
		  lPeriodS[1]=Period();
		  lPeriodS[2]=Period();
		  lPeriodS[3]=Period();
		  lPeriodS[4]=Period();
          lPeriodS[5]=Period();
		  lPeriodS[6]=Period();
		  lPeriodS[7]=Period();
          lPeriodS[8]=Period();
		  //namiram medianata ot trite periodam lPeriodS[1]
		  do{
		    k=0;
			if(lPeriodS[1]>lPeriodS[0]){
			lTemp=lPeriodS[0];
			lPeriodS[0]=lPeriodS[1];
			lPeriodS[1]=lTemp;
			k=1;
			}

			if(lPeriodS[2]>lPeriodS[1]){
			lTemp=lPeriodS[1];
			lPeriodS[1]=lPeriodS[2];
			lPeriodS[2]=lTemp;
			k=1;
			}
			
			if(lPeriodS[3]>lPeriodS[2]){
			lTemp=lPeriodS[2];
			lPeriodS[2]=lPeriodS[3];
			lPeriodS[3]=lTemp;
			k=1;
			}		   

			if(lPeriodS[4]>lPeriodS[3]){
			lTemp=lPeriodS[3];
			lPeriodS[3]=lPeriodS[4];
			lPeriodS[4]=lTemp;
			k=1;
			}		   

			if(lPeriodS[5]>lPeriodS[4]){
			lTemp=lPeriodS[4];
			lPeriodS[4]=lPeriodS[5];
			lPeriodS[5]=lTemp;
			k=1;
			}		   

			if(lPeriodS[6]>lPeriodS[5]){
			lTemp=lPeriodS[5];
			lPeriodS[5]=lPeriodS[6];
			lPeriodS[6]=lTemp;
			k=1;
			}
			
			if(lPeriodS[7]>lPeriodS[6]){
			lTemp=lPeriodS[6];
			lPeriodS[6]=lPeriodS[7];
			lPeriodS[7]=lTemp;
			k=1;
			}	
			
			if(lPeriodS[8]>lPeriodS[7]){
			lTemp=lPeriodS[7];
			lPeriodS[7]=lPeriodS[8];
			lPeriodS[8]=lTemp;
			k=1;
			}		   	
		}while(k);
       
		iFor=lPeriodS[4];   
		//mashtabiram prava i obratna vulna
		iFor=10000000/iFor;
        iFor=iFor-30;
		if(iFor<=0) return 250;
	
		iRev=10000000/iRev;
        iRev=iRev-30;

		if(iRev>iFor) return 250;//greshka e
        if((iFor-iRev)==0) return 250;//da ne deli na 0;
	    
		//iKsw=(iRev+iFor)*100/(iFor-iRev);//SWR predi zapetaq
		
		
		return  (iRev*200/iFor);       	        
} 
		
		
int TunerTest(void)
{
        DWORD dwTime;
	    dwTime=30+GetTickCount();
		
		//Reading from port(0x379) bit5(out of paper)	
			while(PortIn(0x379) & 32)//chaka dokato  5bit stane 0
			{
			  if(GetTickCount()>dwTime) return 0;//time-out vrushta 0, toest nqma tuner
			}
			while(!(PortIn(0x379) & 32))//chaka dokato bit se vdigne
			{
				if(GetTickCount()>dwTime) return 0;//time-out vrushta 0, toest nqma tuner
			}
			return 1;
}
