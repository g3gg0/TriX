.FILE "fsig_asm_s"
.CODE 16
.thumb_func
.global rom_table
.func rom_table
rom_table:
b	.rom_1
b	.rom_2
b	.rom_3
b	.rom_4
b	.rom_5
b	.rom_6
b	.rom_7
b	.rom_8
b	.rom_9
b	.rom_10
b	.fsig_shit
b	.rom_12
b	.rom_13
b	.rom_14
b	.rom_15
b	.rom_16
b	.rom_17
b	.rom_18

.rom_1:
ldr r3, .rom
add r3, #1
bx r3

.rom_2:
ldr r7, .rom
add r7, #3
bx r7

.rom_3:
ldr r7, .rom
add r7, #5
bx r7

.rom_4:
ldr r7, .rom
add r7, #7
bx r7

.rom_5:
ldr r7, .rom
add r7, #9
bx r7

.rom_6:
ldr r7, .rom
add r7, #11
bx r7

.rom_7:
ldr r7, .rom
add r7, #13
bx r7

.rom_8:
push { r5, r6, lr }
ldr r5, .rom
add r5, #15
adr r6, .rom_8_end
add r6, #1
mov lr, r6
bx r5

.rom_8_end:
nop
pop { r5, r6 }
pop { r3 }
bx r3


.rom_9:
ldr r5, .rom
add r5, #17
bx r5

.rom_10:
ldr r2, .rom
add r2, #19
bx r2

.rom_11:
push { lr }
@bl fsig_clone
@bx lr
@ldr r2, .rom
@add r2, #21
@bx r2
@nop
pop { pc }

.rom_12:
ldr r4, .rom
add r4, #23
bx r4

.rom_13:
ldr r7, .rom
add r7, #25
bx r7

.rom_14:
ldr r7, .rom
add r7, #27
bx r7

.rom_15:
mov r0, #0x80
lsl r0, r0, #1
bx lr
@ldr r7, .rom
@add r7, #29
@bx r7

.rom_16:
ldr r7, .rom
add r7, #31
bx r7

.rom_17:
ldr r7, .rom
add r7, #33
bx r7

.rom_18:
ldr r7, .rom
add r7, #35
bx r7 

nop

.rom:
.word 0x840000
.word 0xDEADBEEF
.byte 0xDE, 0xAD
.fsig_shit:
.byte 0xB5, 0xF0, 0xB0, 0xFF, 0xB0, 0xFF, 0xA4, 0x10, 0xA5, 0x38, 0x35, 0x80, 0x46, 0x6E, 0x68, 0x27
.byte 0x60, 0x37, 0x1D, 0x24, 0x1D, 0x36, 0x42, 0xAC, 0xD1, 0xF9, 0x46, 0x6C, 0x1B, 0x36, 0xB4, 0x0F
.byte 0x46, 0x6A, 0x32, 0x10, 0x1C, 0x10, 0x19, 0x92, 0x23, 0xA5, 0x1C, 0x31, 0x46, 0xC0, 0x46, 0xC0
.byte 0xBC, 0x0F, 0x46, 0x6F, 0x1C, 0x7F, 0xA6, 0x01, 0x1C, 0x76, 0x46, 0xB6, 0x47, 0x38, 0xB0, 0x7F
.byte 0xB0, 0x7F, 0xBD, 0xF0, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00
.fsig_shit2:
.byte 0x47, 0x78, 0x46, 0xC0, 0xE1, 0x0F
.byte 0x20, 0x00, 0xE3, 0x82, 0x20, 0xC0, 0xE1, 0x6F, 0xF0, 0x02, 0xE2, 0x8F, 0x20, 0x01, 0xE1, 0x2F
.byte 0xFF, 0x12, 0x46, 0xC0, 0x46, 0xC0, 0x4A, 0x1F, 0x32, 0x26, 0x88, 0x93, 0x2B, 0x00, 0xD1, 0x36
.byte 0x88, 0x53, 0x88, 0x14, 0x19, 0x1B, 0xD0, 0x32, 0x4D, 0x1B, 0x35, 0xA0, 0x68, 0x02, 0x68, 0x43
.byte 0x60, 0x6A, 0x60, 0x2B, 0x3D, 0x74, 0x68, 0x82, 0x60, 0x2A, 0x68, 0xC4, 0x35, 0x34, 0x68, 0x22
.byte 0x68, 0x63, 0x60, 0x2A, 0x60, 0x6B, 0x3D, 0x60, 0x22, 0x03, 0x80, 0x2A, 0x68, 0x85, 0x68, 0xC4
.byte 0x34, 0x08, 0x1E, 0x6D, 0x00, 0x6D, 0x46, 0xC0, 0x4B, 0x0F, 0x33, 0x60, 0xA7, 0x0F, 0x68, 0x38
.byte 0x21, 0x04, 0x68, 0x26, 0x42, 0x84, 0xD1, 0x02, 0x68, 0x7E, 0x37, 0x08, 0x68, 0x38, 0x60, 0x1E
.byte 0x1D, 0x24, 0x40, 0x4B, 0x1E, 0x6D, 0xD1, 0xF4, 0x46, 0xC0, 0x47, 0x78, 0x46, 0xC0, 0xE1, 0x0F
.byte 0x00, 0x00, 0xE3, 0xC0, 0x00, 0xC0, 0xE1, 0x6F, 0xF0, 0x00, 0xE1, 0x2F, 0xFF, 0x1E, 0x47, 0x70
.byte 0x46, 0xC0, 0x46, 0xC0, 0x00, 0x00, 0x06, 0x00, 0xC8, 0x00
.endfunc
.CODE 16
.thumb_func
.global modification
.func modification
modification:
.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF

.endfunc
.end
