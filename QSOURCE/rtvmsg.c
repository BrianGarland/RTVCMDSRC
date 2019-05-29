/*==================================================================*
 *  RTVCMD -- Retrieve Command Source                               *
 *==================================================================*
 *                                                                  *
 *  2001/10/16                                                      *
 *                                                                  *
 *  Dave McKenzie                                                   *
 *  the zbig group, inc.                                            *
 *                                                                  *
 *  Internet:     davemck@zbiggroup.com                             *
 *                                                                  *
 *  This software is hereby placed in the public domain.            *
 *                                                                  *
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR DIRECT, INDIRECT,    *
 *  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF    *
 *  THE USE OF THIS SOFTWARE.                                       *
 *                                                                  *
 *  THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,    *
 *  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF                   *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,              *
 *  AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN "AS IS"  *
 *  BASIS, AND THE AUTHOR HAS NO OBLIGATION TO PROVIDE MAINTENANCE, *
 *  SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.               *
 *==================================================================*/

#undef   PC
#define  AS400

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifdef  AS400
#include <milib.h>
#include <micomput.h>
#include <miptrnam.h>
#include <mispcobj.h>
#pragma linkage( RTVCMDR, OS )
void RTVCMDR( char *, char *, char *, char * );
#endif

#define USRSPC  "RTVCMDSPC"

typedef unsigned char uchr;
typedef unsigned int  uint;
typedef unsigned long ulong;

/*  FUNCTION PROTOTYPES  */

#ifdef PC
FILE *    open_file( char *, char * );
#endif

void          proc_args( int, char ** );
void          zinit( void );
void          zdone( void );
void          put_line( char * );
void          put_cl( char *, char * );
unsigned char get_chr( long );
int           get_int( long );
long          get_long( long );
void          get_str( long, int, char * );
void          get_fixed( void );
void          rtv_msgs( long );
void          rtv_msg( int, int );
void          dbl_quotes( char * );

#ifdef  PC
FILE *InFile;
FILE *OutFile;
unsigned char to_ASCII_tab[257]
         ="\xFF\x01\x02\x03\x9C\x09\x86\x7F\x97\x8D\x8E\x0B\x0C\x0D\x0E\x0F"
          "\x10\x11\x12\x13\x9D\x85\x08\x87\x18\x19\x92\x8F\x1C\x1D\x1E\x1F"
          "\x80\x81\x82\x83\x84\x0A\x17\x1B\x88\x89\x8A\x8B\x8C\x05\x06\x07"
          "\x90\x91\x16\x93\x94\x95\x96\x04\x98\x99\x9A\x9B\x14\x15\x9E\x1A"
          "\x20\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\x5B\x2E\x3C\x28\x2B\x21"
          "\x26\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xB0\xB1\x5D\x24\x2A\x29\x3B\x5E"
          "\x2D\x2F\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\x7C\x2C\x25\x5F\x3E\x3F"
          "\xBA\xBB\xBC\xBD\xBE\xBF\xC0\xC1\xC2\x60\x3A\x23\x40\x27\x3D\x22"
          "\xC3\x61\x62\x63\x64\x65\x66\x67\x68\x69\xC4\xC5\xC6\xC7\xC8\xC9"
          "\xCA\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xCB\xCC\xCD\xCE\xCF\xD0"
          "\xD1\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\xD2\xD3\xD4\xD5\xD6\xD7"
          "\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7"
          "\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49\xE8\xE9\xEA\xEB\xEC\xED"
          "\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xEE\xEF\xF0\xF1\xF2\xF3"
          "\x5C\x9F\x53\x54\x55\x56\x57\x58\x59\x5A\xF4\xF5\xF6\xF7\xF8\xF9"
          "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xFA\xFB\xFC\xFD\xFE\xFF";
#endif

#define A_PARM  0
#define OFF2SINGLE_AREA 0x81
#define LEN_PRM_FX_AREA 0x29
#define LEN_OTH_FX_AREA 0x1B
#define CL_CMD_OFF 13
#define CL_PRM_OFF 24

/*  GLOBAL VARIABLES  */

#ifdef  AS400
_SYSPTR usrspc_syp = NULL;
uchr *  usrspc_spp = NULL;
uchr *  usrspc_bas = NULL;
#endif

#define OUT_STR_LN 70
char OutStr[OUT_STR_LN+1] = "";
char WkStr[OUT_STR_LN+1] = "";
char InFileName[81] = "";
char OutFileName[81] = "";
long Parm_off;

/*  CDO fixed portion  */

   int  CF_qinsept_index;    /* 1-rel index of cpp in QINSEPT              */
   char CF_cpp_name[11];     /* cpp name                                   */
   char CF_cpp_libr[11];     /* cpp library                                */
   char CF_vck_name[11];     /* validity checker name                      */
   char CF_vck_libr[11];     /* validity checker lbrary                    */
   int  CF_mode;             /* valid mode                                 */
   int  CF_allowed;          /* where allowed                              */
   int  CF_off_prompt;       /* off to prompt info                         */
   int  CF_off_interprm;     /* off to interparms test area                */
   int  CF_1st2prompt;       /* 1st parm to prompt for                     */
   int  CF_num_parms;        /* number of command keywords                 */

/*  Single parm fixed area  */

   int  PF_parm_no;
   int  PF_type;
   int  IsParm, IsElem, IsQual, IsDummy, HasElem, HasQual;

   int  PF_off_next_kwd;     /* offset to next parm                        */
   int  PF_off_listqual;     /* offset to list/qual                        */
   char PF_flag_byte;        /* flag byte                                  */
   char PF_var_pres;         /* var field presence                         */
   char PF_val_type;         /* type of data value                         */
   int  PF_val_length;       /* length of value                            */
   char PF_typex_frac_len;   /* frac digs for type(*X)                     */
   char PF_typex_total_len;  /* dec length for type(*X)                    */
   int  PF_min;              /* min number required                        */
   int  PF_max;              /* max number allowed                         */
   int  PF_nest_level;       /* list leg nest level                        */
   char PF_misc1;            /* miscellaneous bits                         */
   char PF_misc1a;           /* miscellaneous bits                         */
   char PF_misc2;            /* miscellaneous bits                         */
   int  PF_off_prompt;       /* offset to prompt text                      */
   char PF_bits_1A;          /* miscellaneous bits (off 1A)                */
   char PF_kwd_name[11];     /* keyword name                               */
   int  PF_kwd_num;          /* keyword number                             */
   int  PF_off_next2prompt;  /* off to next node to be prompted for        */

/*--------------------------------------------------------------------------
 */
main( int argc, char **argv )
{
   proc_args( argc, argv );
   zinit();

   get_fixed();
   rtv_msg( CF_off_prompt+12, CF_off_prompt+21 );   /* CMD prompt */
   rtv_msgs( OFF2SINGLE_AREA );
   zdone();

} /* main */
/*--------------------------------------------------------------------------
 */
void proc_args( int argc, char *argv[] )
{
   int i;
   char dash, swtch;

   for ( i=1; i < argc; i++ )
   {
      dash = argv[i][0];

   /*  if it's a switch... */
      if ( dash == '-' || dash == '/')
      {
         swtch = toupper( argv[i][1] );

         switch ( swtch )
         {
            case 'I':
               strcpy(InFileName,argv[i+1]);
               break;
            case 'O':
               strcpy(OutFileName,argv[i+1]);
               break;
         }
      }
   } /* for i */
} /* proc_args */
/*--------------------------------------------------------------------------
 */
void zinit( void )
{
   char roper[8];
   char rretc[8];
   WkStr[OUT_STR_LN] = '\0';
   OutStr[OUT_STR_LN] = '\0';

#ifdef  PC
   to_ASCII_tab[0] = '\0';
   InFile = open_file( InFileName, "rb" );
   OutFile = open_file( OutFileName, "w" );
#endif

#ifdef  AS400
   usrspc_syp = rslvsp( _Usrspc, USRSPC, "QTEMP", _AUTH_NONE );
   usrspc_bas = setsppfp( usrspc_syp );
   usrspc_spp = usrspc_bas;
   memcpy( roper, "OPNSRC  ", 8);
   RTVCMDR( " ", " ", roper, rretc );
#endif

   put_cl( "PGM", "PARM(&MSGF &MSGFLIB)" );
   put_cl( "", "" );
   put_cl( "DCL", "VAR(&MSGF)    TYPE(*CHAR) LEN(10)" );
   put_cl( "DCL", "VAR(&MSGFLIB) TYPE(*CHAR) LEN(10)" );
   put_cl( "DCL", "VAR(&MSGID)   TYPE(*CHAR) LEN( 7)" );

} /* zinit() */
/*--------------------------------------------------------------------------
 */
void zdone( void )
{
   put_cl( "ENDPGM", "" );

#ifdef  PC
   fclose( InFile );
   fclose( OutFile );
#endif

} /* zdone() */
/*--------------------------------------------------------------------------
 */

#ifdef  PC
FILE * open_file( char *FileName, char *Mode )
{
   FILE *File;
   char errmsg[257];

   if ((File = fopen( FileName, Mode )) == NULL)
   {
      strcpy(errmsg, "Error opening file: ");
      strcat(errmsg, FileName);
      perror(errmsg);
      exit (_doserrno);
   }
   return File;
} /* open_file */
#endif

/*--------------------------------------------------------------------------
 *  put_line: Put line to output file
 */
void put_line( char *lin )
{
#define OUT_LINE_LN 132
   char out_line[OUT_LINE_LN+1];
   char roper[8];
   char rretc[8];
   int lin_len;

#ifdef  PC
   fprintf( OutFile, "%s\n", lin );
#endif

#ifdef  AS400
   lin_len = strlen( lin );
   strcpy( out_line, lin );

/* pad out_line with blanks to OUT_LINE_LN  */
   memset( &out_line[lin_len], ' ', OUT_LINE_LN - lin_len );
   out_line[OUT_LINE_LN] = '\0';

   memcpy( roper, "WRSRC   ", 8);
   RTVCMDR( out_line, " ", roper, rretc );
#endif

} /* put_line */
/*--------------------------------------------------------------------------
 *  put_cl:
 *      Put a CL cmd
 */
void put_cl( char *cl_cmd, char *parms )
{
   char lin[OUT_STR_LN+1];
   int parms_ln;

   memset( lin, ' ', OUT_STR_LN );
   memcpy( lin+CL_CMD_OFF, cl_cmd, strlen(cl_cmd) );
   parms_ln = strlen( parms );
   memcpy( lin+CL_PRM_OFF, parms, parms_ln );
   lin[ CL_PRM_OFF+parms_ln ] = '\0';
   put_line( lin );

} /* put_cl */
/*--------------------------------------------------------------------------
 */
uchr get_chr( long off )
{
   uchr chr;

#ifdef  PC
   fseek( InFile, off, SEEK_SET);
   return getc( InFile );
#endif

#ifdef  AS400
   usrspc_spp = usrspc_bas + off;
   chr = *usrspc_spp;
   return chr;
#endif

} /* get_chr */
/*--------------------------------------------------------------------------
 */
int get_int( long off )
{
   uint u, h, l;

#ifdef  PC
   fseek( InFile, off, SEEK_SET);
   h = getc( InFile );
   l = getc( InFile );
#endif

#ifdef  AS400
   usrspc_spp = usrspc_bas + off;
   h = *usrspc_spp++;
   l = *usrspc_spp++;
#endif

   u = (h << 8) + l;
   return u;

} /* get_int */
/*--------------------------------------------------------------------------
 */
long get_long( long off )
{
   ulong uL;
   uchr  ui1, ui2, ui3;

#ifdef  PC
   fseek( InFile, off, SEEK_SET);
   uL = (ulong) getc( InFile );
   ui1 = getc( InFile );
   ui2 = getc( InFile );
   ui3 = getc( InFile );
#endif

#ifdef  AS400
   usrspc_spp = usrspc_bas + off;
   uL  = *usrspc_spp++;
   ui1 = *usrspc_spp++;
   ui2 = *usrspc_spp++;
   ui3 = *usrspc_spp++;
#endif

   uL <<= 8;
   uL += ui1;
   uL <<= 8;
   uL += ui2;
   uL <<= 8;
   uL += ui3;
   return uL;
} /* get_long */
/*--------------------------------------------------------------------------
 */
void get_str( long off, int len, char *str )
{
   int i;
   uchr ch;

#ifdef  PC
   fseek( InFile, off, SEEK_SET);
   for (i=0; i<len; i++)
   {
      ch = getc( InFile );
      *str++ = to_ASCII_tab[ch];
   }
#endif

#ifdef  AS400
   usrspc_spp = usrspc_bas + off;

   for (i=0; i<len; i++)
   {
      *str++ = *usrspc_spp++;
   }
#endif

   *str++ = '\0';
} /* get_str */
/*--------------------------------------------------------------------------
 *  get_fixed:
 *      Get the fixed portion of the CDO
 */
void get_fixed( void )
{
   CF_qinsept_index      = get_int( 0x0000 );
                           get_str( 0x0002, 10, CF_cpp_name );
                           get_str( 0x000C, 10, CF_cpp_libr );
                           get_str( 0x0008, 10, CF_vck_name );
                           get_str( 0x0012, 10, CF_vck_libr );
   CF_off_prompt         = get_int( 0x0030 );
   CF_off_interprm       = get_int( 0x006E );
   CF_1st2prompt         = get_int( 0x007D );
   CF_num_parms          = get_int( 0x007F );
} /* get_fixed */
/*--------------------------------------------------------------------------
 *  rtv_msgs:
 *      Retrieve msgd's for a chain of parm/elem/qual's
 */
void rtv_msgs( long off )
{
   long list_qual, ent_off;
   unsigned char flag_byte, type, misc1a, ch_flag;
   int off_prompt, atrib=0, ch_off;

/* loop thru chain  */
   for ( ; off>0; off=get_int(off+0x0000) )
   {
      list_qual  = get_int( off + 0x0002 );
      flag_byte  = get_chr( off + 0x0004 );
      misc1a     = get_int( off + 0x0013 );
      off_prompt = get_int( off + 0x0015 );
      type = ( flag_byte & 0xC0 ) >> 6;

   /* (don't do dummies) */
      if ( type != 3 )
      {
         rtv_msg( off_prompt+2, off_prompt+11 );
      }

   /* look thru var area for CHOICE  */

      if (misc1a & 0x08)       /* is a choice */
      {
         ent_off = off +
                  ((type == A_PARM) ? LEN_PRM_FX_AREA : LEN_OTH_FX_AREA);
         while ( atrib != 6 )
         {
            atrib = get_chr( ent_off );
            if ( atrib == 0xFF )
               break;
            if ( atrib != 6 )
            {
               ent_off += get_int( ent_off+1 );
            }
            else
            {
               ch_flag = get_chr( ent_off+3 );
               if ( ch_flag & 0x80 )            /* *PGM */
                  break;
               ch_off = get_int( ent_off+6 );
               rtv_msg( ch_off+2, ch_off+11 );
            }
         } /* while */
      } /* misc1a */

   /* if parm has elem/qual, run thru its chain */

      if ( list_qual > 0 )
         rtv_msgs( list_qual );

   } /* for off */

} /* rtv_msgs */
/*--------------------------------------------------------------------------
 *  rtv_msg:
 *      Retrieve 1 msg
 */
void rtv_msg( int msg_off, int txtln_off )
{
   int txt_len;
   char msg_id[8]="";
   char text[66]="";

   if ( msg_off > 0 && txtln_off > 0 )
   {
      get_str( msg_off, 7, msg_id );
      txt_len = get_int( txtln_off );
      if ( msg_id[0] != ' ' && txt_len > 0 )
      {
         get_str( txtln_off+2, txt_len, text );
         dbl_quotes( text );
         put_cl( "", "" );
         sprintf( WkStr, "&MSGID  %s", msg_id );
         put_cl( "CHGVAR", WkStr );
         put_cl( "ADDMSGD", "MSGID(&MSGID) MSGF(&MSGFLIB/&MSGF) MSG('Hi')" );
         put_cl( "MONMSG", "MSGID(CPF0000)" );
         put_cl( "CHGMSGD", "MSGID(&MSGID) MSGF(&MSGFLIB/&MSGF) +" );
         sprintf( WkStr, "  MSG('%s')", text );
         put_cl( "", WkStr );
      }
   }
} /* rtv_msg */
/*--------------------------------------------------------------------------
 *  dbl_quotes: Double all single quotes in in_out_str
 */
void dbl_quotes( char *in_out_str )
{
#define         LOC_STR_LN 100
   char loc_str[LOC_STR_LN+1];
   char *ip, *op;

   strncpy( loc_str, in_out_str, LOC_STR_LN );
   loc_str[LOC_STR_LN] = '\0';
   ip = loc_str;
   op = in_out_str;

   while ( (*op++ = *ip) != '\0' )
   {
      if ( *ip == '\'' )
         *op++ = *ip;
      ip++;
   }
} /* dbl_quotes */
