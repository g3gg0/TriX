//---------------------------------------------------------------------------

#ifndef UFSThreadH
#define UFSThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------

enum {
  UFSCMD_UFSINIT = 0,
  UFSCMD_BOOT,
  UFSCMD_2NDBOOT,
  UFSCMD_FLASH,
  UFSCMD_GETASK,
  UFSCMD_GETIMEI,
  UFSCMD_WRITEFLASHRPL,
  UFSCMD_WRITEUEMRPL
};

enum {
  BUTTON_OPENCLOSE = 0,
  BUTTON_BOOT,
  BUTTON_GETASK,
  BUTTON_GETUEMIMEI,
  BUTTON_WRITEFLASHRPL,
  BUTTON_WRITEUEMRPL,
  BUTTONS_COUNT
};

#define MAXTEXTLINES    32

class UFSExchange : public TThread
{            
private:
protected:
        void __fastcall Execute();
        void __fastcall PrintLogout();
        void __fastcall Boot();
        void __fastcall Boot2nd();
        void __fastcall Flash();
        void __fastcall GetASK();
        void __fastcall GetIMEI();
        void __fastcall WriteFlashRPL();
        void __fastcall WriteUEMRPL();
        void __fastcall UFSInit();

        DWORD time_now, time_after;
        int rc;
        AnsiString text[ MAXTEXTLINES ];
        int linescount;
        int curcmd;
public:
        int cmd;
        unsigned char data[ 32768 ];
        bool buttonstate[ BUTTONS_COUNT ];
        bool noupdate;
        __fastcall UFSExchange(bool CreateSuspended);
        void __fastcall DisableButtons();
        void __fastcall RestoreButtons();
};
//---------------------------------------------------------------------------
#endif
