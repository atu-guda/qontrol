/***************************************************************************
                          tgraph.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cstring>

#include "miscfun.h"
#include "tgraph.h"
#include "tmodel.h"
#include "toutarr.h"

const char* TGraph::helpstr = "<H1>TGraph</H1>\n"
 "Define, which outputs will be used for plotting and dumping. \n"
 "All outputs must be the same type";

TClassInfo TGraph::class_info = {
 "TGraph", TGraph::create,
 &TDataSet::class_info, helpstr, clpSpecial };


TGraph::TGraph( TDataSet* apar )
       :TDataSet( apar ),
       scd( new ScaleData( this ) )
{
  bgcolor = QColor::fromRgb( 0,0,0x60 );
  title =  "title";
  y0color = QColor::fromRgb(255,255,255); 
  y1color = QColor::fromRgb(255,255,0);
  y2color = QColor::fromRgb(255,0,0);
  y3color = QColor::fromRgb(0,255,0);
  y4color = QColor::fromRgb(0,0,255);
  y5color = QColor::fromRgb(255,0,255);
}

TGraph::~TGraph()
{

}

TDataSet* TGraph::create( TDataSet* apar )
{
  return new TGraph( apar );
}


const TClassInfo* TGraph::getClassInfo(void) const
{
  return &class_info;
}

const char *TGraph::getHelp(void) const
{
  return helpstr;
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
  
  gi->row = gi->col = 0; gi->ny = 1;
  gi->title = objectName();
  if( !par || ! par->isChildOf("TModel") ) {
    qDebug( "err: %s: %s", __PRETTY_FUNCTION__, "parent in not TModel" );
    return -10;
  }
  if( par->getState() < stateDone ) {
    qDebug( "err: %s: %s", __PRETTY_FUNCTION__, "State is not 'Done'" );
    return -11;
  }
  
  TModel *model = static_cast<TModel*>(par);
  // x-data
  arr = model->getOutArr( xname );
  if( !arr ) 
    return -1;
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
  const QString* ynms[] = { &y0name, &y1name, &y2name, &y3name, &y4name, &y5name }; // TODO: replace
  for( i=0; i<6; i++ ) {
    arr = model->getOutArr( *ynms[i] );
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


int TGraph::registered = reg();

int TGraph::reg()
{
  return EFACT.registerElemType( &class_info  );
}

