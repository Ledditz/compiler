//
// 17/041/01
// Lucas Opitz
//
/****************************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "lex.h"
#include "parse.h"
#include "list.h"
#include "code.h"

tMorph Morph = {0};

// Bogendefinitionen
typedef unsigned long ul;

tBog gTerm[] = {
    /*0*/ {BgGr, {(ul)gFact}, NULL, 1, 0},
    /*1*/ {BgNl, {(ul)0}, NULL, 2, 0},
    /*2*/ {BgSy, {(ul)'*'}, NULL, 3, 4},
    /*3*/ {BgGr, {(ul)gFact}, Term3, 1, 0},
    /*4*/ {BgSy, {(ul)'/'}, NULL, 5, 6},
    /*5*/ {BgGr, {(ul)gFact}, Term5, 1, 0},
    /*6*/ {BgEn, {(ul)0}, NULL, 0, 0}};

tBog gExpr[] = {
    /*0*/ {BgSy, {(ul)'-'}, NULL, 1, 2}, // hier was geändert
    /*1*/ {BgGr, {(ul)gTerm}, Expression1, 5, 0},
    /*2*/ {BgSy, {(ul)'+'}, NULL, 4, 3},
    /*3*/ {BgNl, {(ul)0}, NULL, 4, 0},
    /*4*/ {BgGr, {(ul)gTerm}, NULL, 5, 0},
    /*5*/ {BgSy, {(ul)'-'}, NULL, 6, 7},
    /*6*/ {BgGr, {(ul)gTerm}, Expression6, 5, 0},
    /*7*/ {BgSy, {(ul)'+'}, NULL, 8, 9},
    /*8*/ {BgGr, {(ul)gTerm}, Expression8, 5, 0},
    /*9*/ {BgEn, {(ul)0}, NULL, 0, 0}};

tBog gFact[] =
    {/* 0*/ {BgMo, {(ul)mcIdent}, Factor0, 5, 1},
     /* 1*/ {BgMo, {(ul)mcNum}, Factor1, 5, 2},
     /* 2*/ {BgSy, {(ul)'('}, NULL, 3, 0},
     /* 3*/ {BgGr, {(ul)gExpr}, NULL, 4, 0},
     /* 4*/ {BgSy, {(ul)')'}, NULL, 5, 0},
     /* 5*/ {BgEn, {(ul)0}, NULL, 0, 0}};

tBog gCond[] = {
    /*0*/ {BgSy, {(ul)zODD}, NULL, 1, 2},
    /*1*/ {BgGr, {(ul)gExpr}, Condition1, 10, 0},

    /*2*/ {BgGr, {(ul)gExpr}, NULL, 3, 0},
    /*3*/ {BgSy, {(ul)'='}, ConditionSaveOperator, 4, 5},
    /*4*/ {BgGr, {(ul)gExpr}, Condition4, 10, 0},
    /*5*/ {BgSy, {(ul)'#'}, ConditionSaveOperator, 4, 6},
    /*6*/ {BgSy, {(ul)'<'}, ConditionSaveOperator, 4, 7},
    /*7*/ {BgSy, {(ul)'>'}, ConditionSaveOperator, 4, 8},
    /*8*/ {BgSy, {(ul)zLE}, ConditionSaveOperator, 4, 9},
    /*9*/ {BgSy, {(ul)zGE}, ConditionSaveOperator, 4, 0},

    /*10*/ {BgEn, {(ul)0}, NULL, 0, 0}};

tBog gStat[] = {
    /* 0*/ {BgMo, {(ul)mcIdent}, Statement0, 1, 3},
    /* 1*/ {BgSy, {(ul)zERG}, NULL, 2, 0},
    /* 2*/ {BgGr, {(ul)gExpr}, Statement2, 22, 0},

    /* 3*/ {BgSy, {(ul)zIF}, NULL, 4, 7},
    /* 4*/ {BgGr, {(ul)gCond}, Statement4, 5, 0},
    /* 5*/ {BgSy, {(ul)zTHN}, NULL, 6, 0},
    /* 6*/ {BgGr, {(ul)gStat}, Statement6, 22, 0},

    /* 7*/ {BgSy, {(ul)zWHL}, Statement7, 8, 11},
    /* 8*/ {BgGr, {(ul)gCond}, Statement8, 9, 0},
    /* 9*/ {BgSy, {(ul)zDO}, NULL, 10, 0},
    /*10*/ {BgGr, {(ul)gStat}, Statement10, 22, 0},

    /*11*/ {BgSy, {(ul)zBGN}, NULL, 12, 15},
    /*12*/ {BgGr, {(ul)gStat}, NULL, 13, 0},
    /*13*/ {BgSy, {(ul)';'}, NULL, 12, 14}, // HIER
    /*14*/ {BgSy, {(ul)zEND}, NULL, 22, 0},

    /*15*/ {BgSy, {(ul)zCLL}, NULL, 16, 17},
    /*16*/ {BgMo, {(ul)mcIdent}, Statement16, 22, 0},

    /*17*/ {BgSy, {(ul)'?'}, NULL, 18, 19},
    /*18*/ {BgMo, {(ul)mcIdent}, Statement18, 22, 0},

    /*19*/ {BgSy, {(ul)'!'}, NULL, 20, 21},
    /*20*/ {BgGr, {(ul)gExpr}, Statement20, 22, 0},
    // /*20*/ {BgGr, {(ul)gExpr}, NULL, 22, 0},

    /*21*/ {BgNl, {(ul)0}, NULL, 22, 0},
    /*22*/ {BgEn, {(ul)0}, NULL, 0, 0}};

tBog gBlock[] = {
    /*00*/ {BgSy, {(ul)zCST}, NULL, 1, 6},
    /*01*/ {BgMo, {(ul)mcIdent}, Block1, 2, 0},
    /*02*/ {BgSy, {(ul)'='}, NULL, 3, 0},
    /*03*/ {BgMo, {(ul)mcNum}, Block3, 4, 0},
    /*04*/ {BgSy, {(ul)','}, NULL, 1, 5},
    /*05*/ {BgSy, {(ul)';'}, NULL, 6, 0},

    /*06*/ {BgSy, {(ul)zVAR}, NULL, 7, 10},
    /*07*/ {BgMo, {(ul)mcIdent}, Block7, 8, 0},
    /*08*/ {BgSy, {(ul)','}, NULL, 7, 9},
    /*09*/ {BgSy, {(ul)';'}, NULL, 10, 0},

    /*10*/ {BgSy, {(ul)zPRC}, NULL, 11, 16},
    /*11*/ {BgMo, {(ul)mcIdent}, Block11, 12, 0},
    /*12*/ {BgSy, {(ul)';'}, NULL, 13, 0},
    /*13*/ {BgGr, {(ul)gBlock}, NULL, 14, 0},
    /*14*/ {BgSy, {(ul)';'}, NULL, 10, 0},

    /*15*/ {BgGr, {(ul)gStat}, Block15, 17, 0},
    // /*15*/ {BgGr, {(ul)gStat}, NULL, 17, 0},
    /*16*/ {BgNl, {(ul)0}, Block16, 15, 0},
    // /*16*/ {BgNl, {(ul)0}, NULL, 15, 0},
    /*17*/ {BgEn, {(ul)0}, NULL, 0, 0}};

tBog gProg[] = {
    /*0*/ {BgGr, {(ul)gBlock}, NULL, 1, 0},
    /*1*/ {BgSy, {(ul)'.'}, Programm1, 2, 0},
    // /*1*/ {BgSy, {(ul)'.'}, NULL, 2, 0},
    /*2*/ {BgEn, {(ul)0}, NULL, 0, 0}};

int pars(tBog *pGraph);
void print_token(void);
void initNamelist();

int testSearch()
{
    createConst(1);
    createConst(2);
    createConst(3);
    tConst *blah = searchConst(3);
    if (blah != NULL)
        printf("gefnuden: %i\n", blah->Val);
    else
        printf("nope");
}

int main(int argc, void *argv[])
{
    printf("MAIN\n");
    initLex(argv[1]);
    initNamelist();

    // testSearch();
    // return 0;

    if (pars(NULL) != 1)
    {
        printf("parsing error: \n");
        if (Morph.MC != mcEmpty)
        {
            print_token();
        }
        else
        {
            printf("syntax error ");
        }
        printf("%i\n", Morph.PosLine);
        printf(" near Line %i Col %i\n", Morph.PosLine, Morph.PosCol);
        return -1;
    }
    else
    {
        printf("succsessfully parsed");
        closeOFile();
    }

    return 0;
}

void print_token(void)
{
    // printf("unexpected token: ");
    switch (Morph.MC)
    {
    case mcSymb:
        if (Morph.Val.Symb == zERG)
            printf("Symbol ':='\n");
        else if (Morph.Val.Symb == zLE)
            printf("Symbol '<='\n");
        else if (Morph.Val.Symb == zGE)
            printf("Symbol '>='\n");
        else if (Morph.Val.Symb == zBGN)
            printf("Symbol '_BEGIN'\n");
        else if (Morph.Val.Symb == zCLL)
            printf("Symbol '_CALL'\n");
        else if (Morph.Val.Symb == zCST)
            printf("Symbol '_CONST'\n");
        else if (Morph.Val.Symb == zDO)
            printf("Symbol '_DO'\n");
        else if (Morph.Val.Symb == zEND)
            printf("Symbol '_END'\n");
        else if (Morph.Val.Symb == zIF)
            printf("Symbol '_IF'\n");
        else if (Morph.Val.Symb == zODD)
            printf("Symbol '_ODD'\n");
        else if (Morph.Val.Symb == zPRC)
            printf("Symbol '_PROCEDURE'\n");
        else if (Morph.Val.Symb == zTHN)
            printf("Symbol '_THEN'\n");
        else if (Morph.Val.Symb == zVAR)
            printf("Symbol '_VAR'\n");
        else if (Morph.Val.Symb == zWHL)
            printf("Symbol '_WHILE'\n");

        if (isprint(Morph.Val.Symb))
            printf("Symbol '%c'\n", (char)Morph.Val.Symb);
        break;
    case mcNum:
        printf("Zahl   '%ld'\n", Morph.Val.Num);
        break;
    case mcIdent:
        printf("Ident  '%s'\n", (char *)Morph.Val.pStr);
        break;
    }
}

void initNamelist()
{
    IdxProc = 0;
    IdxCnst = 0;
    pCurrPr = createProc(NULL);
    pLConst = CreateList();
    pListLabel = CreateList();

    if (openOFile("test.pl0"))
    {
        printf("alles cool mit dem File\n");
    }
    else
        printf("Fehler mit dem File\n");
}

int i = 200;
int pars(tBog *pGraph)
{
    // printf("Start PARSE\n");
    if (pGraph == NULL)
    {
        pGraph = gProg;
    }
    tBog *pBog = pGraph;
    int succ = 0;
    if (Morph.MC == mcEmpty)
        Morph = Lex();
    while (1)
    {
        // print_token();
        switch (pBog->BgD)
        {
        case BgNl:
            succ = 1;
            break;
        case BgSy:
            succ = (Morph.Val.Symb == pBog->BgX.S);
            break;
        case BgMo:
            succ = (Morph.MC == (tMC)pBog->BgX.M);
            break;
        case BgGr:
            // printf("eins Tiefer\n");
            succ = pars(pBog->BgX.G);

            // printf("====%i===\n", succ);
            break;
        case BgEn:
            return 1; /* Ende erreichet - Erfolg */
        }

        if (succ && pBog->fx != NULL)
        {
            succ = pBog->fx();
        }
        if (!succ) /* Alternativbogen probieren */
            if (pBog->iAlt != 0)
            {
                pBog = pGraph + pBog->iAlt;
            }
            else
            {
                return -1;
            }
        else /* Morphem formal akzeptiert (eaten) */
        {
            // printf("essen\n");
            if (pBog->BgD & BgSy || pBog->BgD & BgMo)
            {
                Morph = Lex();
            }
            // printf("nächster: %i\n", pBog->iNext);
            pBog = pGraph + pBog->iNext;
        }
    } /* while */
}

// TODO: Rename
void dumpConst(t_cnt *pL)
{
    printf("DUMP CONST\n");
    if (pL == NULL)
    {
        return;
    }

    dumpConst(pL->pnext);

    wr2ToCode(((tConst *)(pL->pdata))->Val & 0x0000ffff);
    wr2ToCode((((tConst *)(pL->pdata))->Val & 0xffff0000) >> 16);
    return;
}

int Programm1()
{
    printf("PROGRAMM1\n");
    char *constCode = pCode;
    dumpConst(pLConst->pfirst);
    unsigned short Len = (unsigned short)(pCode - constCode);
    if (Len != fwrite(constCode, sizeof(char), Len, pOFile))
    {
        return 0;
    }
    return 1;
}
int Term3()
{
    code(OpMult);
    return 1;
};
int Term5()
{
    code(OpDiv);
    return 1;
};
int Expression1()
{
    code(vzMinus);
    return 1;
}
int Expression6()
{
    code(OpSub);
    return 1;
}
int Expression8()
{
    code(OpAdd);
    return 1;
}

int Statement0()
{
    // Bez global suchen
    printf("STATEMENT 0\n");
    tBez *pBez;
    printf("    %s\n", Morph.Val.pStr);
    pBez = searchBezGlobal(Morph.Val.pStr);
    if (pBez == NULL)
    {
        // Bez nicht gefunden -> Fehler behandeln
        printf("    Bez nicht gefunden %s\n", Morph.Val.pStr);
        exit(0);
    }
    tVar *pVar = (tVar *)pBez->pObj;
    if (pVar->Kz != KzVar)
    {
        // Bez bezeichnet keine Variable -> Fehler behandeln
        printf("    Bez bezeichnet keine Var %s\n", Morph.Val.pStr);
        exit(0);
    }
    // Bez gefunden und ist Variable
    // TODO: fix error message

    if (pBez->IdxProc == 0)
    {
        // found in Main
        code(puAdrVrMain, pVar->Dspl);
    }
    else if (pBez->IdxProc == pCurrPr->IdxProc)
    {
        // in aktueller Procedur gefunden
        code(puAdrVrLocl, pVar->Dspl);
    }
    else
    {
        code(puAdrVrMain, pVar->Dspl);
    }

    return 1;
}

int Statement2()
{
    printf("STATEMENT 2\n");
    code(storeVal);
    return 1;
}

int Statement4()
{
    printf("STATEMENT 4: %X\n", pCode);
    tLabl *pLabl = creatLabel(KzJnt, pCode);
    pushL(pListLabel, pLabl);
    code(jnot, 0);
    return 1;
}

int Statement6()
{
    printf("STATEMENT 6\n");
    tLabl *pLabl = (tLabl *)pListLabel->pfirst->pdata;
    // printf("    pCode: %X\n", pCode);
    // printf("    ijmp : %X\n", pLabl->iJmp);
    // printf("    calc : %X\n", pCode - pLabl->iJmp);
    short relAdr = pCode - pLabl->iJmp;
    if (pLabl->Kz == KzJnt)
    {
        relAdr -= 3;
    }
    // printf("    calc : %X\n", relAdr);
    wr2ToCodeAtP(relAdr, pLabl->iJmp + 1);
    popL(pListLabel);
    return 1;
}

int Statement7()
{
    printf("STATEMENT 7\n");
    tLabl *pLabl = creatLabel(KzWCnd, pCode);
    pushL(pListLabel, pLabl);
    return 1;
}

int Statement8()
{
    printf("STATEMENT 8\n");
    tLabl *pLabl = creatLabel(KzJnt, pCode);
    pushL(pListLabel, pLabl);
    code(jnot, 0);
    return 1;
}

int Statement10()
{
    printf("STATEMENT 10\n");
    tLabl *pLabl = (tLabl *)pListLabel->pfirst->pdata;
    short relAdr = pCode - pLabl->iJmp;
    printf("    jnt : %X\n", relAdr);
    wr2ToCodeAtP(relAdr, pLabl->iJmp + 1);
    popL(pListLabel);
    pLabl = (tLabl *)pListLabel->pfirst->pdata;
    printf("    jmp : %X\n", relAdr);
    code(jmp, pLabl->iJmp - pCode - 3);
    return 1;
}

int Statement16()
{
    printf("STATEMENT 16\n");
    tBez *pBez;
    pBez = searchBezGlobal(Morph.Val.pStr);
    if (pBez == NULL)
    {
        // Bez nicht gefunden -> Fehler behandeln
        printf("Bez nicht gefunden %s\n", Morph.Val.pStr);
        exit(0);
    }
    tProc *pProc = (tProc *)pBez->pObj;
    if (pProc == NULL)
    {
        printf("pProc ist NULL\n");
        exit(-1);
    }
    if (pProc->Kz != KzPro)
    {
        printf("Fehler: kein Prozedur Bez\n");
        exit(-1);
    }
    code(call, pProc->IdxProc);
    return 1;
}

int Statement18()
{
    printf("STATEMENT 18\n");
    Statement0();
    code(getVal);
    return 1;
}
int Statement20()
{
    printf("STATEMENT 20\n");
    code(putVal);
    return 1;
}

tMorph condOperator;

int ConditionSaveOperator()
{
    printf("Save operator\n");
    condOperator = Morph;
    return 1;
}

int Condition4()
{
    printf("CONDITION 4\n");
    switch (condOperator.MC)
    {
    case mcSymb:
        printf("Operator gefunden\n");
        switch (condOperator.Val.Symb)
        {
        case zGE:
            code(cmpGE);
            return 1;
        case zLE:
            code(cmpLE);
            return 1;
        case '=':
            code(cmpEQ);
            return 1;
        case '<':
            code(cmpLT);
            return 1;
        case '>':
            code(cmpGT);
            return 1;
        case '#':
            code(cmpNE);
            return 1;
        default:
            printf("unerwarteter Operator\n");
            return 0;
        }
    default:
        printf("FEHLER Vergleichsoperator erwartet\n");
        return 0;
    }
}

int Condition1()
{
    printf("CONDITION 1\n");
    code(odd);
    return 1;
}

int Factor1()
{
    printf("FACTOR 1\n");
    tConst *pConst;
    pConst = searchConst(Morph.Val.Num);
    if (pConst == NULL)
    {
        // Neu anlegen
        pConst = createConst(Morph.Val.Num);
        if (pConst == NULL)
            return 0;
    }
    code(puConst, pConst->Idx);
    return 1;
}

int Factor0()
{
    printf("FACTOR 0\n");
    // Bez global suchen
    tBez *pBez;
    pBez = searchBezGlobal(Morph.Val.pStr);
    if (pBez == NULL)
    {
        // Bez nicht gefunden -> Fehler behandeln
        printf("Bez nicht gefunden %c\n", Morph.Val.pStr);
        exit(-1);
    }
    if (((tProc *)pBez->pObj)->Kz == KzPro)
    {
        printf("Found Proc. invalid type of %d\n", pBez->Kz);
        exit(-1);
    }
    if (((tConst *)pBez->pObj)->Kz == KzCnt)
    {
        tConst *pConst = (tConst *)pBez->pObj;
        printf("Const gefunden\n");
        code(puConst, pConst->Idx);
    }
    else if (((tVar *)pBez->pObj)->Kz == KzVar)
    {
        tVar *pVar = (tVar *)pBez->pObj;
        printf("Var gefunden\n");
        if (pBez->IdxProc == 0)
        {
            code(puValVrMain, pVar->Dspl);
        }
        else if (pBez->IdxProc == pCurrPr->IdxProc)
        {
            code(puValVrLocl, pVar->Dspl);
        }
        else
        {
            code(puValVrGlob, pVar->Dspl, pCurrPr->IdxProc);
        }
    }
    else
    {
        printf("invalid type of %d %d %d %d\n", pBez->Kz, KzPro, KzCnt, KzVar);
        printf("invalid type of %d\n", ((tProc *)pBez->pObj)->Kz);
        printf("invalid type of %d\n", ((tConst *)pBez->pObj)->Kz);
        printf("invalid type of %d\n", ((tVar *)pBez->pObj)->Kz);
        exit(-1);
    }

    return 1;
}

int Block1()
{
    printf("BLOCK 1\n");
    tBez *pBez;
    pBez = searchBez(pCurrPr, (char *)Morph.Val.pStr);
    if (pBez != NULL)
    {
        printf("    Bez bereit vorhanden %s\n", Morph.Val.pStr);
        return 0;
    }
    if (createBez((char *)Morph.Val.pStr) == NULL)
    {
        printf("    Fehler beim anlegen von  %s\n", Morph.Val.pStr);
        return 0;
    }
    return 1;
}

int Block3()
{
    printf("BLOCK 3\n");
    tBez *pBez;
    tConst *pCnst;
    pBez = (tBez *)pCurrPr->pLBez->pfirst->pdata;
    if ((pCnst = searchConst(Morph.Val.Num)) != NULL)
        pBez->pObj = pCnst;
    else
    {
        pCnst = createConst(Morph.Val.Num);
        if (pCnst == NULL)
            return 0;
        pBez->pObj = pCnst;
    }
    return 1;
}

//TODO: nochmal checken !!!!

int Block7()
{
    printf("BLOCK 7\n");
    printf("    %s\n", (char *)Morph.Val.pStr);
    tBez *pBez = NULL;
    pBez = searchBez(pCurrPr, (char *)Morph.Val.pStr);
    if (pBez != NULL)
    {
        // Bezeichner schon vorhanden
        return 0;
    }
    // printf("create Bez\n");
    if (createBez((char *)Morph.Val.pStr) == NULL)
    {
        // Fehler beim anlegen des Bezeichners
        return 0;
    }
    return createVar(KzVar);
}

int Block11()
{
    printf("BLOCK 11\n");
    tBez *pBez;
    tProc *pProc;
    pBez = searchBez(pCurrPr, (char *)Morph.Val.pStr);
    if (pBez != NULL)
    {
        printf("    Bez bereits vorhanden %s\n", Morph.Val.pStr);
        return 0;
    }
    if (createBez((char *)Morph.Val.pStr) == NULL)
    {
        printf("    fehler beim anlegen des bez %s\n", Morph.Val.pStr);
        return 0;
    }
    pProc = createProc(pCurrPr);
    pBez->pObj = pProc;
    pCurrPr = pProc;

    return 1;
}

int Block14()
{
    // printf("BLOCK14\n");
    while (popL(pCurrPr->pLBez) != -1)
        ;
    pCurrPr = pCurrPr->pParent;
    return 1;
}

int Block15()
{
    printf("BLOCK15\n");
    code(retProc);
    CodeOut();
    // TODO Namensliste der Proc auflösen
    return 1;
}

int Block16()
{
    printf("BLOCK16\n");
    // Codeausgabepuffer initialisieren
    code(entryProc, 0, pCurrPr->IdxProc, pCurrPr->SpzzVar);
    return 1;
}