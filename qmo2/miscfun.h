/***************************************************************************
                          miscfun.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/



#ifndef __MISCFUN_H
#define __MISCFUN_H

#include <QString>
#include "defs.h"


const int max_graphs = 7; // TODO: variable
/** Structure containg data needed to plot graphs, export, view.
 *  filled by fillGraphInfo in TOutArr and TGraph
 * */

struct GraphInfo {
  int row = 0;
  int col = 0;
  int ny = 0;
  QString title = "";
  QString label[max_graphs] = { "", "", "", "", "", "", "" };
  const dvector *dat[max_graphs] =
  { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

/** converts string to int with some special values: RND, IMIN, IMAX */
int QString2IntEx( const QString &s, bool *ok = nullptr );

/** gets real time in seconds.ms */
double get_real_time(void);

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

/** fourier tranform TODO: use fft */
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
   double su;
   std::vector<double> d;
};


#endif




