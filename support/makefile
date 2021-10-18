
all: dynamic_code.le.o dynamic_code.be.o timer_trace.le.o timer_trace.be.o

%.le.o: %.s
	as -mthumb -EB -o $@ $^

%.be.o: %.s
	as -mthumb -EL -o $@ $^

%.le.o: %.c
	gcc.exe -mthumb -Os -fno-common -mlittle-endian -c $^ -o $@

%.be.o: %.c
	gcc.exe -mthumb -Os -fno-common -mbig-endian -c $^ -o $@

%.o: %.c
	gcc.exe -mthumb -Os -fno-common -mbig-endian -c $^ -o $@

