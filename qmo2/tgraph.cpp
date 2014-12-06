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




int TGraph::fillGraphInfo( GraphInfo *gi ) const
{
  int i, row, col, ny, out_nn;
  TOutArr *arr;
  QString label;
  if( gi == 0 ) {
    qDebug( "err: %s: %s", __PRETTY_FUNCTION__, "gi==0" );
    return -1;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model  ) {
    DBGx( "warn: fail to find model from \"%s\"", qP(getFullName()) );
    return -10;
  }
  if( model->getState() < stateDone ) {
    DBGx( "warn: model state is not 'Done' : \"%s\"", qP(getFullName()) );
    return -11;
  }

  gi->row = gi->col = 0; gi->ny = 1;
  gi->title = objectName();

  // x-data
  arr = model->getOutArr( xname.cval() );
  if( !arr ) {
    return -1;
  }

  out_nn = -1; ny = -1;
  const dvector *dat = arr->getArray();
  arr->getData( "n", &out_nn );
  arr->getData( "ny", &ny );
  if( dat == 0 || out_nn < 1 || ny < 0 ) return -1;
  row = out_nn;
  gi->dat[0] = dat;
  arr->getData( "label", label );
  if( label.isEmpty() ) {
    label = "x";
  };
  gi->label[0] = label;
  col = 1; // unlike show, x and y[] in single index
  QString ynms[]  =
    { y0name.cval(), y1name.cval(), y2name.cval(),
      y3name.cval(), y4name.cval(), y5name.cval() }; // TODO: replace
  for( i=0; i<6; i++ ) {
    arr = model->getOutArr( ynms[i] );
    if( !arr )
      continue;
    out_nn = -1;
    dat = arr->getArray();
    arr->getData( "n", &out_nn );
    if( dat == 0 || out_nn != row ) continue;
    gi->dat[col] = dat;
    label = "";
    arr->getData( "label", label );
    if( label.isEmpty() ) {
      label = "y";
    };
    gi->label[col] = label;
    col++;
  };
  gi->col = col; gi->row = row; gi->ny = ny;
  return 0;
}


int  TGraph::dump( const char *fn, char delim /* = ' ' */ )
{
  int k;
  GraphInfo gi;
  k = fillGraphInfo( &gi );
  if( k != 0 )
    return k;
  k = dumpDatas( fn, &gi, delim );
  return k;
}

int TGraph::gnuPlot( int otp, const char *fn, const char *atitle,
                     const char *eps_fn, const char *dat_fn )
{
  int k;
  GraphInfo gi;
  k = fillGraphInfo( &gi );
  if( k != 0 )
    return k;
  if( atitle != 0  && atitle[0] != 0 ) {
    gi.title = atitle;
  };
  k = gnuplotDatas( otp, &gi, fn, eps_fn, dat_fn );
  return k;
}

void TGraph::post_set()
{
  TDataSet::post_set();
  // migration: TODO: remove after conversion
  QString s;
  s = xname.cval();

  GraphElem *ge = getElemT<GraphElem*>( "x" );
  if( ge ) { // already converted
    return;
  }
  bgcolor = QColor( 255, 255, 255 );

  ge = addObj<GraphElem>( "x" );
  if( ge ) {
    ge->setData( "src", s );
    ge->setData( "label", "x" );
    ge->setParm( "sep", "block" );
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
    DBGx( "dbg: color for \"%s\" : %X", qP(nm_old_s), li_col );
    if( li_col == 0xFFFFFF ) { // white->black
      li_col = 0;
    }
    li_col = QColor( QRgb(li_col) ).darker( 200 ).rgb();

    if( ! s.isEmpty() ) {
      gy = getElemT<GraphElem*>( nm_new );
      if( !gy ) {
        gy = addObj<GraphElem>( nm_new );
      }
      if( gy ) {
        gy->setData( "src", s );
        gy->setData( "label", nm_new );
        gy->setData( "color", li_col );
      }
    }
  }

}

DEFAULT_FUNCS_REG(TGraph);

