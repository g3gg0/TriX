#define JUMP_BIT		0x01
#define CALL_BIT		0x02
#define DATA_BIT		0x04
#define XREF_BIT		0x08
#define RET_BIT			0x10
#define PUSH_BIT		0x20
#define VAL_BIT			0x40

#define ADDR_MASK	(JUMP_BIT | CALL_BIT)

typedef struct _tagINFO {
	unsigned int mask;
	unsigned int jump_addr;
	unsigned int data_addr;
	unsigned int xref_addr;
	unsigned int value;
} INFO;

