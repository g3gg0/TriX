/****************************************************************/
/*																*/
/*	PRIMUS - Ein Paket zum Rechnen mit langen Zahlen			*/
/*																*/
/*		Version 1.0												*/
/*																*/
/*																*/
/*	arith.h: Grundlegender Header-File fuer PRIMUS				*/
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


#ifndef ARITH_H


#define ARITH_H

#include <stdio.h> 

#define LONG      unsigned long
#define LONGINT   long
#define SHORT     unsigned short
#define PRIM_TYPE unsigned short

#define LONGBITS  32L
#define SHORTBITS 16L

#define LAENGE         80L
#define VLAENGE        (LAENGE + 1)
#define RAT_LAENGE     (LAENGE + VLAENGE)

#define EINGABE_LAENGE (11L * LAENGE)
#define AUSGABE_LAENGE (11L * LAENGE)
#define STRING_ENDE    0
#define RETURN         10

#define MS_DOS      0
#define DREIVIERTEL 0
#define MONTMEXP    0
#define GGTSHIFT    1
#define ANZAHL_PRIM               6542 
#define ANZAHL_PRIM_POT           6634 
#define ANZAHL_DATEI_PRIM_POT     34
#define ANZAHL_TEILERFREMD_ZU_210 48

#define MULT_GGT(p1, p2, p3, p4, p5) \
switch (p5) { \
	case 1: \
		GGT(p1, p2, p3, p4); \
		break; \
	case 2: \
		mggt_2(p1, p2, p3, p4); \
		break; \
	case 4: \
		mggt_4(p1, p2, p3, p4); \
		break; \
	case 8: \
		mggt_8(p1, p2, p3, p4); \
		break; \
}

#define MULT_NUR_GGT(p1, p2, p3, p4) \
switch (p4) { \
	case 1: \
		compute_ggt(p1, p2, p3); \
		break; \
	case 2: \
		mnurggt_2(p1, p2, p3); \
		break; \
	case 4: \
		mnurggt_4(p1, p2, p3); \
		break; \
	case 8: \
		mnurggt_8(p1, p2, p3); \
		break; \
}

#define EQUAL0(ZAHL) \
(!(*(ZAHL)))

#define NOTEQUAL0(ZAHL) \
(*(ZAHL))

#define EQUAL1(ZAHL) \
( (*(ZAHL) == 1L) && (*((ZAHL) + 1) == 1L) )

#define NOTEQUAL1(ZAHL) \
( (*(ZAHL) != 1L) || (*((ZAHL) + 1) != 1L) )



/* Das folgende Makro benutzt die Variablen 					*/
/* SHORT A01, A00, B01, B00;									*/
/* LONG  E1, E0, H01, H10;										*/
/* und berechnet damit 											*/
/* E1 * 2^32 + E0 := (A01 * 2^16 + A00) * (B01 * 2^16 + B00);	*/

#define LONG_MULTIPLIKATION \
E0 = A00 * B00;\
H01 = A00 * B01;\
H10 = A01 * B00;\
E1 = A01 * B01;\
if ( (E0 += (H01 << SHORTBITS)) < (H01 << SHORTBITS) )\
	E1++;\
if ( (E0 += (H10 << SHORTBITS)) < (H10 << SHORTBITS) )\
	E1++;\
E1 += (H01 >> SHORTBITS) + (H10 >> SHORTBITS);\
/* Ende des Makros LONG_MULTIPLIKATION.	*/


/* Das folgende Makro benutzt die Variablen						*/
/* SHORT	A01, A00, B01, B00;									*/
/* LONG		E1, E0, H01;										*/
/* und berechnet damit 											*/
/* E1 * 2^32 + E0 := (A01 * 2^16 + A00) * (B01 * 2^16 + B00);	*/

#define LONG_QUADRAT \
E0 = A00 * A00;\
H01 = A00 * A01;\
E1 = A01 * A01;\
if ( (E0 += (H01 << SHORTBITS)) < (H01 << SHORTBITS) )\
	E1++;\
if ( (E0 += (H01 << SHORTBITS)) < (H01 << SHORTBITS) )\
	E1++;\
E1 += ( (H01 >> SHORTBITS) << 1);\
/* Ende des Makros LONG_QUADRAT.	*/


/* Das folgende Makro benutzt die Variablen				*/
/* SHORT	Z11, Z10, Z01, Z00, N01, N00, E01, E00;		*/
/* LONG		Z1, Z0, N0, E0, R0;							*/
/* und berechnet damit 									*/
/* E0 := (Z1 * 2^LONGBITS + Z0) / N0;					*/
/* R0 := (Z1 * 2^LONGBITS + Z0) % N0;					*/
/* In den folgenden Kommentaren wird anstelle von		*/
/* LONGBITS die Zahl 32 und anstelle von SHORTBITS die	*/
/* Zahl 16 benutzt.										*/

#define LONG_DIVISION \
Z11 = (SHORT)(Z1 >> SHORTBITS);\
Z10 = (SHORT) Z1;\
Z01 = (SHORT)(Z0 >> SHORTBITS);\
Z00 = (SHORT) Z0;\
N01 = (SHORT)(N0 >> SHORTBITS);\
N00 = (SHORT) N0;\
/* Es gilt 							*/\
/* Z1 = Z11 * 2^SHORTBITS + Z10 ,	*/\
/* Z0 = Z01 * 2^SHORTBITS + Z00 ,	*/\
/* N0 = N01 * 2^SHORTBITS + N00 .	*/\
\
/* Wir berechnen zunaechst	 										*/\
/* E01 = ( Z11 * 2^32 + Z10 * 2^16 + Z01 ) / (N01 * 2^16 + N00) .	*/\
\
if (N01 + 1)\
	E01 = (SHORT) (Z1 / (N01 + 1));	/*   Es gilt  N01 + 1 <= 2^16	*/\
else\
	E01 = Z11;\
\
/*   Es gilt auf Grund der Voraussetzungen  E01 < 2^16 .  Ferner ist E01 definiert durch					*/\
/*   E01 * (N01 + 1) <= Z1 < (E01 + 1) * (N01 + 1) .														*/\
/*   Es folgt																								*/\
/*   E01 * (N01 * 2^16 + N00) <= E01 * (N01 + 1) * 2^16 <= Z1 * 2^16 <= Z11 * 2^32 + Z10 * 2^16 + Z01 .		*/\
/*   Andererseits gilt wegen  E01 < 2^16  und  2^31 <= N0 , also  2^15 <= N01								*/\
/*   (E01 + 3) * N01 = (E01 + 1) * N01 + 2 * N01 >= (E01 + 1) * N01 + 2^16 >= (E01 + 1) * (N01 + 1) > Z1 .	*/\
/*   Es folgt 																								*/\
/*   (E01 + 3) * N01 >= Z1 + 1																				*/\
/*   und somit																								*/\
/*   (E01 + 3) * N0 = (E00 + 3) * (N01 * 2^16 + N00) >= (Z1 + 1) * 2^16 > Z11 * 2^32 + Z10 * 2^16 + Z01 .	*/\
/*   Insgesamt ist deshalb der gesuchte Wert zwischen E01 und E01 + 2 .										*/\
\
/* Wir subtrahieren jetzt E01 * (N01 * 2^16 + N00) von Z11 * 2^32 + Z10 * 2^16 + Z01 .						*/\
help = E01 * N00;												/* help <= (2^16 - 1)^2 .					*/\
if (Z01 < (SHORT)(help)) {\
	Z01 -= (SHORT)(help);\
	help = (help >> SHORTBITS) + E01 * N01 + 1;					/* help <= (2^16 - 1)^2 + 2^16 < 2^32 .		*/\
}\
else {\
	Z01 -= (SHORT)(help);\
	help = (help >> SHORTBITS) + E01 * N01;			 			/* help <= (2^16 - 1)^2 + 2^16 - 1 < 2^32 .	*/\
}\
if (Z10 < (SHORT)(help))\
	Z11 -= (SHORT)(help >> SHORTBITS) + 1;\
else\
	Z11 -= (SHORT)(help >> SHORTBITS);\
Z10 -= (SHORT)(help);\
/* Die Subtraktion ist nun beendet. Es gilt Z11 in {0, 1, 2} .	*/\
\
/* Wir muessen nun feststellen, wie oft wir noch									*/\
/* (N01 * 2^16 + N00) von (Z11 * 2^32 + Z10 * 2^16 + Z01) subtrahieren koennen.		*/\
/* Wenn dies nicht mehr moeglich ist, ist E01 bereits richtig berechnet.			*/\
/* Wenn dies ein  mal moeglich ist, muss noch 1 auf E01 addiert werden.				*/\
/* Wenn dies zwei mal moeglich ist, muss noch 2 auf E01 addiert werden.				*/\
\
Z_MID = (Z10 << SHORTBITS) | Z01;				/* Z_MID = Z10 * 2^16 + Z01 .	*/\
switch (Z11) {\
	case 0:\
		if (N0 <= Z_MID) {\
			E01++;\
			Z_MID -= N0;\
			Z10 = (SHORT)(Z_MID >> SHORTBITS);\
			Z01 = (SHORT)Z_MID;\
		}\
		break;\
	case 1:\
		if ( (N0 << 1) <= Z_MID ) {\
			E01 += 2;\
			Z_MID -= (N0 << 1);\
		}\
		else {\
			E01++;\
			Z_MID -= N0;\
		}\
		Z10 = (SHORT)(Z_MID >> SHORTBITS);\
		Z01 = (SHORT)Z_MID;\
		break;\
	case 2:\
		E01 += 2;\
		Z_MID -= (N0 << 1);\
		Z10 = (SHORT)(Z_MID >> SHORTBITS);\
		Z01 = (SHORT)Z_MID;\
}\
/* Nun ist E01 richtig berechnet.	*/\
\
/* Wir berechnen nun				 															*/\
/* E00 = (Z_MID * 2^16 + Z00) / N0 == ( Z10 * 2^32 + Z01 * 2^16 + Z00 ) / (N01 * 2^16 + N00) .	*/\
\
if (N01 + 1)\
	E00 = (SHORT) (Z_MID / (N01 + 1));	/*   Es gilt  N01 + 1 <= 2^16	*/\
else\
	E00 = Z10;\
\
/*   Es gilt auf Grund der Voraussetzungen  E00 < 2^16 .  Ferner ist E00 definiert durch						*/\
/*   E00 * (N01 + 1) <= Z_MID < (E00 + 1) * (N01 + 1) .															*/\
/*   Es folgt																									*/\
/*   E00 * (N01 * 2^16 + N00) <= E00 * (N01 + 1) * 2^16 <= Z_MID * 2^16 <= Z10 * 2^32 + Z01 * 2^16 + Z00 .		*/\
/*   Andererseits gilt wegen  E00 < 2^16  und  2^31 <= N0 , also  2^15 <= N01									*/\
/*   (E00 + 3) * N01 = (E00 + 1) * N01 + 2 * N01 >= (E00 + 1) * N01 + 2^16 >= (E00 + 1) * (N01 + 1) > Z_MID .	*/\
/*   Es folgt 																									*/\
/*   (E00 + 3) * N01 >= Z_MID + 1																				*/\
/*   und somit																									*/\
/*   (E00 + 3) * N0 = (E00 + 3) * (N01 * 2^16 + N00) >= (Z_MID + 1) * 2^16 > Z10 * 2^32 + Z01 * 2^16 + Z0 .		*/\
/*   Insgesamt ist deshalb der gesuchte Wert zwischen E00 und E00 + 2 .											*/\
\
/* Wir subtrahieren jetzt E00 * (N01 * 2^16 + N00) von Z10 * 2^32 + Z01 * 2^16 + Z00 .							*/\
help = E00 * N00;												/* help <= (2^16 - 1)^2 .						*/\
if (Z00 < (SHORT)(help)) {\
	Z00 -= (SHORT)(help);\
	help = (help >> SHORTBITS) + E00 * N01 + 1;					/* help <= (2^16 - 1)^2 + 2^16 < 2^32 .			*/\
}\
else {\
	Z00 -= (SHORT)(help);\
	help = (help >> SHORTBITS) + E00 * N01;		 				/* help <= (2^16 - 1)^2 + 2^16 - 1 < 2^32 .		*/\
}\
if (Z01 < (SHORT)(help))\
	Z10 -= (SHORT)(help >> SHORTBITS) + 1;\
else\
	Z10 -= (SHORT)(help >> SHORTBITS);\
Z01 -= (SHORT)(help);\
/* Die Subtraktion ist nun beendet. Es gilt Z10 in {0, 1, 2} .	*/\
\
/* Wir muessen nun feststellen, wie oft wir noch									*/\
/* (N01 * 2^16 + N00) von (Z10 * 2^32 + Z01 * 2^16 + Z00) subtrahieren koennen.		*/\
/* Wenn dies nicht mehr moeglich ist, ist E00 bereits richtig berechnet.			*/\
/* Wenn dies ein  mal moeglich ist, muss noch 1 auf E00 addiert werden.				*/\
/* Wenn dies zwei mal moeglich ist, muss noch 2 auf E00 addiert werden.				*/\
Z_MID = (Z01 << SHORTBITS) | Z00;				/* Z_MID = Z01 * 2^16 + Z00 .	*/\
switch (Z10) {\
	case 0:\
		if (N0 <= Z_MID) {\
			E00++;\
			Z_MID -= N0;\
			Z01 = (SHORT)(Z_MID >> SHORTBITS);\
			Z00 = (SHORT)Z_MID;\
		}\
		break;\
	case 1:\
		if ( (N0 << 1) <= Z_MID ) {\
			E00 += 2;\
			Z_MID -= (N0 << 1);\
		}\
		else {\
			E00++;\
			Z_MID -= N0;\
		}\
		Z01 = (SHORT)(Z_MID >> SHORTBITS);\
		Z00 = (SHORT)Z_MID;\
		break;\
	case 2:\
		E00 += 2;\
		Z_MID -= (N0 << 1);\
		Z01 = (SHORT)(Z_MID >> SHORTBITS);\
		Z00 = (SHORT)Z_MID;\
}\
/* Nun ist E00 richtig berechnet.	*/\
E0 = (E01 << SHORTBITS) | E00;\
R0 = (Z01 << SHORTBITS) | Z00;\
/* Ende des Makros LONG_DIVISION.	*/

#define TRANS(ZAHL, ERGEBNIS, E_ZAHL) \
E_ZAHL = ZAHL + *ZAHL;\
do\
	*ERGEBNIS++ = *ZAHL++;\
while (ZAHL <= E_ZAHL);

#define ZWEI_ANTEIL(Z_ZA, V) \
while (!(HELP & TEST)) {\
	V++;\
	if (!(TEST <<= 1)) {\
		HELP = *(++Z_ZA);\
		TEST = 1L;\
	}\
}


#endif
