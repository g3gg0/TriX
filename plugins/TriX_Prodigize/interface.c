
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <sys/io.h>
typedef unsigned char* HANDLE;
#endif

#include "defines.h"
#include "options.h"
#include "mem.h"

#include "trixplug.h"
#include "prodigize.h"

#include "trixplug_wrapper.h"



#ifdef WIN32
#include "device.h"
#else
#include "static/types.h"
#include "static/LOWDEV.h"
#include "static/LOWFLASH.h"
#endif

#include "interface.h"

extern int verbosity;
extern struct trix_functions *ft;
HANDLE pDevHandle = NULL;

#define ShowMessage printf


// Open device
int OpenPdDriver(void)
{
#ifdef WIN32
   int ver,osv = 0;
   unsigned short i;

   ver = GetOsVersion(&osv);
   if(!ver){
     ShowMessage("Unable to detect operating system!\n");
     return 0;
   }

   if(!osv){
     ShowMessage("This program will not run on Windows 98!\n");
     return 0;
   }

   i = OpenAfdDriver(&pDevHandle,0x378);
   if(i){
	   ShowMessage("ERROR %i: Unable to open Prodigy driver!\n", i );
     return 0;
   }

   ver = SetDeviceMode(0x06,0x00);
   if(!ver){
     ShowMessage("ERROR %i: Unable to set flash mode!\n", i);
     return 0;
   }
#else
    printf ( "[*] Setting up port permissions\n" );
    if (ioperm(0x378, 3, 1)) 
    {
	perror("ioperm"); 
	return 0;
    }
	
#endif
	if ( verbosity > 2 )
   ShowMessage("Opened Prodigy driver\n");
   return 1;
}

// Close device
void ClosePdDriver(void)
{
#ifdef WIN32
   CloseNTDriver();
#else
    printf ( "[*] Quitting\n" );
    if (ioperm(0x378, 3, 0)) 
	perror("ioperm");
#endif
}

int SetSpeeds (unsigned char *deftimes)
{
#ifdef WIN32
	return SetDefaultTimesA ( deftimes );
#else
	return SetDefaultTimes ( deftimes );
#endif
}

// Read UPP ID
//
int CheckBootRomVersion(unsigned char *pUpp )
{
   unsigned short res;

   // Idle state
#ifdef WIN32
   res = SetIdleStateXP(pUpp);
#else
   res = SetIdleState(pUpp);
#endif
   if(res != 0)
   {
	 if ( verbosity > 1 )
	 ShowMessage ( "ERROR %i while SetIdleStateXP()\n", res );
     return 0;
   }

   return 1;
}

// Load u_2nd
//
int LoadSecond ( unsigned char *pConfig, unsigned char *csize, unsigned char *loader, int loader_length )
{
   unsigned short res;
   unsigned char *pBsize,*pMRes;

   pBsize = (unsigned char *)malloc(2);
   pMRes = (unsigned char *)malloc(2);

   // Init size
   *pBsize = loader_length >> 8;
   *(pBsize + 1) = loader_length & 0xFF;

   // Loader
#ifdef WIN32
   res = InitBootstrapXP ( loader, pBsize, pConfig, pMRes, 0x01 );
#else
   res = InitBootstrap ( loader, pBsize, pConfig, pMRes, 0x01 );
#endif
   if ( res != 0 )
   {
	 if ( verbosity > 1 )
	 ShowMessage ( "ERROR %i while InitBootstrapXP()\n", res );
     free(pBsize);
     free(pMRes);
     return 0;
   }

   // Return readed size
   *csize = *(pMRes + 1);

   free(pBsize);
   free(pMRes);

   return 1;
}


// load third
int LoadThird ( unsigned short *err, unsigned char *loader, int loader_length )
{
	unsigned char  *pLoader;

	pLoader = (unsigned char *)malloc ( loader_length + 2 );

	*pLoader++ = (unsigned char)(loader_length >> 8);
	*pLoader++ = (unsigned char)(loader_length & 0xFF);

	memcpy ( pLoader,loader,loader_length );

	pLoader -= 2;

	// Send it
#ifdef WIN32
	*err = SendAlgorithmXP ( pLoader,(loader_length + 2));
#else
	*err = SendAlgo ( pLoader,(loader_length + 2));
#endif
	if ( *err != 0 )
	{
	 if ( verbosity > 1 )
		ShowMessage ( "ERROR %i while SendAlgorithmXP()\n", *err );
		free(pLoader);
		return 0;
	}

	free(pLoader);


	return 1;
}



// Read status
int ReadStatus(void)
{
   unsigned char  err[20];
   unsigned short res;

   // Read status
#ifdef WIN32
   res = ProgrammingStatusXP(err);
#else
   res = ProgrammingStatus(err);
#endif
   if(res)
     return 0;

   return 1;
}

unsigned char NegChkSum(unsigned char *pMessage,unsigned char mSize)
{
   unsigned char i,k=0xFF;

   for(i=0;i<mSize;i++)
   {
      k -= *pMessage++;
   }

   return k;
}

int SendRawData ( unsigned char *data, int length )
{
	int res = 0;
	// send data
#ifdef WIN32
	res = SendToPhoneXP(data,length,0x80000,0x80000);
#else
	res = SendToPhone(data,length,0x80000,0x80000);
#endif
	if(res)
	{
	 if ( verbosity > 1 )
		ShowMessage ( "ERROR %i while SendToPhoneXP()\n", res );
		return 0;
	}

	return 1;
}


int ReadRawData ( unsigned char *data, int bytes )
{
	int res = 0;
	// read data
#ifdef WIN32
	res = UEMCalls ( data, bytes, 0x00, 0x01 );
#else
	res = UemSwitch ( data, bytes, 0x00, 0x01 );
#endif
	if(res)
	{
	 if ( verbosity > 1 )
		ShowMessage ( "ERROR %i while UEMCalls()\n", res );
		return 0;
	}

	return 1;
}
