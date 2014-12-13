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




int  TGraph::dump( const QString &fn, const QString &delim )
{
  QStringList nms;
  QVector<const dvector *> ves;
  int nn = IMAX;

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    DBGx( "warn: not found model in \"%s\"", qP(getFullName()) );
    return 0;
  }

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
    const dvector *ve = arr->getArray();
    if( !ve ) {
      continue;
    }
    if( nn_c < nn ) {
      nn = nn_c;
    }
    QString lbl = QString( "v_%1" ).arg( QSN( ves.size() ) );
    ge->getData( "label", lbl );
    nms.push_back( lbl );
    ves.push_back( ve );
  }

  if( ves.size() < 1 || nn < 1 ) {
    DBGx( "warn: no data to output: outs: %d n: %d", ves.size(), nn );
    return 0;
  }


  QFile of( fn );
  if( ! of.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
    DBGx( "warn: fail to open file \"%s\"", qP( fn ) );
    return 0;
  }
  QTextStream os( &of );

  os << "#";
  for( auto nm : nms ) {
    os << nm << delim;
  }
  os << endl;

  for( int i=0; i<nn; ++i ) {
    for( auto ve : ves ) {
      os << (*ve)[i] << delim;
    }
    os << endl;
  }


  return 1;
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

