/***************************************************************************
                          miscfun.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <sys/time.h>

#include <QString>
#include <QRegExp>

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

int QString2IntEx( const QString &s, bool *ok )
{
  bool r_ok;
  int v = s.toInt( &r_ok, 0 ); // first try as simple int
  if( ! r_ok ) {
    if( s == "RND" ) {
      v = rand(); r_ok = true;
    } else if( s == "IMIN" ) {
      v = IMIN; r_ok = true;
    } else if( s == "IMAX" ) {
      v = IMAX; r_ok = true;
    }
  }
  if( ok != nullptr )
    *ok = r_ok;
  return v;
}



double get_real_time()
{
  double t;
  timeval tv;
  gettimeofday( &tv, 0 );
  t = (double)( tv.tv_sec ) + 1e-6 * (double)( tv.tv_usec );
  return t;
}

NameType splitName( const QString &name, QString &first, QString &rest, int &idx )
{
  QRegExp re( RE_NAME_IDX );
  if( re.indexIn( name ) != -1 ) {
    first = re.cap(1); rest = QString();
    idx = re.cap(2).toInt();
    return simpleName;
  }
  QRegExp ref( RE_FULLNAME );
  if( ref.indexIn( name ) != -1 ) {
    first = ref.cap(1); rest = ref.cap(2);
    idx = 0; // no array of objects for now
    return complexName;
  }
  first = rest = QString(); idx = 0;
  return badName;
}


int isGoodName( const QString &s )
{
  QRegExp re( RE_NAME );
  if( re.indexIn( s ) == -1 )
    return 0;
  return 1;
}

QString flags2str( int fl ) {
  static const char *lbl[elm_known_flags+1]
     { "MR", "NRC", "ND", "ROD", "NS", "RO" , "Im", "?1" };
  QString s = "";
  for( int j=0; j<elm_known_flags; j++ ) {
    if( fl & ( 1 << j ) ) {
      s += lbl[j]; s += ",";
    };
  };
  return s;
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
  if( x0 < x0*(numeric_limits<double>::min()) )
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



int DatasInfo::dump( QTextStream& os, const QString &delim )
{
  int no = 0;
  int dl_sz0 = delim.size() + 2, dl_sz = dl_sz0 - 1;
  os << "#";
  for( auto lbl : labels ) {
    os << qSetFieldWidth( prec+6 ) << lbl << qSetFieldWidth( dl_sz ) << delim;
    dl_sz = dl_sz0; // to more actions, but less then 'if'
  }
  os << endl;
  os.setRealNumberPrecision( prec );

  for( int i=0; i<nn; ++i ) {
    for( auto ve: ves ) {
      os << qSetFieldWidth( prec+6 ) << (*ve)[i] << qSetFieldWidth( dl_sz ) << delim;
    }
    os << endl;
    ++no;
  }
  return no;
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

double limitAngleDeg( double a )
{
  int ia = int( a / 360 );
  if( a < 0 ) { --ia; }
  a -= ia * 360;
  return a;
}

// ------------- miscelanios classes -----------------------------

TCircBuf::TCircBuf( unsigned nn ) :
  nb(nn), s(0), nf(0), ni(0), su(0),
  d( nb + 2, 0 )
{
  reset();
}

TCircBuf::~TCircBuf()
{
  reset();
}

void TCircBuf::reset()
{
  s = nf = ni = 0; su = 0;
}

void TCircBuf::add( double a )
{
  if( nf >= nb ) {
    su -= d[s];
  }

  d[s++] = a; su += a;
  if( s >= nb ) {
    s = 0;
  }
  if( nf < nb ) {
    nf++;
  }
  ni++;

  if( ni > recalc_after ) {
    (void) sumCalc();
  }
}

double TCircBuf::operator[]( int i ) const
{
  int j;
  if( i < 0 || (unsigned)i >= nf ) { // first comparison rejects bad values for second
    return 0;
  }
  j = s - i - 1;
  if( j < 0 ) {
    j += nb;
  }
  return d[j];
}


double TCircBuf::sumCalc()
{
  su = 0; ni = 0;
  for( unsigned i=0; i<nf; ++i ) {
    su += d[i];
  }
  return su;
}


// ****************************************


