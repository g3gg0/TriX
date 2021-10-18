#include <stdio.h>
#include <stdlib.h>
#include "crypto_arith.h"

/* jprim testet auf der Basis den Miller-Rabin-Primzahltests eine Zahl vom Typ
   LONG darauf eine Primzahl zu sein */

int jprim (LONG n[LAENGE], unsigned long durchlaeufe);

/* jmexpo potenziert zwei grosse zahlen auf der basis des 
   Algorithmus zur schnellen modularen Exponentiation */

int jmexpo (LONG basis[LAENGE], LONG exponent [LAENGE], LONG modul[LAENGE], LONG ergb[LAENGE]);

/* jinvers berechnet das Inverse der "zahl" mod "modul". Das Ergebnis wird an inverses zurueckgeliefert. 
   Desweiteren berechnet jinvers die Darstellung des ggt's als Linearkombination von zahl und modul. 
   ggt = zahl * n - modul * m
   Die entsprechenden Koeffizieten n,m werden in n m uebergeben */ 

int jinvers( const LONG zahl[LAENGE],  const LONG modul[LAENGE], 
	     LONG inverses[LAENGE], LONG n[LAENGE], 
	     LONG m[LAENGE] );

/* jgcd berechnet den ggt der beiden Zahlen zahl1 und zahl2. 
   Die Loesung wird in der Variabel ergb uebergeben */

int jgcd ( const LONG zahl1[LAENGE], const LONG zahl2[LAENGE], 
	   LONG ergb[LAENGE]);




