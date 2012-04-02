/***************************************************************************
                          toutputany.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#include <string.h>
#include <stdlib.h>
#include "miscfun.h"
#include "tmodel.h"
#include "toutputany.h"

const char* TOutputAny::helpstr = "<H1>TOutputAny</H1>\n"
 "Puts signal to given target: <br>\n"
 "1-st form: #12 - put models var #12 <br>\n"
 "2-nd form: name relative to model: source.u <br>\n"
 "Have 1 parameter: <b>name</b>.";

TClassInfo TOutputAny::class_info = {
  CLASS_ID_TOutputAny, "TOutputAny", TOutputAny::create,
  &TMiso::class_info, helpstr };

TDataInfo TOutputAny::toutputany_d_i[13] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "Zzz", "", "Dialog for TOutputAny"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   60,  50, 180,  20, 0,  0.0, 0.0, 0, 0, "l_name", "", "Name of output source"},
 { dtpStr,        0,  80,   20,  70, 280,  20, 2,  0.0, 0.0, 0, 0, "name", "name", ""},
 { dtpInt,   dtpsSw,   0,   20, 150, 180,  20, 2,  0.0, 0.0, 0, 0, "useEnable", "", "Use u[1] as enable"},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TOutputAny::TOutputAny( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( name, "Sink name" ),
	 PRM_INIT( useEnable, "u[1] is Enable" )
{
  int i;
  type = -1; 
  useEnable = 0; ne = -1; pel = 0;
  d_i = toutputany_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &name; ptrs[5] = &useEnable;
  // from TMiso 
  ptrs[9] = links;
  ptrs[10] = &vis_x; ptrs[11] = &vis_y;

  PRMI(name).setDescr( "Name of sink to ouput" ); // TODO: checial field
  PRMI(useEnable).setDescr( "Use u[1] as enable" );
}

TOutputAny::~TOutputAny()
{
}

TDataSet* TOutputAny::create( TDataSet* apar )
{
  return new TOutputAny( apar );
}

int TOutputAny::getClassId(void) const 
{
  return CLASS_ID_TOutputAny;
}

const char* TOutputAny::getClassName(void) const 
{
  return "TOutputAny";
}

const TClassInfo* TOutputAny::getClassInfo(void) const
{
  return &class_info;
}

const char *TOutputAny::getHelp(void) const
{
  return helpstr;
}

const char** TOutputAny::getIcon(void) const
{
  return icon;
}

int TOutputAny::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int i, k, l, rc;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN], tname[MAX_INPUTLEN];
  TDataSet* cob; const TDataInfo *di;
  rc = TMiso::preRun( run_tp, an, anx, any, adt );
  type = -1; ne = -1; pel = 0;
  l = name.size();
  if( l < 2 ) return 0; // smallest: '#0'
  if( name[0] == '#' ) { // access to model vars by number
    type = 1; 
    ne = atoi( name.toLocal8Bit().constData() + 1 );
    return 0;
  };
  cob = parent; tname[0] = 0; 
  strncat( tname, name.toLocal8Bit().constData(), MAX_INPUTLEN-1 );
  while( 1 ) {
    k = splitName( tname, fname, rname );
    if( k < 0 )  return 0;  // bad name
    i = cob->getDataIdx( fname );
    if( i < 0 )   // no such name
	return 0;
    if( k == 1 ) { // only left part of name w/o '.'
      pel = cob; ne = i; type = 0; break;
    } else {  // both component of name: aa.bb.cc -> aa  bb.cc
      di = cob->getDataInfo( i );
      if( di->tp != dtpObj ) 
	return 0;
      cob = static_cast<TDataSet*>( cob->getObj( i ) );
      if( cob == 0 ) return 0;
      tname[0] = 0; strncat( tname, rname, sizeof(tname)-1 );
    };
  };
  return rc;
}

double TOutputAny::f( const double* u, double /* t */ )
{
  if( useEnable && u[1] < 0.1 )
    return u[0];
  switch( type ) {
    case 0: pel->setDataID( ne, u[0], 1 ); break;
    case 1: model->setVar( ne, u[0] ); break;
    default: ;
  };
  return u[0];
}

/* XPM */
const char* TOutputAny::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #008080",
"e c #808080",
"f c #c0c0c0",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjmmmmmmmmmmmmmmmmmmmmmmmijjjjjj",
"jjiiiiiiiiiiiiiiiiiiiiiiiiajjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjiiiajjjjj",
"jjjjjjjjjjjjjjjjjjjjjiiiiiiijjjj",
"jjjjjjjjjjjjjjjjjjjjjjiiiiiaajjj",
"jjjjjjjjjjjjjjjjjjjjjjjiiiaajjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjiaajjjjj",
"jjjjjjjjjjjjhhhhhhhhhhhhhhhhhjjj",
"jjjjjjjjjjjjhhhhhhhhhhhhhhhhh#jj",
"jjjjjjjjjjjjhhhhhhhhhhhhhhhhh#jj",
"jjjjjjjjjjjjhhhhhhhhhhhhhhhhh#jj",
"jjjjjjjjjjjjhhhhhhhhhhhhhhhhh#jj",
"jjjjjjjjjjjjhhhhhhhhhhhhhhhhh#jj",
"jjjjjjjjjjjjj#################jj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};

// end of toutputany.cpp


