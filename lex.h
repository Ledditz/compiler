// Arnold Beck
typedef enum T_Fx
{
    ifl = 0x0,
    ifb = 0x10,
    ifgl = 0x20,
    ifsl = 0x30,
    ifslb = 0x40
} tFx;

/* Morphemcodes */
typedef enum T_MC
{
    mcEmpty,
    mcSymb,
    mcNum,
    mcIdent,
    mcStrng
} tMC;

typedef enum T_ZS
{
    zNIL,
    zERG = 128,
    zLE,
    zGE,
    zBGN,
    zCLL,
    zCST,
    zDO,
    zEND,
    zIF,
    zODD,
    zPRC,
    zTHN,
    zVAR,
    zWHL
} tZS;

typedef struct
{
    tMC MC;      /* Morphemcode */
    int PosLine; /* Zeile       */
    int PosCol;  /* Spalte      */
    union VAL
    {
        long Num;
        char *pStr;
        int Symb;
    } Val;
    int MLen; /* Morpheml„nge*/
} tMorph;

typedef struct
{
    int Zn;
    void (*fkt)();
} tAutomat;

typedef struct
{
    char *KeyW;
    int keyWordCode;
} tKeyWordTab;

int initLex(char *fname);

tMorph Lex(void);
