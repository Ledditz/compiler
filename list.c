#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include <stdint.h>

tList *CreateList()
{
    tList *list = malloc(sizeof(tList));
    if (list == NULL)
    {
        puts("error malloc");
        exit(-1);
    }
    list->pfirst = (t_cnt *)NULL;
    return list;
}

int pushL(tList *list, void *new)
{
    t_cnt *newConnector = malloc(sizeof(t_cnt));
    if (newConnector == NULL)
    {
        puts("error malloc");
        return (-1);
    }
    newConnector->pdata = new;
    // printf("insert Element, Data: %s\n", newConnector->pdata);
    if (list->pfirst == NULL)
    {
        newConnector->pnext = NULL;
        list->pfirst = newConnector;
    }
    else
    {
        newConnector->pnext = list->pfirst;
        list->pfirst = newConnector;
    }
    return 1;
}

int popL(tList *list)
{
    if (list->pfirst == NULL)
    {
        puts("nothing to pop");
        return -1;
    }
    else
    {
        t_cnt *oldConnector = list->pfirst;
        list->pfirst = list->pfirst->pnext;
        printf("poped: %d\n", oldConnector->pdata);
        free(oldConnector);
    }
    return 0;
}

void printList(tList *list)
{
    if (list->pfirst == NULL)
    {
        printf("nothing to print, List empty\n");
        return;
    }
    t_cnt *element = list->pfirst;
    int i = 0;
    do
    {
        printf("Item[%d]: %d\n", i++, element->pdata);
        element = element->pnext;
    } while (element != NULL);
}

tProc *createProc(tProc *pParent)
{
    tProc *pP = (tProc *)malloc(sizeof(tProc));
    if (pP == NULL)
    {
        return NULL;
    }
    pP->Kz = KzPro;
    pP->IdxProc = IdxProc++;
    pP->pParent = pParent;
    pP->pLBez = CreateList();
    pP->SpzzVar = 0;
    pCurrPr = pP;

    return pP;
}

tBez *searchBez(tProc *pProc, char *pBez)
{
    t_cnt *pB;
    // printf("suche nach: %s\n", pBez);
    pB = pProc->pLBez->pfirst;
    for (pB = pProc->pLBez->pfirst;
         pB != NULL && strcmp(((tBez *)pB->pdata)->pName, pBez) != 0;
         pB = pB->pnext)
        ;
    if (pB == NULL)
    {
        // printf("nix jefunden\n");
        return NULL;
    }
    // printf("jefunden %s\n", ((tBez *)pB->pdata)->pName);
    return (tBez *)pB->pdata;
}

tBez *searchBezGlobal(char *pBez)
{
    tProc *pPr = pCurrPr;
    tBez *pB;
    while (pPr != NULL && (pB = searchBez(pPr, pBez)) == NULL)
        pPr = pPr->pParent;
    return pB;
}

tBez *createBez(char *pBez)
{
    tBez *pB;
    pB = malloc(sizeof(tBez));
    if (pB == NULL)
    {
        return NULL;
    }

    pB->Kz = KzBez;
    pB->pObj = 0;
    pB->IdxProc = pCurrPr->IdxProc;
    pB->Len = strlen(pBez);
    pB->pName = malloc(strlen(pBez) + 1);

    if (pB->pName == NULL)
    {
        free(pB);
        return NULL;
    }
    else
    {
        strcpy(pB->pName, pBez);
    }
    if (pushL(pCurrPr->pLBez, pB) == -1)
    {
        free(pB->pName);
        free(pB);
        return NULL;
    }
    return pB;
}

tConst *createConst(long Val)
{
    tConst *pCnst;
    pCnst = malloc(sizeof(tConst));
    if (pCnst == NULL)
    {
        return NULL;
    }
    pCnst->Kz = KzCnt;
    pCnst->Val = Val;
    pCnst->Idx = IdxCnst++;

    if (pushL(pLConst, pCnst) == -1)
    {
        free(pCnst);
        return NULL;
    }
    return pCnst;
}

tConst *searchConst(long Val)
{
    t_cnt *pCnst;
    tConst *dummy;
    for (pCnst = pLConst->pfirst;
         pCnst != NULL && ((tConst *)pCnst->pdata)->Val != Val;
         pCnst = pCnst->pnext)
        // printf("Value searchCnst%i\n", ((tConst *)pCnst->pdata)->Val);
        ;
    if (pCnst)
        return (tConst *)pCnst->pdata;
    else
        return (tConst *)NULL;
}

int createVar(tKz KZ)
{
    tVar *pVar;
    tBez *pBez;
    if ((pVar = malloc(sizeof(tVar))) == NULL)
        return -1;

    pVar->Kz = KZ;
    pVar->Dspl = pCurrPr->SpzzVar;
    pCurrPr->SpzzVar += sizeof(uint32_t);

    if ((pBez = (tBez *)pCurrPr->pLBez->pfirst->pdata) == NULL) // ???!?!?!?!?!?!?
        return -1;
    pBez->pObj = pVar;
    return 1;
}

tLabl *creatLabel(tKz type, char *postion)
{
    tLabl *pLabl = malloc(sizeof(tLabl));
    if (pLabl == NULL)
        return NULL;
    pLabl->Kz = type;
    pLabl->iJmp = postion;
    return pLabl;
}