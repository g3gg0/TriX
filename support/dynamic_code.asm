

//
//////////////////////////////////////////////////////
.sub hot_code
//////////////////////////////////////////////////////
//
.ens LITTLE_ENDIAN
.org 0x0C0B0410
.hex

.inj 0
.size 0x0400

//
///////////////////////////////////////
push	lr

pop	pc
nop
ldr     r0, @dest_addr

bx	pc
nop

// ARM32 here

db 10 1F 10 EE
// MRC     p15, 0, R1,c0,c0 ; ID
db 30 2F 10 EE
// MRC     p15, 0, R2,c0,c0, 1 ; Cache Type
db 10 3F 11 EE
// MRC     p15, 0, R3,c1,c0 ; Control
db 0E 00 A0 E8
// STMIA   R0!, {R1-R3}
db 10 1F 12 EE
// MRC     p15, 0, R1,c2,c0 ; TTB base
db 10 2F 13 EE
// MRC     p15, 0, R2,c3,c0 ; Domain access
db 10 3F 16 EE
// MRC     p15, 0, R3,c6,c0 ; Fault address
db 0E 00 A0 E8
// STMIA   R0!, {R1-R3}
db 10 1F 15 EE 
// MRC     p15, 0, R1,c5,c0 ; Fault status
db 10 2F 1A EE
// MRC     p15, 0, R2,c10,c0 ; D TLB lock
db 30 3F 1A EE
// MRC     p15, 0, R3,c10,c0, 1 ; I TLB lock
db 0E 00 A0 E8
// STMIA   R0!, {R1-R3}
db 18 1F 1F EE
// MRC     p15, 0, R1,c15,c8 ; ARM925 status
db 11 2F 1F EE
// MRC     p15, 0, R2,c15,c1 ; ARM925 config
db 14 3F 1F EE
// MRC     p15, 0, R3,c15,c4 ; Thread ID
db 0E 00 A0 E8
// STMIA   R0!, {R1-R3}


// ARM32 end

dd 0xE28F0001
dd 0xE12FFF10

pop	pc
///////////////////////////////////////
//
.align 4
@dest_addr:
dd 0x0C0B0500

