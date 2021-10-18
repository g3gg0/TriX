
all: dynamic_code.o


%.o: %.s
	as -mthumb -EL -o $@ $^

%.o: %.c
	gcc.exe -mthumb -Os -fno-common -mlittle-endian -c $^
