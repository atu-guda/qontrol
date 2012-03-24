/***************************************************************************
                          miscfun.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include <fstream>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <sys/time.h>
#include "../config.h"

#include <QString>

#include "miscfun.h"

using namespace std;

const char* getStateString( int stat_num )
{
  static const char * const statStr[] = {
    "Bad", "Good", "Done", "Run", "Pre", "Post", "??6", "??7", "???"
  };
  if( stat_num < 0 || stat_num > 8 )
    stat_num = 8;
  return statStr[ stat_num ];
}
      

int hashVal( const char* astr )
{
  int i, v, c;
  v = 0;
  for( i=0; (c=int(astr[i])) != 0; i++ ) {
    v += 17*c + c*c*c*3;
  };
  return v;
}

double get_real_time(void)
{
  double t;
  timeval tv;
  gettimeofday( &tv, 0 );
  t = (double)( tv.tv_sec ) + 1e-6 * (double)( tv.tv_usec );
  return t;
}  

int isCharType( int c, int ctp, int start )
{
  const char *s;
  switch( ctp ) {
    case ctpSpace: s = index( _space_chars, c );
                   return ( s == 0 ) ? 0 : 1;
    case ctpName:  if( start ) s = index( _name_chars1, c );
                         else s = index( _name_chars, c );
		   return ( s == 0 ) ? 0 : 1;	 
    case ctpOneChar: if( c & 0x80 ) return 1;
                     if( c < ' ' || c == '\\' || c == '"' || c == 127 ) 
		           return 0;
		     return 1;
    case ctpTwoChar: s = index( _twochar_chars, c );		        
                     if ( s == 0 ) return 0;
		     return 1 + int(  s - _twochar_chars );
  };
  return 0;
}

int typeOfLine( const char *s, int maxlen, int *ival, char *v1, QString *v2  )
{
  int i, j, k, l, l1, was_bs, id;
  char buf1[MAX_NAMELEN], buf2[MAX_INPUTLEN];
  char *eptr;
  l = strlen0( s );
  char c;
  if( l < 1 || l >= MAX_INPUTLEN ) return ltpComment;
  // skip leading spaces & tabs
  i = 0;
  while( i<l && isspace( s[i] ) ) i++;
  c = s[i];
  if( i >= l-1 ) return ltpComment;
  switch( c ) {
    case '@': return ltpStruct;  // structure definition comment
    case '}': return ltpEnd;     // end of object
    case '#': case ';': case '%': case '!': case '*': return ltpComment;
  };
  
  if( c == '"' ) {   // may be multi-line string
    was_bs = 0;
    for( j=i+1; j<l; j++ ) {
      c = s[j];
      if( was_bs ) { was_bs = 0; continue; }; // skip char after '\'
      if( c == '\\' ) { was_bs = 1; continue;  };
      if( c == '"' ) {
        k = deQuoteString( s+i, buf2, MAX_INPUTLEN ); 
        if( k != 0 ) return ltpUnk;
	l1 = strlen0( buf2 );
	if( ival != 0 ) *ival = l1;
	if( *v2 != 0 && l1 < maxlen ) 
	  *v2 = QString::fromLocal8Bit( buf2 ); 
        return ltpStr;
      }; 	
    };
    return ltpUnk;
  };
  
  if( isdigit( c ) ) {   // may be object start
    id = (int) strtol( s+i, &eptr, 10 );
    if( eptr == s+i || eptr[0] != '(' ) return ltpUnk;
    eptr++;
    l1 = strlen0( eptr );
    if( l1 < 4 ) return ltpUnk;
    c = eptr[0];
    if( !isCharType( c, ctpName, 1 ) ) return ltpUnk;
    buf1[0] = eptr[0]; buf1[1] = 0; k = 1;
    for( j=1; j<l1; j++ ) {
      c = eptr[j];
      if( ! isCharType( c, ctpName, 0 ) ) break;
      if( k >= MAX_NAMELEN-1 ) return ltpUnk;
      buf1[k++] = c; buf1[k] = 0;
    };
    if( c != ')' || j > l1-3 ) return ltpUnk;
    j++;
    while( j<l1 && isspace( eptr[j] ) ) j++;  // skip spaces
    if( j > l1-2 || eptr[j] != '=' ) return ltpUnk;
    j++;
    while( j<l1 && isspace( eptr[j] ) ) j++;  // skip spaces
    if( j > l1-1 || eptr[j] != '{' ) return ltpUnk;
    if( ival != 0 ) *ival = id;
    if( v1 != 0 ) { v1[0] = 0; strncat( v1, buf1, MAX_NAMELEN-1 ); };
    return ltpStart;
  };
  
  if( isCharType( c, ctpName, 1 ) ) {     // may be value
    if( i > l-3 ) return ltpUnk;
    k = 1; buf1[0] = c; buf1[1] = 0;
    for( j=i+1; j<l; j++ ) {              // find name
      c = s[j];
      if( ! isCharType( c, ctpName, 0 ) ) break;
      if( k >= MAX_NAMELEN-1 ) return ltpUnk;
      buf1[k++] = c; buf1[k] = 0;
    };
    if( j > l-2 ) return ltpUnk;
    i = j;
    while( i<l && isspace( s[i] ) ) i++;    // skip spaces
    if( i > l-2  || s[i] != '=' ) return ltpUnk;
    i++;
    while( i<l && isspace( s[i] ) ) i++;    // skip spaces
    if( i > l-1 ) return ltpUnk;
    if( s[i] == '@' ) {         // start of multi-line string
      k = deQuoteString( s+i, buf2, MAX_INPUTLEN );
      if( k != 0 ) return ltpUnk;
      l1 = strlen0( buf2 );
      if( l1 >= maxlen || l1 < 2 ) return ltpUnk;
      if( ival != 0 ) *ival = l1-1;
      if( v1 != 0 ) { v1[0] = 0; strncat( v1, buf1, MAX_NAMELEN-1 ); };
      if( v2 != 0 ) { *v2 = QString::fromLocal8Bit( buf2+1 ); v2->truncate( maxlen-1 ); };
      return ltpValStart;
    };
    k = deQuoteString( s+i, buf2, MAX_INPUTLEN );
    if( k != 0 ) return ltpUnk;
    l1 = strlen0( buf2 );
    if( l1 >= maxlen ) return ltpUnk;
    if( ival != 0 ) *ival = l1;
    if( v1 != 0 ) { v1[0] = 0; strncat( v1, buf1, MAX_NAMELEN-1 ); };
    if( v2 != 0 ) { *v2 = QString::fromLocal8Bit( buf2 ); v2->truncate( maxlen ); };
    return ltpVal;
  };
  return ltpUnk;
}


int strQuLen( const char *s )
{
  int i, l, ls;
  char c;
  if( s == 0 )
    return 0;
  l = strlen0( s ); ls = 0;
  for( i=0; i<l; i++ ) {
    c = s[i];
    if( isCharType( c, ctpOneChar, 0 ) ) { ls++; continue; };
    if( isCharType( c, ctpTwoChar, 0 ) ) { ls += 2; continue; };
    ls += 4;
  };
  return ls + 2;
}

int quoteString( const char *src, char *dst, int ml )
{
  int i, j, l, l1, k, d1, d2;
  char c;
  const char xdig[] = "0123456789ABCDEF???????????";
  if( ml < 3 ) return -1;
  l1 = strQuLen( src );
  if( l1+3 >= ml ) return -1;
  dst[0] = '"'; dst[1] = 0; j = 1;
  if( l1 == 0 ) { dst[j++] = '"'; dst[j] = 0;  return 0; };
  l = strlen0( src );
  if( l == 0 ) { dst[j++] = '"'; dst[j] = 0;  return 0; };
  for( i=0; i<l; i++ ) {
    c = src[i];
    if( isCharType( c, ctpOneChar, 0 ) ) { dst[j++] = c; continue; };
    if( ( k = isCharType( c, ctpTwoChar, 0 ) ) ) { 
      dst[j++] = '\\'; dst[j++] = _twochar_c2[k-1]; continue; 
    };
    dst[j++] = '\\'; dst[j++] = 'x'; 
    d1 = c & 0x0F; d2 = ((unsigned char)c) >> 4;
    dst[j++] = xdig[d2]; dst[j++] = xdig[d1];
  };
  dst[j++] = '"'; dst[j] = 0;
  return 0;
}

int quoteTo( const char *src, char *dst, int ml )
{
  int i, j, k, l, n, d1, d2;
  char c;
  const char xdig[] = "0123456789ABCDEF???????????";
  if( ml < 8 ) return -1;
  l = strlen0( src ); j = 0;
  dst[j++] = '"'; dst[j] = 0; n = 0;
  for( i=0; i<l; i++ ) {
    if( j >= ml-6 ) break;
    c = src[i];
    if( c == '\n' ) { dst[j++] = '\\'; dst[j++] = 'n'; n++; break; };
    if( isCharType( c, ctpOneChar, 0 ) ) { dst[j++] = c; n++; continue; };
    if( (k = isCharType( c, ctpTwoChar, 0 )) != 0 ) { 
      dst[j++] = '\\'; dst[j++] = _twochar_c2[k-1];  n++; continue; 
    };
    dst[j++] = '\\'; dst[j++] = 'x'; 
    d1 = c & 0x0F; d2 = ((unsigned char)c) >> 4;
    dst[j++] = xdig[d2]; dst[j++] = xdig[d1];  n++;
  };
  dst[j++] = '"'; dst[j] = 0;
  return n;
}

int saveStr( ostream *os, const char *s )
{
  int l, k;
  l = strQuLen( s );
  if( l < MAX_INPUTLEN - MAX_NAMELEN - 32 )
    k = saveStr0( os, s );
  else
    k = saveStr1( os, s );
  return k;    
}

int saveStr0( ostream *os, const char *s )
{
  int l1, k;
  char buf[MAX_INPUTLEN]; // limited length - all above be truncated
  l1 = strQuLen( s );
  if( l1 >= MAX_INPUTLEN - MAX_NAMELEN - 32 ) return -1;
  k = quoteString( s, buf, MAX_INPUTLEN - MAX_NAMELEN - 32 );
  if( k !=0 ) return k;
  *os << buf;
  return 0;
}

int saveStr1( ostream *os, const char *s )
{
  int i, k, l, ml, rnd1, rnd2;
  char buf[MAX_INPUTLEN];
  // char ter[MAX_INPUTLEN];
  rnd1 = rand(); rnd2 = rand();
  *os << "@-----" << rnd1 << "---" << rnd2 << "---cut_here-----\n";
  i = 0; l = strlen0( s ); ml = MAX_INPUTLEN - MAX_NAMELEN - 32;
  while( i < l ) {
    k = quoteTo( s+i, buf, ml );
    if( k < 1 ) return -1;
    *os << buf << '\n';
    i += k;
  };
  *os << "-----" << rnd1 << "---" << rnd2 << "---cut_here-----\n";
  return 0;
}


int deQuoteString( const char *src, char *dst, int maxlen )
{
  int i, j, l, is_quo;
  char c, c1, ch1, ch2;
  dst[0] = 0;
  if( maxlen > MAX_INPUTLEN ) maxlen = MAX_INPUTLEN;
  l = strlen0( src );
  if( l >= maxlen ) return -1;
  if( l == 0 ) return 0;
  j = -1;
  for( i=0; i<l; i++ ) {
    c = src[i];  
    if( ! isCharType( c, ctpSpace, 0 ) ) {
      j = i;
      break;
    };
  };
  if( j < 0 ) 
    return 0;
  is_quo = 0; i = j; j = 0;
  if( src[i] == '"' ) { i++; is_quo = 1; };
  if( is_quo == 0 ) {  // no quoting
    while( (c=src[i]) != 0 ) {
      if( j >= maxlen-2 ) { dst[j] = 0; return -2; };
      if( isspace(c) ) break;
      dst[j++] = c; i++;
    };
    dst[j] = 0;
    return 0;
  }; // end noquoted string 
  while( (c=src[i]) !=0 ) {
    if( j >= maxlen-2 ) { dst[j] = 0; return -2; };
    if( c == '"' ) {
      dst[j++] = 0; break;
    };
    if( (unsigned char)c < ' ' ) {
      dst[j] = 0; return -3;
    };
    if( c == '\\' ) {
       c1 = src[++i];
       if( i >= maxlen-2 ) {
         dst[j] = 0; return -2;
       };
       switch( c1 ) {
         case 'r': dst[j++] = '\r'; break;
         case 'n': dst[j++] = '\n'; break;
         case 't': dst[j++] = '\t'; break;
         case 'f': dst[j++] = '\f'; break;
         case 'e': dst[j++] = '\x1B'; break;
         case 'a': dst[j++] = '\a'; break;
	 case 'b': dst[j++] = '\b'; break;
	 case 'v': dst[j++] = '\v'; break;
         case '\\': dst[j++] = '\\'; break;
         case 'x':
           if( i >= l-3 ) { dst[j] = 0; return -4; };
           ch1 = src[++i]; ch2 = src[++i];
           if( !isxdigit(ch1) || !isxdigit(ch2 ) )
             { dst[j] = 0; return -5; };
           ch1 = (char) toupper( ch1 );
           ch2 = (char) toupper( ch2 );
           if( ch1 >= 'A' ) ch1 += - 'A' + 10;
             else           ch1 -= '0';
           if( ch2 >= 'A' ) ch2 += - 'A' + 10;
             else           ch2 -= '0';
           dst[j++] = char( ch2 + (((unsigned int)ch1)<<4));
           break;
         default: dst[j++] = c1;
       };
    } else {
      dst[j++] = c;
    };
    i++;
  };
  dst[j] = 0;
  return 0;
}

int readMlStr( istream *is, QString* buf, int ml, const char *delim )
{
  int j, k, l, lt;
  char bi[MAX_INPUTLEN];
  QString  bq;
  lt = strlen0( delim );
  if( lt < 1  ||  ml < 1 ) return -1;
  l = 0; *buf = QString();
  while( 1 ) {
    is->getline( bi, MAX_INPUTLEN );
    if( ! is->good() ) return -1;
    if( strncmp( delim, bi, lt ) == 0 ) break;
    k = typeOfLine( bi, MAX_INPUTLEN, &j, 0, &bq );
    if( k == ltpComment ) continue;
    if( k != ltpStr ) return -1;
    if( l+j >= ml ) return -1;
    *buf += bq ; l += j;
  };
  return 0;
}

/** splits string like aa.b.cc.d to first="aa" and next="b.cc.d"
    max sizes: name,next: MAX_INPUTLEN; first: MAX_NAMELEN
    @returns: 0 -- both parts, 1 -- only first, -1 -- bad line  */
int splitName( const char *name, char *first, char *next )
{
  char c;
  int i, l, j;
  first[0] = next[0] = 0; 
  l = strlen0( name );
  if( l < 1 || l >= MAX_INPUTLEN ) return -1;
  i = 0;
  while( (c=name[i]) != 0 && i<l ) {
    if( i >= MAX_NAMELEN ) { first[0] = 0; return -1; };
    if( i>0 && c == '.' ) break;
    if( ! isCharType( c, ctpName, (i==0) ? 1 : 0 ) )
       { first[0] = 0; return -1; };
    first[i++] = c;
  };
  first[i] = 0;
  if( c != '.' )
    return 1;
  j = 0; i++;
  while( (c=name[i]) != 0 && i<l ) {
    if( (! isCharType( c, ctpName, (j==0) ? 1 : 0 )) && c != '.' )
       { first[0] = 0; next[0] = 0; return -1; };
    next[j++] = c;
    i++;
  };
  next[j] = 0;
  return 0;
}

int strlen0( const char *s )
{
  if( s[0] == 0 )
    return 0;
  return strlen( s );
}

char* strndup( const char *s, int maxlen )
{
  int l;
  char *r;
  l = strlen0( s );
  if( l > maxlen ) l = maxlen;
  r = new char[ l+2 ];
  r[0] = 0;
  strncat( r, s, maxlen );
  return r;
}


int isGoodName( const char *s )
{
  int i, l;
  l = strlen0( s );
  if( l < 1 || l >= MAX_NAMELEN ) return 0;
  for( i=0; i<l; i++ ) {
    if( !isCharType( s[i], ctpName, i==0 ) )
      return 0;
  };
  return 1;
}

double thetta( double x )
{
  return ( x>0 ) ? 1 : 0;
}


double sign( double x )
{
  return ( x>0 ) ? 1 : ( ( x<0 ) ? -1 : 0 );
}

double deadLine( double x, double x0 )
{
  if( x > x0 )
    return ( x - x0 );
  if( x < -x0 )
    return ( x + x0 );
  return 0;
}

double limitLine( double x, double x0 )
{
  if( x0 < 1e-100 ) // TODO: dont use magic
    return 0;
  if( x > x0 )
    return 1;
  if( x < -x0 )
    return -1;
  return (x / x0);
}

double deadLimitLine( double x, double x0, double x1, double a )
{
  if( x > x1 )
    return (x1-x0) * a;
  if( x < -x1 )
    return -(x1-x0) * a;
  if( x > x0 )
    return (x-x0) * a;
  if( x < - x0 )
    return ( x + x0 ) * a;
  return 0;
}
  

double triangleLine( double x, double x0 )
{
  if( x0 < 1e-15 ) x0 = 1e-15;
  if( x > x0 || x < -x0 )
    return 0;
  if( x < 0 )
    x = -x;  
  return ( x0 - x ) / x0;
}

double rectLine( double x, double xl, double xr )
{
  return ( x >= xl && x <= xr ) ? 1 : 0;
}

double threeStateLine( double x, double x0 )
{
  if( x > x0 )
    return 1;
  if( x < -x0 )
    return -1;
  return 0;
}

double waveWave( double x )
{
  return x * exp( -0.5 * x * x );
}

double waveMhat( double x )
{
  double x2 = x * x;
  return ( 1 - x2 ) * exp( -0.5 * x2 );
}


/* obsoleted: use gsl random generators or TRand 
double drand( double a )
{
  return ( double( rand() ) / RAND_MAX ) * a;
}

double drandc( double a )
{
  return (( double( rand() ) / RAND_MAX ) - 0.5 ) * a;
}
*/


int  fourier( int n , double l, const double *v,
                     int m, double *a, double *b )
{
  int i , j;
  double tc , ts , t1 , t2 , h;
  h = l / n;
  for( i=0; i<m; i++ ) {
     tc = ts = 0; t1 = 2 * M_PI * i / l;
     for( j=0; j<n; j++ ) {
        t2 = t1 * j * h;
        tc += v[j] * cos( t2 );  ts += v[j] * sin( t2 );
     };
     a[i] = 2 * h * tc / l;  b[i] = 2 * h * ts / l;
  };
  return 0;
}

int findNearIndex( int n, const double *a, double v )
{
  int i, imin;
  double lmin, l;
  if( n < 1 ) return -1;
  if( n == 1 ) return 0;
  lmin = fabs( v - a[0] ); imin = 0;
  for( i=1; i<n; i++ ) {
    l = fabs( v - a[i] );
    if( l < lmin ) {
      lmin = l; imin = i;
    };
  };
  return imin;
}

int findNearMin( int n, const double *a, int sidx )
{
  int i, searchRight;
  double dm;
  if( n < 1 ) return -1;
  if( n == 1 || sidx < 0 || sidx >= n )
    return 0;
  
  dm = a[sidx];
  if( sidx == 0 ) {      // search from left point
    searchRight = 1;
  } else {
    if( sidx == n-1 ) { // search from right point
      searchRight = 0;
    } else {            // general case
      searchRight = 0;
      if( a[sidx+1] < dm )
	searchRight = 1;
    };
  };

  if( searchRight ) {
    for( i=sidx+1; i<n; i++ ) {
      if( a[i] > dm )
	return i-1;
      dm = a[i];
    };
    return n-1;
  };
  
  for( i=sidx-1; i>=0; i-- ) {
    if( a[i] > dm )
      return i+1;
    dm = a[i];
  };
  return 0;
}
  

int findNearMax( int n, const double *a, int sidx )
{
  int i, searchRight;
  double dm;
  if( n < 1 ) return -1;
  if( n == 1 || sidx < 0 || sidx >= n )
    return 0;
  
  dm = a[sidx];
  if( sidx == 0 ) {      // search from left point
    searchRight = 1;
  } else {
    if( sidx == n-1 ) { // search from right point
      searchRight = 0;
    } else {            // general case
      searchRight = 0;
      if( a[sidx+1] > dm )
	searchRight = 1;
    };
  };

  if( searchRight ) {
    for( i=sidx+1; i<n; i++ ) {
      if( a[i] < dm )
	return i-1;
      dm = a[i];
    };
    return n-1;
  };
  
  for( i=sidx-1; i>=0; i-- ) {
    if( a[i] < dm )
      return i+1;
    dm = a[i];
  };
  return 0;
}

int findGlobalMin( int n, const double *a )
{
  int i, idx;
  double dm;
  dm = a[0]; idx = 0;
  for( i=1; i<n; i++ ) {
    if( a[i] < dm ) {
      dm = a[i]; idx = i;
    };
  };
  return idx;
}

int findGlobalMax( int n, const double *a )
{
  int i, idx;
  double dm;
  dm = a[0]; idx = 0;
  for( i=1; i<n; i++ ) {
    if( a[i] > dm ) {
      dm = a[i]; idx = i;
    };
  };
  return idx;

}

double euRange( double dx, double dy )
{
  return sqrt( dx * dx + dy * dy );
}

int dumpDatas( const char *fn, const GraphInfo *gi, char delim )
{
  int i, j;
  ofstream os( fn );
  if( !os.good() ) return -1;
  os << "#";  // save sign for gnuplot input data
  for( j=0; j<gi->col; j++ ) {  // labels row
   os << setw( 12 ) << gi->label[j] << delim;
  };
  os << '\n';
  for( i=0; i<gi->row; i++ ) {  // data rows
    for( j=0; j<gi->col; j++ ) {
      os << setw( 12 ) << gi->dat[j][i] << delim;
    };
    os << '\n';
  };
  os.close();
  return 0;
}



// TODO: use structure for gnuplot options, call gnuplot
int gnuplotDatas( int otp, const GraphInfo *gi, 
     const char *fn, const char *eps_fn, const char *dat_fn )
{
  ofstream os, osd;
  int i, j;
  os.open( fn );
  if( !os.good() ) return -2;
  osd.open( dat_fn );
  if( !osd.good() ) return -2;
  os << "# generated by " PACKAGE ":\n";
  os << "# vim: ft=gnuplot\n";
  os << "set terminal ";
  if( otp ) {
    os << "postscript eps enhanced\nset output \x22" << eps_fn << "\x22\n"; 
  } else {
    os << "x11\n";
  };
  if( gi->title[0] == 0 ) {
    os << "set title \x22" << gi->title << "\x22; \n";
  };

  if( gi->ny != 1 ) { // 3D plot
    os << "set grid;\nset border;\n";
    os << "# set view 65, 30, 1.2, 1.2;\n";
    os << "# set view map;\n";
    os << "set contour base; # surface both \n";
    os << "# set cntrparam levels discrete 0.1,0.5,1\n";
    os << "set key right top;\n";
    os << "set hidden3d offset -1 trianglepattern 3;\n";
    os << "# set palette color rgbformulae 32,11,6 model RGB negative;\n";
    os << "# set pm3d at b;\n";
    os << "set label 2 \x22" << gi->label[0] << "\x22 at graph 1.06,-0.1,-0.5\n";
    os << "set label 3 \x22" << gi->label[1] << "\x22 at graph 1.06,1.06,-0.5\n";
    os << "splot \x22" << dat_fn << "\x22 ";
    for( j=2; j<gi->col; j++ ) {
      if( j != 2 )
	os << "  \x22\x22 ";
      os << " using 1:2:" << j+1;
      os << " title \x22" << gi->label[j] << "\x22 w l lt " << j-2;
      if( j < gi->col-1 )
	os << " , \\\n";
    };
  } else {
    os << "set grid lt 2 lw 1.1;\n";
    os << "set border;\nset mxtics 2;\nset mytics 2;\n";
    os << "#set key top right Right width -14 reverse;\n";
    os << "set label 2 \x22" << gi->label[0] << "\x22 at graph 1.02,0\n";
    os << "plot \x22" << dat_fn << "\x22 ";
    for( j=1; j<gi->col; j++ ) {
      if( j != 1 )
	os << "  \x22\x22 ";
      os << " using 1:" << j+1;
      os << " title \x22" << gi->label[j] << "\x22 w l lt " << j;
      if( j < gi->col-1 )
	os << " , \\\n";
    };
  };
  if( otp == 0 )
    os << "\npause -1;\n";
  os << "\n";
  os.close();
  // data file
  for( i=0; i<gi->row; i++ ) {
    if( gi->ny != 1  &&  (i % gi->ny) == 0 )  // 3D sblock separator
      osd << "\n";
    for( j=0; j<gi->col; j++ )
      osd << gi->dat[j][i] << ' ';
    osd << '\n';
  };
  osd.close();
  return 0;
}


double perpLen( double xs, double ys, double xe, double ye, 
                double xp, double yp )
{
  double p, r1, xf, yf;
  xe -= xs; ye -= ys; xp -= xs; yp -= ys;
  r1 = xe * xe + ye * ye;
  if( r1 == 0 ) 
    return 0;
  xf = xe * ( xe * xp + ye * yp ) / r1;
  yf = ye * ( xe * xp + ye * yp ) / r1;
  xf -= xp; yf -= yp;
  p = sqrt( xf * xf + yf * yf );
  return p;
}

// ------------- miscelanios classes -----------------------------

TCircBuf::TCircBuf( int nn )
{
  d = new double[ nn + 2 ]; nb = nn;
  reset();
}

TCircBuf::~TCircBuf()
{
  delete d; d = 0;
  reset();
}

void TCircBuf::reset(void)
{
  s = nf = ni = 0; su = 0;
}

void TCircBuf::add( double a )
{
  if( nf >= nb )
    su -= d[s];
  d[s++] = a; su += a;
  if( s >= nb ) 
    s = 0;
  if( nf < nb )  
    nf++;
  ni++;
  if( ni > 5000 )
    (void) sum( 1 );
}

double TCircBuf::operator[]( int i )
{
  int j;
  if( i < 0 || i >= nf ) return 0;
  j = s - i - 1;
  if( j < 0 ) 
    j += nb;
  return d[j];
}

int TCircBuf::getN(void)
{
  return nf;
}
  
double TCircBuf::sum( int force /* =0 */)
{
  int i;
  if( force ) {
    su = 0; ni = 0;
    for( i=0; i<nf; i++ )
      su += d[i];
  };
  return su;
}


// ****************************************


