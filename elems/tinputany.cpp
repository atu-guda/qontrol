/***************************************************************************
                          tinputany.cpp  -  description
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
#include "tinputany.h"

const char* TInputAny::helpstr = "<H1>TInputAny</H1>\n"
 "Gets signal from given source: <br>\n"
 "1-st form: #12 - get models var #12 <br>\n"
 "2-nd form: name relative to model: source.u <br>\n"
 "Have 1 parameter: <b>name</b>.";

TClassInfo TInputAny::class_info = {
  CLASS_ID_TInputAny, "TInputAny", TInputAny::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TInputAny::tinputany_d_i[12] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "TInputAny_dial", "", "Dialog for TInputAny"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   60,  50, 120,  20, 0,  0.0, 0.0, 0, 0, "l_name", "", "Name of input source"},
 { dtpStr,        0,  80,   20,  70, 320,  20, 2,  0.0, 0.0, 0, 0, "name", "name", ""},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TInputAny::TInputAny( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( name, "Name" )
{
  int i;
  type = -1; 
  ne = -1; pel = 0;
  d_i = tinputany_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &name;
  // from TMiso 
  ptrs[8] = links;
  ptrs[9] = &vis_x; ptrs[10] = &vis_y;

  PRMI(name).setDescr( "Name of element to get values" );
  PRMI(name).setMinMax( 0, 80 );
  // TODO: PRMI(name).setXXX: type is element/output name
}

TInputAny::~TInputAny()
{
}

TDataSet* TInputAny::create( TDataSet* apar )
{
  return new TInputAny( apar );
}

int TInputAny::getClassId(void) const 
{
  return CLASS_ID_TInputAny;
}


const TClassInfo* TInputAny::getClassInfo(void) const
{
  return &class_info;
}

const char *TInputAny::getHelp(void) const
{
  return helpstr;
}

const char** TInputAny::getIcon(void) const
{
  return icon;
}

int TInputAny::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int k, l, rc;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN], tname[MAX_INPUTLEN];
  TDataSet *cob, *nob;
  lastname = "";
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
    if( k < 0 )  
      return 0;  // bad name
    nob = static_cast<TDataSet*>( cob->getObj( fname ) ); // danger, check !!!!
    if( !nob ) {  // no such name 
      qDebug( "DBG: %s: not found fname: \"%s\" ", __FUNCTION__, fname );
      return 0;
    }
    if( k == 1 ) { // only left part of name w/o '.'
      pel = cob; lastname = L8B(fname); type = 0; break;
    } else {  // both component of name: aa.bb.cc -> aa  bb.cc
      // TODO: FIXME: real check!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // di = cob->getDataInfo( i );
      //if( di->tp != dtpObj ) 
      // return 0;
      cob = nob;
      if( cob == 0 ) 
	return 0;
      tname[0] = 0; strncat( tname, rname, sizeof(tname)-1 );
    };
  };
  return rc;
}

double TInputAny::f( const double* /* u */, double /* t */ )
{
  double v;
  switch( type ) {
    case 0: pel->getDataSD( qPrintable(lastname), &v, 1 ); break;
    case 1: model->getVar( ne ); break;
    default: v = 0;
  };
  return v;
}

int TInputAny::registered = reg();

int TInputAny::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TInputAny::icon[] = {
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
"jjjjjjjjjjjjjjjjjjjjjjmjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjmijjjjjjjj",
"jjjjjjjmmmmmmmmmmmmmmmmiijjjjjjj",
"jjjjjjjiiiiiiiiiiiiiiiiiiijjjjjj",
"jjjjjjjiiiiiiiiiiiiiiiiiiaajjjjj",
"jjjjjjjiiaaaaaaaaaaaaaiiaajjjjjj",
"jjjjjjjiiajjjjjjjjjjjjiaajjjjjjj",
"jjjjjjjiiajjjjjjjjjjjjjajjjjjjjj",
"jjjjjjjiiajjjjjjjjjjjjjjjjjjjjjj",
"jjjhhhhhhhhhhjjjjjjjjjjjjjjjjjjj",
"jjjhhhhhhhhhh#jjjjjjjjjjjjjjjjjj",
"jjjhhhhhhhhhh#jjjjjjjjjjjjjjjjjj",
"jjjhhhhhhhhhh#jjjjjjjjjjjjjjjjjj",
"jjjhhhhhhhhhh#jjjjjjjjjjjjjjjjjj",
"jjjhhhhhhhhhh#jjjjjjjjjjjjjjjjjj",
"jjjj##########jjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};


// end of tinputany.cpp


