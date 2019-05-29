/*==================================================================*/
/*  RTVCMD -- Retrieve Command Source                               */
/*==================================================================*/
/*                                                                  */
/*  1997/10/15                                                      */
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

/*  CPP:  RTVCMDC    */

             CMD        PROMPT('Retrieve Command Source')
             PARM       KWD(CMD) TYPE(CMD) MIN(1) +
                          PROMPT('Command')
             PARM       KWD(SRCFILE) TYPE(SRC) +
                          PROMPT('Source file')
             PARM       KWD(SRCMBR) TYPE(*NAME) +
                          DFT(*CMD) +
                          SPCVAL(*CMD) +
                          PROMPT('Source member')
             PARM       KWD(PROMPT) TYPE(*CHAR) LEN(5) +
                          RSTD(*YES) +
                          DFT(*TEXT) +
                          SPCVAL((*TEXT) (*MSG)) +
                          PROMPT('Prompt text or msg IDs')
 CMD:        QUAL       TYPE(*NAME) LEN(10)
             QUAL       TYPE(*NAME) LEN(10) +
                          DFT(*LIBL) +
                          SPCVAL(*LIBL *CURLIB) +
                          PROMPT('Library')
 SRC:        QUAL       TYPE(*NAME) LEN(10) +
                          DFT(QCMDSRC)
             QUAL       TYPE(*NAME) LEN(10) +
                          DFT(*LIBL) +
                          SPCVAL(*LIBL *CURLIB) +
                          PROMPT('Library')
