/***************************************************************************
                          miscfun.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/



#ifndef __MISCFUN_H
#define __MISCFUN_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include "defs.h"



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

double limitAngleDeg( double a ); // limits angle to [0;360)

/** fourier tranform TODO: use fft */
int  fourier( int n , double l, const double *v,
               int m, double *a, double *b );
/** computes length of perpendicular from point p to line s-e */
// double perpLen( double xs, double ys, double xe, double ye,
//                 double xp, double yp );

// ------------- miscelanios classes -----------------------------

class TCircBuf {
 public:
   explicit TCircBuf( unsigned nn );
   TCircBuf( const TCircBuf &r ) = delete;
   ~TCircBuf();
   TCircBuf& operator=( const TCircBuf &r ) = delete;
   void reset();
   void add( double a );
   int getN() const { return nf; }
   double operator[]( int i ) const;
   double sum() const { return su; }
   double sumCalc(); // force recalc sum
 protected:
   unsigned nb; //* buffer size
   unsigned s;  //* start index
   unsigned nf; //* number of inserted points [0;nb-1]
   unsigned ni; //* number of insertion after sum recalc
   double su;   //* current sum
   std::vector<double> d;
   static const constexpr unsigned recalc_after = 10000;
};


#endif




