/*** START HEADER FILE SPECIFICATIONS *****************************/
/*                                                                */
/* Header File Name: H/QCDRCMDI                                   */
/*                                                                */
/* Descriptive Name: Retrieve Command Information.                */
/*                                                                */
/* 5763-SS1  (C) Copyright IBM Corp. 1994,1994                    */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/*                                                                */
/*                                                                */
/* Description: The Retrieve Command Information API retrieves    */
/*              information about a command definition object and */
/*              places it into a single variable in the calling   */
/*              program.                                          */
/*                                                                */
/* Header Files Included: None.                                   */
/*                                                                */
/* Macros List: None.                                             */
/*                                                                */
/* Structure List: Qcd_Help_Book_Info_t                           */
/*                 Qcd_Exit_Entries_t                             */
/*                 qcd_CMDI0100_t                                 */
/*                 qcd CMDI0200_t                                 */
/*                                                                */
/*                                                                */
/*                                                                */
/* Function Prototype List: QCDRCMDI                              */
/*                                                                */
/* Change Activity:                                               */
/*                                                                */
/* CFD List:                                                      */
/*                                                                */
/* FLAG REASON       LEVEL DATE   PGMR      CHANGE DESCRIPTION    */
/* ---- ------------ ----- ------ --------- ----------------------*/
/* $A0= D2862000     3D10  940420 BRINKER:  New Include           */
/*                                                                */
/* End CFD List.                                                  */
/*                                                                */
/*  Additional notes about the Change Activity                    */
/* End Change Activity.                                           */
/*** END HEADER FILE SPECIFICATIONS *******************************/

#ifndef QCDRCMDI_h
#define QCDRCMDI_h

/******************************************************************/
/* Prototype for calling QCDRCMDI API                             */
/******************************************************************/
#ifdef  __ILEC400__
      #pragma linkage(QCDRCMDI,OS,nowiden)
#else
      extern "OS"
#endif

void QCDRCMDI (void *,    /* Receiver variable                    */
               int,       /* Length of receiver variable          */
               char *,    /* Format name                          */
               void *,    /* Qualified command name               */
               void *);   /* Error code                           */

/******************************************************************/
/* Type Definition for the Help book information format.          */
/******************************************************************/
typedef _Packed struct Qcd_Help_Book_Info
{
  char Reserved[71];
  char Help_Bookshelf[8];
} Qcd_Help_Book_Info_t;

/******************************************************************/
/* Type Definition for the REXX Exit Entries format.              */
/******************************************************************/
typedef _Packed struct Qcd_Exit_Entries
{
  char Exit_Name[10];
  char Exit_Lib_Name[10];
  int  Exit_Code;
} Qcd_Exit_Entries_t;

/******************************************************************/
/* Type Definition for the CMDI0100 format.                       */
/****                                                           ***/
/* NOTE: The following type definition only defines the fixed     */
/*       portion of the format.  Any varying length field will    */
/*       have to be defined by the user.                          */
/******************************************************************/
typedef _Packed struct Qcd_CMDI0100
{
  int  Bytes_Returned;
  int  Bytes_Available;
  char Cmd_Name[10];
  char Cmd_Lib_Name[10];
  char Cmd_Proc_Name[10];
  char Cmd_Proc_Lib_Name[10];
  char Source_Name[10];
  char Source_Lib_Name[10];
  char Source_Member_Name[10];
  char Val_Name[10];
  char Val_Lib_Name[10];
  char Mode_Info[10];
  char Where_Run[15];
  char Allow_Limited[1];
  int  Max_Parameters;
  char Prompt_Name[10];
  char Prompt_Lib_Name[10];
  char Message_Name[10];
  char Message_Lib_Name[10];
  char Help_Panel_Name[10];
  char Help_Panel_Lib_Name[10];
  char Help_Id[10];
  char Search_Name[10];
  char Search_Lib_Name[10];
  char Current_Lib[10];
  char Product_Lib[10];
  char P_Override_Name[10];
  char P_Override_Lib_Name[10];
  char Restricted_Release[6];
  char Text[50];
  char Call_State[2];
  char Val_Check_Call_State[2];
  char P_Override_Call_State[2];
  int  Offset_Help_Book;
  int  Length_Help_Book;
  int  CCSID;
  char Enable_GUI;
  char Threadsafe;
  char Multithreaded_Job_Action;
  char Reserved[15];
} Qcd_CMDI0100_t;

/******************************************************************/
/* Type Definition for the CMDI0200 format.                       */
/****                                                          ****/
/* NOTE: The following type definition only defines the fixed     */
/*       portion of the format.  Any varying length field will    */
/*       have to be defined by the user.                          */
/******************************************************************/
typedef _Packed struct Qcd_CMDI0200
{
  Qcd_CMDI0100_t       cmdi0100;
  char                 Source_Name[10];
  char                 Source_Lib_Name[10];
  char                 Source_Member_Name[10];
  char                 Env_Name[10];
  char                 Env_Lib_Name[10];
  char                 Reserved[40];
  int                  Num_Exit_Entries;
  int                  Length_Exit_Entries;
/*Qcd_Exit_Entries_t   Entries[];*//* Varying length              */
/*Qcd_Help_Book_Info_t Help_Book_Info;*//* Varying length         */
} Qcd_CMDI0200_t;

#endif



