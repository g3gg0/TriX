//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <time.h>
#include <inifiles.hpp>

#pragma hdrstop
#include "Main.h"
#include "ftd2xx.h"
#include "FIA.h"

#define RPL_SECTION_NAME "SER_PROG_DATA_OUT"
#define RPL_DATA1_SIZE 40
#define RPL_DATA2_SIZE 88

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMainForm *MainForm;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
  UFSEx = new UFSExchange( true );

  UFSEx->noupdate = true;
  UFSEx->DisableButtons();

  UFS = new TUFSx();

  Refresh->Click();
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::BootClick(TObject *Sender)
{
  Info->Lines->Add( "Booting..." );

  UFSEx->cmd = UFSCMD_BOOT;
  UFSEx->Resume();

}

void __fastcall TMainForm::Flash()
{
  int rc, i;
  AnsiString text;
  unsigned char tBuf[ 64 ];

  UFSEx->cmd = UFSCMD_2NDBOOT;

  flashLdr = UFS->SearchFIA( "*.fia", FIA_FLASH, UFS->asicID, UFS->FlashID[ 0 ].ID );
  if( flashLdr == NULL )
  {
    rc = UFSERR_NOFIA;
    Info->Lines->Add( "Unable to find flash loader!" );
  }
  else
    rc = UFSERR_OK;

  if( rc == UFSERR_OK )
  {
    text.sprintf( "Loading %s...", flashLdr->filepath );
    Info->Lines->Add( text );

    text = "";
    flashLdr->GetSection( 0xC3, tBuf, &i );
    if( i != 0 )
      text.cat_sprintf( "Algorithm: %s, ", tBuf );

    flashLdr->GetSection( 0xDB, tBuf, &i );
    if( i != 0 )
      text.cat_sprintf( "version: %u.%u.%u, ", tBuf[ 0 ], tBuf[ 1 ], tBuf[ 2 ] );

    flashLdr->GetSection( 0xD8, tBuf, &i );
    if( i != 0 )
      text.cat_sprintf( "rev: %u.%u", tBuf[ 0 ], tBuf[ 1 ] );

    Info->Lines->Add( text );

    Info->Lines->Add( "Sending Flash loader..." );

    UFSEx->cmd = UFSCMD_FLASH;
  }
  else
    delete flashLdr;
}

void __fastcall TMainForm::Boot2nd()
{
  int rc, i;
  AnsiString text;
  unsigned char tBuf[ 64 ];

  UFSEx->cmd = UFSCMD_BOOT;

  boot2ndLdr = UFS->SearchFIA( "*.fia", FIA_2NDBOOT, UFS->asicID );
  if( boot2ndLdr == NULL )
  {
    rc = UFSERR_NOFIA;
    Info->Lines->Add( "Unable to find 2nd boot loader!" );
  }
  else
    rc = UFSERR_OK;

  if( rc == UFSERR_OK )
  {
    text.sprintf( "Loading %s...", boot2ndLdr->filepath );
    Info->Lines->Add( text );

    text = "";
    boot2ndLdr->GetSection( 0xC2, tBuf, &i );
    if( i != 0 )
      text.cat_sprintf( "Algorithm: %s, ", tBuf );
    boot2ndLdr->GetSection( 0xDB, tBuf, &i );
    if( i != 0 )
      text.cat_sprintf( "version: %u.%u.%u, ", tBuf[ 0 ], tBuf[ 1 ], tBuf[ 2 ] );
    boot2ndLdr->GetSection( 0xD8, tBuf, &i );
    if( i != 0 )
      text.cat_sprintf( "rev: %u.%u", tBuf[ 0 ], tBuf[ 1 ] );
    Info->Lines->Add( text );

    Info->Lines->Add( "Sending 2nd boot..." );

    UFSEx->cmd = UFSCMD_2NDBOOT;
  }
  else
    delete boot2ndLdr;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::OpenCloseClick(TObject *Sender)
{
  AnsiString text;
  int rc;
  char sn[ 32 ];

  if( OpenClose->Caption == "Open" )
  {
    sscanf( DeviceSelect->Text.c_str(), "%s", sn );
    rc = UFS->Open( sn );
    if( rc == UFSERR_OK )
    {
      DeviceSelect->Enabled = false;

      UFSEx->cmd = UFSCMD_UFSINIT;
      UFSEx->Resume();
    }
    else
    {
      text.sprintf( "Error: %s", UFS->GetError( rc ));
      Info->Lines->Add( text );
    }
  }
  else
  {
    UFS->Close();
    OpenClose->Caption = "Open";

    UFSEx->noupdate = true;
    UFSEx->DisableButtons();

    DeviceSelect->Enabled = true;
    OpenClose->Enabled = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GetASKClick(TObject *Sender)
{

  Info->Lines->Add( "Getting ASK data..." );

  UFSEx->cmd = UFSCMD_GETASK;
  UFSEx->Resume();

}

//---------------------------------------------------------------------------

void __fastcall TMainForm::GetIMEIClick(TObject *Sender)
{
  int rc;
  AnsiString text;

  cbusLdr = UFS->SearchFIA( "*_imei.fia", FIA_CBUS, UFS->asicID );
  if( cbusLdr == NULL )
  {
    rc = UFSERR_NOFIA;
    Info->Lines->Add( "Unable to find UEM IMEI loader!" );
  }
  else
    rc = UFSERR_OK;

  if( rc == FIAERR_OK )
  {
    text.sprintf( "Loading %s...", cbusLdr->filepath );
    Info->Lines->Add( text );

    UFSEx->cmd = UFSCMD_GETIMEI;
    UFSEx->Resume();
  }
  else
    delete cbusLdr;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  UFSEx->Terminate();
  UFSEx->Resume();

  UFSEx->WaitFor();

  delete UFS;
  delete UFSEx;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WriteFlashRPLClick(TObject *Sender)
{
  TIniFile *rplFile;
  AnsiString rplData;
  unsigned char *pdata;

  OpenFile->Filter = "RPL files (*.rpl)|*.rpl";

  if( OpenFile->Execute())
  {
    rplFile = new TIniFile( OpenFile->FileName );

    Info->Lines->Add( "File: " + OpenFile->FileName );

    rplData = rplFile->ReadString( RPL_SECTION_NAME, "DATA1", "" );
    if( rplData.Length() == RPL_DATA1_SIZE * 2 )
    {
      ConvertData( rplData.c_str(), UFSEx->data, RPL_DATA1_SIZE );
      Info->Lines->Add( "Writing Flash RPL data..." );

      UFSEx->cmd = UFSCMD_WRITEFLASHRPL;
      UFSEx->Resume();

    }
    else
      if( rplData.Length() != 0 )
        Info->Lines->Add( "Bad Flash data length!" );

    rplFile->Free();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ConvertData( char *indata, char *outdata, int size )
{
  int i;
  unsigned int dbyte;

  for( i = 0; i < size; i++ )
  {
    sscanf( indata, "%02X", &dbyte );
    indata += 2;
    outdata[ i ] = dbyte;
  }
}

void __fastcall TMainForm::WriteUEMRPLClick(TObject *Sender)
{
  TIniFile *rplFile;
  AnsiString rplData;
  unsigned char *pdata;

  OpenFile->Filter = "RPL files (*.rpl)|*.rpl";

  if( OpenFile->Execute())
  {
    rplFile = new TIniFile( OpenFile->FileName );

    Info->Lines->Add( "File: " + OpenFile->FileName );

    rplData = rplFile->ReadString( RPL_SECTION_NAME, "DATA2", "" );
    if( rplData.Length() == RPL_DATA2_SIZE * 2 )
    {
      ConvertData( rplData.c_str(), UFSEx->data, RPL_DATA2_SIZE );
      Info->Lines->Add( "Writing UEM RPL data..." );

      UFSEx->cmd = UFSCMD_WRITEUEMRPL;
      UFSEx->Resume();

    }
    else
      if( rplData.Length() != 0 )
        Info->Lines->Add( "Bad UEM data length!" );

    rplFile->Free();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RefreshClick(TObject *Sender)
{
  FT_STATUS ftStatus;
  FT_DEVICE_LIST_INFO_NODE *ftDevList;
  DWORD numDevs, i;
  AnsiString text;

  DeviceSelect->Items->Clear();

  ftStatus = FT_CreateDeviceInfoList( &numDevs );
  if( ftStatus == FT_OK )
  {
    if( numDevs != 0 )
    {
      ftDevList = (FT_DEVICE_LIST_INFO_NODE *)malloc( numDevs * sizeof( FT_DEVICE_LIST_INFO_NODE ));
      ftStatus = FT_GetDeviceInfoList( ftDevList, &numDevs );
      if( ftStatus == FT_OK )
      {
        for( i = 0; i < numDevs; i++ )
        {
          if( ftDevList[ i ].ID == UFS_ID )
          {
            text.sprintf( "%s , %s", ftDevList[ i ].SerialNumber, ftDevList[ i ].Description );
            DeviceSelect->Items->Add( text );
          }
        }
      }
      free( ftDevList );
    }
    else
      Info->Lines->Add( "No devices found!" );
  }
  else
    Info->Lines->Add( "Unable to get device list!" );

  if( DeviceSelect->Items->Count >= 1 )
  {
    DeviceSelect->ItemIndex = 0;
    OpenClose->Enabled = true;
  }
}
//---------------------------------------------------------------------------

