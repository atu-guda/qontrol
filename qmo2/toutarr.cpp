/***************************************************************************
                          toutarr.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2014 by atu
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
  n = ny = 0;
}

QVariant TOutArr::dataObj( int col, int role ) const
{
  if( role == Qt::BackgroundRole ) {
    if( col != 0 || n < 1 ) {
      return QVariant();
    }
    if( n == arrsize ) {
      return QBrush( QColor(0,254,0) ) ;
    }
    return QBrush( QColor(0,64,128) ) ;
  }

  else if ( role == Qt::ToolTipRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    QString s = name.cval() + " [" + QSN(n) + "]";
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



int TOutArr::alloc( int sz, int a_ny )
{
  if( sz == arrsize ) {
    n = 0; dmin = 0; dmax = 1; state = 2;
    return 0;
  };
  ny = a_ny;
  if( sz < 1 ) sz = 1;
  if( ny < 1 ) ny = 1;
  arr.resize( sz );
  arrsize = sz; n = 0; dmin = 0; dmax = 1; cnq = 0;
  so = nullptr;
  TModel *mod = getAncestorT<TModel>();
  ltype_t lt; const TDataSet *so_ob;
  if( mod ) {
    so = mod->getSchemeDoublePtr( name, &lt, &so_ob, 0 );
  }
  if( !so ) {
    so = &fake_so;
  }
  return 0;
}

int TOutArr::free()
{
  // no real delete
  arrsize = 0;  n = 0; state = 1;
  return 0;
}

int TOutArr::reset( int level )
{
  if( level >= type ) {
    n = 0; dmin = 0; dmax = 1; state = 2;
  };
  return 0;
}



int TOutArr::take_val( int level )
{
  double v = *so;
  if( n >= arrsize )
    return -1;
  if( level < type )
    return 0;
  if( cnq == lnq ) {
    if( n == 0 ) {
     dmin = dmax = v;
    } else {
      if( v > dmax ) dmax = v;
      if( v < dmin ) dmin = v;
    };
    arr[n] = v;
    n++;
  };
  cnq++;
  if( cnq >= nq )
    cnq = 0;
  return n;
}

int TOutArr::preRun( int /*run_tp*/, int an, int anx, int any, double /*adt*/ )
{
  n = 0; dmin = 0; dmax = 1; state = 2; // really reset
  if( any < 1 ) {
    any = 1;
  }
  if( anx < 1 ) {
    anx = 1;
  }

  arrsize = 0;
  switch( type ) {
    case OutArrType::outSimple:
      arrsize = an / nq; nx = an; ny = 1;
      break;
    case OutArrType::outParm1:
      arrsize = anx; nx = anx; ny = 1; // ignore nq here
      break;
    case OutArrType::outParm2:
      arrsize = anx * any; nx = anx; ny = any;
      break;
    default:
      break;
  }
  if( arrsize < 1 ) {
    arrsize = 1;
  }
  arr.resize( arrsize );

  so = nullptr;
  TModel *mod = getAncestorT<TModel>();
  ltype_t lt; const TDataSet *so_ob;
  if( mod ) {
    so = mod->getSchemeDoublePtr( name, &lt, &so_ob, 0 );
  }
  if( !so ) {
    so = &fake_so; // TODO: indicate this
  }

  return 1;
}

int TOutArr::postRun( int /*good*/ )
{
  return 1;

}

int TOutArr::startLoop( int acnx, int acny )
{

  return 1;
}

int TOutArr::endLoop( int acnx, int acny )
{
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

DEFAULT_FUNCS_REG(TOutArr)



// end of toutarr.cpp

