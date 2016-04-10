#pragma once
/***************************************************************************
                          miscfun.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/




#include <QString>
#include <QStringList>
#include <QTextStream>
#include "defs.h"

class HolderData;

//* describe data arrays for dump, show....
struct DatasInfo {
  long nn = 0; //* total number of data: minimal of present arrays
  long nx = 0; //* x-dimension, used only in 1 array output
  long ny = 1; //* y-dimension, the same
  QStringList labels; //* labels of arrays
  std::vector<const dvector*> ves; //* prts to arrays itself
  QString title; //* obviosly, used for display
  void reset() { nn = nx = 0; ny = 1; labels.clear(); ves.clear(); title = QString(); }
  long size() const { return ves.size(); }
  int dump( QTextStream& os, const QString &delim = " " );
};

/** converts string to int with some special values: RND, IMIN, IMAX */
int  QString2IntEx(  const QString &s, bool *ok = nullptr );
long QString2LongEx( const QString &s, bool *ok = nullptr );

/** gets real time in seconds.ms */
double get_real_time();

enum NameType {
  simpleName = 1,
  complexName = 2,
  badName = -1
};

/** splits string like aa.b.cc.d to first="aa" and rest="b.cc.d"
    @returns: complexName -- both parts, simpleName -- only first, -1 -- bad line
    fills idx in SIMLE! name contains [N] part*/
NameType splitName( const QString &name, QString &first, QString &rest, int &idx );
/** checks if name syntax correct aaa_bbb2 */
int isGoodName( const QString &s );
QString flags2str( int fl );

/** signum function y = -1:0:1 */
inline double sign( double x ) {  return ( x>0 ) ? 1 : ( ( x<0 ) ? -1 : 0 ); };
/** Heaviside */
inline double thetta( double x ) { return ( x>0 ) ? 1 : 0; };
inline double posval( double x ) { return ( x>=0 ) ? x : 0; };
inline bool isInBounds(   double l, double x, double r ) { return (x>=l) && (x<=r); };
inline bool isInBounds(      int l,    int x,    int r ) { return (x>=l) && (x<=r); };
inline bool isInBounds(     long l,   long x,   long r ) { return (x>=l) && (x<=r); };
inline bool isInBoundsNE( double l, double x, double r ) { return (x>l)  && (x<r);  };
inline bool isInBoundsNE(    int l,    int x,    int r ) { return (x>l)  && (x<r);  };
inline bool isInBoundsNE(   long l,   long x,   long r ) { return (x>l)  && (x<r);  };
inline double vBound(   double l, double x, double r )
  { auto t = x; if( t > r ) { t=r; }; if( t<l ) { t=l;}  return t; };
inline int    vBound(      int l,    int x,    int r )
  { auto t = x; if( t > r ) { t=r; }; if( t<l ) { t=l;}  return t; };
inline long    vBound(     long l,   long x,   long r )
  { auto t = x; if( t > r ) { t=r; }; if( t<l ) { t=l;}  return t; };
template< typename T > bool isGoodIndex( long idx, const T& container )
  {
    static_assert( sizeof(long) >= sizeof(typename T::size_type), "index size is incompatible to long!");
    return (idx>=0) && idx<(long)container.size();
  }
template< typename T >
const typename T::value_type atd( const T& container, typename T::size_type idx, const typename T::value_type &def )
  {
    if( idx < 0 && idx >= container.size() ) {
      return def;
    }
    return container[idx];
  }
double deadLine( double x, double x0 );
double limitLine( double x, double x0 );
double deadLimitLine( double x, double x0, double x1, double a );
double triangleLine( double x, double x0 );
double rectLine( double x, double xl, double xr );
double threeStateLine( double x, double x0 );
double waveWave( double x );
double waveMhat( double x );

double limitAngleDeg( double a ); // limits angle to [0;360)

void beautifyScale( double &a, double &b ); // make gooa scale for plot

//* implementation in tex2label.cpp
QString tex2label( const QString &t, bool noSub = false );

/** computes length of perpendicular from point p to line s-e */
// double perpLen( double xs, double ys, double xe, double ye,
//                 double xp, double yp );

class QDomNode;
QString getDomText( QDomNode &p );

// regexp to subsitute text in plot labels @{schems.main_s.a0.a:10:5}
const constexpr char* SUBST_RE { "@\\{([a-zA-z0-9_.%:]+)\\}" };
QString substValues( const QString &s, const HolderData *ho );

// ------------- for iface ---------------------------------------

void handleError( QWidget* par, const QString &s );
void handleWarn( QWidget* par, const QString &s );
bool confirmDelete( QWidget *par, const QString &obj, const QString &nm );
bool editObj( QWidget *par, HolderData *obj );
bool showTree( QWidget *par, HolderData *obj );

QString getClipboardStr();
bool    setClipboardStr( const QString &s );





