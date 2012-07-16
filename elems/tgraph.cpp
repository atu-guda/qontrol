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
 CLASS_ID_TGraph, "TGraph", TGraph::create,
 &TDataSet::class_info, helpstr, clpSpecial };


TGraph::TGraph( TDataSet* apar )
       :TDataSet( apar ),
       PRM_INIT( title, "Title" ),
       PRM_INIT( xname, "X name" ),
       PRM_INIT( y0name, "y0 name" ),
       PRM_INIT( y1name, "y1 name" ),
       PRM_INIT( y2name, "y2 name" ),
       PRM_INIT( y3name, "y3 name" ),
       PRM_INIT( y4name, "y4 name" ),
       PRM_INIT( y5name, "y5 name" ),
       PRM_INIT( bgcolor, "BG color" ),
       PRM_INIT( y0color, "y0 color" ),
       PRM_INIT( y1color, "y1 color" ),
       PRM_INIT( y2color, "y2 color" ),
       PRM_INIT( y3color, "y3 color" ),
       PRM_INIT( y4color, "y4 color" ),
       PRM_INIT( y5color, "y5 color" )
{
  bgcolor = QColor::fromRgb( 0,0,0x60 );
  title =  "title";
  y0color = QColor::fromRgb(255,255,255); 
  y1color = QColor::fromRgb(255,255,0);
  y2color = QColor::fromRgb(255,0,0);
  y3color = QColor::fromRgb(0,255,0);
  y4color = QColor::fromRgb(0,0,255);
  y5color = QColor::fromRgb(255,0,255);

  PRMI(title).setDescr( "Plot title" );
  PRMI(title).setMinMax( 0, MAX_LABELLEN );
  PRMI(title).setParm( "sep", "blockend" );
  PRMI(xname).setDescr( "X name" );
  PRMI(xname).setMinMax( 0, MAX_NAMELEN );
  PRMI(y0name).setDescr( "Name of y0 output" );
  PRMI(y0name).setMinMax( 0, MAX_NAMELEN );
  PRMI(y1name).setDescr( "Name of y1 output" );
  PRMI(y1name).setMinMax( 0, MAX_NAMELEN );
  PRMI(y2name).setDescr( "Name of y2 output" );
  PRMI(y2name).setMinMax( 0, MAX_NAMELEN );
  PRMI(y3name).setDescr( "Name of y3 output" );
  PRMI(y3name).setMinMax( 0, MAX_NAMELEN );
  PRMI(y4name).setDescr( "Name of y4 output" );
  PRMI(y4name).setMinMax( 0, MAX_NAMELEN );
  PRMI(y5name).setDescr( "Name of y5 output" );
  PRMI(y5name).setMinMax( 0, MAX_NAMELEN );
  PRMI(y5name).setParm( "sep", "colend" );
  PRMI(bgcolor).setDescr( "Background color" );
  PRMI(y0color).setDescr( "color of y0 data" );
  PRMI(y1color).setDescr( "color of y1 data" );
  PRMI(y2color).setDescr( "color of y2 data" );
  PRMI(y3color).setDescr( "color of y3 data" );
  PRMI(y4color).setDescr( "color of y4 data" );
  PRMI(y5color).setDescr( "color of y5 data" );
}

TGraph::~TGraph()
{

}

TDataSet* TGraph::create( TDataSet* apar )
{
  return new TGraph( apar );
}

int TGraph::getClassId(void) const 
{
  return CLASS_ID_TGraph;
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
  int i, row, col, ny, out_nn, out_nu;
  TOutArr *arr;
  const double *dat;
  QString label;
  if( gi == 0 )
    return -1;
  gi->row = gi->col = 0; gi->ny = 1; gi->title[0] = 0; 
  for( i=0; i<7; i++ ) gi->label[i][0] = 0;
  strncat( gi->title, qPrintable( objectName()), 
           sizeof( gi->title )-1 ); // TODO: real QString in GraphInfo
  if( parent == 0 || parent->getClassId() != CLASS_ID_TModel 
      || parent->getState() < stateDone )
    return -10;
  
  TModel *model = static_cast<TModel*>(parent);
  // x-data
  out_nu = model->outname2out_nu( xname.toLocal8Bit().constData() );
  if( out_nu < 0 ) return -1;
  arr = model->getOutArr( out_nu );
  if( arr == 0 ) return -1;
  out_nn = -1; ny = -1;
  dat = arr->getArray(); 
  arr->getData( "n", &out_nn );
  arr->getData( "ny", &ny );
  if( dat == 0 || out_nn < 1 || ny < 0 ) return -1;
  row = out_nn; gi->dat[0] = dat;
  arr->getData( "label", label );
  if( label.isEmpty() ) {
    label = "x"; 
  };
  strncat( gi->label[0], label.toLocal8Bit().constData(), MAX_LABELLEN-1 );
  col = 1; // unlike show, x and y[] in single index
  const QString* ynms[] = { &y0name, &y1name, &y2name, &y3name, &y4name, &y5name }; // TODO: replace
  for( i=0; i<6; i++ ) {
    out_nu = model->outname2out_nu( ynms[i]->toLocal8Bit().constData() );
    if( out_nu < 0 ) continue;
    arr = model->getOutArr(  out_nu );
    if( arr == 0 ) continue;
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
    strncat( gi->label[col], label.toLocal8Bit().constData(), MAX_LABELLEN-1 );
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
    gi.title[0] = 0;
    strncat( gi.title, atitle, sizeof( gi.title )-1 );
  };
  k = gnuplotDatas( otp, &gi, fn, eps_fn, dat_fn );
  return k;
}


int TGraph::registered = reg();

int TGraph::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}

