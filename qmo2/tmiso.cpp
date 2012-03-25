/***************************************************************************
                          tmiso.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 2000
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

#include <limits>
#include "tmiso.h"
#include "tmodel.h"

using namespace  std;

const char* TElmLink::helpstr = "<H1>TElmLink</H1> \n"
 "Defines signal and parametric inputs for given element. \n"
 "Each element may have up to 4 both signal and parametric inputs\n";


TClassInfo TElmLink::class_info = {
 CLASS_ID_TElmLink, "TElmLink", TElmLink::create,
 &TDataSet::class_info, helpstr };

TDataInfo TElmLink::telmlink_d_i[40] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 410, 410, 0,  0.0, 0.0, 0, 0, "links_dial", "", "Object links"},
 { dtpGroup,      0,   0,   10,  10, 180, 140, 0,  0.0, 0.0, 0, 0, "grp_i", "", "Inputs"},
 { dtpLabel,      0,   0,   30,  30,  30,  20, 0,  0.0, 0.0, 0, 0, "li0", "", "0"},
 { dtpStr,        0,  32,   60,  30, 100,  20, 2,  0.0, 0.0, 0, 0, "inps0", "Input 0", ""},
 { dtpLabel,      0,   0,   30,  60,  30,  20, 0,  0.0, 0.0, 0, 0, "li1", "", "1"},
 { dtpStr,        0,  32,   60,  60, 100,  20, 2,  0.0, 0.0, 0, 0, "inps1", "Input 1", ""},
 { dtpLabel,      0,   0,   30,  90,  30,  20, 0,  0.0, 0.0, 0, 0, "li2", "", "2"},
 { dtpStr,        0,  32,   60,  90, 100,  20, 2,  0.0, 0.0, 0, 0, "inps2", "Input 2", ""},
 { dtpLabel,      0,   0,   30, 120,  30,  20, 0,  0.0, 0.0, 0, 0, "li3", "", "3"},
 { dtpStr,        0,  32,   60, 120, 100,  20, 2,  0.0, 0.0, 0, 0, "inps3", "Input 3", ""},
 { dtpInt,   dtpsSw,   0,  200,  30,  80,  20, 2,  0.0, 0.0, 0, 0, "noauto", "No auto processing", "No auto"},
 { dtpInt,   dtpsSw,   0,  200,  60,  80,  20, 2,  0.0, 0.0, 0, 0, "locked", "Locked element", "Lock"},
 { dtpInt,   dtpsSw,   0,  200,  90,  80,  20, 2,  0.0, 0.0, 0, 0, "onlyFirst", "run only first time", "only First"},
 { dtpInt,   dtpsSw,   0,  200, 120,  80,  20, 2,  0.0, 0.0, 0, 0, "onlyLast", "run only last time", "only Last"},
 { dtpInt,   dtpsSw,   0,  200, 150,  80,  20, 2,  0.0, 0.0, 0, 0, "flip", "flip element icon", "flip"},
 { dtpInt,   dtpsSw,   0,  200, 180,  80,  20, 2,  0.0, 0.0, 0, 0, "noIcon", "dont show element icon, only frame", "no Icon"},
 
 { dtpGroup,      0,   0,   10, 200, 380, 160, 0,  0.0, 0.0, 0, 0, "grp_p",  "", "Parmetric inputs"},
 { dtpLabel,      0,   0,   20, 240,  20,  20, 0,  0.0, 0.0, 0, 0, "lpi0", "", "0"},
 { dtpStr,        0,  32,   40, 240, 100,  20, 2,  0.0, 0.0, 0, 0, "pinps0", "Parm 0", ""},
 { dtpLabel,      0,   0,   20, 270,  20,  20, 0,  0.0, 0.0, 0, 0, "lpi1", "", "1"},
 { dtpStr,        0,  32,   40, 270, 100,  20, 2,  0.0, 0.0, 0, 0, "pinps1", "Parm 1", ""},
 { dtpLabel,      0,   0,   20, 300,  20,  20, 0,  0.0, 0.0, 0, 0, "lpi2", "", "2"},
 { dtpStr,        0,  32,   40, 300, 100,  20, 2,  0.0, 0.0, 0, 0, "pinps2", "Parm 2", ""},
 { dtpLabel,      0,   0,   20, 330,  20,  20, 0,  0.0, 0.0, 0, 0, "lpi3", "", "3"},
 { dtpStr,        0,  32,   40, 330, 100,  20, 2,  0.0, 0.0, 0, 0, "pinps3", "Parm 3", ""},
 { dtpStr,        0,  32,  160, 240, 100,  20, 2,  0.0, 0.0, 0, 0, "pnames0", "Name 0", ""},
 { dtpStr,        0,  32,  160, 270, 100,  20, 2,  0.0, 0.0, 0, 0, "pnames1", "Name 1", ""},
 { dtpStr,        0,  32,  160, 300, 100,  20, 2,  0.0, 0.0, 0, 0, "pnames2", "Name 2", ""},
 { dtpStr,        0,  32,  160, 330, 100,  20, 2,  0.0, 0.0, 0, 0, "pnames3", "Name 3", ""},
 { dtpInt,   dtpsSw,   0,  280, 240,  80,  20, 2,  0.0, 0.0, 0, 0, "pflags0", "flag 0", ""},
 { dtpInt,   dtpsSw,   0,  280, 270,  80,  20, 2,  0.0, 0.0, 0, 0, "pflags1", "flag 1", ""},
 { dtpInt,   dtpsSw,   0,  280, 300,  80,  20, 2,  0.0, 0.0, 0, 0, "pflags2", "flag 2", ""},
 { dtpInt,   dtpsSw,   0,  280, 330,  80,  20, 2,  0.0, 0.0, 0, 0, "pflags3", "flag 3", ""},
 { dtpLabel,      0,   0,   40, 220, 100,  20, 0,  0.0, 0.0, 0, 0, "l1", "", "Out Name"},
 { dtpLabel,      0,   0,  160, 220, 100,  20, 0,  0.0, 0.0, 0, 0, "l2", "", "Param. name"},
 { dtpLabel,      0,   0,  280, 220, 100,  20, 0,  0.0, 0.0, 0, 0, "l3", "", "First only"},
 { dtpButton,     0,   0,   20, 370,  70,  26, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  160, 370,  70,  26, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  300, 370,  70,  26, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpEnd, 0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, 0, 0 , "", "", "" }
};


TElmLink::TElmLink( TDataSet* apar )
         :TDataSet( apar ),
	  ho_inps0( &inps[0], "inps0", "Input 0", this ),
	  ho_inps1( &inps[1], "inps1", "Input 1", this ),
	  ho_inps2( &inps[2], "inps2", "Input 2", this ),
	  ho_inps3( &inps[3], "inps3", "Input 3", this ),
	  ho_noauto( &noauto, "noauto", "No Auto", this ),
	  ho_locked( &locked, "locked", "Locked", this ),
	  ho_onlyFirst( &onlyFirst, "onlyFirst", "only First", this ),
	  ho_onlyLast( &onlyLast, "onlyLast", "only Last", this ),
	  ho_flip( &flip, "flip", "flip image", this ),
	  ho_noIcon( &noIcon, "noIcon", "no Icon", this ),
	  ho_pinps0( &pinps[0], "pinps0", "Parm. input 0", this ),
	  ho_pinps1( &pinps[1], "pinps1", "Parm. input 1", this ),
	  ho_pinps2( &pinps[2], "pinps2", "Parm. input 2", this ),
	  ho_pinps3( &pinps[3], "pinps3", "Parm. input 3", this ),
	  ho_pnames0( &pnames[0], "pnames0", "Parm. name 0", this ),
	  ho_pnames1( &pnames[1], "pnames1", "Parm. name 1", this ),
	  ho_pnames2( &pnames[2], "pnames2", "Parm. name 2", this ),
	  ho_pnames3( &pnames[3], "pnames3", "Parm. name 3", this ),
	  ho_pflags0( &pflags[0], "pflags0", "only First 0", this ),
	  ho_pflags1( &pflags[1], "pflags1", "only First 1", this ),
	  ho_pflags2( &pflags[2], "pflags2", "only First 2", this ),
	  ho_pflags3( &pflags[3], "pflags3", "only First 3", this )
{
  int i;
  noauto = locked = onlyFirst = onlyLast = flip = noIcon = 0;
  for( i=0; i<4; i++ ) {
    pflags[i] = 0;
  };
  d_i = telmlink_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[3] = &inps[0]; ptrs[5] = &inps[1]; ptrs[7] = &inps[2]; ptrs[9] = &inps[3];
  ptrs[10] = &noauto; ptrs[11] = &locked;
  ptrs[12] = &onlyFirst; ptrs[13] = &onlyLast;
  ptrs[14] = &flip; ptrs[15] = &noIcon;
  ptrs[18] = &pinps[0]; ptrs[20] = &pinps[1]; 
  ptrs[22] = &pinps[2]; ptrs[24] = &pinps[3];
  ptrs[25] = &pnames[0]; ptrs[26] = &pnames[1]; 
  ptrs[27] = &pnames[2]; ptrs[28] = &pnames[3];
  ptrs[29] = &pflags[0]; ptrs[30] = &pflags[1];
  ptrs[31] = &pflags[2]; ptrs[32] = &pflags[3];
  ho_inps0.setFlags( efNoRunChange );
  ho_inps1.setFlags( efNoRunChange );
  ho_inps2.setFlags( efNoRunChange );
  ho_inps3.setFlags( efNoRunChange );
  ho_pinps0.setFlags( efNoRunChange );
  ho_pinps1.setFlags( efNoRunChange );
  ho_pinps2.setFlags( efNoRunChange );
  ho_pinps3.setFlags( efNoRunChange );
  ho_pnames0.setFlags( efNoRunChange );
  ho_pnames1.setFlags( efNoRunChange );
  ho_pnames2.setFlags( efNoRunChange );
  ho_pnames3.setFlags( efNoRunChange );
}

TDataSet* TElmLink::create( TDataSet* apar )
{
  return new TElmLink( apar );
}

int TElmLink::getClassId(void) const 
{
  return CLASS_ID_TElmLink;
}

const char* TElmLink::getClassName(void) const 
{
  return "TElmLink";
}

const TClassInfo* TElmLink::getClassInfo(void) const
{
  return &class_info;
}

const char *TElmLink::getHelp(void) const
{
  return helpstr;
}

// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

TClassInfo TMiso::class_info = {
 CLASS_ID_TMiso, "TMiso", TMiso::create,
 &TDataSet::class_info, helpstr };

TDataInfo TMiso::tmiso_d_i[2] = {
 { dtpObj, CLASS_ID_TElmLink,   0,   0, 0, 0, 0, 0,  0.0, 0.0, 0, 0, "link",  "", "Object links"},
 { dtpEnd, 0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, 0, 0 , "", "", "" }
};

TMiso::TMiso( TDataSet* aparent )
      :TDataSet( aparent ),
       ho_ord( &ord, "ord", "order", this ), 
       ho_descr( &descr, "descr", "description", this ),
       ho_vis_x( &vis_x, "vis_x", "visual x", this ), 
       ho_vis_y( &vis_y, "vis_y", "visual_y", this ),
       links( new TElmLink( this ) ),
       ho_links( links, "links", "object Links", this )
{
  d_i = tmiso_d_i;
  ord = -1; 
  vis_x = vis_y = 0; tdt = 0; model_nn = 0; 
  model = 0;
  initHash();
  ptrs[0] = links;
  ho_ord.setFlags( efRO | efNoRunChange );
  ho_ord.setMinMax( 0, IMAX );
  ho_descr.setMinMax( 0, 128 ); // TODO: define
  ho_vis_x.setFlags( efNoDial | efNoRunChange );
  ho_vis_y.setFlags( efNoDial | efNoRunChange );

}

TMiso::~TMiso()
{
  // delete links; --- it's part of TMiso, deleted by ~TDataSet
}

TDataSet* TMiso::create( TDataSet* /* apar */ )
{
  // return new TMiso( apar );
  return 0; // cannot create abstract object
}

int TMiso::getClassId(void) const 
{
  return CLASS_ID_TMiso;
}

const char* TMiso::getClassName(void) const 
{
  return "TMiso";
}

const TClassInfo* TMiso::getClassInfo(void) const
{
  return &class_info;
}

const char *TMiso::getHelp(void) const
{
  return helpstr;
}

double TMiso::f( const double* /* u */, double /* t */ )
{
  return 0;
}

int TMiso::preRun( int /* run_tp */, int an, 
                   int /* anx */, int /* any */, double adt )
{
  tdt = adt; model_nn = an; 
  model = static_cast<TModel*>(parent); 
  return (state > 0 ) ? 0 : 1;
}

int TMiso::postRun( int good )
{
  state = good ? stateDone : stateGood;
  model = 0;
  return 0;
}

int TMiso::startLoop( int /* acnx */, int /* acny */ )
{
  state = stateRun;
  return 0;
}

int TMiso::endLoop(void)
{
  state = stateGood;
  return 0;
}

const char** TMiso::getIcon(void) const
{
  return icon;
}

/* XPM */
const char* TMiso::icon[] = {
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
"e c #c0c0c0",
"f c #808080",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"`````i``````iiiiii``````````````",
"``````````iiiiiiiiii````````````",
"````````iiii``````iiii``````````",
"```````iii``````````iii`````````",
"```````ii````````````ii`````````",
"``````ii```g```````g``ii````````",
"``````ii``ggg`````ggg`ii````````",
"`````ii````ggg```ggg```ii```````",
"`````ii`````ggg`ggg````ii```````",
"`````ii``````ggggg`````ii```````",
"`````ii```````ggg``````ii```````",
"`````ii``````ggggg`````ii```````",
"`````ii`````ggg`ggg````ii```````",
"``````ii```ggg```ggg``ii````````",
"``````ii``ggg`````ggg`ii````````",
"```````ii``g```````g`ii`````````",
"```````iii``````````iii`````````",
"````````iiii``````iiii``````````",
"``````````iiiiiiiiii````````````",
"````````````iiiiii``````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````",
"````````````````````````````````"
};
