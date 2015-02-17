#include <windows.h>
#include "mystruct.h"

inty64 Takt(void); //broi taktove za 100us
inty64 Rdtsc(void); //broi taktova ot kakto e pusnat procesora
VOID MyWait(inty64);         // int e: v micro sec.
inty64 us100;  //broi taktove za 100us




//--------funkciii za timer-a -------------
inty64 Takt(void)
{
	DWORD i;
	inty64 lBz, dBz;
	lBz=Rdtsc();		   
    for(i=GetTickCount(); GetTickCount()-100<i; )
    dBz=Rdtsc();
		  
    return (dBz-lBz)/1000; // vrushta broi takta(na proc) za 100us
}

inty64 Rdtsc(void)
{
	_asm{RDTSC} // vrushta broi taktove na procesora ot kakto e pusnat
}

VOID MyWait(inty64 iWait)  
{
    inty64 i;


	for(i=Rdtsc(); Rdtsc()-((us100/100)*iWait)<i; );
    return;
}
//---------------------------krai na funkcii za timer	
	