
volatile void (*func) ( unsigned int R0, unsigned int R1, unsigned int R2, unsigned int R3 );

unsigned char volatile *addr = 0xFFFF0000;

void example_function (  )
{
	int i = 1000;

	while ( i )
		i--;

	addr[0] = 1;
//	func ( 1,2,3,4 );

	return;
}

int main ( )
{
	func = 0xA00DC044;
	example_function ();
	return 0;
}