/* SYSIFCOPT(*IFSIO) TERASPACE(*YES *TSIFC) STGMDL(*SNGLVL) */
/* SYSIFCOPT(*IFSIO) TERASPACE(*YES *TSIFC) STGMDL(*SNGLVL) */
/* ------------------------------------------------------------- *
 * Company . . . : System & Method A/S                           *
 * Design  . . . : Niels Liisberg                                *
 * Function  . . : Generic XML / JSON Parser                     *
 *                                                               *
 * By     Date     Task    Description                           *
 * NL     02.06.03 0000000 New program                           *
 * NL     27.02.08 0000510 Allow also no namespace for *:tag     *
 * NL     27.02.08 0000510 jx_NodeCopy                           *
 * NL     13.05.08 0000577 jx_NodeAdd / WriteNote                *
 * NL     13.05.08 0000577 Support for refference location       *
 * ------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <leod.h>
#include <decimal.h>
#include <wchar.h>
// #include <errno.h>

#include <sys/stat.h>
#include "ostypes.h"
#include "xlate.h"
#include "jsonxml.h"
#include "parms.h"
#include "rtvsysval.h"
#include "utl100.h"
#include "mem001.h"
#include "varchar.h"

extern int   InputCcsid , OutputCcsid;
extern iconv_t xlateEto1208;
extern iconv_t xlate1208toE;


/* --------------------------------------------------------------------------- *\
    TODO - move to environmet
\* --------------------------------------------------------------------------- */
void ensureOpenXlate(void) {
   static BOOL isOpen = false;
   if (isOpen) return;
   xlateEto1208 = XlateOpenDescriptor(0   , 1208, false);
   xlate1208toE = XlateOpenDescriptor(1208, 0   , false);
   isOpen = true;
}

/* --------------------------------------------------------------------------- */
PUCHAR c2s(UCHAR c)
{
   static UCHAR s [5];
   if (c <= ' ') {
       sprintf(s , "0x%02X" , c);
   } else {
       sprintf(s , "%c" , c);
   }
   return s;
}
/* ------------------------------------------------------------- */
PUCHAR strTrim(PUCHAR s)
{
    PUCHAR e;
    for(e = s + strlen(s); e > s && *e <= ' '; e--);
    *(e+1) = '\0';
    return (s);
}
/* ------------------------------------------------------------- */
UCHAR hex (UCHAR c)
{
    if (c >= '0' && c <= '9') {
       return (c - '0');
    }
    if (c >= 'A' && c <= 'F') {
       return (c - 'A' + 10);
    }
    if (c >= 'a' && c <= 'f') {
       return (c - 'a' + 10);
    }
}
/* --------------------------------------------------------------------------- */
PUCHAR findchr (PUCHAR base , PUCHAR chars, SHORT charslen)
{
   SHORT i;
   for  (;*base; base++) {
      for (i = 0; i< charslen; i++) {
         if (*base == chars[i]) {
             return base;
         }
      }
   }
   return NULL;
}
/* --------------------------------------------------------------------------- */
LONG xlateMem  (iconv_t xid , PUCHAR out , PUCHAR in, LONG len)
{
    size_t buflen, outbytes, outlen, inbytesleft, outbytesleft;
    PUCHAR outWork = out;
    inbytesleft  = len;
    outbytesleft = 2 * len;
    iconv ( xid , &in , &inbytesleft, &outWork, &outbytesleft);
    outlen = outWork - out;;
    out[outlen] = '\0';
    return outlen;
}
/* --------------------------------------------------------------------------- */
void xlatecpy( PJXCOM pJxCom ,PUCHAR out , PUCHAR in  , LONG len)
{
    size_t buflen, before, inbytesleft, outbytesleft;

    if ( pJxCom->UseIconv) {
         xlateMem ( pJxCom->Iconv, out, in , len);
    } else {
         memcpy (out , in , len);
    }
}
/* ---------------------------------------------------------------------------
    --------------------------------------------------------------------------- */
#pragma convert(1252)
void iconvWrite( FILE * f, iconv_t * pIconv, PUCHAR Value, BOOL Esc)
{
    int len, outlen;
    size_t buflen, before, inbytesleft, outbytesleft;
    PUCHAR pOut, pTemp;

    if (Value == NULL) return;
    if (Value[0] == '\0') return;

    len = strlen( Value );
    outlen = 4 * len;

    pOut = pTemp = malloc   (outlen);
    outbytesleft = outlen;
    inbytesleft  = len;
    buflen       = len;

    iconv ( * pIconv, &Value , &inbytesleft, &pOut, &outbytesleft);
    outlen = pOut - pTemp;

    if (Esc == TRUE) {
       PUCHAR escStr =  malloc (outlen * 6); // If all was " it will expand six times
       PUCHAR OutBuf = escStr;
       PUCHAR InBuf = pTemp;
       UCHAR  c;
       int i;

       for (i=0;i<outlen;i++) {
          c = *InBuf;
          switch(c) {
               case '<' : OutBuf += cpy (OutBuf , "&lt;")   ; break;
               case '>' : OutBuf += cpy (OutBuf , "&gt;")   ; break;
               case '&' : OutBuf += cpy (OutBuf , "&amp;")  ; break;
               case '\'': OutBuf += cpy (OutBuf , "&apos;") ; break;
               case '\"': OutBuf += cpy (OutBuf , "&quot;") ; break;
               default  : *(OutBuf++) = c;
          }
          InBuf++;
       }
       outlen = OutBuf - escStr;
       fwrite ( escStr , 1 , outlen , f);
       free  ( escStr);
    } else {
       fwrite ( pTemp , 1 , outlen , f);
    }

    free (pTemp);
}
#pragma convert(0)
/* ---------------------------------------------------------------------------
    --------------------------------------------------------------------------- */
void iconvPutc( FILE * f, iconv_t * pIconv, UCHAR c)
{
    int len, outlen;
    size_t buflen, before, inbytesleft, outbytesleft;
    UCHAR  Out [4];
    PUCHAR pOut, pValue = &c;

    pOut = Out;
    outbytesleft = 4;
    inbytesleft  = 1;
    buflen       = len;

    iconv ( * pIconv, &pValue , &inbytesleft, &pOut, &outbytesleft);
    outlen = pOut - Out;

    fwrite ( Out , 1 , outlen , f);

}
#pragma convert(0)
/* ---------------------------------------------------------------------------
    --------------------------------------------------------------------------- */
void  swapEndian(PUCHAR buf, LONG len)
{
   LONG i;
   UCHAR c;

   for (i=0;i<len ; i+=2) {
      c = buf[i];
      buf[i] = buf[i+1];
      buf[i+1] = c;
   }
}
/* ---------------------------------------------------------------------------
    --------------------------------------------------------------------------- */
LONG  swapEndianString(PUCHAR buf)
{
   LONG lengthInBytes = 0;
   UCHAR c;
   PUCHAR p1 = buf, p2 = buf+1;

   while (*p1 || *p2) {
      c = *p1;
      *p1 = *p2;
      *p2 = c;
      p1+=2;
      p2+=2;
      lengthInBytes += 2;
   }
   return lengthInBytes;
}
LONG  strlenUnicode(PUCHAR buf)
{
   LONG lengthInBytes = 0;
   PUCHAR p1 = buf, p2 = buf+1;

   while (*p1 || *p2) {
      p1+=2;
      p2+=2;
      lengthInBytes += 2;
   }
   return lengthInBytes;
}
/* ---------------------------------------------------------------------------
    --------------------------------------------------------------------------- */
 LONG xlate(PJXCOM pJxCom, PUCHAR outbuf, PUCHAR inbuf , LONG len)
{
   size_t buflen, inbytesleft, outbytesleft;
   int l;

   if (pJxCom->LittleEndian) {
      swapEndian(inbuf , len);
   }
   // printf("\n i/o len: %d\n" , len);
   inbytesleft  = len;
   outbytesleft = 2 *len;
   buflen       = 2 *len;
   // errno =0;
   l = iconv ( pJxCom->Iconv, &inbuf , &inbytesleft, &outbuf, &outbytesleft);
   if (l == -1 ) {
      return -1;
   }
   len = buflen - outbytesleft;
   return len;
}
/* ---------------------------------------------------------------------------
    Escape utf-8 into unicde
    \uFFFF where FFFF is the hexadecimal unicode value
    --------------------------------------------------------------------------- */
void utf8toUnicode (PUCHAR * inbuf , size_t * inbytesleft, PUCHAR * outbuf, size_t *outbytesleft , ESCAPE_ENCODE encode )
{
   int patchLen ;
   patchLen = sprintf(*outbuf,"\\FFFF");
   *outbuf += patchLen;
   outbytesleft -= patchLen;
   *inbuf += 2;
   inbytesleft -= 2;
}
// -------------------------------------------------------------
BOOL isTerm(UCHAR c, PUCHAR term)
{
   while (*term) {
      if (c == *term) return TRUE;
      term++;
   }
   return FALSE;
}

// -------------------------------------------------------------
 BOOL isTimeStamp(PUCHAR p)
{
   // If the format is 2011-01-02T12:13:14
   return (
         p[4]  == '-'
   &&  p[7]  == '-'
   &&  p[10] == 'T'
   &&  p[13] == ':'
   &&  p[16] == ':');
}
// -------------------------------------------------------------
// Convert  2011-01-02T12:13:14 to 2011-01-02-12.13.14.000000

 int formatTimeStamp(PUCHAR p , PUCHAR s)
{
   memcpy ( p , s, 19) ; // Year month and day hh min and sec
   p[10] = '-';             // Separator Year month
   p[13] = '.';             // Separator Year month
   p[16] = '.';             // Separator Year month
   p[19] = '.';             // Separator Year month
   memcpy ( p+ 19 , ".000000" , 8) ; // Separator Year month
   return (26);
}
/* -------------------------------------------------------------*/

UCHAR unicode2ebcdic (USHORT c)
{
   UCHAR ret;
   PUCHAR in , out;
   size_t inbytesleft, outbytesleft;
   static BOOL doOpen = TRUE;
   static iconv_t ic;
   // if  (ic.cd  == NULL) ic = OpenXlate (13488, 0);
   if (doOpen) {
      ic = XlateOpenDescriptor (1200  , 0 , false);
      doOpen = FALSE;
   }
   outbytesleft = 1  ;
   inbytesleft  = 2  ;
   in = (PUCHAR) &c;
   out = &ret;
   // swapEndian(in , 2  );
   iconv ( ic , &in , &inbytesleft, &out, &outbytesleft);
   return ret;
}
/* -------------------------------------------------------------*/

 int parsehex(UCHAR c)
{
   if (c >= '0' && c <= '9') return ( c - '0');
   if (c >= 'A' && c <= 'F') return ( c - 'A' + 10);
   if (c >= 'a' && c <= 'f') return ( c - 'a' + 10);
   return 0;
}
