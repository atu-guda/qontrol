/***************************************************************************
                          toutarr.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cmath>
#include <gsl/gsl_statistics.h>

#include <QBrush>

#include "miscfun.h"
#include "toutarr.h"
#include "tmodel.h"

using namespace std;


const char* TOutArr::helpstr = "<H1>TOutArr</H1>\n"
 "Collector of output during simulation.\n"
 "Stores output from element, given by name\n"
 "Type selects when output be collected.";

STD_CLASSINFO(TOutArr,clpSpecial);

CTOR(TOutArr,LinkedObj)
{
  // needReadInputsRecurse = true;
}

QVariant TOutArr::dataObj( int col, int role ) const
{
  const QString &nm = name.getDataD( QSL("source"), QSL("") ) ; //.cval();
  if( role == Qt::BackgroundRole ) {
    if( col != 0 ) {
      return TDataSet::dataObj( col, role );
    }
    if( !isfin ) {
      return QBrush( QColor(254,0,0) ) ;
    }
    if( name.getLinkType() == LinkBad && type != OutArrType::outSpec ) {
      return QBrush( QColor(254,128,128) ) ;
    }

    bool isPrm =  ( nm == QSL("t") || nm == QSL("prm0") || nm == QSL("prm1")  );
    if( n < 1 ) {
      if( isPrm ) {
        return QBrush( QColor( 120,220,252 ).darker( 130 ) );
      }
      return TDataSet::dataObj( col, role );
    }
    QColor bgc{ 64, 64, 128 }; // not filled to top
    if( n == arrsize ) {
      bgc = QColor( 0, 255, 0 );
    }
    if( isPrm ) {
      bgc = bgc.darker( 130 );
    }
    return QBrush( bgc );
  }

  else if ( role == Qt::ToolTipRole ) {
    if( col != 0 ) {
      return TDataSet::dataObj( col, role );
    }
    return textVisual();
  }

  else if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
    if( col > 1 ) {
      return TDataSet::dataObj( col, role );
    }
    QString s = objectName() % QChar(0x21A2) %  nm;
    return s;
  }

  return TDataSet::dataObj( col, role );
}

QString TOutArr::textVisual() const
{
  int idx = getMyIndexInParent();
  QString s = QSL("(") % QSN(idx) % QSL(") ") % name.getDataD( QSL("source"), QSL("?") ) % QSL(" [") % QSN(n)
    % QSL("] [")  % QSN(nx) % " x " % QSN(ny) % QSL("]");
  return s;
}

QIcon TOutArr::getIcon() const
{
  int xtype = type;
  if( xtype > OutArrType::outSpec ) { // bad type
    xtype = OutArrType::outSpec + 1;
  }
  QString iconName = QSL(":icons/elm_toutarr_") % QSN(xtype) + QSL(".png");
  QIcon el_ico( iconName );
  return el_ico;
}



int TOutArr::alloc( long anx, long any )
{
  if( type != OutArrType::outSpec ) {
    qWarning() << "Try to alloc ordinary array " << NWHE;
    return 0;
  }
  if( any < 1 ) {
    any = 1;
  }
  if( anx < 1 ) {
    anx = 1;
  }

  nx = anx; ny = any; arrsize = nx * ny;
  n = 0;
  arr.resize( arrsize );
  reset_stat();
  return 1;
}


void TOutArr::do_reset()
{
  n = 0; cnq = 0;
  isfin = 1;
  reset_stat();

  set_link();
}

void TOutArr::do_structChanged()
{
  set_link();
}

void TOutArr::set_link()
{
  enable.set_link();
  name.set_link();
}

long TOutArr::take_val()
{
  if( type != OutArrType::outSimple ) {
    return 0;
  }
  ct = *p_t_model;
  if( ct >= t_s && ct <= t_e ) {
    put_next_val();
  }
  return n;
}

int TOutArr::do_preRun()
{
  reset();
  int any = rinf->ny; int anx = rinf->nx; int an = rinf->N;
  if( any < 1 ) {  any = 1;  }
  if( anx < 1 ) {  anx = 1;  }
  if( nq  < 1 ) {  nq  = 1;  }

  arrsize = 0;
  switch( type ) {
    case OutArrType::outSimple:
      {
      double r_t_s = rinf->t_0;
      double r_t_e = r_t_s + rinf->T;
      r_t_s = max( r_t_s, t_s.cval() );
      r_t_e = min( r_t_e, t_e.cval() );
      if( r_t_e < r_t_s ) { r_t_e = r_t_s + D_EPS; }
      double fill_k = ( r_t_e - r_t_s ) / rinf->T;
      arrsize = long( ceil( an * fill_k / nq )); nx = arrsize; ny = 1;
      }
      break;
    case OutArrType::outParm1:
      arrsize = anx / nq; nx = anx; ny = 1;
      break;
    case OutArrType::outParm2:
      arrsize = anx * any / nq; nx = anx; ny = any;
      break;
    default:
      break;
  }
  arr.resize( arrsize );

  return 1;
}

int TOutArr::do_postRun( int /*good*/ )
{
  calc_stat(); // calc even in case of non-full array
  return 1;
}

int TOutArr::do_startLoop( long acnx, long /*acny*/ )
{
  switch( type ) {
    case OutArrType::outSimple:
      reset();
      break;
    case OutArrType::outParm1:
      if( acnx == 0 ) {
        reset();
      }
      break;
    case OutArrType::outParm2:
      // reset at preRun
      break;
    default:
      break;
  }

  return 1;
}

int TOutArr::do_endLoop()
{
  switch( type ) {
    case OutArrType::outSimple:
      break;
    case OutArrType::outParm1:
      put_next_val();
      break;
    case OutArrType::outParm2:
      put_next_val();
      break;
    default:
      break;
  }

  return 1;
}


int TOutArr::fillDatasInfo( DatasInfo *di ) const
{
  if( !di ) {
    return 0;
  }
  di->reset();
  di->title = objectName();
  di->labels << label.cval();
  (di->ves).push_back( &arr );
  if( n < 1 ) {
    return 0;
  }
  di->nn = n; di->ny = (ny > 0) ? ny: 1;
  di->nx = nx;
  return di->nn;
}

int TOutArr::dump( const QString &fn, const QString &delim )
{
  DatasInfo di;
  int n_filled = fillDatasInfo( &di );
  if( !n_filled ) {
    return 0;
  }

  QFile of( fn );
  if( ! of.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
    qWarning() << "fail to open file "<< fn << NWHE;
    return 0;
  }
  QTextStream os( &of );

  n_filled = di.dump( os, delim );
  return n_filled;
}


long TOutArr::importTxt( const QString &fn, int col, long n_start, long n_max, char sep )
{
  static const constexpr int buf_sz  = 1024*32;
  if( type != OutArrType::outSpec ) {
    qWarning() << "import allowed only to special arrays" << NWHE;
    return 0;
  }
  if( col < 0 ) {
    qWarning() << "Matrix read unimplemented now" << NWHE; // TODO: implement
    return 0;
  }
  QFile file( fn );
  if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qWarning() << "Fail to open data file " << file.fileName() << NWHE;
    return 0;
  }
  QByteArray lin;
  long skipped = 0;
  reset(); arr.resize( 0 ); arr.reserve( 1024 );

  for( long i=0; i<n_max && !file.atEnd(); ++i ) { // skip lines before
    lin = file.readLine( buf_sz ).simplified();
    if( lin.size() < 1 || lin[0] == '#' || lin[0] == ';' ) { // empty or comment
      continue;
    }
    if( skipped < n_start ) {
      ++skipped;
      continue;
    }
    double v = 0;
    QList<QByteArray> lba = lin.split( sep );
    if( col < lba.size() ) {
      v = toDoubleEx( lba[col] );
    }
    arr.push_back( v );
  }
  n = nx = arrsize = arr.size(); ny = 1;
  calc_stat();

  return n;
}

void TOutArr::put_next_val()
{
  name.readInput();
  enable.readInput();
  add( name.cval() );
}

double TOutArr::at( long i ) const
{
  if( i >=n || i<0 ) {
    return 0;
  }
  return arr[i];
}

double TOutArr::at( long x, long y ) const
{
  return at( x + y * nx );
}

double TOutArr::getLast() const
{
  if( n < 1 ) {
    return 0;
  }
  return arr[n-1];
}

double TOutArr::atT( double T ) const
{
  if( type != OutArrType::outSimple  ||  n < 1 ) {
    return 0;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    return 0;
  }
  double tdt = model->getDataD( "tdt", 1.0 );
  long idx = (long)( 0.5 + (T-t_s) / ( tdt * nq ) );
  return at( idx );
}

void TOutArr::put( long i, double v )
{
  if( i >=n || i<0 ) {
    return;
  }
  arr[i] = v;
  if( v != v /*!isfinite( v )*/ ) {
    isfin = 0;
  }
  need_calc_stat = true;
}

void TOutArr::put( long x, long y, double v )
{
  return put( x + y*nx, v );
}

void TOutArr::add( double v )
{
  if( n >= arrsize ) {
    qWarning() << " n (" << n << ") >= arrsize (" << arrsize << ") " << NWHE;
    return;
  }

  if( !enable.lval() ) {
    return;
  }

  if( cnq == lnq ) {
    arr[n] = v;
    ++n;
    if( /* !isfinite( v ) || */ (v != v) ) {
      isfin = 0;
    }
    need_calc_stat = true;
  };
  ++cnq;
  if( cnq >= nq ) {
    cnq = 0;
  }
  if( n == arrsize ) {
    calc_stat();
  }

}

void  TOutArr::transLin( double a, double b ) // every x: x*a+b
{
  for( auto &x : arr ) {
    x = x * a + b;
  }
  calc_stat();
}

long  TOutArr::fill( long nn, double start, double step )
{
  if( type != OutArrType::outSpec ) {
    qWarning() << "Filling is allowed only to special arrays" << NWHE;
    return 0;
  }
  alloc( nn, 1 ); n = nn;
  for( long i=0; i<nn; ++i ) {
    arr[i] = start + i *step;
  };
  calc_stat();
  return n;
}

void TOutArr::reset_stat()
{
  dmin = 0; dmax = 0.5; // bad, but safe values
  imin = imax = -1;
  s_x = s_x2 = a_x = a_x2 = acorr = var_x = sd_x = absdev_x = 0;
  need_calc_stat = true;
}

void TOutArr::calc_stat( bool force, bool forceAll )
{
  if( forceAll && acorr == 0 ) {
    force = true;
  }
  if( !force && !need_calc_stat ) {
    return;
  }
  reset_stat();
  dmin = DMAX; dmax = DMIN;

  for( long i=0; i<n; ++i ) {
    double v = arr[i];
    if( v < dmin ) {
      dmin = v; imin = i;
    }
    if( v > dmax ) {
      dmax = v; imax = i;
    }
    s_x += v;
    s_x2 += v*v;
  }
  a_x = s_x / n;  a_x2 = s_x2 / n;
  if( allStat || forceAll ) {
    acorr = gsl_stats_lag1_autocorrelation_m( arr.data(), 1, n, a_x );
    var_x =  gsl_stats_variance_m( arr.data(), 1, n, a_x );
    sd_x = sqrt( var_x );
    absdev_x =  gsl_stats_absdev_m( arr.data(), 1, n, a_x );
  }
  need_calc_stat = false;
}

QString TOutArr::getAllStats( QString sep ) const
{
  QChar sum( 0x03A3 ), mu( 0x03BC );
  QString s =
    QSL("n = ") % QSN( n ) % sep %
    QSL("min = ") % QSN( dmin ) % QSL(" at " ) % QSN( imin ) % sep %
    QSL("max = ") % QSN( dmax ) % QSL(" at " ) % QSN( imax ) % sep %
    sum % QSL("x = " ) % QSN( s_x ) % sep %
    sum % QSL("x") % QChar( 0x00B2 ) % QSL(" = " ) % QSN( s_x2 ) % sep %
    mu  % QSL("x = " ) % QSN( a_x ) % sep %
    mu  % QSL("x") % QChar( 0x00B2 ) % QSL(" = " ) % QSN( a_x2 ) % sep %
    QSL("Dx = ") % QSN( var_x ) % sep %
    QChar( 0x03C3) % QSL("x = ") % QSN( sd_x ) % sep %
    QSL("absdev(x) = ") % QSN( absdev_x ) % sep %
    QSL("Acorr(x) = ") % QSN( acorr );

  return s;
}

DEFAULT_FUNCS_REG(TOutArr)



// end of toutarr.cpp

