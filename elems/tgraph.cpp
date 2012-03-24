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
 &TDataSet::class_info, helpstr };

TDataInfo TGraph::tgraph_d_i[28] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 300, 370, 0,  0.0, 0.0, 0, 0, "Graph",  "",   "Graph description"},
 { dtpLabel,      0,   0,   20,  10, 100,  20, 0,  0.0, 0.0, 0, 0, "l_title", "",   "title"},
 { dtpStr,        0,  80,   20,  30, 160,  20, 0,  0.0, 0.0, 0, 0, "title", "title for graph", ""},
 { dtpLabel,      0,   0,   20,  60, 100,  20, 0,  0.0, 0.0, 0, 0, "l_x",   "",   "X values"},
 { dtpStr,        0,  MAX_NAMELEN,   20,  80, 100,  20, 0,  0.0, 0.0, 0, 0, "xname",   "Name of x output", ""},
 { dtpLabel,      0,   0,   20, 210, 100,  20, 0,  0.0, 0.0, 0, 0, "l_y0",   "",   "Y0 values"},
 { dtpStr,        0,  MAX_NAMELEN,   20, 130, 100,  20, 0,  0.0, 0.0, 0, 0, "y0name",   "Name of y0 output", ""},
 { dtpLabel,      0,   0,   20, 150, 100,  20, 0,  0.0, 0.0, 0, 0, "l_y1",   "",   "Y1/Z0 values"},
 { dtpStr,        0,  MAX_NAMELEN,   20, 170, 100,  20, 0,  0.0, 0.0, 0, 0, "y1name",   "Name of y1 output", ""},
 { dtpLabel,      0,   0,   20, 190, 100,  20, 0,  0.0, 0.0, 0, 0, "l_y2",   "",   "Y2 values"},
 { dtpStr,        0,  MAX_NAMELEN,   20, 210, 100,  20, 0,  0.0, 0.0, 0, 0, "y2name",   "Name of y2 output", ""},
 { dtpLabel,      0,   0,   20, 230, 100,  20, 0,  0.0, 0.0, 0, 0, "l_y3",   "",   "Y3 values"},
 { dtpStr,        0,  MAX_NAMELEN,   20, 250, 100,  20, 0,  0.0, 0.0, 0, 0, "y3name",   "Name of y3 output", ""},
 { dtpLabel,      0,   0,   20, 270, 100,  20, 0,  0.0, 0.0, 0, 0, "l_y4",   "",   "Y4 values"},
 { dtpStr,        0,  MAX_NAMELEN,   20, 290, 100,  20, 0,  0.0, 0.0, 0, 0, "y4name",   "Name of y4 output", ""},
 { dtpLabel,      0,   0,   20, 310, 100,  20, 0,  0.0, 0.0, 0, 0, "l_y5",   "",   "Y5 values"},
 { dtpStr,        0,  MAX_NAMELEN,   20, 330, 100,  20, 0,  0.0, 0.0, 0, 0, "y5name",   "Name of y5 output", ""},
 { dtpInt,dtpsColor,   0,  130,  80,  50,  20, 0,  0.0, 0.0, 0, 0, "bgcolor",   "Background color", ""},
 { dtpInt,dtpsColor,   0,  130, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "y0color",   "Y0 color", ""},
 { dtpInt,dtpsColor,   0,  130, 170,  50,  20, 0,  0.0, 0.0, 0, 0, "y1color",   "Y1 color", ""},
 { dtpInt,dtpsColor,   0,  130, 210,  50,  20, 0,  0.0, 0.0, 0, 0, "y2color",   "Y2 color", ""},
 { dtpInt,dtpsColor,   0,  130, 250,  50,  20, 0,  0.0, 0.0, 0, 0, "y3color",   "Y3 color", ""},
 { dtpInt,dtpsColor,   0,  130, 290,  50,  20, 0,  0.0, 0.0, 0, 0, "y4color",   "Y4 color", ""},
 { dtpInt,dtpsColor,   0,  130, 330,  50,  20, 0,  0.0, 0.0, 0, 0, "y5color",   "Y5 color", ""},
 { dtpButton,     0,   0,  200,  20,  80,  30, 0,  0.0, 0.0, 0, 0, "btn_ok",   "",   "OK"},
 { dtpButton,     1,   0,  200,  70,  80,  30, 0,  0.0, 0.0, 0, 0, "btn_can",   "",   "Cancel"},
 { dtpButton,     2,   0,  200, 120,  80,  30, 0,  0.0, 0.0, 0, 0, "btn_help",   "",   "Help"}, 
 { dtpEnd, 0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, 0, 0 , "", "", "" }
};


TGraph::TGraph( TDataSet* apar )
       :TDataSet( apar )
{
  int i;
  xname[0] = 0; bgcolor = 0x000060;
  strcpy( title, "title" );
  for( i=0; i<6; i++ ) {
    yname[i][0] = 0;
  };
  ycolor[0] = 0xFFFFFF; ycolor[1] = 0xFFFF00; ycolor[2] = 0xFF0000;
  ycolor[3] = 0x00FF00; ycolor[4] = 0x0000FF; ycolor[5] = 0xFF00FF;
  d_i = tgraph_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[2] = title; 
  ptrs[4] = xname; 
  ptrs[6] = yname[0];    ptrs[8] = yname[1];    ptrs[10] = yname[2]; 
  ptrs[12] = yname[3];   ptrs[14] = yname[4];   ptrs[16] = yname[5]; 
  ptrs[17] = &bgcolor;   ptrs[18] = &ycolor[0]; ptrs[19] = &ycolor[1];
  ptrs[20] = &ycolor[2]; ptrs[21] = &ycolor[3]; ptrs[22] = &ycolor[4];
  ptrs[23] = &ycolor[5];
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

const char* TGraph::getClassName(void) const 
{
  return "TGraph";
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
  char label[MAX_LABELLEN];
  if( gi == 0 )
    return -1;
  gi->row = gi->col = 0; gi->ny = 1; gi->title[0] = 0; 
  for( i=0; i<7; i++ ) gi->label[i][0] = 0;
  strncat( gi->title, getName(), sizeof( gi->title )-1 );
  if( parent == 0 || parent->getClassId() != CLASS_ID_TModel 
      || parent->getState() < stateDone )
    return -10;
  
  TModel *model = static_cast<TModel*>(parent);
  // x-data
  out_nu = model->outname2out_nu( xname );
  if( out_nu < 0 ) return -1;
  arr = model->getOutArr( out_nu );
  if( arr == 0 ) return -1;
  out_nn = -1; ny = -1;
  dat = arr->getArray(); 
  arr->getDataSI( "n", &out_nn, 0 );
  arr->getDataSI( "ny", &ny, 0 );
  if( dat == 0 || out_nn < 1 || ny < 0 ) return -1;
  row = out_nn; gi->dat[0] = dat;
  arr->getDataSS( "label", label, MAX_LABELLEN, 1 );
  if( label[0] == 0 ) { label[1] = 0; label[0] = 'x'; };
  strncat( gi->label[0], label, MAX_LABELLEN-1 );
  col = 1; // unlike show, x and y[] in single index
  for( i=0; i<6; i++ ) {
    out_nu = model->outname2out_nu( yname[i] );
    if( out_nu < 0 ) continue;
    arr = model->getOutArr(  out_nu );
    if( arr == 0 ) continue;
    out_nn = -1;
    dat = arr->getArray(); arr->getDataSI( "n", &out_nn, 0 );
    if( dat == 0 || out_nn != row ) continue;
    gi->dat[col] = dat;
    label[0] = 0;
    arr->getDataSS( "label", label, MAX_LABELLEN, 1 );
    if( label[0] == 0 ) { label[1] = 0; label[0] = 'y'; };
    strncat( gi->label[col], label, MAX_LABELLEN-1 );
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

