/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *
 *  Version:    C, Version 2.1
 *
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */

#include "dhry.h"
#include "dhry_portme.h"

/* Global Variables: */

Rec_Pointer Ptr_Glob, Next_Ptr_Glob;
int Int_Glob;
Boolean Bool_Glob;
char Ch_1_Glob, Ch_2_Glob;
int Arr_1_Glob[50];
int Arr_2_Glob[50][50];

/* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
Boolean Reg = false;
#define REG
/* REG becomes defined as empty */
/* i.e. no register variables   */
#else
Boolean Reg = true;
#endif

/* variables for time measurement: */

#define Too_Small_Time 2
/* Measurements should last at least 2 seconds */

double User_Time;
double Microseconds, Dhrystones_Per_Second, DMips_Per_MHz;

/* end of variables for time measurement */

void dhrystone(void)
/*****/
/* main program, corresponds to procedures        */
/* Main and Proc_0 in the Ada version             */
{
    One_Fifty Int_1_Loc;
    REG One_Fifty Int_2_Loc;
    One_Fifty Int_3_Loc;
    REG char Ch_Index;
    Enumeration Enum_Loc;
    Str_30 Str_1_Loc;
    Str_30 Str_2_Loc;
    REG int Run_Index;
    REG int Number_Of_Runs;

    /* Initializations */

    Next_Ptr_Glob = (Rec_Pointer)malloc(sizeof(Rec_Type));
    Ptr_Glob = (Rec_Pointer)malloc(sizeof(Rec_Type));

    Ptr_Glob->Ptr_Comp = Next_Ptr_Glob;
    Ptr_Glob->Discr = Ident_1;
    Ptr_Glob->variant.var_1.Enum_Comp = Ident_3;
    Ptr_Glob->variant.var_1.Int_Comp = 40;
    strcpy(Ptr_Glob->variant.var_1.Str_Comp, "DHRYSTONE PROGRAM, SOME STRING");
    strcpy(Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

    Arr_2_Glob[8][7] = 10;
    /* Was missing in published program. Without this statement,    */
    /* Arr_2_Glob [8][7] would have an undefined value.             */
    /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
    /* overflow may occur for this array element.                   */

    PRINTF("Dhrystone Benchmark, Version 2.1 (Language: C)\r\n");
    if (Reg)
    {
        PRINTF("Program compiled with 'register' attribute\r\n");
    }
    else
    {
        PRINTF("Program compiled without 'register' attribute\r\n");
    }

    Number_Of_Runs = NUMBER_OF_RUNS;
    PRINTF("Execution starts, %d runs through Dhrystone\r\n", Number_Of_Runs);

    /***************/
    /* Start timer */
    /***************/
    start_time();
    for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
    {
        Proc_5();
        Proc_4();
        /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
        Int_1_Loc = 2;
        Int_2_Loc = 3;
        strcpy(Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
        Enum_Loc = Ident_2;
        Bool_Glob = !Func_2(Str_1_Loc, Str_2_Loc);
        /* Bool_Glob == 1 */
        while (Int_1_Loc < Int_2_Loc) /* loop body executed once */
        {
            Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
            /* Int_3_Loc == 7 */
            Proc_7(Int_1_Loc, Int_2_Loc, &Int_3_Loc);
            /* Int_3_Loc == 7 */
            Int_1_Loc += 1;
        } /* while */
          /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
        Proc_8(Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
        /* Int_Glob == 5 */
        Proc_1(Ptr_Glob);
        for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
        /* loop body executed twice */
        {
            if (Enum_Loc == Func_1(Ch_Index, 'C'))
            /* then, not executed */
            {
                Proc_6(Ident_1, &Enum_Loc);
                strcpy(Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
                Int_2_Loc = Run_Index;
                Int_Glob = Run_Index;
            }
        }
        /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
        Int_2_Loc = Int_2_Loc * Int_1_Loc;
        Int_1_Loc = Int_2_Loc / Int_3_Loc;
        Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
        /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
        Proc_2(&Int_1_Loc);
        /* Int_1_Loc == 5 */

    } /* loop "for Run_Index" */
    /**************/
    /* Stop timer */
    /**************/
    end_time();
    User_Time = secs;

    PRINTF("Execution ends\r\n");
    PRINTF("\r\n");
    PRINTF("Final values of the variables used in the benchmark:\r\n");
    PRINTF("Int_Glob: %d, it should be %d\r\n", Int_Glob, 5);
    PRINTF("Bool_Glob: %d, it should be %d\r\n", Bool_Glob, 1);
    PRINTF("Ch_1_Glob: %c, it should be %c\r\n", Ch_1_Glob, 'A');
    PRINTF("Ch_2_Glob: %c, it should be %c\r\n", Ch_2_Glob, 'B');
    PRINTF("Arr_1_Glob[8]: %d, it should be %d\r\n", Arr_1_Glob[8], 7);
    PRINTF("Arr_2_Glob[8][7]: %d, it should be Number_Of_Runs + 10\r\n", Arr_2_Glob[8][7]);
    PRINTF("Ptr_Glob->\r\n");
    PRINTF("  Ptr_Comp: %d, it should be implementation-dependent\r\n", (int)Ptr_Glob->Ptr_Comp);
    PRINTF("  Discr: %d, it should be %d\r\n", Ptr_Glob->Discr, 0);
    PRINTF("  Enum_Comp: %d, it should be %d\r\n", Ptr_Glob->variant.var_1.Enum_Comp, 2);
    PRINTF("  Int_Comp: %d, it should be %d\r\n", Ptr_Glob->variant.var_1.Int_Comp, 17);
    PRINTF("  Str_Comp: %s, it should be DHRYSTONE PROGRAM, SOME STRING\r\n", Ptr_Glob->variant.var_1.Str_Comp);
    PRINTF("Next_Ptr_Glob->\r\n");
    PRINTF("  Ptr_Comp: %d, it should be implementation-dependentr\r\n", (int)Next_Ptr_Glob->Ptr_Comp);
    PRINTF("  Discr: %d, it should be %d\r\n", Next_Ptr_Glob->Discr, 0);
    PRINTF("  Enum_Comp: %d, it should be %d\r\n", Next_Ptr_Glob->variant.var_1.Enum_Comp, 1);
    PRINTF("  Int_Comp: %d, it should be %d\r\n", Next_Ptr_Glob->variant.var_1.Int_Comp, 18);
    PRINTF("  Str_Comp: %s, it should be DHRYSTONE PROGRAM, SOME STRING\r\n", Next_Ptr_Glob->variant.var_1.Str_Comp);
    PRINTF("Int_1_Loc: %d, it should be %d\r\n", Int_1_Loc, 5);
    PRINTF("Int_2_Loc: %d, it should be %d\r\n", Int_2_Loc, 13);
    PRINTF("Int_3_Loc: %d, it should be %d\r\n", Int_3_Loc, 7);
    PRINTF("Enum_Loc: %d, it should be %d\r\n", Enum_Loc, 1);
    PRINTF("Str_1_Loc: %s, it should be DHRYSTONE PROGRAM, 1'ST STRING\r\n", Str_1_Loc);
    PRINTF("Str_2_Loc: %s, it should be DHRYSTONE PROGRAM, 2'ND STRING\r\n", Str_2_Loc);
    PRINTF("\r\n");

    PRINTF("User Time is %6.2fs\r\n", User_Time);

    if (User_Time < Too_Small_Time)
    {
        PRINTF("Measured time too small to obtain meaningful results\n");
        PRINTF("Please increase number of runs\n");
        PRINTF("\n");
    }
    else
    {
        Microseconds = User_Time * Mic_secs_Per_Second / (double) Number_Of_Runs;
        Dhrystones_Per_Second = (double) Number_Of_Runs / User_Time;
        DMips_Per_MHz = Dhrystones_Per_Second / CORE_FREQ_MHz / 1757.0;

        PRINTF("Microseconds for one run through Dhrystone: %6.2f \r\n", Microseconds);
        PRINTF("Dhrystones per Second: %6.2f \r\n", Dhrystones_Per_Second);
        PRINTF("DMIPS: %2.4f \r\n", DMips_Per_MHz * CORE_FREQ_MHz);
        PRINTF("DMIPS/MHz: %2.4f \r\n", DMips_Per_MHz);
        PRINTF("\r\n");
    }
}

void Proc_1(REG Rec_Pointer Ptr_Val_Par)
/******************/
/* executed once */
{
    REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;
    /* == Ptr_Glob_Next */
    /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
    /* corresponds to "rename" in Ada, "with" in Pascal           */

    structassign(*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob);
    Ptr_Val_Par->variant.var_1.Int_Comp = 5;
    Next_Record->variant.var_1.Int_Comp = Ptr_Val_Par->variant.var_1.Int_Comp;
    Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
    Proc_3(&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp
                        == Ptr_Glob->Ptr_Comp */
    if (Next_Record->Discr == Ident_1)
    /* then, executed */
    {
        Next_Record->variant.var_1.Int_Comp = 6;
        Proc_6(Ptr_Val_Par->variant.var_1.Enum_Comp, &Next_Record->variant.var_1.Enum_Comp);
        Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
        Proc_7(Next_Record->variant.var_1.Int_Comp, 10, &Next_Record->variant.var_1.Int_Comp);
    }
    else /* not executed */
        structassign(*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */

void Proc_2(One_Fifty *Int_Par_Ref)
/******************/
/* executed once */
/* *Int_Par_Ref == 1, becomes 4 */
{
    One_Fifty Int_Loc;
    Enumeration Enum_Loc;

    Int_Loc = *Int_Par_Ref + 10;
    do /* executed once */
        if (Ch_1_Glob == 'A')
        /* then, executed */
        {
            Int_Loc -= 1;
            *Int_Par_Ref = Int_Loc - Int_Glob;
            Enum_Loc = Ident_1;
        }                        /* if */
    while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */

void Proc_3(Rec_Pointer *Ptr_Ref_Par)
/******************/
/* executed once */
/* Ptr_Ref_Par becomes Ptr_Glob */
{
    if (Ptr_Glob != Null)
        /* then, executed */
        *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
    Proc_7(10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */

void Proc_4(void) /* without parameters */
/*******/
/* executed once */
{
    Boolean Bool_Loc;

    Bool_Loc = Ch_1_Glob == 'A';
    Bool_Glob = Bool_Loc | Bool_Glob;
    Ch_2_Glob = 'B';
} /* Proc_4 */

void Proc_5(void) /* without parameters */
/*******/
/* executed once */
{
    Ch_1_Glob = 'A';
    Bool_Glob = false;
} /* Proc_5 */

/* Procedure for the assignment of structures,          */
/* if the C compiler doesn't support this feature       */
#ifdef NOSTRUCTASSIGN
memcpy(d, s, l) register char *d;
register char *s;
register int l;
{
    while (l--)
        *d++ = *s++;
}
#endif
