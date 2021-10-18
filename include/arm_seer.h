

// INSERT_DECLARATIONS

unsigned int ARMFind ( unsigned int type, int start, int end, unsigned int direction ) { return __arm_find( GetWorkspace(), type, start, end, direction ); }
unsigned int ARMGet ( unsigned int x ) { return __arm_get ( GetWorkspace(), x ); }
unsigned int ARMSet(unsigned int x, int y, int z) { return __arm_set(GetWorkspace(),x,y,z); }
unsigned int ARMFindSrcOfDest(int a, int b, int c, int d, unsigned int e) { __arm_find_src_of_dest(GetWorkspace(),a,b,c,d,e); }

#define BL  0
#define B   1
#define BEQ 2
#define BNE 3
#define BCS 4
#define BCC 5
#define BMI 6
#define BPL 7
#define BVS 8
#define BVC 9
#define BHI 10
#define BLS 11
#define BGE 12
#define BLT 13
#define BGT 14
#define BLE 15
#define LDR 16
#define ADR 17
#define BLX 18

#define ADDRESS 0xFF
