#include <windows.h>
#include <string.h>


extern  BOOL bCommQuit;
extern  VOID MyWait(inty64);
BOOL    RadioCheck(HWND);//proverqva dali ima vurzano radio
VOID    ChangeMode(HWND, char);
VOID    ChangeBand(HWND, char, char); //Smenq banda
VOID    BaudRate(HWND, HINSTANCE, int);  //inicializira baud rate-a
extern  VOID SwitchOffTrc(void);
char    FmOn(HWND hwnd, int iPower); //Puska na FM kato predava
VOID    FmOff(HWND hwnd, char cMode);
extern  BOOL CALLBACK AboutCommPortName(HWND, UINT, WPARAM, LPARAM);

VOID    CloseCom(void); 

int k=0;//v FmOn--> e k++; stane li >0 puskam oshte edin PurgeComm i WaitCommEvent
        //zashtoto po nqkakva  prichina poluchavam po 2 Ehota??
char szComPort[20];

COMMTIMEOUTS    timeouts;
HANDLE		    hCom1 = INVALID_HANDLE_VALUE;
DCB				dcbCom1;
DWORD			wWritten; //bytes written
TCHAR           szBuffer[50]; //for testing


VOID BaudRate(HWND hwnd, HINSTANCE hInstance, int iBaud)
{

    //pravi se, dokato ne se otvori COMMporta(dokato ne se napishe validno ime)
    do
    {
      if(bCommQuit) 
          return;

      hCom1 = CreateFile(szComPort, GENERIC_READ | GENERIC_WRITE,
		               0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	  if(hCom1 == INVALID_HANDLE_VALUE)
	  { 
         TCHAR szBuffer[50];
         wsprintf(szBuffer, "Can't open ComPort: %s", szComPort);
		 MessageBox(hwnd, szBuffer, TEXT("Error..."), MB_OK);
		 DialogBox(hInstance, (LPSTR)121, hwnd, AboutCommPortName); 
	  }
	}while(INVALID_HANDLE_VALUE==hCom1);
    
	if(!GetCommTimeouts(hCom1, &timeouts))
	  MessageBox(hwnd, TEXT("error getting COMMTIMEOUTS"), TEXT("error getting COMMTIMEOUTS"), MB_OK);
 
    //naglasqm timeout-ite za RadioCheck i FmOn otdelno, poneje 
	//za FmON trqbwa da sa vsichki na 0. Za  RadioCheck trqbwa MAXDWORD na purvoto
	//Za da vrushta vednega nezavisimo dali e prochelo neshto ili ne
    timeouts.ReadIntervalTimeout=0;
    timeouts.ReadTotalTimeoutConstant=0;
    timeouts.ReadTotalTimeoutMultiplier=0;
    timeouts.WriteTotalTimeoutMultiplier=0;
    timeouts.WriteTotalTimeoutConstant=0;
    if(!SetCommTimeouts(hCom1, &timeouts))
		MessageBox(hwnd, TEXT("Cant SetCommTimeouts"), TEXT("error"), MB_OK);
    ///end timeouts



	
	if(!GetCommState(hCom1, &dcbCom1))
		MessageBox(hwnd, TEXT("Cant retrieve CommState"), TEXT("error"), MB_OK);
	
    //baude rate and others
	dcbCom1.DCBlength   = sizeof dcbCom1;
	dcbCom1.BaudRate    = iBaud;
    dcbCom1.ByteSize    = 8;
	dcbCom1.StopBits    = ONESTOPBIT;
	dcbCom1.Parity      = NOPARITY;
    dcbCom1.fRtsControl = 1;
	if(!SetCommState(hCom1, &dcbCom1))
		MessageBox(hwnd, TEXT("Cant retrieve CommState"), TEXT("error"), MB_OK);


    
	return;
}

	
VOID ChangeBand(HWND hwnd,char cMhz,char cKhz)
{
    BOOL bResult = TRUE;    // will be FALSE in case of an error
	BYTE buffer;            // byte to be send

    if(hCom1 == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, TEXT("ComPort is not initialized: Restart the program!"), TEXT("error"), MB_OK);
        return;
    }
        

    PurgeComm(hCom1, PURGE_TXCLEAR);
	PurgeComm(hCom1, PURGE_TXCLEAR | PURGE_RXCLEAR);
	
    buffer=0xFE;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=0x5C;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=0xE0;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=0x5;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

    buffer=0x0;	
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=0x0;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=cKhz;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=cMhz;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	buffer=0xFD;
	bResult = bResult && WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
		
	if(bResult == FALSE)
    {
        MessageBox(hwnd, TEXT("Error writing to ComPort: check cable!"), TEXT("error"), MB_OK);
    }

}



char  FmOn(HWND hwnd, int iPower)
{ 
	int     i=0;
    DWORD   dwCommEvent;
	DWORD   dwRead=0;//Bytes read
	BYTE    cSave[100]={0};//responce from rig
    BYTE    cVid=0;//byte read
    BYTE    buffer;
	char    bufo[150]={0};// test buffer

   
    if(hCom1 == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, TEXT("ComPort is not initialized: Restart the program!"), TEXT("error"), MB_OK);
        return 0;
    }


    PurgeComm(hCom1, PURGE_TXCLEAR | PURGE_RXCLEAR);
	//Cheta tekushtiq wid na rabota------------
	buffer=0xFE;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);	
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x4;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	

    
	SetCommMask(hCom1,  EV_RXCHAR | EV_RXFLAG);
    WaitCommEvent(hCom1, &dwCommEvent, NULL);
	PurgeComm(hCom1,  PURGE_RXCLEAR);
    if(k>0){//minalo e edno izpulnenie na FmOn i polucham po dve ehota, za tova puskam dva puti PurgeComm
	 WaitCommEvent(hCom1, &dwCommEvent, NULL);
     PurgeComm(hCom1,  PURGE_RXCLEAR);
	}


    do 
    {
        ReadFile(hCom1, &cVid, 1, &dwRead, NULL);
        cSave[i]=cVid;   
        i++;
	}while(cVid!=0xFD);
	
	   
	PurgeComm(hCom1,  PURGE_RXCLEAR);


	//krai chetene     
    buffer=0xFE;
	//Slaga na FM
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0x5C;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0xE0;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0x1;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
	MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
    buffer=0x5;	
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0xFD;
	if(!WriteFile(hCom1, &buffer, sizeof buffer, &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);

    
	
	
	//Naglasq moshtnostta na 80w
	buffer=0xFE;
	wWritten=0;
	PurgeComm(hCom1, PURGE_TXCLEAR);
	
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0x5C;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0xE0;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0x14;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
	    MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
    buffer=0xA;	
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
	    MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
    
	
	if(iPower>95 &&iPower<=100)//za razlichni varianti na moshtnostta
	{
		buffer=0x2; //80w	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x55; //80w
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	
	else if(iPower>90 && iPower<=95)//za razlichni varianti na moshtnostta
	{
		buffer=0x2; //80w	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x42; //80w
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}

    else if(iPower>85 && iPower<=90)//
	{
		buffer=0x2; //50w	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x29; //50w
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	
	else if(iPower>80 && iPower<=85)//
	{
		buffer=0x2; //	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x17; //50w
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	
	else if(iPower>75 && iPower<=80)//
	{
		buffer=0x2; //	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x10; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>70 && iPower<=75)//k
	{
		buffer=0x1; //	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x98; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	
	else if(iPower>65 && iPower<=70)//
	{
		buffer=0x1; //	
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x85; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}

	else if(iPower>60 && iPower<=65)//
	{
		buffer=0x1; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x78; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	
	else if(iPower>55 && iPower<=60)//
	{
		buffer=0x1; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x70; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}

	else if(iPower>50 && iPower<=55)//
	{
		buffer=0x1; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x65; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>45 && iPower<=50)//
	{
		buffer=0x1; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x53; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>40 && iPower<=45)//
	{
		buffer=0x1; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x40; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>35 && iPower<=40)//
	{
		buffer=0x1; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x28; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>30 && iPower<=35)//
	{
		buffer=0x0; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x10; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>25 && iPower<=30)//
	{
		buffer=0x0; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x97; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>20 && iPower<=25)//
	{
		buffer=0x0; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x85; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>15 && iPower<=20)//
	{
		buffer=0x0; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x70; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	else if(iPower>10 && iPower<=15)//
	{
		buffer=0x0; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x50; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
		
	
	
	
	else //
	{
		buffer=0x0; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
		buffer=0x25; //
			if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
			MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	}
	
	

	
	buffer=0xFD;
	if(!WriteFile(hCom1, &buffer, sizeof buffer, &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	//---------krai naglasqne Moshtnost----------------------
		
 
	///PUska na predavane
	buffer=0xFE;
	wWritten=0;
	PurgeComm(hCom1, PURGE_TXCLEAR);
	
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0x5C;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0xE0;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0x1C;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
	MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
    buffer=0x0;	
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
    buffer=0x01;	
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	buffer=0xFD;
	if(!WriteFile(hCom1, &buffer, sizeof buffer, &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error"), MB_OK);
	

	 k++;//stane li >0 puskam oshte edin PurgeComm
	PurgeComm(hCom1, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return cSave[5];
}

VOID  FmOff(HWND hwnd, char cMode)
{
    BYTE buffer;

    if(hCom1 == INVALID_HANDLE_VALUE)
        return;

	PurgeComm(hCom1, PURGE_TXCLEAR);
	PurgeComm(hCom1, PURGE_TXCLEAR);
	
    //Puska na priemane
	buffer=0xFE;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error(FmOff)"), MB_OK);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x1C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    buffer=0x0;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    buffer=0x0;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);

	
	PurgeComm(hCom1, PURGE_TXCLEAR);
	
    ///Vkliuchva na priemane
    buffer=0xFE;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error(FmOff)"), MB_OK);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x1;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    buffer=cMode;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	


	PurgeComm(hCom1, PURGE_TXCLEAR);
	
    //Naglasq moshtnostta
	buffer=0xFE;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error(FmOff)"), MB_OK);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x14;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xA;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x2;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    buffer=0x55;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
     

   return;
}


VOID ChangeMode(HWND hwnd, char cMode)
{
    BYTE buffer;

    if(hCom1 == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, TEXT("ComPort is not initialized: Restart the program!"), TEXT("error"), MB_OK);
        return;
    }

    
	PurgeComm(hCom1, PURGE_TXCLEAR);

    //Slaga na opredeleniq mode
    buffer=0xFE;
    if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error(ChangeMode)"), MB_OK);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x1;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    buffer=cMode;//lsb-0x00; usb-0x01; cw-0x03;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	
	return;
}



BOOL  RadioCheck(HWND hwnd)
{
	DWORD dwRead1;//Bytes read
    BYTE cVid1=0;//byte read
	char bufo1[150]={0};// test buffer
    DWORD dwSecStart;
    BYTE buffer;
	

    if(hCom1 == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, TEXT("ComPort is not initialized: Restart the program!"), TEXT("error"), MB_OK);
        return FALSE;
    }

    //naglasqm timeout-ite za RadioCheck i FmOn otdelno, poneje 
	//za FmON trqbwa da sa vsichki na 0. Za  RadioCheck trqbwa MAXDWORD na purvoto
	//Za da vrushta vednega nezavisimo dali e prochelo neshto ili ne
    timeouts.ReadIntervalTimeout=MAXDWORD;
    timeouts.ReadTotalTimeoutConstant=0;
    timeouts.ReadTotalTimeoutMultiplier=0;
    timeouts.WriteTotalTimeoutMultiplier=0;
    timeouts.WriteTotalTimeoutConstant=0;
    if(!SetCommTimeouts(hCom1, &timeouts))
		MessageBox(hwnd, TEXT("Cant SetCommTimeouts"), TEXT("error"), MB_OK);
///end timeouts



	//Cheta tekushtiq wid na rabota------------
	buffer=0xFE;
	if(!WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0))
		MessageBox(hwnd, TEXT("Error writing to COM1"), TEXT("error(RadioCheck)"), MB_OK);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x04;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    
	//SetCommMask(hCom1,  EV_RXCHAR | EV_RXFLAG);
	//WaitCommEvent(hCom1, &dwCommEvent, NULL);
    //PurgeComm(hCom1,  PURGE_RXCLEAR | PURGE_TXCLEAR);
    // chakam 1 sec da vidq dali she doide neshto ot IComa
    dwSecStart=GetTickCount();
	while(GetTickCount()<(dwSecStart+1000));
	 ReadFile(hCom1, &cVid1, 1, &dwRead1, NULL);//cheta i ako FE: All is OK
	
	
	 
	  //naglasqm timeout-ite za RadioCheck i FmOn otdelno, poneje 
	//za FmON trqbwa da sa vsichki na 0. Za  RadioCheck trqbwa MAXDWORD na purvoto
	//Za da vrushta vednega nezavisimo dali e prochelo neshto ili ne
    timeouts.ReadIntervalTimeout=0;
    timeouts.ReadTotalTimeoutConstant=0;
    timeouts.ReadTotalTimeoutMultiplier=0;
    timeouts.WriteTotalTimeoutMultiplier=0;
    timeouts.WriteTotalTimeoutConstant=0;
    if(!SetCommTimeouts(hCom1, &timeouts))
		MessageBox(hwnd, TEXT("Cant SetCommTimeouts"), TEXT("error"), MB_OK);
    ///end timeouts //Vrushtam starite timeouts
	 
	 
	if(cVid1==0xFE)
	{
		//wsprintf(bufo1, TEXT("%x"), cVid);
		MessageBox(hwnd, TEXT("Transceiver initialized!"),TEXT("Transceiver initialized!"), MB_OK);
		return TRUE;
	}
	else
	{
		//wsprintf(bufo1, TEXT("%x"), cVid);
		MessageBox(hwnd, TEXT("Transceiver NOT FOUND!"),TEXT("Transceiver NOT FOUND!"), MB_OK);
		return FALSE;
	}
}

VOID SwitchOffTrc(void)
{
    BYTE buffer;

	buffer=0xFE;
	PurgeComm(hCom1, PURGE_TXCLEAR);
    //Slaga na opredeleniq mode
    WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x5C;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xE0;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0x18;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
    buffer=0x0;	
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	buffer=0xFD;
	WriteFile(hCom1, &buffer, sizeof (buffer), &wWritten, 0);
	
	return;
}

VOID  CloseCom(void)
{
	CloseHandle(hCom1);

	return;
}
