/***************************************************************************
                          toutarr.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2015 by atu
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

#include <gsl/gsl_statistics.h>

#include <QBrush>

#include "miscfun.h"
#include "toutarr.h"
#include "tmodel.h"


const char* TOutArr::helpstr = "<H1>TOutArr</H1>\n"
 "Collector of output during simulation.\n"
 "Stores output from element, given by name\n"
 "Type selects when output be collected.";

STD_CLASSINFO(TOutArr,clpSpecial);

CTOR(TOutArr,TDataSet)
{
}

TOutArr::~TOutArr()
{
  n = nx = ny = 0;
}

QVariant TOutArr::dataObj( int col, int role ) const
{
  if( role == Qt::BackgroundRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    if( so == &fake_so && type != OutArrType::outSpec ) {
      return QBrush( QColor(254,128,128) ) ;
    }
    if( n < 1 ) {
      return QVariant();
    }
    if( n == arrsize ) {
      return QBrush( QColor(0,254,0) ) ;
    }
    return QBrush( QColor(64,64,128) ) ;
  }

  else if ( role == Qt::ToolTipRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    QString s = name.cval() % " [" % QSN(n) % "]\n["
        % QSN(nx) % " x " % QSN(ny) % "]";
    return s;
  }

  return TDataSet::dataObj( col, role );
}

QIcon TOutArr::getIcon() const
{
  int xtype = type;
  if( xtype >= 4 ) { // bad type
    xtype = 4;
  }
  QString iconName = QString( ":icons/elm_toutarr_" ) + QSN(xtype) + ".png";
  QIcon el_ico( iconName );
  return el_ico;
}



int TOutArr::alloc( int anx, int any )
{
  if( type != OutArrType::outSpec ) {
    return 0;
  }
  if( any < 1 ) {
    any = 1;
  }
  if( anx < 1 ) {
    anx = 1;
  }

  nx = anx; ny = any; arrsize = nx * ny;
  n = 0; dmin = 0; dmax = 1;
  arr.resize( arrsize );
  n = 0; dmin = 0; dmax = 1; cnq = 0;
  so = nullptr;
  reset_stat();
  return 1;
}


void TOutArr::do_reset()
{
  n = 0; cnq = 0;
  reset_stat();

  // TODO: move to separate functions and call hele and after struct changed ???
  so = nullptr;
  TModel *mod = getAncestorT<TModel>();
  ltype_t lt; const TDataSet *so_ob;
  if( mod  &&  type != OutArrType::outSpec ) {
    so = mod->getSchemeDoublePtr( name, &lt, &so_ob, 0 );
  }
  if( !so ) {
    so = &fake_so; // TODO: indicate this
  }
}

int TOutArr::take_val()
{
  if( type != OutArrType::outSimple ) {
    return 0;
  }
  put_next_val();
  return n;
}

int TOutArr::preRun( int /*run_tp*/, int an, int anx, int any, double /*adt*/ )
{
  reset();
  if( any < 1 ) {  any = 1;  }
  if( anx < 1 ) {  anx = 1;  }
  if( nq  < 1 ) {  nq  = 1;  }

  arrsize = 0;
  switch( type ) {
    case OutArrType::outSimple:
      arrsize = an / nq; nx = an; ny = 1;
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

int TOutArr::postRun( int /*good*/ )
{
  calc_stat(); // calc even in case of non-full array
  return 1;
}

int TOutArr::startLoop( int acnx, int acny )
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
      if( acnx == 0 && acny == 0 ) { // not much sense, as reset at preRun
        reset();
      }
      break;
    default:
      break;
  }

  return 1;
}

int TOutArr::endLoop( int /*acnx*/, int /*acny*/ )
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
  int n;
  DatasInfo di;
  n = fillDatasInfo( &di );
  if( !n ) {
    return 0;
  }

  QFile of( fn );
  if( ! of.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
    DBGx( "warn: fail to open file \"%s\"", qP( fn ) );
    return 0;
  }
  QTextStream os( &of );

  n = di.dump( os, delim );
  return n;
}

void TOutArr::put_next_val()
{
  add( *so );
}

double TOutArr::at( int i ) const
{
  if( i >=n || i<0 ) {
    return 0;
  }
  return arr[i];
}

double TOutArr::at( int x, int y ) const
{
  return at( x + y * nx );
}

void TOutArr::put( int i, double v )
{
  if( i >=n || i<0 ) {
    return;
  }
  arr[i] = v;
  need_calc_stat = true;
}

void TOutArr::put( int x, int y, double v )
{
  return put( x + y*nx, v );
}

void TOutArr::add( double v )
{
  if( n >= arrsize ) {
    return;
  }

  if( cnq == lnq ) {
    arr[n] = v;
    n++;
    need_calc_stat = true;
  };
  cnq++;
  if( cnq >= nq ) {
    cnq = 0;
  }
  if( n == arrsize ) {
    calc_stat();
  }

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

  for( int i=0; i<n; ++i ) {
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
