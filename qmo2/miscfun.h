/***************************************************************************
                          miscfun.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/



#ifndef __MISCFUN_H
#define __MISCFUN_H

#include <fstream>
#include <QString>
#include "defs.h"
using namespace std;


const int max_graphs = 7; // TODO: variable
/** Structure containg data needed to plot graphs, export, view.
 *  filled by fillGraphInfo in TOutArr and TGraph
 * */

struct GraphInfo {
  int row;
  int col;
  int ny;
  char title[MAX_LABELLEN];
  char label[max_graphs][MAX_LABELLEN]; 
  const double *dat[max_graphs];
};

// ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz

const char _name_chars[] = 
  "0123456789abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char _name_chars1[] = 
  "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char _space_chars[] = " \t\b\f";
const char _twochar_chars[] = "\r\n\t\f\b\a\x1B\\";
const char _twochar_c2[] = "rntfbae\\";

enum { ctpSpace=0, ctpName=1, ctpOneChar=2, ctpTwoChar=3 };
enum { ltpUnk=0, ltpComment=1, ltpStruct=2, ltpEnd=3, ltpStart=4, 
       ltpVal=5, ltpValStart=6, ltpStr=7 };

/** hash value for string */
int hashVal( const char* astr );
/** is char c belong type ctp (ctpSpace,ctpName,ctpOneChar,ctpTwoChar) */
int isCharType( int c, int ctp, int start );
/** gets real time in seconds.ms */
double get_real_time(void);

/** return type of line: ltpComment, ltpStruct .. 
     v1 - filled if name detected (MAX_NAMELEN) ltpVal, ltpStart 
     v2 - filled if value detected (maxlen) ltpVal, ltpStr, ltpValStart; 
          len->*ival; if real_len > maxlen - ltpUnk returs
     if ltpStart id->*ival, className->v1
     it is safe to use 0-ptr in arguments  */
int typeOfLine( const char *s, int maxlen, int *ival, char *v1, QString *v2 );

/** computes lentgh of string in out stream: count \n \t as 2, \x2F as 4 */
int strQuLen( const char *s );
/** converts string to output-safe representation */
int quoteString( const char *src, char *dst, int ml );
/** saves string, selecting apropriate method */
int saveStr( ostream *os, const char *s );
/** saves one-line string in double quotes */
int saveStr0( ostream *os, const char *s );
/** saves multi-line string with delimiters, each line in double quotes */
int saveStr1( ostream *os, const char *s );
/** restores original representation of string after reading from stream */
int deQuoteString( const char *src, char *dst, int maxlen );
/** converts good part of src to pritable form, returns n of used chars */
int quoteTo( const char *src, char *dst, int ml );
/** converts QChar to QString quote-aware representation */
QString quoteChar( QChar c );
/** converts QString to xxxx \t \n \\ \" \xFF  representation w/o " "*/ 
QString quoteString( const QString &s );
/** reads multiline string up to delim, but no more then ml char with 0 */
int readMlStr( istream *is, QString* buf, int ml, const char *delim );
/** splits string like aa.b.cc.d to first="aa" and next="b.cc.d"
    max sizes: name,next: MAX_INPUTLEN; first: MAX_NAMELEN
    @returns: 0 -- both parts, 1 -- only first, -1 -- bad line  */
int splitName( const char *name, char *first, char *next );
/** safe strlen -- returns 0 if s == 0 */
int strlen0( const char *s );
/** safe and limited strdup -- if s == 0 alloc "", alloc no more than given */
char* strndup( const char *s, int maxlen );
/** checks if name syntax correct aaa_bbb2 */
int isGoodName( const char *s );
int isGoodName( const QString &s );
QString flags2str( int fl );

/** signum function y = -1:0:1 */
double sign( double x );
/** Heaviside */
double thetta( double x );
double deadLine( double x, double x0 );
double limitLine( double x, double x0 );
double deadLimitLine( double x, double x0, double x1, double a );
double triangleLine( double x, double x0 );
double rectLine( double x, double xl, double xr );
double threeStateLine( double x, double x0 );
double waveWave( double x );
double waveMhat( double x );

/** obsoleted: random [0;a]*/
// double drand( double a );
/** obsoleted:  central random [-a;a] */
// double drandc( double a );
/** fourier tranform */
int  fourier( int n , double l, const double *v,
               int m, double *a, double *b );
/** find index with minimal fabs(a[i]-v) */
int findNearIndex( int n, const double *a, double v );
/** find index nearest to given index with local minimum */
int findNearMin( int n, const double *a, int sidx );
/** find index nearest to given index with local maximum */
int findNearMax( int n, const double *a, int sidx );
/** find index of global minimum */
int findGlobalMin( int n, const double *a );
/** find index of global maximum */
int findGlobalMax( int n, const double *a );
/** euclid range */
double euRange( double dx, double dy );
/** computes length of perpendicular from point p to line s-e */
double perpLen( double xs, double ys, double xe, double ye, 
                double xp, double yp );

/** dumps data to given file, possibly with labels */
int dumpDatas( const char *fn, const GraphInfo *gi, char delim = ' ' );
/** exports data to given files suitable for gnuplot */
int gnuplotDatas( int otp, const GraphInfo *gi, 
     const char *fn, const char *eps_fn, const char *dat_fn );

// ------------- miscelanios classes -----------------------------

class TCircBuf {
 public:
   explicit TCircBuf( int nn );
   ~TCircBuf();
   void reset(void);
   void add( double a );
   int getN(void);
   double operator[]( int i );
   double sum( int force = 0 );
 protected:
   int nb, s, nf, ni;
   double *d, su;
};


#endif




