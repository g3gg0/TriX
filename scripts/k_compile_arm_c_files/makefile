
all: dump_objects.o rotate_lcd.o dump_io.o pin_toggle.o fonts.o lcd.o k_own_ati_handler.o k_lcd_irq_count.o


%.o: %.s
	as -mthumb -EB -o $@ $^

%.o: %.c
	gcc -mthumb -Os -fno-common -mbig-endian -c $^
	
