//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UFSThread.h"
#include "Main.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall UFSExchange::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall UFSExchange::UFSExchange(bool CreateSuspended)
        : TThread(CreateSuspended)
{
  noupdate = false;
  Priority = tpHighest;
}
//---------------------------------------------------------------------------
void __fastcall UFSExchange::Execute()
{
  int i;

  while( !Terminated )
  {
    Synchronize( DisableButtons );

    curcmd = cmd;

    for( i = 0; i < MAXTEXTLINES; i++ )
      text[ i ] = "";
    linescount = 0;

    time_now = GetTickCount();

    switch( curcmd )
    {
    case UFSCMD_UFSINIT:
      UFSInit();
      break;
    case UFSCMD_BOOT:
      Boot();
      break;
    case UFSCMD_2NDBOOT:
      Boot2nd();
      break;
    case UFSCMD_FLASH:
      Flash();
      break;
    case UFSCMD_GETASK:
      GetASK();
      break;
    case UFSCMD_GETIMEI:
      GetIMEI();
      break;
    case UFSCMD_WRITEFLASHRPL:
      WriteFlashRPL();
      break;
    case UFSCMD_WRITEUEMRPL:
      WriteUEMRPL();
      break;
    }

    time_after = GetTickCount();

    Synchronize( PrintLogout );

    if( cmd == curcmd )
      Suspend();
  }
}
//---------------------------------------------------------------------------

void __fastcall UFSExchange::WriteFlashRPL()
{
  int i;

  rc = MainForm->UFS->WriteFlashRPL( data );
  if( rc == UFSERR_OK )
    text[ linescount++ ] = "Done!";
}

void __fastcall UFSExchange::WriteUEMRPL()
{
  int i;

  rc = MainForm->UFS->WriteUEMRPL( data );
  if( rc == UFSERR_OK )
    text[ linescount++ ] = "Done!";
}

void __fastcall UFSExchange::GetASK()
{
  int i;

  rc = MainForm->UFS->GetASK( data );
  if( rc == UFSERR_OK )
  {
    text[ linescount ] = "";
    for( i = 0; i < 74; i++ )
      text[ linescount ].cat_sprintf( "%02X", data[ i ] );
    linescount++;
  }
}

void __fastcall UFSExchange::UFSInit()
{
  rc = MainForm->UFS->Init();
  if( rc == UFSERR_OK )
  {
    text[ linescount++ ].sprintf( "%s, ID: %04X", MainForm->UFS->BootVer, MainForm->UFS->AtmelID );
    text[ linescount++ ].sprintf( "%s, S/N: %u", MainForm->UFS->FWVer, MainForm->UFS->SerNum );

    buttonstate[ BUTTON_BOOT ] = true;
    buttonstate[ BUTTON_GETASK ] = false;
    buttonstate[ BUTTON_GETUEMIMEI ] = false;
  }
}

void __fastcall UFSExchange::Boot()
{
  rc = MainForm->UFS->BootLdr( &(MainForm->UFS->asicID) );
  if( rc == UFSERR_OK )
  {
    text[ linescount++ ].sprintf( "ASIC ID: %04X", MainForm->UFS->asicID );
    cmd = UFSCMD_2NDBOOT;
  }
}

void __fastcall UFSExchange::Boot2nd()
{
  int i;

  rc = MainForm->UFS->Boot2ndLdr( MainForm->boot2ndLdr );
  if( rc == UFSERR_OK )
  {
    text[ linescount++ ] = "Ready!";
    for( i = 0; i < MainForm->UFS->FlashNum; i++ )
    {
      if( MainForm->UFS->FlashID[ i ].sector == 0 )
        continue;
      text[ linescount++ ].sprintf( "Flash ID %lu: %08X @ 0x%02X000000", i,
        MainForm->UFS->FlashID[ i ].ID,
        MainForm->UFS->FlashID[ i ].address );
    }
    text[ linescount ] = "Flash data: ";
    for( i = 0; i < 16; i++ )
      text[ linescount ].cat_sprintf( "%02X", MainForm->UFS->ExtBUSC[ i ] );

    linescount++;
    cmd = UFSCMD_FLASH;
  }

  delete MainForm->boot2ndLdr;
}

void __fastcall UFSExchange::Flash()
{
  rc = MainForm->UFS->FlashLdr( MainForm->flashLdr );
  if( rc == UFSERR_OK )
  {
    text[ linescount++ ] = "Ready!";
    buttonstate[ BUTTON_GETASK ] = true;
    buttonstate[ BUTTON_GETUEMIMEI ] = true;
    buttonstate[ BUTTON_WRITEFLASHRPL ] = true;
    buttonstate[ BUTTON_WRITEUEMRPL ] = true;
  }

  delete MainForm->flashLdr;
}

void __fastcall UFSExchange::GetIMEI()
{
  int i;

  rc = MainForm->UFS->CBusLdr( MainForm->cbusLdr );
  if( rc == UFSERR_OK )
  {
    text[ linescount++ ] = "Ready!";
    if((( MainForm->UFS->IMEIESN[ 7 ] & 0xF0 ) == 0xF0 ) &&
      (( MainForm->UFS->IMEIESN[ 0 ] & 0x0F ) == 0x03 ))
    {
    // IMEI found
      text[ linescount ] = "UEM IMEI: ";
      for( i = 1; i < 15; i++ )
        text[ linescount ].cat_sprintf( "%1X", ( MainForm->UFS->IMEIESN[ i >> 1 ] >> ( 4 * ( i & 1 ))) & 0x0F );
    }
    else if( MainForm->UFS->IMEIESN[ 7 ] == 0xEF )
    {
    // ESN found
      text[ linescount ] = "UEM ESN: ";
      text[ linescount ].cat_sprintf( "%03u-%08lu", MainForm->UFS->IMEIESN[ 0 ],
        ((unsigned long)MainForm->UFS->IMEIESN[ 1 ] << 16 ) |
        ((unsigned long)MainForm->UFS->IMEIESN[ 2 ] << 8 ) |
        ((unsigned long)MainForm->UFS->IMEIESN[ 3 ] ));
    }
    else
    {
      for( i = 0; i < 8; i++ )
        if( MainForm->UFS->IMEIESN[ i ] != 0 )
          break;
      if( i == 8 )
      {
        // UEM Empty
        text[ linescount ] = "Empty UEM";
      }
      else
      {
        // Unknown
        text[ linescount ] = "Unknown: ";
        for( i = 0; i < 8; i++ )
          text[ linescount ].cat_sprintf( "%02X", MainForm->UFS->IMEIESN[ i ] );
      }
    }

    linescount++;
  }

  delete MainForm->cbusLdr;
}

void __fastcall UFSExchange::PrintLogout()
{
  int i;

  if( rc != UFSERR_OK )
    text[ linescount++ ].sprintf( "Error: %s", MainForm->UFS->GetError( rc ));

//  text[ linescount++ ].sprintf( "Operation took %.2fs", ((double)( time_after - time_now ) / 1000 ) + 0.005 );
  time_now = time_after;

  for( i = 0; i < linescount; i++ )
    if( text[ i ].Length() != 0 )
      MainForm->Info->Lines->Add( text[ i ] );

  switch( curcmd )
  {
  case UFSCMD_BOOT:
    if( cmd == UFSCMD_2NDBOOT )
    {
      noupdate = true;
      MainForm->Boot2nd();
    }
    break;
  case UFSCMD_2NDBOOT:
    if( cmd == UFSCMD_FLASH )
    {
      noupdate = true;
      MainForm->Flash();
    }
    break;
  }

  if( curcmd == cmd )
    RestoreButtons();
}

void __fastcall UFSExchange::DisableButtons()
{
  if( !noupdate )
  {
    buttonstate[ BUTTON_OPENCLOSE ] = MainForm->OpenClose->Enabled;
    buttonstate[ BUTTON_BOOT ] = MainForm->Boot->Enabled;
    buttonstate[ BUTTON_GETASK ] = MainForm->GetASK->Enabled;
    buttonstate[ BUTTON_GETUEMIMEI ] = MainForm->GetIMEI->Enabled;
    buttonstate[ BUTTON_WRITEFLASHRPL ] = MainForm->WriteFlashRPL->Enabled;
    buttonstate[ BUTTON_WRITEUEMRPL ] = MainForm->WriteUEMRPL->Enabled;
  }
  MainForm->OpenClose->Enabled = false;
  MainForm->Boot->Enabled = false;
  MainForm->GetASK->Enabled = false;
  MainForm->GetIMEI->Enabled = false;
  MainForm->WriteFlashRPL->Enabled = false;
  MainForm->WriteUEMRPL->Enabled = false;

  noupdate = false;
}

void __fastcall UFSExchange::RestoreButtons()
{
  MainForm->OpenClose->Enabled = buttonstate[ BUTTON_OPENCLOSE ];
  if( buttonstate[ BUTTON_OPENCLOSE ] )
    MainForm->OpenClose->Caption = "Close";
  else
    MainForm->DeviceSelect->Enabled = true;
  MainForm->Boot->Enabled = buttonstate[ BUTTON_BOOT ];
  MainForm->GetASK->Enabled = buttonstate[ BUTTON_GETASK ];
  MainForm->GetIMEI->Enabled = buttonstate[ BUTTON_GETUEMIMEI ];
  MainForm->WriteFlashRPL->Enabled = buttonstate[ BUTTON_WRITEFLASHRPL ];
  MainForm->WriteUEMRPL->Enabled = buttonstate[ BUTTON_WRITEUEMRPL ];
}
