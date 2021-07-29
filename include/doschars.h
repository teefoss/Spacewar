/* The MS-DOS ASCII Character Set */
#ifndef doschars_h
#define doschars_h

enum
{
    DOS_NUL               = 0x00,
    DOS_FACE1             = 0x01,
    DOS_FACE2             = 0x02,
    DOS_HEART             = 0x03,
    DOS_DIAMOND           = 0x04,
    DOS_CLUB              = 0x05,
    DOS_SPADE             = 0x06,
    DOS_DOT1              = 0x07,
    DOS_DOT2              = 0x08,
    DOS_RING1             = 0x09,
    DOS_RING2             = 0x0A,
    DOS_MALE              = 0x0B,
    DOS_FEMALE            = 0x0C,
    DOS_NOTE1             = 0x0D,
    DOS_NOTE2             = 0x0E,
    DOS_STAR              = 0x0F,
    DOS_TRI_RIGHT         = 0x10,
    DOS_TRI_LEFT          = 0x11,
    DOS_UPDOWNARROW1      = 0x12,
    DOS_DBL_EXCLAM        = 0x13,
    DOS_PARAGRAPH         = 0x14,
    DOS_SECTION           = 0x15,
    DOS_CURSOR            = 0x16,
    DOS_UPDOWNARROW2      = 0x17,
    DOS_UPARROW           = 0x18,
    DOS_DOWNARROW         = 0x19,
    DOS_RIGHTARROW        = 0x1A,
    DOS_LEFTARROW         = 0x1B,
    DOS_REV_INV_NOT       = 0x1C,
    DOS_RIGHTLEFTARROW    = 0x1D,
    DOS_TRI_UP            = 0x1E,
    DOS_TRI_DOWN          = 0x1F,
    
    /* printable characters omitted */
    
    DOS_DELETE            = 0x7F,
    DOS_C_CEDI_UPPER      = 0x80,
    DOS_U_UMLT_LOWER      = 0x81,
    DOS_E_ACUT_LOWER      = 0x82,
    DOS_A_CIRC_LOWER      = 0x83,
    DOS_A_UMLT_LOWER      = 0x84,
    DOS_A_GRAV_LOWER      = 0x85,
    DOS_A_RING_LOWER      = 0x86,
    DOS_C_CEDI_LOWER      = 0x87,
    DOS_E_CIRC_LOWER      = 0x88,
    DOS_E_UMLT_LOWER      = 0x89,
    DOS_E_GRAV_LOWER      = 0x8A,
    DOS_I_UMLT_LOWER      = 0x8B,
    DOS_I_CIRC_LOWER      = 0x8C,
    DOS_I_GRAV_LOWER      = 0x8D,
    DOS_A_UMLT_UPPER      = 0x8E,
    DOS_A_RING_UPPER      = 0x8F,
    DOS_E_ACUT_UPPER      = 0x90,
    DOS_AE_LOWER          = 0x91,
    DOS_AE_UPPER          = 0x92,
    DOS_O_CIRC_LOWER      = 0x93,
    DOS_O_UMLT_LOWER      = 0x94,
    DOS_O_GRAV_LOWER      = 0x95,
    DOS_U_CIRC_LOWER      = 0x96,
    DOS_U_GRAV_LOWER      = 0x97,
    DOS_Y_UMLT_LOWER      = 0x98,
    DOS_O_UMLT_UPPER      = 0x99,
    DOS_U_UMLT_UPPER      = 0x9A,
    DOS_CENT              = 0x9B,
    DOS_POUND             = 0x9C,
    DOS_YEN               = 0x9D,
    DOS_PESETA            = 0x9E,
    DOS_F_HOOK            = 0x9F,
    DOS_A_ACUT_LOWER      = 0xA0,
    DOS_I_ACUT_LOWER      = 0xA1,
    DOS_O_ACUT_LOWER      = 0xA2,
    DOS_U_ACUT_LOWER      = 0xA3,
    DOS_N_TILD_LOWER      = 0xA4,
    DOS_N_TILD_UPPER      = 0xA5,
    DOS_FEM_ORD           = 0xA6,
    DOS_MASC_ORD          = 0xA7,
    DOS_INV_QUESTION      = 0xA8,
    DOS_NOT_REVERSED      = 0xA9,
    DOS_NOT               = 0xAA,
    DOS_FRAC_HALF         = 0xAB,
    DOS_FRAC_QUARTER      = 0xAC,
    DOS_INV_EXCLAMATION   = 0xAD,
    DOS_LEFT_ANG_QUOTE    = 0xAE,
    DOS_RIGHT_AND_QUOTE   = 0xAF,
    DOS_BLOCK_LIGHT       = 0xB0,
    DOS_BLOCK_MEDIUM      = 0xB1,
    DOS_BLOCK_DARK        = 0xB2,

    /* box-drawing characters omitted */
    
    DOS_BLOCK_FULL        = 0xDB,
    DOS_BLOCK_BOTTOM      = 0xDC,
    DOS_BLOCK_LEFT        = 0xDD,
    DOS_BLOCK_RIGHT       = 0xDE,
    DOS_BLOCK_TOP         = 0xDF,
    DOS_ALPHA             = 0xE0,
    DOS_BETA              = 0xE1,
    DOS_GAMMA             = 0xE2,
    DOS_PI                = 0xE3,
    DOS_SIGMA_UPPER       = 0xE4,
    DOS_SIGMA_LOWER       = 0xE5,
    DOS_MU                = 0xE6,
    DOS_TAU               = 0xE7,
    DOS_PHI_UPPER         = 0xE8,
    DOS_THETA             = 0xE9,
    DOS_OMEGA             = 0xEA,
    DOS_DELTA             = 0xEB,
    DOS_INFINITY          = 0xEC,
    DOS_PHI_LOWER         = 0xED,
    DOS_EPSILON           = 0xEE,
    DOS_INTERSECTION      = 0xEF,
    DOS_IDENTICAL         = 0xF0,
    DOS_PLUSMINUS         = 0xF1,
    DOS_GT_OR_EQ          = 0xF2,
    DOS_LT_OR_EQ          = 0xF3,
    DOS_INTEGRAL_TOP      = 0xF4,
    DOS_INTEGRAL_BOTTOM   = 0xF5,
    DOS_DIVISION          = 0xF6,
    DOS_ALMOST_EQUALS     = 0xF7,
    DOS_DEGREE            = 0xF8,
    DOS_INTERPUNCT1       = 0xF9,
    DOS_INTERPUNCT2       = 0xFA,
    DOS_RADICAL           = 0xFB,
    DOS_SUPER_N           = 0xFC,
    DOS_SUPER_2           = 0xFD,
    DOS_SQUARE            = 0xFE,
    DOS_NBSP              = 0xFF
};

#endif /* doschars_h */
