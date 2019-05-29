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
#include <xcdrcmdi.h>    /* V4R4  qcdrcmdi.h  */
#include <qsyrusra.h>
#pragma linkage( RTVCMDR, OS )
void RTVCMDR( char *, char *, char *, char * );
#endif

#define iserror1(f,p1)  { printf(f,p1); }
#define iserror2(f,p1,p2)  { printf(f,p1,p2); }
#define iserror3(f,p1,p2,p3)  { printf(f,p1,p2,p3); }

typedef unsigned char uchr;
typedef unsigned short ushrt;
typedef unsigned long ulong;

typedef _Packed struct API_Error {
  int       bytesProvided;
  int       bytesAvail;
  char      exceptionID[7];
  char      reserved[1];
  char      exception[128];
} API_Error_t;

#define A_PARM  0
#define A_ELEM  1
#define A_QUAL  2
#define A_DUMMY 3
#define OFF2SINGLE_AREA 0x81
#define LEN_PRM_FX_AREA 0x29
#define LEN_OTH_FX_AREA 0x1B

enum kwd_type_IDs {  TY_LABEL     =  0,
                     TY_DEC       =  1,
                     TY_INT2      =  2,
                     TY_INT4      =  3,
                     TY_CHAR      =  4,
                     TY_NAME      =  5,
                     TY_GENERIC   =  6,
                     TY_LGL       =  7,
                     TY_HEX       =  8,
                     TY_VARNAME   =  9,
                     TY_DATE      = 10,
                     TY_TIME      = 11,
                     TY_ZEROELEM  = 12,
                     TY_X         = 13,
                     TY_CMD       = 14,
                     TY_NULL      = 15,
                     TY_CMDSTR    = 16,
                     TY_SNAME     = 17,
                     TY_CNAME     = 18,
                     TY_PNAME     = 19,
                     TY_UINT2     = 20,
                     TY_UINT4     = 21  };

struct parm_ary_struct {
   short    off_to_parm;
   short    pmt_ord;
};

/*  FUNCTION PROTOTYPES  */

#ifdef PC
FILE *    open_file( char *, char * );
#endif
void      proc_args( int argc, char *argv[] );
void      zinit( void );
void      zdone( void );
void      rplc_plus( char );
void      rplc_str( char *, char * );
void      put_line( char * );
void      wr_line( void );
unsigned  char get_chr( long off );
short     get_int( long off );
long      get_long( long off );
void      get_str( long off, short, char * );
void      dbl_quotes( char * );
void      get_fixed( void );
void      load_parms( short );
void      load_ords( void );
void      add_parm( short );
int       compare(const void *, const void * );
void      rtv_crt( void );
int       rtv_crt_API( void );
void      rtv_cmd( void );
void      rtv_parm( void );
void      get_parm_no( short );
void      get_parm( void );
void      cal_kwd_type( void );
void      cal_kwd_len( void );
void      get_dft_con( char * );
void      atr_len_str( long );
void      get_spcl( short *, short, char * );
long      advance2elm( short );
int       spcl_charsP( char *str );
void      get_rel( void );
void      get_values( void );
void      put_val( long, short, short, char * );
void      put_date( long, short, char * );
void      put_strval( long, long, short, char * );
void      put_hexval( long, short, short, short, char * );
short     nonNameChar( unsigned char chIn );
short     ckOutStr( short );
short     get_hexval( char *, short, long, short, short );
void      put_pckval( long, long, char * );
void      get_choice( void );
void      get_pmtctl( void );
void      get_pmtctlpgm( void );
void      put_pmt( short );
void      put_src( char *, char *, char );
void      put_kwd( char * );
void      put_opt( char *, short, char * );
void      trim_str( char * );
void      rtv_pmtctl( void );
void      rtv_interparms( void );
void      rtv_interparm( long, short, char * );

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

#define SPCVAL 1
#define SNGVAL 2
#define PARM_OFF 13
#define KWD_OFF 24
#define WK_STR_LN 100
#define MAX_SRC_LN 132

/*  GLOBAL VARIABLES  */

#ifdef  AS400
API_Error_t API_Err;
_SYSPTR usrspc_syp = NULL;
uchr *  usrspc_spp = NULL;
uchr *  usrspc_bas = NULL;
#endif

int srcRecl;
int term_ast_slsh;
short Comment=0;
char InFileName[81] = "";
char OutFileName[81] = "";
char CmdName[11] = "";
char CmdLibr[11] = "";
char SrcFile[11] = "";
char SrcLibr[11] = "";
char SrcMbr [11] = "";
char CmdText[101] = "";
char WkStr[WK_STR_LN+1] = "";
char OutStr[MAX_SRC_LN+1] = "";
char Lin_buf[MAX_SRC_LN+1] = "";
short Lin_len=0;
char Kwd_type[16] = "";
short Num_quals=0, label_sw=0, IsLabel;
short Parm_off;
long Lo_pmtctl_off=0;
char Rtv_prompts = 'M';      /* M = rtv msg id's for prompts  */
                             /* T = rtv text for prompts      */

char *Rels[]={"*EXIST","*GT","*EQ","*GE","*LT","*NE","*LE","TRUE"};

/* sorted array of off's to parm/elem/qual's  */
#define SZ_parm_offs 1000
short parm_offs[ SZ_parm_offs ];
short n_offs=0;

/* sorted array of parm off's/pmt ord#'s  */
#define SZ_parm_ary 200
struct parm_ary_struct parm_ary[ SZ_parm_ary ];
short n_parms=0;
short is_out_o_ord=0;  /* 1 if parm pmt order is diff from physical order */

/*  CDO fixed portion  */

   short CF_qinsept_index;    /* 1-rel index of cpp in QINSEPT             */
   char  CF_cpp_name[11];     /* cpp name                                  */
   char  CF_cpp_libr[11];     /* cpp library                               */
   char  CF_vck_name[11];     /* validity checker name                     */
   char  CF_vck_libr[11];     /* validity checker library                  */
   short CF_mode;             /* valid mode                                */
   short CF_allow;            /* where allowed                             */
   short CF_off_prompt;       /* off to prompt info                        */
   short CF_off_pmtfile;      /* off to pmtfile                            */
   char  CF_dep_msgf[11];     /* DEP msgf (MSGF parm)                      */
   char  CF_dep_msgl[11];     /* DEP msgf lib                              */
   short CF_maxpos;           /* max positional parms                      */
   char  CF_curlib[11];       /* CURLIB parm                               */
   char  CF_prdlib[11];       /* PRDLIB parm                               */
   short CF_off_hlppnl;       /* off to hlppnlgrp                          */
   short CF_off_interprm;     /* off to interparms test area               */
   short CF_1st2prompt;       /* 1st parm to prompt for                    */
   short CF_num_parms;        /* number of command keywords                */

/*  Single parm fixed area  */

   short  PF_parm_no;
   short  PF_type;
   short  IsParm, IsElem, IsQual, IsDummy, HasElem, HasQual;

   short PF_off_next_kwd;     /* offset to next parm                       */
   short PF_off_listqual;     /* offset to list/qual                       */
   char  PF_flag_byte;        /* flag byte                                 */
   char  PF_var_pres;         /* var field presence                        */
   char  PF_val_type;         /* type of data value                        */
   short PF_val_length;       /* length of value                           */
   char  PF_typex_frac_len;   /* frac digs for type(*X)                    */
   char  PF_typex_total_len;  /* dec length for type(*X)                   */
   short PF_min;              /* min number required                       */
   short PF_max;              /* max number allowed                        */
   short PF_nest_level;       /* list leg nest level                       */
   char  PF_misc1;            /* miscellaneous bits                        */
   char  PF_misc1a;           /* miscellaneous bits                        */
   char  PF_misc2;            /* miscellaneous bits                        */
   short PF_off_prompt;       /* offset to prompt text                     */
   char  PF_bits_1A;          /* miscellaneous bits (off 1A)               */
   char  PF_kwd_name[11];     /* keyword name                              */
   short PF_kwd_num;          /* keyword number                            */
   short PF_off_next2prompt;  /* off to next node to be prompted for       */

   static char *TypeEnts[] = { "PARM","ELEM","QUAL","DUMMY" };
   static char *FileUs[] = { "*NO","*IN","*OUT","*INOUT","*UPD",
                             "","","","*UNSPFD" };
#define NUM_KWD_TYPES (sizeof kwd_types / sizeof kwd_types[0] )
   static char *kwd_types[] = {  "*LABEL",
                                 "*DEC",
                                 "*INT2",
                                 "*INT4",
                                 "*CHAR",
                                 "*NAME",
                                 "*GENERIC",
                                 "*LGL",
                                 "*HEX",
                                 "*VARNAME",
                                 "*DATE",
                                 "*TIME",
                                 "*ZEROELEM",
                                 "*X",
                                 "*CMD",
                                 "*NULL",
                                 "*CMDSTR",
                                 "*SNAME",
                                 "*CNAME",
                                 "*PNAME",
                                 "*UINT2",
                                 "*UINT4"     };

/*--------------------------------------------------------------------------
 */
main( int argc, char **argv )
{
   short i;

   zinit();
   proc_args( argc, argv );

#ifdef  PC
   InFile = open_file( InFileName, "rb" );
   OutFile = open_file( OutFileName, "w" );
#endif

#ifdef  AS400
   usrspc_syp = rslvsp( _Usrspc, "RTVCMDSPC", "QTEMP", _AUTH_NONE );
   usrspc_bas = setsppfp( usrspc_syp );
   usrspc_spp = usrspc_bas;
#endif

   get_fixed();
   rtv_crt();
   rtv_cmd();

/* if there's room for at least 1 parm/elem/qual... */
   if ( CF_off_hlppnl >= OFF2SINGLE_AREA + LEN_OTH_FX_AREA )
   {

   /* load & sort array of parm/elem/qual's */
      load_parms( OFF2SINGLE_AREA );

   /* load & sort array of parm prompt order #'s */
      load_ords();

   /* retrieve source for all parm/elem/qual's */
      PF_parm_no = 0;

      /* loop thru array  */
      for (i=0; i < n_offs ; i++ )
      {
         Parm_off = parm_offs[i];
         rtv_parm();
      }

   /* retrieve source for all pmtctl's */
      if ( Lo_pmtctl_off )
         rtv_pmtctl();

   /* retrieve source for interparm desc's  */
      rtv_interparms();

   } /* CF_off_hlppnl */

   zdone();

} /* main */
/*--------------------------------------------------------------------------
 */
void proc_args( int argc, char *argv[] )
{
   short i;
   char dash, swtch0, swtch1;

   for ( i=1; i < argc; i++ )
   {
      dash = argv[i][0];

   /*  if it's a switch... */
      if ( dash == '-' || dash == '/')
      {
         swtch0 = toupper( argv[i][1] );
         swtch1 = toupper( argv[i][2] );

         switch ( swtch0 )
         {
            case 'N':
               strncpy(CmdName,argv[i+1],10);
               break;
            case 'L':
               strncpy(CmdLibr,argv[i+1],10);
               break;
            case 'S':
               switch ( swtch1 )
               {
                  case 'F':
                     strncpy(SrcFile,argv[i+1],10);
                     break;
                  case 'L':
                     strncpy(SrcLibr,argv[i+1],10);
                     break;
                  case 'M':
                     strncpy(SrcMbr,argv[i+1],10);
                     break;
               }
               break;
            case 'X':
               strncpy(CmdText,argv[i+1],50);
               CmdText[50] = '\0';
               break;
            case 'I':
               strcpy(InFileName,argv[i+1]);
               break;
            case 'O':
               strcpy(OutFileName,argv[i+1]);
               break;
            case 'M':
               Rtv_prompts = 'M';            /* msg id's */
               break;
            case 'T':
               Rtv_prompts = 'T';            /* text */
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
   char zrecl[6] = "00000";

#ifdef  PC
 /* put 00 in 1st position of ASCII table  */
   to_ASCII_tab[0] = '\0';
   srcRecl = MAX_SRC_LN;
#endif

#ifdef  AS400
   memcpy( roper, "OPNSRC  ", 8);
   RTVCMDR( " ", " ", roper, rretc );
   memcpy(zrecl, rretc, 5);
   srcRecl = atoi(zrecl) - 12;
#endif

   term_ast_slsh = srcRecl - 3;
   WkStr[WK_STR_LN] = '\0';
   Lin_buf[srcRecl] = '\0';
   OutStr[srcRecl] = '\0';
   strcpy(CmdName,"THECOMMAND");
   strcpy(CmdLibr,"THELIBRARY");
   strcpy(SrcFile,"QCMDSRC");
   strcpy(SrcLibr,"*LIBL");
   strcpy(SrcMbr ,"*CMD");
   strcpy(CmdText ,"*SRCMBRTXT");
   SrcFile[10] = '\0';
   SrcLibr[10] = '\0';
   SrcMbr [10] = '\0';
   CmdText[100] = '\0';
   CF_cpp_libr[10] = '\0';
   CF_cpp_name[10] = '\0';
} /* zinit() */
/*--------------------------------------------------------------------------
 */
void zdone( void )
{
   /* flush Lin_buf  */

   if (Lin_len > 0)
      wr_line();

#ifdef  PC
   fclose( InFile );
   fclose( OutFile );
#endif

} /* zdone() */

#ifdef  PC
/*--------------------------------------------------------------------------
 */
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
 *  rplc_plus: Replace final '+' in Lin_buf, if any
 */
void rplc_plus( char rpl_char )
{
   if ( Lin_buf[Lin_len-1] == '+' )
      Lin_buf[Lin_len-1] = rpl_char;

} /* rplc_plus */
/*--------------------------------------------------------------------------
 *  rplc_str: Replace a substring in Lin_buf
 */
void rplc_str( char *str, char *rpl_str )
{
   char *p;

   if ( (p=strstr(Lin_buf,str)) != NULL )
      memcpy( p, rpl_str, strlen(rpl_str) );

} /* rplc_str */
/*--------------------------------------------------------------------------
 *  put_line: Put line to output file, buffering 1 line;
 */
void put_line( char *lin )
{
   int len2go, thisLen;
   char *lP, *lastP;

   len2go = strlen( lin );
   for (lastP=lin+len2go-1; *lastP == ' '; lastP--);
   lP = lin;

   /*  Write line in srcRecl pieces,
    *  with continuation char '-'
    */
   while (len2go > 0) {
      if (Lin_len > 0)
         wr_line();
      thisLen = len2go;
      strncpy( Lin_buf, lP, srcRecl );

      if ( thisLen > srcRecl && lP <= lastP)
      {
         thisLen = srcRecl-1;
         Lin_buf[thisLen] = '-';
      }
      if ( thisLen > srcRecl)
         thisLen = srcRecl;
      Lin_len  = thisLen;
      len2go  -= thisLen;
      lP      += thisLen;
   }
   /* if Comment, wrap in delimiters  */

   if ( Comment )
   {
   /* pad w/blanks  */
      memset( Lin_buf + Lin_len, ' ', srcRecl - Lin_len );
      Lin_len = srcRecl;
      memcpy( Lin_buf, "/*", 2 );
      memcpy( Lin_buf + term_ast_slsh, "*/", 2 );
   }

} /* put_line */
/*--------------------------------------------------------------------------
 *  wr_line: Write Lin_buf to output file
 */
void wr_line( void )
{
#define OUT_LINE_LN 132
   char out_line[OUT_LINE_LN+1];
   char roper[8];
   char rretc[8];
   short buf_ln;
   short pad_ln;

#ifdef  PC
   fprintf( OutFile, "%s\n", Lin_buf );
#endif

#ifdef  AS400
   buf_ln = strlen( Lin_buf );
   strcpy( out_line, Lin_buf );

   /* pad out_line with blanks to OUT_LINE_LN  */

   pad_ln = OUT_LINE_LN - buf_ln;
   if (pad_ln > 0) {
     memset( &out_line[buf_ln], ' ', pad_ln );
   }
   out_line[OUT_LINE_LN] = '\0';

   memcpy( roper, "WRSRC   ", 8);
   RTVCMDR( out_line, " ", roper, rretc );

#endif
} /* wr_line */
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
short get_int( long off )
{
   ushrt u, h, l;

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
   uL = (unsigned long) getc( InFile );
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
void get_str( long off, short len, char *str )
{
   short i;
   unsigned char ch;

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
 *  dbl_quotes: Double all single quotes in in_out_str
 */
void dbl_quotes( char *in_out_str )
{
   char loc_str[WK_STR_LN+1];
   char *ip, *op;

   strncpy( loc_str, in_out_str, WK_STR_LN );
   loc_str[WK_STR_LN] = '\0';
   ip = loc_str;
   op = in_out_str;

   while ( (*op++ = *ip) != '\0' )
   {
      if ( *ip == '\'' )
         *op++ = *ip;
      ip++;
   }
} /* dbl_quotes */
/*--------------------------------------------------------------------------
 *  get_fixed:
 *      Get the fixed portion of the CDO
 */
void get_fixed( void )
{
   CF_qinsept_index     = get_int( 0x0000 );
                          get_str( 0x0002, 10, CF_cpp_name );
                          get_str( 0x000C, 10, CF_cpp_libr );
                          get_str( 0x0018, 10, CF_vck_name );
                          get_str( 0x0022, 10, CF_vck_libr );
   CF_mode              = get_chr( 0x002D );
   CF_allow             = get_int( 0x002E );
   CF_off_prompt        = get_int( 0x0030 );
   CF_off_pmtfile       = get_int( 0x0032 );
                          get_str( 0x0034, 10, CF_dep_msgf );
                          get_str( 0x003E, 10, CF_dep_msgl );
   CF_maxpos            = get_int( 0x0048 );
                          get_str( 0x004A, 10, CF_curlib   );
                          get_str( 0x0054, 10, CF_prdlib   );
   CF_off_hlppnl        = get_int( 0x0065 );
   CF_off_interprm      = get_int( 0x006E );
   CF_1st2prompt        = get_int( 0x007D );
   CF_num_parms         = get_int( 0x007F );
} /* get_fixed */
/*--------------------------------------------------------------------------
 *  load_parms:
 *      Load a chain of parm/elem/qual off's to array
 */
void load_parms( short off )
{
   short list_qual, type;
   unsigned char flag_byte;

/* loop thru chain  */
   while ( off > 0 )
   {
      flag_byte = get_chr( (long) off + 0x0004 );
      type = (flag_byte & 0xC0) >> 6;
   /* (don't add dummies) */
      if ( type != 3 )
         add_parm( off );
      list_qual = get_int( (long) off + 0x0002 );
      off = get_int( (long) off + 0x0000 );

   /* if parm has elem/qual, run thru its chain */
      if ( list_qual > 0 )
         load_parms( list_qual );
   }
} /* load_parms */
/*--------------------------------------------------------------------------
 *  load_ords:
 *      Load parm_ary array in prompting order, then sort in offset order
 */
void load_ords( void )
{
   short off, last_off=0, ord=1;

   off = CF_1st2prompt;

/* loop thru parms in prompt order */

   while ( off > 0 )
   {
      PF_off_next2prompt = get_int( off + 0x0027 );
      parm_ary[n_parms  ].off_to_parm = off;
      parm_ary[n_parms++].pmt_ord = ord++;
      if ( last_off > off )
         is_out_o_ord=1;          /* prompts are out of order */
      last_off = off;
      off = PF_off_next2prompt;
   }

   if ( is_out_o_ord )
      qsort( parm_ary, n_parms, sizeof(parm_ary[0]), &compare );

} /* load_ords */
/*--------------------------------------------------------------------------
 *  add_parm:
 *      Add offset to parm/elem/qual into sorted array, if not already in it.
 */
void add_parm( short off )
{
   if ( n_offs < SZ_parm_offs )
   {
      if ( bsearch(  &off,
                     parm_offs,
                     n_offs,
                     sizeof(parm_offs[0]),
                     compare ) == NULL )
      {
         parm_offs[n_offs++] = off;
         qsort( parm_offs, n_offs, sizeof(parm_offs[0]), compare );
      }
   }
} /* add_parm */
/*--------------------------------------------------------------------------
 *  compare:
 *      Compare 2 short int's for add_parm
 */
int compare( const void *arg1, const void *arg2 )
{
   return *(short*)arg1 - *(short*)arg2;
}
/*--------------------------------------------------------------------------
 *  rtv_crt
 *      Generate a CRTCMD
 */
void rtv_crt( void )
{
   short text_fits, text_len;
   char *term;
   char pmtfile[11];
   char pmtlibr[11];
   char hlppnlgrp[11];
   char hlppnllib[11];
   char hlpid[11];
   char hlpschidx [11];
   char hlpschidxl[11];
   long  off_rexx=0, off_rexx_ab=0, rexx_ex_off=0, rexx_ex_off_ab=0;
   short num_rexx_ex=0, rexx_exit_cd=0;
   char rexx_srcf [11];
   char rexx_srcl [11];
   char rexx_srcm [11];
   char rexx_env  [11];
   char rexx_envl [11];
   char rexx_exit [11];
   char rexx_exitl[11];

   /*  Try using API  */

#ifdef  AS400
   if (0==rtv_crt_API())
   {
      return;
   }
#endif
   Comment = 1;

   memset( OutStr, ' ', srcRecl );
   memcpy( OutStr+PARM_OFF, "CRTCMD", 6 );
   trim_str( CmdLibr );
   sprintf( WkStr, "%s/%s", CmdLibr, CmdName );
   put_src( "CMD", WkStr, '+' );
   trim_str( CF_cpp_libr );
   trim_str( CF_cpp_name );
   if ( strlen(CF_cpp_libr) == 0 )
   {
      put_src( "PGM", CF_cpp_name, '+' );
   }
   else
   {
      sprintf( WkStr, "%s/%s", CF_cpp_libr, CF_cpp_name );
      put_src( "PGM", WkStr, '+' );
   }
   trim_str( SrcLibr );
   sprintf( WkStr, "%s/%s", SrcLibr, SrcFile );
   put_src( "SRCFILE", WkStr, '+' );
   put_src( "SRCMBR", SrcMbr, '+' );

   if ( strcmp(CF_cpp_name,"*REXX") == 0 && CF_off_hlppnl )
   {
      off_rexx = get_int( CF_off_hlppnl );
      if ( off_rexx )
      {
         off_rexx_ab = CF_off_hlppnl + off_rexx;
         rexx_ex_off = get_int( off_rexx_ab );
         rexx_ex_off_ab = off_rexx_ab + rexx_ex_off;
         get_str( off_rexx_ab+ 2, 10, rexx_srcf );
         get_str( off_rexx_ab+12, 10, rexx_srcl );
         get_str( off_rexx_ab+22, 10, rexx_srcm );
         get_str( off_rexx_ab+32, 10, rexx_env  );
         get_str( off_rexx_ab+42, 10, rexx_envl );
         trim_str( rexx_srcf );
         trim_str( rexx_srcl );
         trim_str( rexx_srcm );
         trim_str( rexx_env  );
         trim_str( rexx_envl );
         sprintf( WkStr, "%s/%s", rexx_srcl, rexx_srcf );
         put_src( "REXSRCFILE", WkStr, '+' );
         put_src( "REXSRCMBR", rexx_srcm, '+' );
         if ( strlen(rexx_envl) == 0 )
         {
            put_src( "REXCMDENV", rexx_env, '+' );
         }
         else
         {
            sprintf( WkStr, "%s/%s", rexx_envl, rexx_env );
            put_src( "REXCMDENV", WkStr, '+' );
         }
         if ( rexx_ex_off )
         {
            short ri;
            long re_off;

            num_rexx_ex = get_int( rexx_ex_off_ab+4 );
            re_off = rexx_ex_off_ab+6;
            for (ri=0; ri < num_rexx_ex; ri++)
            {
               if ( ri == 0 )
                  put_kwd( "REXEXITPGM" );
               get_str( re_off   , 10, rexx_exit  );
               get_str( re_off+10, 10, rexx_exitl );
               rexx_exit_cd = get_int( re_off+20 );
               trim_str( rexx_exit );
               trim_str( rexx_exitl );
               term = (ri < num_rexx_ex-1) ? " +" : ") +";
               sprintf( &OutStr[KWD_OFF+2], "(%s/%s %d)%s",
                               rexx_exitl, rexx_exit, rexx_exit_cd, term );
               put_line( OutStr );
               re_off += 22;
            } /* ri */
         } /* rexx_ex_off */
      } /* off_rexx */
   } /* *REXX */

   text_fits = (term_ast_slsh - KWD_OFF) - 11;
   trim_str( CmdText );
   dbl_quotes( CmdText );
   text_len = strlen(CmdText);
   if ( text_len <= 0 )
   {
      put_src( "TEXT", "*BLANK", '+' );
   }
   else
   {
      if ( text_len <= text_fits )
      {
         sprintf( WkStr, "'%s'", CmdText );
         put_src( "TEXT", WkStr, '+' );
      }
      else
      {
         strncpy( WkStr, CmdText, text_fits );
         WkStr[text_fits] = '\0';
         memset( OutStr, ' ', KWD_OFF );
         sprintf( &OutStr[KWD_OFF], "TEXT('%s+", WkStr );
         put_line( OutStr );
         sprintf( &OutStr[KWD_OFF], "%s') +", &CmdText[text_fits] );
         put_line( OutStr );
      }
   }

   put_kwd( "MODE" );
   if ( (CF_mode & 0xE0) == 0xE0 )
   {
         put_opt( "*ALL", KWD_OFF+2, ") +" );
   }
   else
   {
      if (CF_mode & 0x80)
      {
         put_opt( "*PROD", KWD_OFF+2, " +" );
      }
      if (CF_mode & 0x40)
      {
         put_opt( "*DEBUG", KWD_OFF+2, " +" );
      }
      if (CF_mode & 0x20)
      {
         put_opt( "*SERVICE", KWD_OFF+2, " +");
      }
      rplc_str( " + ", ") +" );
   }

   if ( !(CF_allow & 0x8000) )
   {
      put_kwd( "TYPE" );
      if ( CF_allow == 1 )
         put_opt( "*CDS", KWD_OFF+2, ") +" );
      if ( CF_allow == 2 )
         put_opt( "*CMDPMT", KWD_OFF+2, ") +" );
   }

   if ( CF_allow & 0x8000 )
   {
      put_kwd( "ALLOW" );
      if ( CF_allow & 0x007F == 0x007F )
      {
         put_opt( "*ALL", KWD_OFF+2, ") +" );
      }
      else
      {
         if (CF_allow & 0x0040)
         {
            put_opt( "*IREXX", KWD_OFF+2, " +" );
         }
         if (CF_allow & 0x0020)
         {
            put_opt( "*BREXX", KWD_OFF+2, " +" );
         }
         if (CF_allow & 0x0010)
         {
            put_opt( "*BPGM", KWD_OFF+2, " +" );
         }
         if (CF_allow & 0x0008)
         {
            put_opt( "*IPGM", KWD_OFF+2, " +" );
         }
         if (CF_allow & 0x0004)
         {
            put_opt( "*EXEC", KWD_OFF+2, " +" );
         }
         if (CF_allow & 0x0002)
         {
            put_opt( "*INTERACT", KWD_OFF+2, " +" );
         }
         if (CF_allow & 0x0001)
         {
            put_opt( "*BATCH", KWD_OFF+2, " +" );
         }
         rplc_str( " + ", ") +" );
      } /*  0x007F  */
   } /*  0x8000  */

   if ( CF_maxpos == 0 )
   {
      put_src( "MAXPOS", "*NOMAX", '+' );
   }
   else
   {
      sprintf( WkStr, "%d", CF_maxpos );
      put_src( "MAXPOS", WkStr, '+' );
   }

   get_str( CF_off_pmtfile, 10, pmtfile );
   trim_str( pmtfile );
   if ( strcmp(pmtfile,"*NONE") == 0 )
   {
      put_src( "PMTFILE", "*NONE", '+' );
   }
   else
   {
      get_str( CF_off_pmtfile+10, 10, pmtlibr );
      trim_str( pmtlibr );
      sprintf( WkStr, "%s/%s", pmtlibr, pmtfile );
      put_src( "PMTFILE", WkStr, '+' );
   }

   if ( CF_off_hlppnl )
   {
      get_str( CF_off_hlppnl+ 76, 10, hlpschidx );
      get_str( CF_off_hlppnl+ 86, 10, hlpschidxl);
      trim_str( hlpschidx );
      trim_str( hlpschidxl );

      if ( strlen(hlpschidx) > 0 )
      {
         if ( strlen(hlpschidxl) == 0 )
         {
            put_src( "HLPSCHIDX", hlpschidx, '+' );
         }
         else
         {
            sprintf( WkStr, "%s/%s", hlpschidxl, hlpschidx );
            put_src( "HLPSCHIDX", WkStr, '+' );
         }
      }

      get_str( CF_off_hlppnl+  2, 10, hlppnlgrp );
      trim_str( hlppnlgrp );
      if ( strcmp(hlppnlgrp,"*NONE") == 0 )
      {
         put_src( "HLPPNLGRP", "*NONE", '+' );
      }
      else
      {
         get_str( CF_off_hlppnl+12, 10, hlppnllib );
         trim_str( hlppnllib );
         sprintf( WkStr, "%s/%s", hlppnllib, hlppnlgrp );
         put_src( "HLPPNLGRP", WkStr, '+' );
      }
      get_str( CF_off_hlppnl+22, 10, hlpid );
      put_src( "HLPID", hlpid, ' ' );
   } /* CF_off_hlppnl */

   Comment = 0;
   put_line( " " );

   return;
} /* rtv_crt */
/*--------------------------------------------------------------------------
 *  rtv_crt_API:
 *      Retrieve source for CMD stmt using QCDRCMDI API
 *      If successful, return 0.
 */
#ifdef  AS400

int  rtv_crt_API( void )
{
   int                      rcv_len;
   int                      bytes_ret;
   int                      Exit_Ents_len;
   int                      ri;
   int                      max_pos_parms;
   int                      ccsid;
   int                      num_rexx_ex;
   int                      rexx_exit_cd;
   short                    text_fits;
   short                    text_len;
   char                     enb_gui;
   char                     threadsafe;
   char                     mlt_thd_acn;
   char                     wkParm[64];
   char                     qual_cmd[20];
   char                     cmdi_fmt[] = "CMDI0200";
   char                     usra_fmt[] = "USRA0100";
   char                     user_pub[] = "*PUBLIC   ";
   char                     type_cmd[] = "*CMD      ";
   char                    *term;
   Qcd_Exit_Entries_t      *Exit_Ents_P;
   Qcd_Exit_Entries_t      *Exit_Ent_P;
   Qcd_Help_Book_Info_t    *Help_Book_P;
   char                    *Bookshelf_P;
   char                     pub_aut   [11] = "          ";
   char                     cpp_name  [11] = "          ";
   char                     cpp_libr  [11] = "          ";
   char                     vld_ckr   [11] = "          ";
   char                     vld_ckrl  [11] = "          ";
   char                     alw_lmt   [ 2] = " ";
   char                     mode_info [11] = "          ";
   char                     where_run [16] = "               ";
   char                     pmt_msgf  [11] = "          ";
   char                     pmt_msgfl [11] = "          ";
   char                     msgf      [11] = "          ";
   char                     msgfl     [11] = "          ";
   char                     hlp_pnl   [11] = "          ";
   char                     hlp_pnll  [11] = "          ";
   char                     hlp_id    [11] = "          ";
   char                     hlp_idx   [11] = "          ";
   char                     hlp_idxl  [11] = "          ";
   char                     cur_lib   [11] = "          ";
   char                     prd_lib   [11] = "          ";
   char                     pmt_ovr   [11] = "          ";
   char                     pmt_ovrl  [11] = "          ";
   char                     rst_rel   [ 7] = "       ";
   char                     cpp_state [ 3] = "  ";
   char                     vld_state [ 3] = "  ";
   char                     pmt_state [ 3] = "  ";

   char                     rexx_srcf [11] = "          ";
   char                     rexx_srcl [11] = "          ";
   char                     rexx_srcm [11] = "          ";
   char                     rexx_env  [11] = "          ";
   char                     rexx_envl [11] = "          ";
   char                     rexx_exit [11] = "          ";
   char                     rexx_exitl[11] = "          ";
   char                     help_shelf[ 9] = "        ";
   Qsy_RUSRA_USRA0100_t     usra0100;

#define MAX_EXIT_ENTS   16
#define EXIT_ENTS_SIZE  MAX_EXIT_ENTS * sizeof(Qcd_Exit_Entries_t)

   _Packed struct {
      Qcd_CMDI0200_t        cmdi0200;
      char                  Exit_Ents[EXIT_ENTS_SIZE];
      char                  Help_Book[sizeof(Qcd_Help_Book_Info_t)];
   } rcv;

   Comment = 1;
   rcv_len = sizeof(rcv);
   memset(&rcv, 0, sizeof(rcv));
   memcpy(&qual_cmd[ 0], CmdName, 10);
   memcpy(&qual_cmd[10], CmdLibr, 10);
   API_Err.bytesProvided = sizeof(API_Err);
   API_Err.bytesAvail = 0;

   QCDRCMDI(&rcv, rcv_len, cmdi_fmt, qual_cmd, &API_Err);

   if (API_Err.bytesAvail > 0)
   {
      return -1;
   }
   bytes_ret = rcv.cmdi0200.cmdi0100.Bytes_Returned;
   num_rexx_ex = rcv.cmdi0200.Num_Exit_Entries;
   Exit_Ents_len = num_rexx_ex * rcv.cmdi0200.Length_Exit_Entries;
   Exit_Ents_P = (Qcd_Exit_Entries_t *) rcv.Exit_Ents;
   Help_Book_P = (Qcd_Help_Book_Info_t *)
                 (((char *)&rcv) + rcv.cmdi0200.cmdi0100.Offset_Help_Book);

   /* Inexplicably, Help_Bookshelf in qcdrcmdi.h starts at the
    * second character, after the '*'.
    */
   Bookshelf_P = ((char *)&Help_Book_P->Help_Bookshelf) - 1;
   memcpy( help_shelf, Bookshelf_P, 8 );

   max_pos_parms = rcv.cmdi0200.cmdi0100.Max_Parameters;
   ccsid         = rcv.cmdi0200.cmdi0100.CCSID;
   enb_gui       = rcv.cmdi0200.cmdi0100.Enable_GUI;
   threadsafe    = rcv.cmdi0200.cmdi0100.Threadsafe;
   mlt_thd_acn   = rcv.cmdi0200.cmdi0100.Multithreaded_Job_Action;
   memcpy( cpp_name,   rcv.cmdi0200.cmdi0100.Cmd_Proc_Name, 10);
   memcpy( cpp_libr,   rcv.cmdi0200.cmdi0100.Cmd_Proc_Lib_Name, 10);
   memcpy( vld_ckr,    rcv.cmdi0200.cmdi0100.Val_Name, 10);
   memcpy( vld_ckrl,   rcv.cmdi0200.cmdi0100.Val_Lib_Name, 10);
   memcpy( mode_info,  rcv.cmdi0200.cmdi0100.Mode_Info, 10);
   memcpy( where_run,  rcv.cmdi0200.cmdi0100.Where_Run, 15);
   memcpy( alw_lmt,    rcv.cmdi0200.cmdi0100.Allow_Limited, 1);
   memcpy( pmt_msgf,   rcv.cmdi0200.cmdi0100.Prompt_Name, 10);
   memcpy( pmt_msgfl,  rcv.cmdi0200.cmdi0100.Prompt_Lib_Name, 10);
   memcpy( msgf,       rcv.cmdi0200.cmdi0100.Message_Name, 10);
   memcpy( msgfl,      rcv.cmdi0200.cmdi0100.Message_Lib_Name, 10);
   memcpy( hlp_pnl,    rcv.cmdi0200.cmdi0100.Help_Panel_Name, 10);
   memcpy( hlp_pnll,   rcv.cmdi0200.cmdi0100.Help_Panel_Lib_Name, 10);
   memcpy( hlp_id,     rcv.cmdi0200.cmdi0100.Help_Id, 10);
   memcpy( hlp_idx,    rcv.cmdi0200.cmdi0100.Search_Name, 10);
   memcpy( hlp_idxl,   rcv.cmdi0200.cmdi0100.Search_Lib_Name, 10);
   memcpy( cur_lib,    rcv.cmdi0200.cmdi0100.Current_Lib, 10);
   memcpy( prd_lib,    rcv.cmdi0200.cmdi0100.Product_Lib, 10);
   memcpy( pmt_ovr,    rcv.cmdi0200.cmdi0100.P_Override_Name, 10);
   memcpy( pmt_ovrl,   rcv.cmdi0200.cmdi0100.P_Override_Lib_Name, 10);
   memcpy( rst_rel,    rcv.cmdi0200.cmdi0100.Restricted_Release, 6);
   memcpy( cpp_state,  rcv.cmdi0200.cmdi0100.Call_State, 2);
   memcpy( vld_state,  rcv.cmdi0200.cmdi0100.Val_Check_Call_State, 2);
   memcpy( pmt_state,  rcv.cmdi0200.cmdi0100.P_Override_Call_State, 2);
   memcpy( rexx_srcf,  rcv.cmdi0200.Source_Name, 10);
   memcpy( rexx_srcl,  rcv.cmdi0200.Source_Lib_Name, 10);
   memcpy( rexx_srcm,  rcv.cmdi0200.Source_Member_Name, 10);
   memcpy( rexx_env,   rcv.cmdi0200.Env_Name, 10);
   memcpy( rexx_envl,  rcv.cmdi0200.Env_Lib_Name, 10);

   memset( OutStr, ' ', srcRecl );
   memcpy( OutStr+PARM_OFF, "CRTCMD", 6 );
   trim_str( CmdLibr );
   sprintf( WkStr, "%s/%s", CmdLibr, CmdName );
   put_src( "CMD", WkStr, '+' );
   trim_str( cpp_libr );
   trim_str( cpp_name );
   sprintf( WkStr, "%s/%s", cpp_libr, cpp_name );
   put_src( "PGM", WkStr, '+' );
   trim_str( SrcLibr );
   sprintf( WkStr, "%s/%s", SrcLibr, SrcFile );
   put_src( "SRCFILE", WkStr, '+' );
   put_src( "SRCMBR", SrcMbr, '+' );

   if ( 0==memcmp(cpp_name,"*REXX", 5) )
   {
      trim_str( rexx_srcf );
      trim_str( rexx_srcl );
      trim_str( rexx_srcm );
      trim_str( rexx_env  );
      trim_str( rexx_envl );
      sprintf( WkStr, "%s/%s", rexx_srcl, rexx_srcf );
      put_src( "REXSRCFILE", WkStr, '+' );
      put_src( "REXSRCMBR", rexx_srcm, '+' );
      if ( strlen(rexx_envl) == 0 )
      {
         put_src( "REXCMDENV", rexx_env, '+' );
      }
      else
      {
         sprintf( WkStr, "%s/%s", rexx_envl, rexx_env );
         put_src( "REXCMDENV", WkStr, '+' );
      }
      if ( num_rexx_ex > 0 )
      {
         Exit_Ent_P = Exit_Ents_P;

         for (ri=0; ri < num_rexx_ex; ri++)
         {
            if ( ri == 0 )
               put_kwd( "REXEXITPGM" );
            memcpy(rexx_exit, Exit_Ent_P->Exit_Name, 10);
            memcpy(rexx_exitl, Exit_Ent_P->Exit_Lib_Name, 10);
            rexx_exit_cd = Exit_Ent_P->Exit_Code;
            trim_str( rexx_exit );
            trim_str( rexx_exitl );
            term = (ri < num_rexx_ex-1) ? " +" : ") +";
            sprintf( &OutStr[KWD_OFF+2], "(%s/%s %d)%s",
                            rexx_exitl, rexx_exit, rexx_exit_cd, term );
            put_line( OutStr );
            Exit_Ent_P++;
         }
      } /* num_rexx_ex */
   } /* *REXX */

   if (bytes_ret >= 334)
   {
      if ( threadsafe == '0' )
         put_src( "THDSAFE", "*NO", '+' );
      if ( threadsafe == '1' )
         put_src( "THDSAFE", "*YES", '+' );
      if ( threadsafe == '2' )
         put_src( "THDSAFE", "*COND", '+' );
   }

   if (bytes_ret >= 335)
   {
      if ( mlt_thd_acn == '0' )
         put_src( "MLTTHDACN", "*SYSVAL", '+' );
      if ( mlt_thd_acn == '1' )
         put_src( "MLTTHDACN", "*RUN", '+' );
      if ( mlt_thd_acn == '2' )
         put_src( "MLTTHDACN", "*MSG", '+' );
      if ( mlt_thd_acn == '3' )
         put_src( "MLTTHDACN", "*NORUN", '+' );
   }

   text_fits = (term_ast_slsh - KWD_OFF) - 11;
   trim_str( CmdText );
   dbl_quotes( CmdText );
   text_len = strlen(CmdText);

   if ( text_len <= 0 )
   {
      put_src( "TEXT", "*BLANK", '+' );
   }
   else
   {
      if ( text_len <= text_fits )
      {
         sprintf( WkStr, "'%s'", CmdText );
         put_src( "TEXT", WkStr, '+' );
      }
      else
      {
         strncpy( WkStr, CmdText, text_fits );
         WkStr[text_fits] = '\0';
         memset( OutStr, ' ', KWD_OFF );
         sprintf( &OutStr[KWD_OFF], "TEXT('%s+", WkStr );
         put_line( OutStr );
         sprintf( &OutStr[KWD_OFF], "%s') +", &CmdText[text_fits] );
         put_line( OutStr );
      }
   } /* text_len <= 0 */

   trim_str( vld_ckr );
   if ( vld_ckrl[0] == ' ' )
   {
      put_src( "VLDCKR", vld_ckr, '+' );
   }
   else
   {
      trim_str( vld_ckrl );
      sprintf( WkStr, "%s/%s", vld_ckrl, vld_ckr );
      put_src( "VLDCKR", WkStr, '+' );
   }

   put_kwd( "MODE" );
   if ( 0==memcmp(mode_info, "111", 3) )
      put_opt( "*ALL", KWD_OFF+2, ") +" );
   else
   {
      if (mode_info[0] == '1')
         put_opt( "*PROD", KWD_OFF+2, " +" );
      if (mode_info[1] == '1')
         put_opt( "*DEBUG", KWD_OFF+2, " +" );
      if (mode_info[2] == '1')
         put_opt( "*SERVICE", KWD_OFF+2, " +");
      rplc_str( " + ", ") +" );
   }

   put_kwd( "ALLOW" );
   if ( 0==memcmp(where_run, "111111111", 9) )
   {
      put_opt( "*ALL", KWD_OFF+2, ") +" );
   }
   else
   {
      if (where_run[0] == '1')
         put_opt( "*BPGM", KWD_OFF+2, " +" );
      if (where_run[1] == '1')
         put_opt( "*IPGM", KWD_OFF+2, " +" );
      if (where_run[2] == '1')
         put_opt( "*EXEC", KWD_OFF+2, " +" );
      if (where_run[3] == '1')
         put_opt( "*INTERACT", KWD_OFF+2, " +" );
      if (where_run[4] == '1')
         put_opt( "*BATCH", KWD_OFF+2, " +" );
      if (where_run[5] == '1')
         put_opt( "*BREXX", KWD_OFF+2, " +" );
      if (where_run[6] == '1')
         put_opt( "*IREXX", KWD_OFF+2, " +" );
      if (where_run[7] == '1')
         put_opt( "*BMOD", KWD_OFF+2, " +" );
      if (where_run[8] == '1')
         put_opt( "*IMOD", KWD_OFF+2, " +" );
      rplc_str( " + ", ") +" );
   } /*  111111111 */

   if ( alw_lmt[0] == '1' )
   {
      put_src( "ALWLMTUSR", "*YES", '+' );
   }
   else
   {
      put_src( "ALWLMTUSR", "*NO", '+' );
   }

   if ( max_pos_parms == -1 )
   {
      put_src( "MAXPOS", "*NOMAX", '+' );
   }
   else
   {
      sprintf( WkStr, "%d", max_pos_parms );
      put_src( "MAXPOS", WkStr, '+' );
   }

   trim_str( pmt_msgf );
   if ( 0==memcmp(pmt_msgf,"*NONE", 5) )
   {
      put_src( "PMTFILE", "*NONE", '+' );
   }
   else
   {
      trim_str( pmt_msgfl );
      sprintf( WkStr, "%s/%s", pmt_msgfl, pmt_msgf );
      put_src( "PMTFILE", WkStr, '+' );
   }

   trim_str( msgf );
   if ( msgfl[0] == ' ' )
   {
      put_src( "MSGF", msgf, '+' );
   }
   else
   {
      trim_str( msgfl );
      sprintf( WkStr, "%s/%s", msgfl, msgf );
      put_src( "MSGF", WkStr, '+' );
   }

   trim_str( help_shelf );
   if ( 0 != memcmp(help_shelf,"*NONE", 5) )
   {
      put_src( "HLPSHELF", help_shelf, '+' );
   }

   trim_str( hlp_pnl );
   if ( 0==memcmp(hlp_pnl,"*NONE", 5) )
   {
      put_src( "HLPPNLGRP", "*NONE", '+' );
   }
   else
   {
      trim_str( hlp_pnll );
      sprintf( WkStr, "%s/%s", hlp_pnll, hlp_pnl );
      put_src( "HLPPNLGRP", WkStr, '+' );
   }
   put_src( "HLPID", hlp_id, '+' );

   trim_str( hlp_idx );
   if ( hlp_idxl[0] == ' ' )
   {
      put_src( "HLPSCHIDX", hlp_idx, '+' );
   }
   else
   {
      trim_str( hlp_idxl );
      sprintf( WkStr, "%s/%s", hlp_idxl, hlp_idx );
      put_src( "HLPSCHIDX", WkStr, '+' );
   }

   put_src( "CURLIB", cur_lib, '+' );

   put_src( "PRDLIB", prd_lib, '+' );

   trim_str( pmt_ovr );
   if ( pmt_ovrl[0] == ' ' )
   {
      put_src( "PMTOVRPGM", pmt_ovr, '+' );
   }
   else
   {
      trim_str( pmt_ovrl );
      sprintf( WkStr, "%s/%s", pmt_ovrl, pmt_ovr );
      put_src( "PMTOVRPGM", WkStr, '+' );
   }

   rcv_len = sizeof(usra0100);
   API_Err.bytesProvided = sizeof(API_Err);
   API_Err.bytesAvail = 0;

   QSYRUSRA(&usra0100, rcv_len, usra_fmt, user_pub, qual_cmd, type_cmd,
            &API_Err);

   strcpy(pub_aut, "*LIBCRTAUT");
   if (API_Err.bytesAvail == 0)
   {
      if (0==memcmp(usra0100.Object_Auth, "*USE      ", 10) ||
          0==memcmp(usra0100.Object_Auth, "*ALL      ", 10) ||
          0==memcmp(usra0100.Object_Auth, "*CHANGE   ", 10) ||
          0==memcmp(usra0100.Object_Auth, "*EXCLUDE  ", 10))
      {
         memcpy(pub_aut, usra0100.Object_Auth, 10);
      }
   }
   put_src( "AUT", pub_aut, '+' );

   put_src( "REPLACE", "*YES", '+' );

   if (bytes_ret >= 333)
   {
      if ( enb_gui == '1' )
      {
         put_src( "ENBGUI", "*YES", ' ' );
      }
      else
      {
         put_src( "ENBGUI", "*NO", ' ' );
      }
   }

   /*  Now put internal attributes (those not in CRTCMD cmd)  */

   Comment = 0;
   put_line( " " );
   Comment = 1;
   put_line( "    Internal info" );

   if ( rst_rel[0] != ' ' )
   {
      sprintf( WkStr, "      Restricted Release:       %s", rst_rel );
      put_line( WkStr );
   }

   strcpy(wkParm,cpp_state);
   if ( 0==memcmp(cpp_state,"*S", 2) )
      strcpy(wkParm,"*SYSTEM");
   if ( 0==memcmp(cpp_state,"*U", 2) )
      strcpy(wkParm,"*USER");
   sprintf( WkStr, "      PGM call state:           %s", wkParm );
   put_line( WkStr );

   if ( 0!=memcmp(vld_ckr,"*NONE", 5) &&
        0!=memcmp(vld_ckr,"     ", 5) )
   {
      strcpy(wkParm,vld_state);
      if ( 0==memcmp(vld_state,"*S", 2) )
         strcpy(wkParm,"*SYSTEM");
      if ( 0==memcmp(vld_state,"*U", 2) )
         strcpy(wkParm,"*USER");
      sprintf( WkStr, "      VLDCKR call state:        %s", wkParm );
      put_line( WkStr );
   }

   if ( 0!=memcmp(pmt_ovr,"*NONE", 5) &&
        0!=memcmp(pmt_ovr,"     ", 5) )
   {
      strcpy(wkParm,pmt_state);
      if ( 0==memcmp(pmt_state,"*S", 2) )
         strcpy(wkParm,"*SYSTEM");
      if ( 0==memcmp(pmt_state,"*U", 2) )
         strcpy(wkParm,"*USER");
      sprintf( WkStr, "      PMTOVRPGM call state:     %s", wkParm );
      put_line( WkStr );
   }

   if (bytes_ret >= 332)
   {
      sprintf( WkStr, "      CCSID:                    %d", ccsid );
      put_line( WkStr );
   }

   Comment = 0;
   put_line( " " );

   return 0;
} /* rtv_crt_API */

#endif  /*  AS400  */
/*--------------------------------------------------------------------------
 *  rtv_cmd:
 *      Retrieve source for CMD stmt
 */
void rtv_cmd( void )
{
   long off_pmt;
   short pmt_len, txt_len;
   char msg_id[8]="";
   char pmt_txt[63]="";

   memset( OutStr, ' ', srcRecl );
   memcpy( OutStr+PARM_OFF, "CMD", 3 );
   off_pmt = CF_off_prompt;
   get_str( off_pmt + 0x000C, 7, msg_id );
   pmt_len = get_int( off_pmt + 0x0015 );
   if ( msg_id[0] == ' ' && pmt_len == 0 )
   {
      put_src( "PROMPT", "*NONE", ' ' );
   }
   else
   {
      if ( msg_id[0] != ' ' && Rtv_prompts == 'M' )
      {
         strcpy( pmt_txt, msg_id );
      }
      else
      {
         pmt_txt[0] = '\'';
         get_str( off_pmt + 0x0017, pmt_len, &pmt_txt[1] );
         dbl_quotes( &pmt_txt[1] );
         txt_len = strlen( pmt_txt );
         pmt_txt[txt_len  ] = '\'';
         pmt_txt[txt_len+1] = '\0';
      }
      put_src( "PROMPT", pmt_txt, ' ' );
   }
} /* rtv_cmd */
/*--------------------------------------------------------------------------
 *  rtv_parm:
 *      Retrieve source for one parm/elem/qual
 */
void rtv_parm( void )
{
   short misc2l, misc2r, dspinp, max_spcl_len = 0;
   get_parm();
   memset( OutStr, ' ', srcRecl );
   OutStr[srcRecl] = '\0';
   memcpy( OutStr+PARM_OFF, TypeEnts[PF_type], strlen(TypeEnts[PF_type]) );
   IsLabel = 0;
   if ( IsParm )
   {
      PF_parm_no++;
      put_src( "KWD", PF_kwd_name, '+' );
      label_sw = 0;
   }
   else
   {
      if ( !label_sw )
      {
   /* put label: on 1st elem/qual of a group */
         char *p;

         IsLabel = 1;
         rplc_plus( ' ' );
         label_sw = 1;
         p = ( IsElem ) ? "E" : "Q";
         sprintf( WkStr, "%s%04X:", p, Parm_off );
         memcpy( OutStr, WkStr, 6 );
      }
      if ( PF_off_next_kwd == 0 )   /* last of group */
         label_sw = 0;
   }
   cal_kwd_type();
   put_src( "TYPE", Kwd_type, '+' );
   if ( !HasElem && !HasQual )
      cal_kwd_len();
   if (PF_misc1 & 0x04)
      put_src( "RTNVAL", "*YES", '+' );
   if (PF_var_pres & 0x80)
      get_dft_con( "CONSTANT" );
   if (PF_misc1 & 0x08)
      put_src( "RSTD", "*YES", '+' );
   if ((PF_var_pres & 0x40) && (PF_min == 0))
      get_dft_con( "DFT" );
   if (PF_var_pres & 0x10)
      get_values();
   if (PF_var_pres & 0x08)
      get_rel();
   if (PF_var_pres & 0x20)
      get_spcl( &max_spcl_len, SPCVAL, "SPCVAL" );
   if (PF_var_pres & 0x20)
      get_spcl( &max_spcl_len, SNGVAL, "SNGVAL" );
   if (PF_min != 0)
   {
      sprintf( WkStr, "%d", PF_min );
      put_src( "MIN", WkStr, '+' );
   }
   if (PF_max != 1)
   {
      sprintf( WkStr, "%d", PF_max );
      put_src( "MAX", WkStr, '+' );
   }
   if (PF_misc1a & 0x80)
      put_src( "ALWUNPRT", "*NO", '+' );
   if (PF_misc1a & 0x40)
      put_src( "ALWVAR", "*NO", '+' );

   misc2l = PF_misc2 & 0xF0;
   misc2r = PF_misc2 & 0x0F;
   if (misc2l == 0x00 && misc2r > 0 && misc2r <= 8)
      put_src( "FILE", FileUs [misc2r], '+' );
   if (misc2l == 0x10)
      put_src( "DTAARA", "*YES", '+' );
   if (misc2l == 0x20)
      put_src( "PGM", "*YES", '+' );

   if (PF_misc1 & 0x80)
      put_src( "FULL", "*YES", '+' );
   if ((PF_misc1 & 0x40) && !HasElem && !HasQual )
      put_src( "EXPR", "*YES", '+' );

   if ( (PF_flag_byte & 0x0C) == 0 )   /* no VARY/PASSATR if type is label */
   {
      if (PF_misc1 & 0x20)
      {
         if (PF_bits_1A & 0x40)
            put_src( "VARY", "*YES *INT4", '+' );
         else
            put_src( "VARY", "*YES", '+' );
      }
      if (PF_misc1 & 0x10)
         put_src( "PASSATR", "*YES", '+' );
   }
   if (PF_misc1 & 0x01)
      put_src( "PASSVAL", "*NULL", '+' );
   if (PF_bits_1A & 0x20 &&
       !HasElem && !HasQual &&
       (PF_val_type == TY_CHAR || PF_val_type == TY_PNAME))
      put_src( "CASE", "*MIXED", '+' );
   if (PF_bits_1A & 0x10)
      put_src( "LISTDSPL", "*INT4", '+' );
   dspinp = PF_misc2 & 0xC0;
   if (dspinp == 0x80)
      put_src( "DSPINPUT", "*PROMPT", '+' );
   if (dspinp == 0xC0)
      put_src( "DSPINPUT", "*NO", '+' );

/* no CHOICE if CONSTANT or *ZEROELEM or *NULL  */
   if (0 == (PF_var_pres & 0x80)  &&
       PF_val_type != 12          &&
       PF_val_type != 15        ) {
     if (PF_misc1a & 0x08)
        get_choice();
     else
        put_src( "CHOICE", "*NONE", '+' );
   }
   if (PF_misc1a & 0x20)
      put_src( "PMTCTL", "*PMTRQS", '+' );
   if (PF_misc1a & 0x10)
      get_pmtctl();
   if (PF_misc1a & 0x04)
      get_pmtctlpgm();
   if (PF_bits_1A & 0x80)
      put_src( "KEYPARM", "*YES", '+' );
   put_pmt( max_spcl_len );
   rplc_plus( ' ' );

} /* rtv_parm */
/*--------------------------------------------------------------------------
 * get_parm_no:  Read a parm by parm number
 */
void get_parm_no( short prm_no )
{
   Parm_off = parm_ary[prm_no-1].off_to_parm;
   get_parm();
} /* get_parm_no */
/*--------------------------------------------------------------------------
 * get_parm:  Read a parm
 */
void get_parm( void )
{
   PF_off_next_kwd      = get_int( Parm_off + 0x0000 );
   PF_off_listqual      = get_int( Parm_off + 0x0002 );
   PF_flag_byte         = get_chr( Parm_off + 0x0004 );
   PF_var_pres          = get_chr( Parm_off + 0x0005 );
   PF_val_type          = get_chr( Parm_off + 0x0007 );
   PF_val_length        = get_int( Parm_off + 0x0008 );
   PF_typex_frac_len    = get_chr( Parm_off + 0x000A );
   PF_typex_total_len   = get_chr( Parm_off + 0x000B );
   PF_min               = get_int( Parm_off + 0x000C );
   PF_max               = get_int( Parm_off + 0x000E );
   PF_nest_level        = get_int( Parm_off + 0x0010 );
   PF_misc1             = get_chr( Parm_off + 0x0012 );
   PF_misc1a            = get_chr( Parm_off + 0x0013 );
   PF_misc2             = get_chr( Parm_off + 0x0014 );
   PF_off_prompt        = get_int( Parm_off + 0x0015 );
   PF_bits_1A           = get_chr( Parm_off + 0x001A );
                          get_str( Parm_off + 0x001B, 10, PF_kwd_name );
   PF_kwd_num           = get_int( Parm_off + 0x0025 );
   PF_off_next2prompt   = get_int( Parm_off + 0x0027 );

   PF_type = (unsigned char) (PF_flag_byte & 0xC0) >> 6;
   IsParm  = PF_type == A_PARM;
   IsElem  = PF_type == A_ELEM;
   IsQual  = PF_type == A_QUAL;
   IsDummy = PF_type == A_DUMMY;
   HasElem = ( PF_flag_byte & 0x08 ) ? 1 : 0;
   HasQual = ( PF_flag_byte & 0x04 ) ? 1 : 0;

} /* get_parm */
/*--------------------------------------------------------------------------
 * cal_kwd_type:
 *      Calc keyword data type
 */
void cal_kwd_type( )
{

/* if type is elem or qual... */
   if ( PF_flag_byte & 0x0C )
   {
      short off, off_listqual;
      char c;
      unsigned char flag_byte;

   /* if parm points to dummy, run down elem/qual chain to non-dummy */
      off_listqual = PF_off_listqual;
      do
      {
         off = off_listqual;
         off_listqual  = get_int( (long) off + 0x0002 );
         flag_byte = get_chr( (long) off + 0x0004 );
      }
      while ( (flag_byte & 0xC0) == 0xC0 && off_listqual );

      c = ( flag_byte & 0x80 ) ? 'Q' : 'E';
      sprintf( Kwd_type, "%c%04X", c, off );
   }
   else
   {
/* ...else data type  */

      if ( PF_val_type >= 0 && PF_val_type < NUM_KWD_TYPES )
      {
         strcpy( Kwd_type, kwd_types[PF_val_type]);
      }
   }
} /* cal_kwd_type  */
/*--------------------------------------------------------------------------
 *  cal_kwd_len:
 *      Calc & output kwd length parm
 */
void cal_kwd_len( void )
{
   short fracdig, totdig;

   switch ( PF_val_type )
   {
      case TY_INT2:
      case TY_INT4:
      case TY_DATE:
      case TY_TIME:
      case TY_CMD:
      case TY_ZEROELEM:
      case TY_NULL:
      case TY_UINT2:
      case TY_UINT4:
         return;                  /* no LEN parm */

      case TY_DEC:
         fracdig = (unsigned short) PF_val_length >> 8;
         totdig = PF_val_length & 0xFF;
         sprintf( WkStr, "%d %d", totdig, fracdig );
         break;

      case TY_X:
         fracdig = PF_typex_frac_len;
         totdig = PF_typex_total_len;
         sprintf( WkStr, "%d %d %d", PF_val_length, totdig, fracdig );
         break;

      default:
      sprintf( WkStr, "%d", PF_val_length );
         break;
   } /* switch */

   put_src( "LEN", WkStr, '+' );

} /* cal_kwd_len  */
/*--------------------------------------------------------------------------
 *  get_dft_con:
 *      Retrieve & output default or constant data
 */
void get_dft_con( char *kwd )
{
   long ent_off;

/* no CONSTANT if...  */
   if ( strcmp(kwd,"CONSTANT") == 0 )
   {
      switch ( PF_val_type )
      {
         case TY_CMD:
         case TY_ZEROELEM:
         case TY_NULL:
            return;
      }
      if ((PF_max > 1)         ||
          (PF_var_pres & 0x40) ||                          /* DFT */
          (PF_misc1 & 0x04)    ||                          /* RTNVAL */
          ((PF_misc1 & 0x40) && !HasElem && !HasQual ))    /* EXPR */
         return;
   } /* CONSTANT */

   ent_off = advance2elm( 1 );
   atr_len_str( ent_off+3 );

   put_src( kwd, WkStr, '+' );

} /* get_dft_con */
/*--------------------------------------------------------------------------
 *  advance2elm:
 *      Advance to var_length_parm_elem of specified type.
 *      Return offset of found elem (attribute byte), -1 if none.
 */
long advance2elm( short elm_type )
{
   short atrib=0;
   long ent_off;

   ent_off = Parm_off +
            ((PF_type == A_PARM) ? LEN_PRM_FX_AREA : LEN_OTH_FX_AREA);
   while ( atrib != elm_type )
   {
      atrib = get_chr( ent_off );
      if ( atrib == 0xFF )
         return -1;
      if ( atrib != elm_type )
         ent_off += get_int( ent_off+1 );
   }
   return ent_off;

} /* advance2elm */
/*--------------------------------------------------------------------------
 *  atr_len_str:
 *      Retrieve (into WkStr) atr & off pointing to len & str
 */
void atr_len_str( long atr_off )
{
   short w=0, attr_byte, data_len, quotd=0;
   long data_off;

   memset( WkStr, ' ', WK_STR_LN );
   attr_byte = 0x7F & get_chr( atr_off );   /* (strip "specified" bit) */
   data_off = get_int( atr_off+1 );
   data_len = get_int( (long) data_off );

   if ( attr_byte == 0x0C )   /* hex val  */
   {
      WkStr[0] = 'X';
      WkStr[1] = '\'';
      get_str( data_off+2, data_len, &WkStr[2] );
      strcat( WkStr, "'");
   }
   else
   {
      if ( attr_byte == 0x45 ||               /* quoted string */
           attr_byte == 0x48 )                /* logical       */
         quotd = 1;

      if ( quotd )
         WkStr[w++] = '\'';

      get_str( data_off+2, data_len, &WkStr[w] );

      if ( quotd )
      {
         dbl_quotes( &WkStr[w] );
         strcat( WkStr, "'");
      }
   } /* 0x0C */
} /* atr_len_str */
/*--------------------------------------------------------------------------
 *  get_spcl:
 *      Retrieve & output spcval/sngval's
 */
void get_spcl( short *max_spcl_lenP, short elm_type, char *kwd )
{
   short i, to_len, attr_byte, num_spcls, num_sngvals, num, out_off;
   short leftQuoted;
   long ent_off, spcl_off, from_off, to_off;
   char *term;

   ent_off = advance2elm( 2 );
   if (ent_off == -1)
      return;
   num_spcls   = get_int( ent_off+3 );
   num_sngvals = get_int( ent_off+5 );

   if ( elm_type == SNGVAL )
   {
      spcl_off = ent_off + 7;
      if ((num = num_sngvals) <= 0)
         return;
   }
   else
   {
      spcl_off = ent_off + 7 + (num_sngvals * 5);
      if ((num = num_spcls-num_sngvals) <= 0)
         return;
   }

   put_kwd( kwd );

   for (i=0; i < num; i++)
   {
     short attr_byte, data_len, quoted;

      attr_byte = 0x7F & get_chr( spcl_off );  /* (strip "specified" bit) */
      quoted = (attr_byte == 0x45);            /* quoted string */
      from_off  = get_int( spcl_off+1 );
      to_off    = get_int( spcl_off+3 );
      data_len  = get_int( (long) from_off );
      if (*max_spcl_lenP < data_len)
          *max_spcl_lenP = data_len;
      get_str( from_off+2, data_len, WkStr );
      leftQuoted = (spcl_charsP(WkStr) || (data_len==0));
      if (IsQual && IsLabel)
        leftQuoted = 0;

      if (leftQuoted) {
         sprintf( &OutStr[KWD_OFF+2], "('%s' ", WkStr );
      } else {
         sprintf( &OutStr[KWD_OFF+2], "(%s ", WkStr );
      }
      out_off = strlen( OutStr );
      term = (i < num-1) ? ") +" : ")) +";

      if ( from_off == to_off &&
           PF_val_type != TY_INT2 &&
           PF_val_type != TY_UINT2 )
      {
         sprintf( &OutStr[out_off], "%s", term );
         put_line( OutStr );
      }
      else
      {
         if ( attr_byte == 0x0C )   /* hex val  */
         {
            to_len = 0;

            /*  Hex int's don't have len before val  */

            if ( PF_val_type == TY_INT2  ||
                 PF_val_type == TY_INT4  ||
                 PF_val_type == TY_UINT2 ||
                 PF_val_type == TY_UINT4)
            {
              to_len = PF_val_length;
            }
            OutStr[out_off++] = 'X';
            put_hexval( to_off, out_off, to_len, 1, term );
         }
         else
         {
            put_val( to_off, out_off, quoted, term );
         }
      }
      spcl_off += 5;
   } /* for i < num */

} /* get_spcl */
/*--------------------------------------------------------------------------
 *  spcl_charsP:
 *      Does str contain special chars?
 *      (non-alphameric, other than '*')
 *      (or *CAT, *BCAT, *TCAT)
 */
int spcl_charsP( char *str )
{
   char  c;
   char  *p;

   if (0==strcmp(str, "*CAT")  ||
       0==strcmp(str, "*BCAT") ||
       0==strcmp(str, "*TCAT"))
   {
     return 1;
   }

   for (p=str; *p; p++)
   {
      c = *p;
      if (!isalnum(c) && c != '*')
      {
        return 1;
      }
   }
   return 0;

} /* spcl_charsP */
/*--------------------------------------------------------------------------
 *  get_rel:
 *      Retrieve & output rel or range
 */
void get_rel( void )
{
   short num_rels, out_off, quoted;
   long ent_off, relation, rel;
   long off2val1, off2val2;
   char *term;

   ent_off = advance2elm( 4 );
   num_rels = get_int( ent_off+3 );
   relation = get_chr( ent_off+5 );
   off2val1  = get_int( ent_off+6 );
   rel = relation & 0x07;
   switch ( PF_val_type )
   {
      case TY_NAME:
         quoted = 0;
         break;
      default:
         quoted = -1;
         break;
   }
   if ( num_rels == 1 )
   {
      sprintf( &OutStr[KWD_OFF], "REL(%s ", Rels[rel] );
      out_off = KWD_OFF + 8;
      put_val( off2val1, out_off, quoted, ") +" );
   }
   else
   {
      off2val2  = get_int( ent_off+9 );
      out_off = KWD_OFF + 2;
      put_kwd( "RANGE" );
      put_val( off2val1, out_off, quoted, " +" );
      put_val( off2val2, out_off, quoted, ") +" );
   }
} /* get_rel */
/*--------------------------------------------------------------------------
 *  get_values:
 *      Retrieve  & output values
 */
void get_values( void )
{
   short i, num_vals, out_off;
   long ent_off, off_off, off2val;
   char *term;

   put_kwd( "VALUES" );
   ent_off = advance2elm( 3 );
   num_vals = get_int( ent_off+3 );
   off_off = ent_off + 5;
   out_off = KWD_OFF+2;          /* indent */

   for (i=0; i < num_vals; i++ )
   {
      off2val = get_int( off_off );
      off_off += 2;
      term = (i < num_vals-1) ? " +" : ") +";
      put_val( off2val, out_off, -1, term );
   }
} /* get_values */
/*--------------------------------------------------------------------------
 *  put_val
 *      Get, edit & output a value
 */
void put_val( long off2val, short out_off, short quoted, char *term )
{
   short int2, quoted2pass, data_len;
   ushrt ushrt2;
   long int4;
   ulong ulong4;

   switch ( PF_val_type )
   {
      case TY_INT2:
         int2 = get_int( off2val );
         sprintf( &OutStr[out_off], "%d%s", int2, term );
         put_line( OutStr );
         break;
      case TY_UINT2:
         ushrt2 = get_int( off2val );
         sprintf( &OutStr[out_off], "%u%s", ushrt2, term );
         put_line( OutStr );
         break;
      case TY_INT4:
         int4 = get_long( off2val );
         sprintf( &OutStr[out_off], "%ld%s", int4, term );
         put_line( OutStr );
         break;
      case TY_UINT4:
         ulong4 = get_long( off2val );
         sprintf( &OutStr[out_off], "%lu%s", ulong4, term );
         put_line( OutStr );
         break;
      case TY_DEC:
         put_pckval( off2val, out_off, term );
         break;
      case TY_CHAR:
      case TY_VARNAME:
      case TY_CMD:
      case TY_CMDSTR:
      case TY_PNAME:
         quoted2pass = (quoted == -1) ? 1 : quoted;
         put_strval( off2val, out_off, quoted2pass, term );
         break;
      case TY_NAME:
      case TY_GENERIC:
      case TY_SNAME:
      case TY_CNAME:
         put_strval( off2val, out_off, -2, term );
         break;
      case TY_TIME:
         put_strval( off2val, out_off, 0, term );
         break;
      case TY_LGL:
         put_strval( off2val, out_off, 1, term );
         break;
      case TY_HEX:
         put_hexval( off2val, out_off, 0, 0, term );
         break;
      case TY_DATE:
         put_date( off2val, out_off, term );
         break;
      case TY_ZEROELEM:
      case TY_X:
      case TY_NULL:
         break;
   } /* switch */

   memset( OutStr, ' ', srcRecl );

} /* put_val */
/*--------------------------------------------------------------------------
 * put_date:
 */
void put_date( long off2val, short out_off, char *term )
{
   short data_len;
   char cyymmdd[8];

   data_len  = get_int( off2val );
   get_str( off2val+2, data_len, cyymmdd );
   strncpy( &OutStr[out_off  ], cyymmdd+3, 4 );
   strncpy( &OutStr[out_off+4], cyymmdd+1, 2 );
   strcpy(  &OutStr[out_off+6], term );
   put_line( OutStr );

} /* put_date */
/*--------------------------------------------------------------------------
 * put_strval:
 *      Put a string value: 2-byte bin length, chars
 */
void put_strval( long off2val, long out_off, short quotedIn, char *term )
{
   short i, j, length, quoted, ckNames, needQuotes;
   long off, ou_off;
   unsigned char ch;

   quoted = quotedIn;
   ckNames = 0;
   if (quoted == -2) {
      ckNames = 1;
      quoted = 0;
   }
   ou_off = out_off;
   length = get_int( off2val );
   off = off2val + 2;
   needQuotes = 0;

   /*  Check for chars requiring quotes  */

   for (j=0; j < length; j++ )
   {
      ch = get_chr(off++);

      if (ch == ' ')
         needQuotes = 1;
      if (ckNames && nonNameChar(ch))
         needQuotes = 1;

      /*  If value contains non-display chars, put hex instead  */

      if (ch < 0x40)
      {
         ou_off = ckOutStr( ou_off );
         OutStr[ou_off++] = 'X';
         put_hexval( off2val, ou_off, 0, 1, term );

         return;             /*  NOTE return  */
      }
   } /* for j < length */

   if (needQuotes)
      quoted = 1;
   off = off2val + 2;

/* if quoted string...  */
   if ( quoted )
   {
      ou_off = ckOutStr( ou_off );
      OutStr[ou_off++] = '\'';
   }

   for (i=0; i < length; i++ )
   {
      ch = get_chr(off++);

#ifdef  PC
      ch = to_ASCII_tab[ ch ];
#endif

      ou_off = ckOutStr( ou_off );
      OutStr[ou_off++] = ch;

   /* if quote, double it */
      if ( ch == '\'' && quoted )
      {
         ou_off = ckOutStr( ou_off );
         OutStr[ou_off++] = ch;
      }
   } /* for i < length */

   if ( quoted )
   {
      ou_off = ckOutStr( ou_off );
      OutStr[ou_off++] = '\'';
   }

   ou_off = ckOutStr( ou_off );
   strcpy( &OutStr[ou_off], term );

/* if val is *N, get again, quoted (because *N means "null")  */
   if ( length == 2 && !quoted && strncmp(&OutStr[out_off],"*N",2)==0 )
      put_strval( off2val, out_off, 1, term );
   else
      put_line( OutStr );

} /* put_strval */
/*--------------------------------------------------------------------------
 * nonNameChar:
 *      Return 1 if char is not valid in a name; else 0.
 */
short nonNameChar( unsigned char chIn )
{
   short isNonName = 0;
   unsigned char ch;

   ch = toupper(chIn);

   if (ch >= '0' && ch <= '9')      isNonName = 0;
   else if (ch >= 'A' && ch <= 'I') isNonName = 0;
   else if (ch >= 'J' && ch <= 'R') isNonName = 0;
   else if (ch >= 'S' && ch <= 'Z') isNonName = 0;
   else if (ch == '$' ||
            ch == '@' ||
            ch == '#' ||
            ch == '*' ||
            ch == '.' ||
            ch == '_' ) isNonName = 0;
   else isNonName = 1;

   return isNonName;

} /* nonNameChar */
/*--------------------------------------------------------------------------
 * ckOutStr:
 *      Check overflow in OutStr, outputting when full
 */
short ckOutStr( short out_off )
{
   short nu_out_off;

   nu_out_off = out_off;

   if (nu_out_off >= srcRecl-1) {
     OutStr[nu_out_off] = '+';
     put_line( OutStr );
     memset(OutStr, ' ', srcRecl);
     nu_out_off = KWD_OFF;
   }
   return nu_out_off;
} /* ckOutStr  */
/*--------------------------------------------------------------------------
 * put_hexval:
 *      Put a hex value to put_line()
 */
void put_hexval( long off2val, short out_off, short len, short quoted,
                 char *term )
{
   short nu_out_off;

   nu_out_off = get_hexval( OutStr, out_off, off2val, len, quoted );
   nu_out_off = ckOutStr( nu_out_off );
   strcpy( &OutStr[nu_out_off], term );
   put_line( OutStr );

} /* put_hexval */
/*--------------------------------------------------------------------------
 * get_hexval:
 *      Get a hex value
 */
short get_hexval( char *dest, short dest_off, long off2val, short val_len,
                  short quoted )
{
   short out_off, i, len, num_nibs;
   long off;
   int toOutStr;
   unsigned char byte, nib, dig;

   out_off = dest_off;
   off = off2val;
   len = val_len;

   /*  If no len passed in, it's at beginning of value  */

   if (len == 0)
   {
     len = get_int( off );
     off += 2;
   }
   num_nibs = 2 * len;
   toOutStr = (dest == OutStr);
   byte = get_chr(off++);     /* prime pump */

   if ( quoted )
   {
      if (toOutStr) out_off = ckOutStr( out_off );
      dest[out_off++] = '\'';
   }

/* loop thru nibbles */
   for ( i=0; i < num_nibs; i++ )
   {
      if ((i & 1)==0)         /* if even */
      {
         nib = (unsigned char) (byte & 0xF0) >> 4;
      }
      else               /* else odd */
      {
         nib = byte & 0x0F;
         byte = get_chr(off++);
      }

      dig = (nib < 10) ? (nib + '0') : (nib - 10 + 'A');
      if (toOutStr) out_off = ckOutStr( out_off );
      dest[out_off++] = dig;
   } /* for i */

   if ( quoted )
   {
      if (toOutStr) out_off = ckOutStr( out_off );
      dest[out_off++] = '\'';
   }
   return out_off;

} /* get_hexval */
/*--------------------------------------------------------------------------
 * put_pckval:
 *      Put a packed value
 */
void put_pckval( long off2val, long out_off, char *term )
{
   short i, odd_nibs, num_bytes, num_digs, num_decs, perdi, signif=0;
   short lasti;
   long off;
   unsigned char byte, nib, dig;

   off = off2val;
   num_digs = PF_val_length & 0xFF;
   num_decs = (unsigned short) PF_val_length >> 8;

   /* odd_nibs = # nibbles excl sign, incl xtra hi nib if #decs is even */

   if (num_digs & 1)         /* if odd */
   {
      odd_nibs = num_digs;
      i = 0;                   /* start at 1st nibble */
   }
   else                      /* else even */
   {
      odd_nibs = num_digs + 1;
      i = 1;                   /* start at 2nd nibble */
   }
   num_bytes = (odd_nibs+1) / 2;
   perdi = odd_nibs - num_decs;     /* index of 1st dig after '.' */
   lasti = odd_nibs - 1;

/* get sign */
   byte = get_chr( off + num_bytes - 1 );
   if ((byte & 0x0F) == 0x0D)
      OutStr[out_off++] = '-';

   byte = get_chr(off++);     /* prime pump */

/* loop thru nibbles */

   for ( ; i < odd_nibs; i++ )
   {
      if (i == perdi)
      {
         OutStr[out_off++] = '.';
         signif = 1;
      }

      if ((i & 1)==0)         /* if even */
      {
         nib = (unsigned char) (byte & 0xF0) >> 4;
      }
      else               /* else odd */
      {
         nib = byte & 0x0F;
         byte = get_chr(off++);
      }

      dig = nib + '0';
      if (dig != '0')
         signif = 1;
      if (i == lasti)
         signif = 1;

      if (signif)
         OutStr[out_off++] = dig;
   } /* for i < odd_nibs */

   strcpy( &OutStr[out_off], term );
   put_line( OutStr );

} /* put_pckval */
/*--------------------------------------------------------------------------
 *  get_choice:
 *      Get choice
 */
void get_choice( void )
{
   long ent_off, off_off, ch_off;
   short i, num_offs, txt_len;
   unsigned char ch_flag;
   char ch_pgm[11];
   char ch_lib[11];
   char msg_id[8];
   char choice[61];

/* no CHOICE if CONSTANT */
   if (PF_var_pres & 0x80)
      return;

/* no CHOICE if *ZEROELEM or *NULL  */
   if ( PF_val_type==12 || PF_val_type==15 )
      return;

   ent_off = advance2elm( 6 );
   if ( ent_off > 0 )
   {
      ch_flag = get_chr( ent_off+3 );
      if ( ch_flag & 0x80 )
      {
         put_src( "CHOICE", "*PGM", '+' );
         get_str( ent_off +  4, 10, ch_pgm );
         get_str( ent_off + 14, 10, ch_lib );
         trim_str( ch_lib );
         sprintf( WkStr, "%s/%s", ch_lib, ch_pgm );
         put_src( "CHOICEPGM", WkStr, '+' );
      }
      else if ( (ch_flag & 0x20) == 0 )        /* not *VALUES */
      {
         num_offs = get_int( ent_off+4 );
         off_off = ent_off+6;

         for (i=0; i < num_offs; i++)
         {
            ch_off = get_int( off_off );
            off_off += 2;
            get_str( ch_off + 0x0002, 7, msg_id );
            txt_len = get_int( ch_off + 0x000B );
            if ( msg_id[0] != ' ' || txt_len > 0 )
            {
               if ( msg_id[0] != ' ' && Rtv_prompts == 'M' )
               {
                  strcpy( choice, msg_id );
               }
               else
               {
                  choice[0] = '\'';
                  get_str( ch_off + 0x000D, txt_len, &choice[1] );
                  choice[txt_len+1] = '\'';
                  choice[txt_len+2] = '\0';
               }
               put_src( "CHOICE", choice, '+' );
            } /* msg_id || txt_len */
         } /* for i */
      }
   }
} /* get_choice */
/*--------------------------------------------------------------------------
 *  get_pmtctl:
 *      Put ref to PMTCTL stmt
 */
void get_pmtctl( void )
{
   long ent_off, pmtctl_off;
   char pmtctl_lbl[6];

   ent_off = advance2elm( 5 );
   if ( ent_off > 0 )
   {
      pmtctl_off = get_int( ent_off+3 );
      if ( pmtctl_off > 0 )
      {
         if ( Lo_pmtctl_off == 0 || pmtctl_off < Lo_pmtctl_off )
            Lo_pmtctl_off = pmtctl_off;
         sprintf( pmtctl_lbl, "P%04X", pmtctl_off );
         put_src( "PMTCTL", pmtctl_lbl, '+' );
      }
   }
} /* get_pmtctl */
/*--------------------------------------------------------------------------
 *  get_pmtctlpgm:
 *      Get libr/name of pmtctl pgm
 */
void get_pmtctlpgm( void )
{
   long ent_off;
   char pcpgm[11];
   char pclib[11];

   ent_off = advance2elm( 8 );
   if ( ent_off > 0 )
   {
      get_str( ent_off +  3, 10, pcpgm );
      get_str( ent_off + 13, 10, pclib );
      trim_str( pclib );
      sprintf( WkStr, "%s/%s", pclib, pcpgm );
      put_src( "PMTCTLPGM", WkStr, '+' );
   }
} /* get_pmtctlpgm */
/*--------------------------------------------------------------------------
 * put_pmt:
 *      Put prompt
 */
void put_pmt( short max_spcl_len )
{
   short off_pmt, outin_len, pmt_len, txt_len, calc_pmt_len;
   char msg_id[8];
   char pmt_txt[63];
   char prompt[81];
   char ord[7]="";

   if ( PF_off_prompt )
   {
      off_pmt = PF_off_prompt;
      outin_len = get_int( off_pmt );
      get_str( off_pmt + 0x0002, 7, msg_id );
      pmt_len = get_int( off_pmt + 0x000B );

      if ( PF_val_type == TY_CHAR    ||
           PF_val_type == TY_NAME    ||
           PF_val_type == TY_SNAME   ||
           PF_val_type == TY_CNAME   ||
           PF_val_type == TY_PNAME   ||
           PF_val_type == TY_GENERIC ||
           PF_val_type == TY_CMDSTR  ||
           PF_val_type == TY_HEX     ||
           PF_val_type == TY_X       ||
           PF_val_type == TY_CMD       ) {
         calc_pmt_len = PF_val_length;
         if ( PF_val_type == TY_HEX )
            calc_pmt_len = PF_val_length * 2;
         if ( PF_val_type == TY_X )
            calc_pmt_len = 2 + (PF_val_length * 2);
         if (calc_pmt_len < max_spcl_len )
             calc_pmt_len = max_spcl_len;

         if ( !HasElem &&
              !HasQual &&
              !(PF_misc1 & 0x80) &&           /* full     */
              !(PF_misc1 & 0x08) &&           /* rstd     */
              !(PF_var_pres & 0x80) &&        /* constant */
              !(PF_misc1 & 0x04) &&           /* rtnval   */
              outin_len != 0 &&
              outin_len != calc_pmt_len ) {

            if ( outin_len == -1 ) {
               put_src( "INLPMTLEN", "*PWD", '+' );
            } else {
               /*
                *  Don't know how to calc calc_pmt_len for *X.
                *  Just suppress it.
                */
               if ( PF_val_type != TY_X ) {
                  sprintf( WkStr, "%d", outin_len );
                  put_src( "INLPMTLEN", WkStr, '+' );
               }
            }
         }
      }
      if ( msg_id[0] != ' ' || pmt_len > 0 )
      {
         if ( msg_id[0] != ' ' && Rtv_prompts == 'M' )
         {
            strcpy( pmt_txt, msg_id );
         }
         else
         {
            pmt_txt[0] = '\'';
            get_str( off_pmt + 0x000D, pmt_len, &pmt_txt[1] );
            dbl_quotes( &pmt_txt[1] );
            txt_len = strlen( pmt_txt );
            pmt_txt[txt_len] = '\'';
            pmt_txt[txt_len+1] = '\0';
         }

         if ( PF_type == A_PARM && is_out_o_ord )
            sprintf( ord, " %d", parm_ary[ PF_parm_no-1 ].pmt_ord );
         sprintf( prompt, "%s%s", pmt_txt, ord );
         put_src( "PROMPT", prompt, '+' );
      }
   } /* PF_off_prompt */

} /* put_pmt */
/*--------------------------------------------------------------------------
 * put_src:
 *      Put kwd and value in OutStr & write to OutFile
 *      Plus is '+' (or ' ' for last)
 */
void put_src( char *kwd, char *val, char plus )
{
   strcpy( WkStr, val );
   trim_str( WkStr );
   sprintf( &OutStr[KWD_OFF], "%s(%s) %c", kwd, WkStr, plus );
   put_line( OutStr );
   memset( OutStr, ' ', srcRecl );

} /* put_src */
/*--------------------------------------------------------------------------
 * put_kwd:
 *      Output kwd and '( +'
 */
void put_kwd( char *kwd )
{
   sprintf( &OutStr[KWD_OFF], "%s( +", kwd );
   put_line( OutStr );
   memset( OutStr, ' ', srcRecl );

} /* put_kwd */
/*--------------------------------------------------------------------------
 * put_opt:
 *      Output special value option
 */
void put_opt( char *opt, short out_off, char *term )
{
   sprintf( &OutStr[out_off], "%s%s", opt, term );
   put_line( OutStr );
   memset( OutStr, ' ', srcRecl );

} /* put_opt */
/*--------------------------------------------------------------------------
 *  trim_str:
 *      Shorten string, dropping trailing blanks
 */
void trim_str( char *s )
{
   short las;

   for (las=strlen(s)-1; las>=0 && s[las]==' '; las--) {};
   if (las>=-1)
      s[las+1] = '\0';

} /* trim_str */
/*--------------------------------------------------------------------------
 *  rtv_pmtctl:
 *      Retrieve source for PMTCTL desc's
 */
void rtv_pmtctl( void )
{
   long off2value, pc_off, cond_off;
   short next_off, parm_no, num_true, num_cond, prev_is_last=1;
   short tot_cond;
   short i, rel, out_off, prev_or=0;
   unsigned char ctl_byt, cond_rel;
   char *term;

   pc_off = Lo_pmtctl_off;

/* loop thru all PMTCTL stmts  */

   while ( pc_off > 0 )
   {
      memset( OutStr, ' ', srcRecl );
      OutStr[srcRecl] = '\0';
      next_off      = get_int( pc_off + 0x0000 );
      parm_no      = get_int( pc_off + 0x0002 );
      num_true      = get_int( pc_off + 0x0004 );
      ctl_byt      = get_chr( pc_off + 0x0006 );
      num_cond      = get_int( pc_off + 0x0007 );
      get_parm_no( parm_no );

   /* put label: on 1st PMTCTL of a group */
      if ( prev_is_last )
      {
         sprintf( WkStr, "P%04X:", pc_off );
         memcpy( OutStr, WkStr, 6 );
         prev_or = 0;
      }
      prev_is_last = ((ctl_byt & 0x04) ? 0 : 1);

      memcpy( OutStr+PARM_OFF, "PMTCTL", 6 );
      get_str( Parm_off + 0x001B, 10, PF_kwd_name );
      put_src( "CTL", PF_kwd_name, '+' );

      put_kwd( "COND" );
      term = (num_cond > 0) ? " +" : ") +";
      tot_cond = num_cond;

      if (ctl_byt & 0x10)
      {
         tot_cond += 1;
         sprintf( &OutStr[KWD_OFF+2], "(*SPCFD)%s", term );
         put_line( OutStr );
      }
      else if (ctl_byt & 0x08)
      {
         tot_cond += 1;
         sprintf( &OutStr[KWD_OFF+2], "(*UNSPCFD)%s", term );
         put_line( OutStr );
      }
      cond_off = pc_off + 9;

      /* loop thru conditions */

      for (i=0; i < num_cond; i++ )
      {
         cond_rel = get_chr( cond_off );
         off2value = get_int( cond_off+1 );
         rel = (unsigned char)(cond_rel & 0xE0) >> 5;
         sprintf( &OutStr[KWD_OFF+2], "(%s ", Rels[rel] );
         out_off = KWD_OFF + 7;
         term = (i < num_cond - 1) ? ") +" : ")) +";
         put_val( off2value, out_off, -1, term );
         cond_off += 3;
      }
      rel = (unsigned char)(ctl_byt & 0xE0) >> 5;
      if ( rel != 2 || (tot_cond > 0 && tot_cond != num_true) )  /* 2=*EQ */
      {
         sprintf( WkStr, "%s %d", Rels[rel], num_true );
         put_src( "NBRTRUE", WkStr, '+' );
      }

      if ( prev_or )
         put_src( "LGLREL", "*OR", ' ' );
      prev_or = ((ctl_byt & 0x02) == 0);

      rplc_plus( ' ' );
      pc_off = next_off;

   } /* while pc_off */

} /* rtv_pmtctl */
/*--------------------------------------------------------------------------
 *  rtv_interparms:
 *      Retrieve source for interparm desc's
 */
void rtv_interparms( void )
{
   long ip_off, ip_next, ent_off;
   short i, num_true, num_prms;
   short ctl_rel, true_rel;
   unsigned char ctl_byte;
   char msg_id[8];
   char *term;

   for ( ip_off = CF_off_interprm; ip_off > 0; ip_off = ip_next )
   {
      memset( OutStr, ' ', srcRecl );
      memcpy( OutStr+PARM_OFF, "DEP", 3 );
      ip_next = get_int( ip_off );
      get_str( ip_off+2, 7, msg_id );
      num_true = get_int( ip_off+9 );
      num_prms = get_int( ip_off+11 );
      ctl_byte = get_chr( ip_off+15 );
      true_rel = (unsigned char)(ctl_byte & 0xE0) >> 5;
      ctl_rel = ctl_byte & 0x07;

      if ( ctl_rel == 7 )
      {
         put_src( "CTL", "*ALWAYS", '+' );
      }
      else
      {
         sprintf( &OutStr[KWD_OFF], "CTL");
         rtv_interparm( ip_off+13, KWD_OFF+3, ") +" );
      }

      put_kwd( "PARM" );
      ent_off = ip_off+20;

   /* run thru PARM entries */
      for (i=0; i < num_prms; i++)
      {
         term = (i < num_prms-1) ? ") +" : ")) +";
         rtv_interparm( ent_off, KWD_OFF+2, term );
         ent_off += 7;
      }

      if ( num_true != num_prms ||
           true_rel != 2 )             /* *EQ  */
      {
         sprintf( WkStr, "%s %d", Rels[true_rel], num_true );
         put_src( "NBRTRUE", WkStr, '+' );
      }

      if ( strcmp( msg_id,"CPD0150" ) !=0 )
         put_src( "MSGID", msg_id, ' ' );

      rplc_plus( ' ' );

   } /* for ip_off */

} /* rtv_interparms */
/*--------------------------------------------------------------------------
 *  rtv_interparm:
 *      Retrieve 1 interparm CTL/PARM entry
 */
void rtv_interparm( long ent_off, short out_off, char *term )
{
   short parm1_no, rel, prm_or_off, is_prm, val_off;
   unsigned char rel_byte;
   char parm1[11];
   char parm2[11];

   parm1_no   = get_int( ent_off   );
   rel_byte   = get_chr( ent_off+2 );
   prm_or_off = get_int( ent_off+3 );
   get_parm_no( parm1_no );
   strcpy( parm1, PF_kwd_name );
   trim_str( parm1 );
   is_prm = rel_byte & 0x10;
   rel = rel_byte & 0x07;

   if ( rel == 0 )  /* "exists" */
   {
      sprintf( &OutStr[out_off], "(%s%s", parm1, term );
      put_line( OutStr );
   }
   else
   {
      if ( is_prm )
      {
         get_parm_no( prm_or_off );
         trim_str( PF_kwd_name );
         sprintf( &OutStr[out_off], "(&%s %s &%s%s",
                    parm1, Rels[rel], PF_kwd_name, term );
         put_line( OutStr );
      }
      else
      {
         sprintf( &OutStr[out_off], "(&%s %s ", parm1, Rels[rel] );
         val_off = strlen(OutStr);
         put_val( (long) prm_or_off, val_off, -1, term );
      }
   }
   memset( OutStr, ' ', srcRecl );

} /* rtv_interparm */
/*--------------------------------------------------------------------------
 */
