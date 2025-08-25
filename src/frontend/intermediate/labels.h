#ifndef _FRONT_INTERMEDIATE_LABELS_H
#define _FRONT_INTERMEDIATE_LABELS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

typedef enum LABEL_KIND {
    LBL_Land_false,
    LBL_Land_true,
    LBL_Lbreak,
    LBL_Lcase,
    LBL_Lcontinue,
    LBL_Ldefault,
    LBL_Ldo_while,
    LBL_Ldo_while_start,
    LBL_Lfor,
    LBL_Lfor_start,
    LBL_Lif_else,
    LBL_Lif_false,
    LBL_Lor_false,
    LBL_Lor_true,
    LBL_Lstring,
    LBL_Lswitch,
    LBL_Lternary_else,
    LBL_Lternary_false,
    LBL_Lwhile
} LABEL_KIND;

#endif
