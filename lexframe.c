//
// 17/041/01
// Lucas Opitz
//
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

static FILE *pIF;                 /* Quellfile 				*/
static tMorph Morph, MorphInit;   /* Morphem   				*/
static signed char X;             /* Aktuelles Eingabezeichen 		*/
static int Z;                     /* Aktueller Zustand des Automaten 	*/
static char vBuf[128 + 1], *pBuf; /* Ausgabepuffer */
static int line = 0, col = 0;     // Zeile und Spalte
static int Ende;                  /* Flag 				*/

/*---- Initialisierung der lexiaklischen Analyse ----*/
int initLex(char *fname)
{
    pIF = fopen(fname, "r+t");
    if (pIF)
        X = fgetc(pIF);
    Morph.MC = mcEmpty;
    return (int)pIF;
}

/* Zeichenklassenvektor */
static char vZKl[128] =
    /*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
    /*          --------------------...--------------*/
    /* 0*/ {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* 0*/
    /*10*/  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /*10*/
    /*20*/  7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*20*/
    /*30*/  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 5, 4, 6, 0, /*30*/
    /*40*/  0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /*40*/
    /*50*/  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, /*50*/
    /*60*/  0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /*60*/
    /*70*/  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0} /*70*/;

// Keyword Liste für Binäre Suche
static char *Keyw[] = /* sortierter Vektor der Keywords*/
    {
        "BEGIN", "CALL", "CONST", "DO", "END", "IF", "ODD",
        "PROCEDURE", "THEN", "VAR", "WHILE"};
static int KeywKey[] = {zBGN, zCLL, zCST, zDO, zEND, zIF, zODD, zPRC, zTHN, zVAR, zWHL};
int binary_search(const char **M, // stock
                  int n,          // number
                  const char *X)  // needle
{
    unsigned mitte;
    unsigned links = 0;      /* man beginne beim kleinsten Index */
    unsigned rechts = n - 1; /* Arrays sind 0-basiert */
    int ret = -1;
    int bool;
    do
    {
        if (n <= 0)
            break;
        mitte = links + ((rechts - links) / 2); // Bereich halbieren
        if (rechts < links)
            break; // alles wurde durchsucht, X nicht gefunden
        bool = strcmp(M[mitte], X);
        if (bool == 0)
            ret = mitte;
        else // Element X gefunden?
            if (bool > 0)
            rechts = mitte;
        else                   // im linken Abschnitt weitersuchen
            links = mitte + 1; // im rechten Abschnitt weitersuchen
        n = (n) / 2;
    } while (bool != 0);
    return ret;
}

/* Ausgabefunktionen des Automaten */
/*---- lesen ----*/
static void
fl(void)
{
    X = fgetc(pIF);
    if (X == '\n')
    {
        line++;
        col = 0;
    }
    else
    {
        col++;
    }
};
int n, i;
/*---- beenden ----*/
static void fb(void)
{
    int i, j;
    switch (Z)
    {
    /* Symbol */
    case 3: // :
    case 4: // <
    case 5: // >
    case 0:
        Morph.Val.Symb = vBuf[0];
        Morph.MC = mcSymb;
        break;

    /* Zahl */
    case 1:
        Morph.Val.Num = atol(vBuf);
        Morph.MC = mcNum;
        break;

    /* Bezeichner/Wortsymbol */
    case 2:
        // Key word erkennung binäre Suche
        n = sizeof Keyw / sizeof(char *);
        i = binary_search(Keyw, n, vBuf);
        if (i >= 0)
        {
            Morph.Val.Symb = (long)KeywKey[i];
            Morph.MC = mcSymb;
        }
        else
        {
            Morph.Val.pStr = vBuf;
            Morph.MC = mcIdent;
        }
        break;

    /* Ergibtzeichen*/
    case 6:
        Morph.Val.Symb = (long)zERG;
        Morph.MC = mcSymb;
        break;

    /* Kleiner Gleich */
    case 7:
        Morph.Val.Symb = (long)zLE;
        Morph.MC = mcSymb;
        break;

    /* Groesser Gleich */
    case 8:
        Morph.Val.Symb = (long)zGE;
        Morph.MC = mcSymb;
        break;
        // weitere Casezweige
    }
    Ende = 1; // entfällt bei Variante mit Zustand zEnd
};

/*---- schreiben als Grossbuchstabe, lesen  ----*/
static void fgl(void)
{
    *pBuf = (char)toupper(X);
    *(++pBuf) = 0;
    fl();
};

/*---- schreiben, lesen  ----*/
static void fsl(void)
{
    *pBuf = (char)X;
    *(++pBuf) = 0;
    fl();
};

/*---- schreiben, lesen, beenden  ----*/
static void fslb(void)
{
    fsl();
    fb();
};

/* Automatentabelle */
static tAutomat vSMatrix[][8] =
    /*           So         Zi      Buchstabe    ':'      '='      '<'       '>'       Space
    /*---------------------...-----------------------*/
    /* 0 */ {{{0, fslb}, {1, fsl}, {2, fgl}, {3, fsl}, {0, fslb}, {4, fsl}, {5, fsl}, {0, fl}},
    /* 1 */ { {0, fb},   {1, fsl}, {0, fb},  {0, fb},  {0, fb},   {0, fb},  {0, fb},  {0, fb}},
    /* 2 */ { {0, fb},   {2, fsl}, {2, fgl}, {0, fb},  {0, fb},   {0, fb},  {0, fb},  {0, fb}},
    /* 3 */ { {0, fb},   {0, fb},  {0, fb},  {0, fb},  {6, fsl},  {0, fb},  {0, fb},  {0, fb}},
    /* 4 */ { {0, fb},   {0, fb},  {0, fb},  {0, fb},  {7, fsl},  {0, fb},  {0, fb},  {0, fb}},
    /* 5 */ { {0, fb},   {0, fb},  {0, fb},  {0, fb},  {8, fsl},  {0, fb},  {0, fb},  {0, fb}},
    /* 6 */ { {0, fb},   {0, fb},  {0, fb},  {0, fb},  {0, fb},   {0, fb},  {0, fb},  {0, fb}},
    /* 7 */ { {0, fb},   {0, fb},  {0, fb},  {0, fb},  {0, fb},   {0, fb},  {0, fb},  {0, fb}},
    /* 8 */ { {0, fb},   {0, fb},  {0, fb},  {0, fb},  {0, fb},   {0, fb},  {0, fb},  {0, fb}}};

typedef void (*FX)(void);

static FX vfx[] = {fl, fb, fgl, fsl, fslb};

/*---- Lexikalische Analyse ----*/
tMorph Lex(void)
{
    int Zn;
    Z = 0;
    Morph = MorphInit;
    Morph.PosLine = line;
    Morph.PosCol = col;
    pBuf = vBuf;
    Ende = 0;
    tAutomat next;
    do
    {
        /* Berechnung des Folgezustands */
        next = vSMatrix[Z][vZKl[X]];
        /* Ausfuehrung der Aktion (Ausgabefunktion */
        next.fkt();
        /* Automat schaltet */
        Z = next.Zn;
    } while (Ende == 0);
    return Morph;
}