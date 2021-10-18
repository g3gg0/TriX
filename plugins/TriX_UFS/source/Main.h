//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UFSThread.h"
#include "UFS.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TMemo *Info;
        TComboBox *DeviceSelect;
        TButton *Boot;
        TButton *OpenClose;
        TLabel *Label1;
        TButton *GetASK;
        TButton *GetIMEI;
        TButton *WriteFlashRPL;
        TOpenDialog *OpenFile;
        TButton *WriteUEMRPL;
        TButton *Refresh;
        void __fastcall BootClick(TObject *Sender);
        void __fastcall OpenCloseClick(TObject *Sender);
        void __fastcall GetASKClick(TObject *Sender);
        void __fastcall GetIMEIClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall WriteFlashRPLClick(TObject *Sender);
        void __fastcall WriteUEMRPLClick(TObject *Sender);
        void __fastcall RefreshClick(TObject *Sender);
private:	// User declarations
        UFSExchange *UFSEx;
        void __fastcall ConvertData( char *indata, char *outdata, int size );

public:		// User declarations
        TUFSx *UFS;
        void __fastcall Boot2nd();
        void __fastcall Flash();
        TFIA *boot2ndLdr, *flashLdr, *cbusLdr;
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
