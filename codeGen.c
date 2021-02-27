#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "code.h"
#include "list.h"
#include "lex.h"
#include "parse.h"
// #include "semrtype.h"
// #include "semrfunc.h"
// #include "semrd.h"

tProc *pMainPr;

extern tProc *pCurrPr;
extern tList *pLConst;
extern tList *pSOprtr; /* Operatorenkeller fr Ausdruck   */
extern tList *pSLabl;  /* Keller fuer Label               */
extern int IdxCnst;    /* Zaehler fuer Konstantenindex    */
extern tMorph Morph;
extern int IdxProc; /* Zaehler fuer Prozeduren                   */

/* Weitere Funktionen ... */

/*--------------------------*/

/*--------------------------*/
void wr2ToCode(short x)
{
    *pCode++ = (unsigned char)(x & 0xff);
    *pCode++ = (unsigned char)(x >> 8);
}
void wr2ToCodeAtP(short x, char *pD)
{
    *pD = (unsigned char)(x & 0xff);
    *(pD + 1) = (unsigned char)(x >> 8);
}
int code(tCode Code, ...)
{
    va_list ap;
    short sarg;

    if (pCurrPr->vCode == NULL)
    {
        pCurrPr->vCode = malloc(LenCode);
        pCurrPr->vCode = pCode;
    }

    if (pCode - pCurrPr->vCode + MAX_LEN_OF_CODE >= LenCode)
    {
        char *xCode = realloc(pCurrPr->vCode, (LenCode += 1024));
        if (xCode == NULL)
            // Error(ENoMem);
            exit(-1);
        pCode = xCode + (pCode - pCurrPr->vCode);
        pCurrPr->vCode = xCode;
    }
    *pCode++ = (char)Code;
    va_start(ap, Code);
    switch (Code)
    {
    /* Befehle mit 3 Parametern */
    case entryProc:
        sarg = va_arg(ap, int);
        wr2ToCode(sarg);
    /* Befehle mit 2 Parametern */
    case puValVrGlob:
    case puAdrVrGlob:
        sarg = va_arg(ap, int);
        wr2ToCode(sarg);
    /* Befehle mit 1 Parameter */
    case puValVrMain:
    case puAdrVrMain:
    case puValVrLocl:
    case puAdrVrLocl:
    case puConst:
    case jmp:
    case jnot:
    case call:
        sarg = va_arg(ap, int); /* Prozedurnummer               */
        wr2ToCode(sarg);
        break;

        /* keine Parameter */
    case putStrg:
        if ((int)(pCode - pCurrPr->vCode + strlen(Morph.Val.pStr) + 1) >= LenCode)
        {
            char *xCode = realloc(pCurrPr->vCode, (LenCode += 1024));
            if (xCode == NULL)
                // Error(ENoMem);
                exit(-1);
            pCode = xCode + (pCode - pCurrPr->vCode);
            pCurrPr->vCode = xCode;
        }
        strcpy(pCode, Morph.Val.pStr);
        pCode += strlen(pCode) + 1;
        break;
    default:
        break;
    }
    va_end(ap);
    return 1;
}

int CodeOut(void)
{
    unsigned short Len = (unsigned short)(pCode - pCurrPr->vCode);
    wr2ToCodeAtP((short)Len, pCurrPr->vCode + 1);
    wr2ToCodeAtP((short)pCurrPr->SpzzVar, pCurrPr->vCode + 5);
    if (Len == fwrite(pCurrPr->vCode, sizeof(char), Len, pOFile))
        return 1;
    else
        return 0;
}

/*------------------------------*/
/* Oeffnen/Schliessen des Codeausgabefiles */
int openOFile(char *arg)
{
    long i = 0L;
    char vName[128 + 1];

    strcpy(vName, arg);
    if (strstr(vName, ".pl0") == NULL)
        strcat(vName, ".cl0");
    else
        *(strchr(vName, '.') + 1) = 'c';

    if ((pOFile = fopen(vName, "wb")) != NULL)
    {
        fwrite(&i, sizeof(int32_t), 1, pOFile);
        return 1;
    }
    else
        return 0;
}

int closeOFile(void)
{
    char vBuf2[2];
    fseek(pOFile, 0, SEEK_SET);
    // printf("IdxProc: %i\n", IdxProc);
    wr2ToCodeAtP(IdxProc, vBuf2);
    if (fwrite(vBuf2, 1, 2, pOFile) == 2)
        return 1;
    else
        return 0;
}