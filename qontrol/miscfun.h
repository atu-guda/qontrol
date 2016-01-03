/***************************************************************************
                          miscfun.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/



#ifndef __MISCFUN_H
#define __MISCFUN_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include "defs.h"

class HolderData;

//* describe data arrays for dump, show....
struct DatasInfo {
  int nn = 0; //* total number of data: minimal of present arrays
  int nx = 0; //* x-dimension, used only in 1 array output
  int ny = 1; //* y-dimension, the same
  QStringList labels; //* labels of arrays
  std::vector<const dvector*> ves; //* prts to arrays itself
  QString title; //* obviosly, used for display
  static constexpr const int prec = 12; //* magic prcision
  void reset() { nn = nx = 0; ny = 1; labels.clear(); ves.clear(); title = QString(); }
  int size() const { return ves.size(); }
  int dump( QTextStream& os, const QString &delim = " " );
};

/** converts string to int with some special values: RND, IMIN, IMAX */
int QString2IntEx( const QString &s, bool *ok = nullptr );

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
double deadLine( double x, double x0 );
double limitLine( double x, double x0 );
double deadLimitLine( double x, double x0, double x1, double a );
double triangleLine( double x, double x0 );
double rectLine( double x, double xl, double xr );
double threeStateLine( double x, double x0 );
double waveWave( double x );
double waveMhat( double x );

double limitAngleDeg( double a ); // limits angle to [0;360)

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

#endif




