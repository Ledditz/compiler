//structs
typedef struct connector
{
    struct connector *pnext;
    void *pdata;
} t_cnt;

typedef struct list
{
    t_cnt *pfirst;
} tList;

typedef enum tKZ
{
    KzBez,
    KzVar,
    KzCnt,
    KzPro,
    KzJmp,
    KzJnt,
    KzWCnd
} tKz;

typedef struct tLABL
{
    tKz Kz;
    char *iJmp;
} tLabl;

typedef struct
{
    tKz Kz;        // Kz name (redundant)
    short IdxProc; // Prozedurnummer
    void *pObj;    // Pointer auf Objekt
    int Len;       // Länge (redundant)
    char *pName;   // Pointer auf Name
} tBez;

typedef struct tVAR
{
    tKz Kz;
    int Dspl; // Relativadresse
} tVar;

typedef struct tCONST
{
    tKz Kz;
    long Val;
    int Idx;
} tConst;

typedef struct tPROC
{
    tKz Kz;
    short IdxProc;
    struct tPROC *pParent;
    tList *pLBez;
    int SpzzVar;
} tProc;

//functions
tList *CreateList();
void printList(tList *list);
int pushL(tList *list, void *new);
int popL(tList *list);
tBez *createBez(char *pBez);
tConst *createConst(long Val);
tConst *searchConst(long Val);
tLabl *creatLabel(tKz type, char *postion);
int createVar(tKz KZ);
tProc *createProc(tProc *pParent);
tBez *searchBez(tProc *pProc, char *pBez);
tBez *searchBezGlobal(char *pBez);
int IdxProc;
tProc *pCurrPr;
int IdxCnst;
tList *pLConst;
tList *pListLabel;

int Block1(void);
int Block3(void);
int Block7(void);
int Block11(void);
int Block14(void);
int Block15(void);
int Block16(void);
int Programm1(void);
int Statement0(void);
int Statement2(void);
int Statement4(void);
int Statement6(void);
int Statement7(void);
int Statement8(void);
int Statement10(void);
int Statement16(void);
int Statement18(void);
int Statement20(void);
int Factor1(void);
int Factor0(void);
int Expression1(void);
int Expression6(void);
int Expression8(void);
int Term3(void);
int Term5(void);
int ConditionSaveOperator(void);
int Condition4(void);
int Condition1(void);