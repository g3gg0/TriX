
//
// injected directly into the FIA file
// please change injection offset accordingly
//
//

//
//////////////////////////////////////////////////////
.sub maincode
//////////////////////////////////////////////////////
//
.ens BIG_ENDIAN
.org 0x00001E00


// inject at start of the loader
.inj 0x0048
.size 0x0400

.imp send_byte          0x00000430
.imp read_byte          0x0000059A
.imp tx_0               0x0000052A
.imp tx_1               0x00000520



.imp readmem            0x00002204



@Start:
// success response
bl	@tx_0
bl	@read_byte
bl	@tx_1

@main_loop:

// read command
bl	@read_byte

sub	r0, 0x30
beq	@cmd_30
sub	r0, 0x01
beq	@cmd_31
sub	r0, 0x01
beq	@cmd_32

// set byte, half and word
sub	r0, 0x01
beq	@cmd_33
sub	r0, 0x01
beq	@cmd_34
sub	r0, 0x01
beq	@cmd_35

// read byte, half and word
sub	r0, 0x01
beq	@cmd_36
sub	r0, 0x01
beq	@cmd_37
sub	r0, 0x01
beq	@cmd_38

b	@main_loop


@cmd_30:
bl	@readmem
b	@main_loop

@cmd_31:
bl	@call_func
b	@main_loop

@cmd_32:
bl	@upload_code
b	@main_loop

@cmd_33:
bl	@set_byte
b	@main_loop

@cmd_34:
bl	@set_half
b	@main_loop

@cmd_35:
bl	@set_word
b	@main_loop

@cmd_36:
bl	@get_byte
b	@main_loop

@cmd_37:
bl	@get_half
b	@main_loop

@cmd_38:
bl	@get_word
b	@main_loop

//
// **********************************************
// **********************************************
//


/////////////////////////////////////////////////
// ********* upload code menu handler ******** //
/////////////////////////////////////////////////
@upload_code:
push    lr

// checksum
mov     r4, 0xFF
mov     r3, 0x00
mov     r5, 0x00

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read 2 bytes size
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0
lsl     r5, r5, 8

bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0


// read code
@upload_read_loop:
cmp     r5, 0x00
beq     @upload_finished_read

bl      @read_byte
sub     r4, r4, r0
strb    r0, r3, 0
add     r3, 0x01
sub     r5, 0x01
b       @upload_read_loop


@upload_finished_read:

// read and test chksum
bl      @read_byte
sub     r4, r4, r0

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1


lsl     r4, r4, 0x18
bne     @upload_check_failed

mov     r0, 0x55
bl      @send_byte
b	@upload_end

@upload_check_failed:
mov     r0, 0xEE
bl      @send_byte
b	@upload_end

@upload_end:

pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// *********** set byte menu handler ********* //
/////////////////////////////////////////////////
@set_byte:
push    lr

// checksum
mov     r4, 0xFF
mov     r3, 0x00
mov     r5, 0x00

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read 1 byte data
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0


// read chksum
bl      @read_byte
sub     r4, r4, r0

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// check checksum
lsl     r4, r4, 0x18
bne     @set_byte_check_failed

strb    r5, r3, 0
mov     r0, 0x55
bl      @send_byte
b	@set_byte_end

@set_byte_check_failed:
mov     r0, 0xEE
bl      @send_byte

@set_byte_end:

pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// *********** set half menu handler ********* //
/////////////////////////////////////////////////
@set_half:
push    lr

// checksum
mov     r4, 0xFF
mov     r3, 0x00
mov     r5, 0x00

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read 2 bytes data
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0
lsl     r5, r5, 8
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0


// read chksum
bl      @read_byte
sub     r4, r4, r0

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// check checksum
lsl     r4, r4, 0x18
bne     @set_half_check_failed

strh    r5, r3, 0
mov     r0, 0x55
bl      @send_byte
b	@set_half_end

@set_half_check_failed:
mov     r0, 0xEE
bl      @send_byte


@set_half_end:

pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// *********** set word menu handler ********* //
/////////////////////////////////////////////////
@set_word:
push    lr

// checksum
mov     r4, 0xFF
mov     r3, 0x00
mov     r5, 0x00

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read 4 bytes data
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0
lsl     r5, r5, 8
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0
lsl     r5, r5, 8
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0
lsl     r5, r5, 8
bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0


// read chksum
bl      @read_byte
sub     r4, r4, r0

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// check checksum
lsl     r4, r4, 0x18
bne     @set_word_check_failed

str     r5, r3, 0
mov     r0, 0x55
bl      @send_byte
b	@set_word_end

@set_word_check_failed:
mov     r0, 0xEE
bl      @send_byte


@set_word_end:

pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// ******** read byte menu handler ********* //
/////////////////////////////////////////////////
@get_byte:
push    lr

// checksum
mov     r4, 0xFF

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
mov     r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read and test chksum
bl      @read_byte
sub     r4, r4, r0
lsl     r4, r4, 0x18
bne     @get_byte_ret

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// init chksum
mov     r4, 0x00

// load data
ldrb    r0, r3, 0

// transfer data
add     r4, r4, r0
bl      @send_byte

// send 16 bit chksum
lsr     r0, r4, 8
bl      @send_byte
mov     r0, r4
bl      @send_byte

@get_byte_ret:
pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// ******** read half menu handler ********* //
/////////////////////////////////////////////////
@get_half:
push    lr

// checksum
mov     r4, 0xFF

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
mov     r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read and test chksum
bl      @read_byte
sub     r4, r4, r0
lsl     r4, r4, 0x18
bne     @get_half_ret

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// init chksum
mov     r4, 0x00

// load data
ldrh    r3, r3, 0

// transfer data
lsr     r0, r3, 0x08
lsl     r0, r0, 0x18
lsr     r0, r0, 0x18
add     r4, r4, r0
bl      @send_byte

mov     r0, r3
lsl     r0, r0, 0x18
lsr     r0, r0, 0x18
add     r4, r4, r0
bl      @send_byte

// send 16 bit chksum
lsr     r0, r4, 8
bl      @send_byte
mov     r0, r4
bl      @send_byte

@get_half_ret:
pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// ******** read word menu handler ********* //
/////////////////////////////////////////////////
@get_word:
push    lr

// checksum
mov     r4, 0xFF

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
mov     r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read and test chksum
bl      @read_byte
sub     r4, r4, r0
lsl     r4, r4, 0x18
bne     @get_word_ret

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// init chksum
mov     r4, 0x00

// load data
ldr     r3, r3, 0

// transfer data
lsr     r0, r3, 0x08
lsl     r0, r0, 0x18
lsr     r0, r0, 0x18
add     r4, r4, r0
bl      @send_byte

lsr     r0, r3, 0x10
lsl     r0, r0, 0x18
lsr     r0, r0, 0x18
add     r4, r4, r0
bl      @send_byte

lsr     r0, r3, 0x08
lsl     r0, r0, 0x18
lsr     r0, r0, 0x18
add     r4, r4, r0
bl      @send_byte

mov     r0, r3
lsl     r0, r0, 0x18
lsr     r0, r0, 0x18
add     r4, r4, r0
bl      @send_byte

// send 16 bit chksum
lsr     r0, r4, 8
bl      @send_byte
mov     r0, r4
bl      @send_byte

@get_word_ret:
pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// ******** call function menu handler ******* //
/////////////////////////////////////////////////
@call_func:
push    lr

// checksum
mov     r4, 0xFF
mov     r3, 0x00

bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read chksum
bl      @read_byte
sub     r4, r4, r0

// check checksum
lsl     r4, r4, 0x18
bne     @call_func_check_failed

mov     lr, pc
bx      r3

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// signal that were done and okay
mov     r0, 0x55
bl      @send_byte
b       @call_func_end



@call_func_check_failed:
// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// signal that were done and okay
mov     r0, 0xEE
bl      @send_byte


@call_func_end:

pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////

.end


//
//////////////////////////////////////////////////////
.sub initcode
//////////////////////////////////////////////////////
//
.ens BIG_ENDIAN
.org 0x00002200

// inject into entry area
.inj 0x0448
.size 0x0004


// jump to maincode above
bl @maincode

.end




//
//////////////////////////////////////////////////////
.sub additional_code
//////////////////////////////////////////////////////
//
.ens BIG_ENDIAN
.org 0x00002204

// inject into entry area
.inj 0x044C
.size 0x0100

.imp send_byte          0x00000430
.imp read_byte          0x0000059A
.imp tx_0               0x0000052A
.imp tx_1               0x00000520


/////////////////////////////////////////////////
// ******** read memory menu handler ********* //
/////////////////////////////////////////////////
@readmem:
push    lr

// checksum
mov     r4, 0xFF

// read 4 bytes address
bl      @read_byte
sub     r4, r4, r0
mov     r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0
lsl     r3, r3, 8
bl      @read_byte
sub     r4, r4, r0
add     r3, r3, r0

// read 2 bytes size
bl      @read_byte
sub     r4, r4, r0
mov     r5, r0
lsl     r5, r5, 8

bl      @read_byte
sub     r4, r4, r0
add     r5, r5, r0

// read and test chksum
bl      @read_byte
sub     r4, r4, r0
lsl     r4, r4, 0x18
bne     @read_mem_ret

// success response
bl      @tx_0
bl      @read_byte
bl      @tx_1

// init chksum
mov     r4, 0x00

@mem_read_loop:
ldrb    r0, r3, 0
add     r4, r4, r0
bl      @send_byte

add     r3, 1
sub     r5, 1
bne     @mem_read_loop

// send 16 bit chksum
lsr     r0, r4, 8
bl      @send_byte
mov     r0, r4
bl      @send_byte

@read_mem_ret:
pop     pc
/////////////////////////////////////////////////
// **************  end  ********************** //
/////////////////////////////////////////////////
.end


//////////////////////////////////////////////////////


