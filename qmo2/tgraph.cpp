/***************************************************************************
                          tgraph.cpp  -  description
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


#include "miscfun.h"
#include "tgraph.h"
#include "tmodel.h"
#include "toutarr.h"


const char* GraphElem::helpstr = "<H1>GraphElem</H1>\n"
 "Defines one source and output type for onw  line";

STD_CLASSINFO(GraphElem,clpElem);

CTOR(GraphElem,TDataSet)
{
}

DEFAULT_FUNCS_REG(GraphElem);

// --------------------------------------------------------------------

const char* TGraph::helpstr = "<H1>TGraph</H1>\n"
 "Define, which outputs will be used for plotting and dumping. \n"
 "All outputs must be the same type";

STD_CLASSINFO(TGraph,clpSpecial);

CTOR(TGraph,TDataSet) ,
     scd( new ScaleData( "scd", this, 0, "scale", "scale data", "" ) )
{
  allowed_types = "GraphElem,GraphLabel,+SPECICAL";
}


int TGraph::fillDatasInfo( DatasInfo *di ) const
{
  if( !di ) {
    return 0;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    DBGx( "warn: not found model in \"%s\"", qP(getFullName()) );
    return 0;
  }

  di->reset();
  int nn = IMAX, ny = 0; // 0 is special: not found

  for( auto c : children() ) {
    GraphElem *ge = qobject_cast<GraphElem*>( c );
    if( ! ge ) {
      continue;
    }
    QString s;
    ge->getData( "src", s );
    if( s.isEmpty() ) {
      continue;
    }

    TOutArr *arr = model->getOutArr( s );
    if( !arr ) {
      continue;
    }
    int nn_c = 0;
    arr->getData( "n", &nn_c );
    if( nn_c < 1 ) {
      continue;
    }
    if( ny < 1 ) {
      arr->getData( "ny", &ny );
    }
    const dvector *ve = arr->getArray();
    if( !ve ) {
      continue;
    }
    if( nn_c < nn ) {
      nn = nn_c;
    }
    QString lbl = QString( "v_%1" ).arg( QSN( di->ves.size() ) );
    ge->getData( "label", lbl );
    di->labels.push_back( lbl );
    di->ves.push_back( ve );
  }

  if( di->ves.size() < 1 || nn < 1 ) {
    return 0;
  }
  if( ny < 1 ) { // unlikely
    ny = 1;
  }

  di->title = objectName();
  di->nn = nn; di->nx = nn / ny; di->ny = ny;

  return nn;
}


int  TGraph::dump( const QString &fn, const QString &delim )
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


// migration: TODO: remove after conversion
void TGraph::migrate1()
{
  QString s, lbl;
  s = xname.cval();

  GraphElem *ge = getElemT<GraphElem*>( "x" );
  if( ge ) { // already converted
    DBGx( "dbg: Graph \"%s\" already converted", qP(getFullName()) );
    return;
  }

  if( scd ) {
    scd->bgcolor = QColor( 255, 255, 255 );
  }

  TModel *model = getAncestorT<TModel>();
  if( !model  ) {
    DBGx( "warn: fail to find model from \"%s\"", qP(getFullName()) );
    return;
  }

  ge = addObj<GraphElem>( "x" );
  if( ge ) {
    lbl = "x";
    ge->setData( "type", GraphElem::DataType::DataAxisX );
    ge->setData( "src", s );
    ge->setParm( "sep", "block" );
    TOutArr *arr = model->getOutArr( s );
    if( arr ) {
      arr->getData( "label", lbl );
    } else {
      DBGx( "warn: not found array \"%s\" for converting X axis", qP( s ) );
    }
    ge->setData( "label", lbl );
  }

  GraphElem *gy = nullptr;
  int li_col; // really color

  for( int i=0; i<6; ++i ) {
    s = QString();
    QString nm_new = QStringLiteral( "y" ) + QSN(i);
    QString nm_old_s = QString( "y%1name" ).arg( i );
    QString nm_old_c = QString( "y%1color" ).arg( i );
    getData( nm_old_s, s );
    getData( nm_old_c, &li_col );
    // DBGx( "dbg: color for \"%s\" : %X", qP(nm_old_s), li_col );
    if( ( li_col & 0xFFFFFF  ) == 0xFFFFFF ) { // white->black
      li_col = 0;
    }
    li_col = QColor( QRgb(li_col) ).darker( 200 ).rgb();

    if( ! s.isEmpty() ) {
      gy = getElemT<GraphElem*>( nm_new );
      if( !gy ) {
        gy = addObj<GraphElem>( nm_new );
      }
      if( gy ) {
        gy->setData( "type", GraphElem::DataType::DataPlot );
        gy->setData( "src", s );
        lbl = nm_new;
        gy->setData( "color", li_col );
        TOutArr *arr = model->getOutArr( s );
        if( arr ) {
          arr->getData( "label", lbl );
        }
        gy->setData( "label", lbl );
      }
    }
  }

}

DEFAULT_FUNCS_REG(TGraph);

