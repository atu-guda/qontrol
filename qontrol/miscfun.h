#pragma once
/***************************************************************************
                          miscfun.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2022 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/




#include <QString>
#include <QStringList>
#include <QTextStream>
#include "cmath"
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
  int dump( QTextStream& os, const QString &delim = QSL(" ") );
};

/** converts string to int with some special values: RND, IMIN, IMAX */
int  QString2IntEx(  const QString &s, bool *ok = nullptr );
long QString2LongEx( const QString &s, bool *ok = nullptr );

QString autoVisName( const QString &in_vis_name, const QString &objName );

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

const constexpr double oneMinusEm1 { 1.0 - 1.0/M_E };
const constexpr double eMinus1 { M_E - 1.0 };
const constexpr double logQaScale { oneMinusEm1 / M_LN2 };

/** signum function y = -1:0:1 */
inline double sign( double x ) {  return ( x>0 ) ? 1 : ( ( x<0 ) ? -1 : 0 ); }
/**  Heaviside */
inline double thetta( double x ) { return ( x>0 ) ? 1 : 0; }
inline double posval( double x ) { return ( x>=0 ) ? x : 0; }
// first power functions
inline double pow2( double x ) {  return x * x; }
inline double pow2s( double x ) {  return x * fabs(x); }
inline double pow3( double x ) {  return x * x * x; }
inline double pow4( double x ) {  return pow2( pow2 ( x ) ); }
inline double pow4s( double x ) {  return pow3( x ) * fabs( x ); }
inline double pow5( double x ) {  return pow2( x ) * pow3( x ); }
inline double pow6( double x ) {  return pow2( pow3( x ) ); }
inline double pow6s( double x ) {  return pow5( x ) * fabs( x ); }
inline double sqrtabs( double x ) {  return std::sqrt( std::fabs( x ) ); }
inline double sqrt0( double x ) {  return std::sqrt( posval( x ) ); }
inline double sinqa( double x ) {  return std::sin( std::sqrt( std::fabs( x ) ) ); }
inline double min3( double a, double b, double c ) {  return std::min( a, std::min( b, c ) ); }
inline double max3( double a, double b, double c ) {  return std::max( a, std::max( b, c ) ); }

// misc math functions
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

// limit checks
inline bool isInBounds(   double l, double x, double r ) { return (x>=l) && (x<=r); }
inline bool isInBounds(      int l,    int x,    int r ) { return (x>=l) && (x<=r); }
inline bool isInBounds(     long l,   long x,   long r ) { return (x>=l) && (x<=r); }
inline bool isInBoundsNE( double l, double x, double r ) { return (x>l)  && (x<r);  }
inline bool isInBoundsNE(    int l,    int x,    int r ) { return (x>l)  && (x<r);  }
inline bool isInBoundsNE(   long l,   long x,   long r ) { return (x>l)  && (x<r);  }
// bound to given region: use new std::clamp
// inline double vBound(   double l, double x, double r )
//   { auto t = x; if( t > r ) { t=r; }; if( t<l ) { t=l;}  return t; }
// inline int    vBound(      int l,    int x,    int r )
//   { auto t = x; if( t > r ) { t=r; }; if( t<l ) { t=l;}  return t; }
// inline long    vBound(     long l,   long x,   long r )
//   { auto t = x; if( t > r ) { t=r; }; if( t<l ) { t=l;}  return t; }
// bounds with EPS
inline double limitUpEps( double x, double lv )
  { return ( x < lv - D_EPS ) ? x : (lv - D_EPS); }
inline double limitDownEps( double x, double lv )
  { return ( x > lv + D_EPS ) ? x : (lv + D_EPS); }
// barriers:
double barrierHypUp(      double x, double lv ) noexcept;
double barrierHypDown(    double x, double lv ) noexcept;
double barrierHypUpUp(    double x, double lv ) noexcept;
double barrierHypUpDown(  double x, double lv ) noexcept;
double barrierHyp2Up(     double x, double lv ) noexcept;
double barrierHyp2Down(   double x, double lv ) noexcept;
double barrierHyp2UpUp(   double x, double lv ) noexcept;
double barrierHyp2UpDown( double x, double lv ) noexcept;
// TODO: log, tan

enum QualFunType { // keep in sync with TQuality, TQSearcher
   qaf_Gauss = 0, qaf_Para, qaf_Lin, qaf_Hyper, qaf_Log
};
double qF_fun( double q, double q_gamma, int tp, bool limitF = true );

struct MathConstantsC {
  const char * const name;
  double val;
};
using MapStrDouble = QMap<QString,double>;
extern const MathConstantsC mathConstantsC[];
double toDoubleEx( const QString &s, bool *ok = nullptr );
const QMap<QString,double>& getMathConstants();


// extremum quadtatic approximation funcs and data
struct QuadExtrIn {
  double x_l, x_c, x_r;
  double y_l, y_c, y_r;
  double lim_s, x_min, x_max;
  bool limitX, limitG;
};

struct QuadExtrOut {
  double x_e, y_e; // extremum point itself;
  double x_lt, x_rt, y_lt, y_rt, x_et, y_et; // relaTive to (x_c, y_c) coordinates
  double a_1, a_2;  // coeffs it x, x^2
  bool was_limited;
};

// calculation itself. returns true in some resoanable calculations ins done,
// even if limitations was done;
bool calcQuadExtr( const QuadExtrIn &in, QuadExtrOut &out );


// index checks
template< typename T > bool isGoodIndex( long idx, const T& container )
  {
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

struct TexCodeChar {
  const char *t;
  uint16_t c;
};
//* implementation in tex2label.cpp
QString texword2str( const QString &s, bool esc ); // esc: false: pi => QChar(PI), true: pi => \pi,
QString tex2label( const QString &t, bool noSub = false );

extern const TexCodeChar tex_cc[];

/** computes length of perpendicular from point p to line s-e */
// double perpLen( double xs, double ys, double xe, double ye,
//                 double xp, double yp );

class QDomNode;
QString getDomText( QDomNode &p );

// regexp to subsitute text in plot labels @{name.subname:width=64:format=.7g}
// @{schems.main_s.a0.a:10:12.8f}
// {obj_in_main_s.subobj}
// {V.obj_in_main_s.subobj} -- double value by getSchemeDoublePtr
const constexpr char* SUBST_RE { "@\\{([a-zA-z0-9_.%:]+)\\}" };
QString substValues( const QString &s, const HolderData *ho );

//* split name like aaa_0, bbb123 to base aaa_, bbb and number
//* @returns number, if exists, or -1 (in this case bname = name)
int splitNameNum( const QString &nm, QString &bname );

// ------------- for iface ---------------------------------------

// void handleError( QWidget* par, const QString &s );
// void handleWarn( QWidget* par, const QString &s );
bool confirmDelete( QWidget *par, const QString &obj, const QString &nm );
bool editObj( QWidget *par, HolderData *obj );
bool showTree( QWidget *par, HolderData *obj );

#define HANDLE_ERROR(s) { \
  qCritical() << (s) << WHE; \
  if( ! prog_opts.batch ) { \
    QMessageBox::critical( this, PACKAGE ": Error", (s), QMessageBox::Ok ); \
  } \
}

#define HANDLE_ERROR_W(w,s) { \
  qCritical() << (s) << WHE; \
  if( ! prog_opts.batch ) { \
    QMessageBox::critical( (w), PACKAGE ": Error", (s), QMessageBox::Ok ); \
  } \
}

#define HANDLE_WARN(s) { \
  qWarning() << (s) << WHE; \
  if( ! prog_opts.batch ) { \
    QMessageBox::critical( this, PACKAGE ": Error", (s), QMessageBox::Ok ); \
  } \
}

QString getClipboardStr();
bool    setClipboardStr( const QString &s );





