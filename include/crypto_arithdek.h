/****************************************************************/
/*																*/
/*	PRIMUS - Ein Paket zum Rechnen mit langen Zahlen			*/
/*																*/
/*		Version 1.0												*/
/*																*/
/*																*/
/*	arithdek.h: Deklarationen der Funktionen aus arith.c		*/
/*																*/
/*																*/
/*	Programmentwicklung und Copyright:							*/
/*																*/
/*	Reiner Staszewski											*/
/*	Institut fuer Experimentelle Mathematik						*/
/*	Universitaet Gesamthochschule Essen							*/
/*	Ellernstr. 29												*/
/*	45326 Essen													*/
/*	E-Mail: reiner@exp-math.uni-essen.de						*/
/*																*/
/*																*/
/*	Dieses Programmpaket ist freie Software.					*/
/*	Fuer Details siehe in der Datei COPYING.					*/
/*																*/
/****************************************************************/


void	add(LONG zahl1[], LONG zahl2[], LONG ergebnis[]);
	/*	ergebnis = zahl1 + zahl2;	*/
void	choose(LONG zahl1, LONG zahl2, LONG ergebnis[]);
	/*	ergebnis = zahl1 ueber zahl2	*/
int		comp(LONG zahl1[], LONG zahl2[]);
	/*	Vergleich							*/
	/*	Returncode -1 falls zahl1 > zahl2	*/
	/*	            0 falls zahl1 = zahl2	*/
	/*	           +1 falls zahl1 < zahl2.	*/
int		compute_ggt(LONG zahl1[], LONG zahl2[], LONG ergebnis[]);
	/*	ergebnis = ggt(zahl1, zahl2)	*/
int		decr(LONG  zahl[]);
	/*	zahl = zahl - 1;	*/
int		divi(LONG divident[], LONG divisor[], LONG quotient[], LONG rest[]);
	/*	quotient = divident / divisor;	*/
	/*	rest = divident % divisor;		*/
int		even(LONG zahl[]);
	/*	zahl % 2;	*/
void	expo(LONG zahl[], LONG exponent, LONG ergebnis[]);
	/*	ergebnis = zahl hoch exponent	*/
void	fak(LONG zahl, LONG ergebnis[]);
	/*	ergebnis = zahl!	*/
void	fwrite_nur_zahl(FILE *f, LONG zahl[]);
int		GGT(LONG zahl[], LONG modul[], LONG ergebnis[], LONG invers[]);
void	incr(LONG zahl[]);
int		jacobi(LONG arg[], LONG modul[]);
long	lb(LONG zahl[]);
void	madd(LONG zahl1[], LONG zahl2[], LONG ergebnis[], LONG modul[]);
int		mdiv(LONG zahl1[], LONG zahl2[], LONG ergebnis[], LONG modul[]);
void	mexp(LONG zahl1[], LONG zahl2[], LONG ergebnis[], LONG modul[]);
void	mexp_zwei_8bit(LONG zahl2[], LONG ergebnis[], LONG modul[]);
void	mexp_zwei(LONG zahl2[], LONG ergebnis[], LONG modul[]);
void	mggt_2(LONG *zuinv, LONG modul[], LONG ggt[], LONG *inv);
void	mggt_4(LONG *zuinv, LONG modul[], LONG ggt[], LONG *inv);
void	mggt_8(LONG *zuinv, LONG modul[], LONG ggt[], LONG *inv);
int		mlucas(LONG P[], LONG r[], LONG ergebnis[], LONG modul[]);
void	mmult(LONG zahl1[], LONG zahl2[], LONG ergebnis[], LONG modul[]);
void	mont_init(LONG modul[]);
void	mont_mexp(LONG zahl1[], LONG zahl2[], LONG ergebnis[]);
void	mont_redc(LONG zahl[], LONG ergebnis[]);
void	mnurggt_2(LONG *zuinv, LONG modul[], LONG ggt[]);
void	mnurggt_4(LONG *zuinv, LONG modul[], LONG ggt[]);
void	mnurggt_8(LONG *zuinv, LONG modul[], LONG ggt[]);
LONG	mod4(LONG zahl[]);
LONG	mod8(LONG zahl[]);
void	mqua(LONG zahl[], LONG ergebnis[], LONG modul[]);
void	msub(LONG zahl1[], LONG zahl2[], LONG ergebnis[], LONG modul[]);
void	mult(LONG zahl1[], LONG zahl2[], LONG ergebnis[]);
void	mult_mod_basispotenz(LONG zahl1[], LONG zahl2[], LONG ergebnis[]);
int		n_wurzel(LONG zahl[], LONG n, LONG ergebnis[]);
void	overflow(const char text[]);
void	qua(LONG zahl[], LONG ergebnis[]);
int		quersumme(LONG zahl[], LONG ergebnis[]);
void	read_long(FILE *eingabefile, LONG zahl[]);
void	readparameter(const char text[], LONG zahl[]);
void	readue(const char text[], LONG zahl[]);
void	readuefile(FILE *eingabefile, LONG zahl[]);
int		sub(LONG zahl1[], LONG zahl2[], LONG ergebnis[]);
void	shift(LONG zahl[], long anz_shifts, LONG ergebnis[]);
void	trans(LONG zahl[], LONG ergebnis[]);
void	trans_mod_basispotenz(LONG zahl[], LONG ergebnis[]);
void	write_long(FILE *ausgabefile, LONG zahl[]);
void	write_nur_zahl(LONG zahl[]);
void	writeue(const char text[], LONG zahl[]);
void	writeuefile(FILE *ausgabefile, const char text[], LONG zahl[]);
void	writex(const char text[], LONG zahl[]);
int		wurzel(LONG zahl[], LONG ergebnis[]);
void	zufall(LONG zahl[], LONG anz_bits);
void	zuweis(LONG zahl, LONG ergebnis[]);
int		zuweis_int(LONG zahl[], LONG *ergebnis);
