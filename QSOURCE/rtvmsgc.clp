/*==================================================================*/
/*  RTVCMD -- Retrieve Command Source                               */
/*==================================================================*/
/*                                                                  */
/*  9001/10/16                                                      */
/*                                                                  */
/*  Dave McKenzie                                                   */
/*  the zbig group, inc.                                            */
/*                                                                  */
/*  Internet:     davemck@zbiggroup.com                             */
/*                                                                  */
/*  This software is hereby placed in the public domain.            */
/*                                                                  */
/*  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR DIRECT, INDIRECT,    */
/*  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF    */
/*  THE USE OF THIS SOFTWARE.                                       */
/*                                                                  */
/*  THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,    */
/*  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF                   */
/*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,              */
/*  AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN "AS IS"  */
/*  BASIS, AND THE AUTHOR HAS NO OBLIGATION TO PROVIDE MAINTENANCE, */
/*  SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.               */
/*==================================================================*/

     PGM PARM(&CMDLIB &OFILLIB &OMBR)

     DCL VAR(&CMDLIB)   TYPE(*CHAR) LEN(20)
     DCL VAR(&OFILLIB)  TYPE(*CHAR) LEN(20)
     DCL VAR(&OMBR)     TYPE(*CHAR) LEN(10)

     DCL VAR(&CMDNM)    TYPE(*CHAR) LEN(10)
     DCL VAR(&CMDLI)    TYPE(*CHAR) LEN(10)
     DCL VAR(&MCMDLIB)  TYPE(*CHAR) LEN(20)
     DCL VAR(&OFILE)    TYPE(*CHAR) LEN(10)
     DCL VAR(&OLIBR)    TYPE(*CHAR) LEN(10)
     DCL VAR(&CURLIB)   TYPE(*CHAR) LEN(10)
     DCL VAR(&CMDTEXT)  TYPE(*CHAR) LEN(50)
     DCL VAR(&PROTOS)   TYPE(*CHAR) LEN(10) VALUE('RTVCMDS')
     DCL VAR(&USRSPC)   TYPE(*CHAR) LEN(10) VALUE('RTVCMDSPC')

     DCL VAR(&ERRORSW)  TYPE(*LGL)
     DCL VAR(&EMSGID)   TYPE(*CHAR) LEN(7)
     DCL VAR(&EMSGDTA)  TYPE(*CHAR) LEN(100)
     DCL VAR(&EMSGF)    TYPE(*CHAR) LEN(10)
     DCL VAR(&EMSGFLIB) TYPE(*CHAR) LEN(10)

     MONMSG MSGID(CPF0000) EXEC(GOTO CMDLBL(STDERR1))

     RTVJOBA CURLIB(&CURLIB)
     CHGVAR VAR(&CMDNM) VALUE(%SST(&CMDLIB   1 10))
     CHGVAR VAR(&CMDLI) VALUE(%SST(&CMDLIB  11 10))
     CHGVAR VAR(&OFILE) VALUE(%SST(&OFILLIB  1 10))
     CHGVAR VAR(&OLIBR) VALUE(%SST(&OFILLIB 11 10))
     IF COND(&OMBR = *CMD) THEN(CHGVAR VAR(&OMBR) VALUE(&CMDNM))

     IF COND(&CMDLI *EQ *CURLIB) THEN(DO)
       IF COND(&CURLIB *EQ *NONE) THEN(CHGVAR VAR(&CMDLI) VALUE(QGPL))
         ELSE CMD(CHGVAR VAR(&CMDLI) VALUE(&CURLIB))
     ENDDO

     CHGVAR VAR(%SST(&MCMDLIB 1 10)) VALUE(&CMDNM)
     CHGVAR VAR(%SST(&MCMDLIB 11 10)) VALUE(&CMDLI)

     IF COND(&OLIBR *EQ *CURLIB) THEN(DO)
       IF COND(&CURLIB *EQ *NONE) THEN(CHGVAR VAR(&OLIBR) VALUE(QGPL))
         ELSE CMD(CHGVAR VAR(&OLIBR) VALUE(&CURLIB))
     ENDDO

/*  check for cmd   */
     CHKOBJ OBJ(&CMDLI/&CMDNM) OBJTYPE(*CMD) AUT(*USE)
     RTVOBJD OBJ(&CMDLI/&CMDNM) OBJTYPE(*CMD) TEXT(&CMDTEXT)

/*  check for source file   */
     CHKOBJ OBJ(&OLIBR/&OFILE) OBJTYPE(*FILE) AUT(*CHANGE)

/*  copy a mbr with src type CLP  */
     RMVM FILE(&OLIBR/&OFILE) MBR(&OMBR)
     MONMSG MSGID(CPF0000)
     CPYF FROMFILE(&PROTOS) TOFILE(&OLIBR/&OFILE) FROMMBR(CLP) +
       TOMBR(&OMBR) MBROPT(*REPLACE) FMTOPT(*NOCHK)
     MONMSG MSGID(CPF0000)
     CLRPFM FILE(&OLIBR/&OFILE) MBR(&OMBR)
     MONMSG MSGID(CPF0000)
     CHGPFM FILE(&OLIBR/&OFILE) MBR(&OMBR) TEXT(&CMDTEXT)

/*  call pgm to copy cmd to usrspc   */
     DLTUSRSPC USRSPC(QTEMP/&USRSPC)
     MONMSG MSGID(CPF0000)
     CALL PGM(RTVCMDM) PARM(&MCMDLIB)

/*  call pgm to retrieve cmd msgs        */
     OVRDBF FILE(SRC) TOFILE(&OLIBR/&OFILE) MBR(&OMBR) +
            SECURE(*YES)
     CALL PGM(RTVMSG)

     DLTOVR FILE(SRC)
     MONMSG MSGID(CPF0000)
     DLTUSRSPC USRSPC(QTEMP/&USRSPC)
     MONMSG MSGID(CPF0000)
     DLTF FILE(QTEMP/RTVCMDSF)
     MONMSG MSGID(CPF0000)
     RCLRSC
     MONMSG MSGID(CPF0000)
     RMVMSG PGMQ(*ALLINACT) CLEAR(*ALL)
     RMVMSG CLEAR(*ALL)
     GOTO CMDLBL(ENDPGM)
/*-----------------------------------------------------------------*/
 STDERR1:               /* Standard error handling routine */
             IF         &ERRORSW SNDPGMMSG MSGID(CPF9999) +
                          MSGF(QCPFMSG) MSGTYPE(*ESCAPE) /* Func chk */
             CHGVAR     &ERRORSW '1' /* Set to fail if error occurs */
 STDERR2:    RCVMSG     MSGTYPE(*DIAG) MSGDTA(&EMSGDTA) MSGID(&EMSGID) +
                          MSGF(&EMSGF) MSGFLIB(&EMSGFLIB)
             IF         (&EMSGID *EQ '       ') GOTO STDERR3
             SNDPGMMSG  MSGID(&EMSGID) MSGF(&EMSGFLIB/&EMSGF) +
                          MSGDTA(&EMSGDTA) TOPGMQ(*PRV *) +
                          MSGTYPE(*DIAG)
             GOTO       STDERR2 /* Loop back for addl diagnostics */
 STDERR3:    RCVMSG     MSGTYPE(*EXCP) MSGDTA(&EMSGDTA) MSGID(&EMSGID) +
                          MSGF(&EMSGF) MSGFLIB(&EMSGFLIB)
             SNDPGMMSG  MSGID(&EMSGID) MSGF(&EMSGFLIB/&EMSGF) +
                          MSGDTA(&EMSGDTA) TOPGMQ(*PRV *) +
                          MSGTYPE(*ESCAPE)
/*-----------------------------------------------------------------*/
ENDPGM:
             ENDPGM
