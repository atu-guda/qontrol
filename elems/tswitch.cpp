/***************************************************************************
                          tswitch.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "tswitch.h"

const char* TSwitch::helpstr = "<H1>TSwitch</H1>\n"
 "Connects output to one of inputs, accrding to values of u[3]: <br>\n"
 " if( u[3] &lt; level1 ) out = u[0]; <br>\n"
 " if( u[3] &gt; level2 ) out = u[2]; <br>\n"
 " else out = u[1]. <br>\n"
 "Parameters:<br>\n"
 " - <b>level1</b> -- level of u[3] to switch u[0]->u[1];<br>\n"
 " - <b>level2</b> -- level of u[3] to switch u[1]->u[2].<br>\n"
 "Each parameter can be changed at any time.";

TClassInfo TSwitch::class_info = {
  CLASS_ID_TSwitch, "TSwitch", TSwitch::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };

TDataInfo TSwitch::tswitch_d_i[14] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 200, 0,  0.0, 0.0, 0, 0, "dial", "", "Dialog for TSwitch"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_level1", "", "level 1"},
 { dtpDou,        0,   0,   20,  70, 120,  20, 0,  -1e300, 1e300, 0, 0, "level1", "level1", ""},
 { dtpLabel,      0,   0,   30, 100,  50,  20, 0,  0.0, 0.0, 0, 0, "l_level2", "", "level 2"},
 { dtpDou,        0,   0,   20, 120, 120,  20, 0,  -1e300, 1e300, 0, 0, "level2", "level2", ""},
 { dtpButton,     0,   0,   20, 160,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 160,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 160,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TSwitch::TSwitch( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( level1, "Level 1" ),
	PRM_INIT( level2, "Level 2" )
{
  int i;
  level1 = 1; level2 = 2;
  d_i = tswitch_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &level1; ptrs[6] = &level2;
  // from TMiso 
  ptrs[10] = links;
  ptrs[11] = &vis_x; ptrs[12] = &vis_y;

  PRMI(level1).setDescr( "Level to switch u[0]->u[1]" );
  PRMI(level2).setDescr( "Level to switch u[1]->u[2]" );
}

TSwitch::~TSwitch()
{
}

TDataSet* TSwitch::create( TDataSet* apar )
{
  return new TSwitch( apar );
}

int TSwitch::getClassId(void) const 
{
  return CLASS_ID_TSwitch;
}

const TClassInfo* TSwitch::getClassInfo(void) const
{
  return &class_info;
}

const char *TSwitch::getHelp(void) const
{
  return helpstr;
}

const char** TSwitch::getIcon(void) const
{
  return icon;
}


double TSwitch::f( const double* u, double /* t */ )
{
  double v;
  v = ( u[3] < level1 ) ? u[0] : ( ( u[3]>level2 ) ? u[2] : u[1] );
  return v;
}

int TSwitch::registered = reg();

int TSwitch::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TSwitch::icon[] = {
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
"jjjjjjjjjjjjjjjjhjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjhjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjhjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjhjjjjjjjjjjjjjjj",
"jiiiiiiiijjjjjjjhjjjjjjjjjjjjjjj",
"jiiiiiiiijjjjjjjhjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjhjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjhhhhhjjjjjjjjjjjjj",
"jjjjjjjjjjgjjjjhhhjjjjjjjjjjjjjj",
"jjjjjjjjjggjjjjjhjjjjjjjjiiijjjj",
"jiiiiiijggggggggggggggggggiiiiij",
"jiiiiiijggggggggggggggggggiiiiij",
"jjjjjjjjjggjjjjjjjjjjjjjjiiijjjj",
"jjjjjjjjjjgjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jiiiiiiiijjjjjjjjjjjjjjjjjjjjjjj",
"jiiiiiiiijjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};


// end of tswitch.cpp

