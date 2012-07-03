/***************************************************************************
                          tlorenz.cpp  -  description
                             -------------------
    begin                : Fri Mar 09 2012
    copyright            : (C) 2012 by atu
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

#include <time.h>
#include <math.h> 
#include "miscfun.h" 
#include "tmodel.h"
#include "tlorenz.h"

const char* TLorenz::helpstr = "<H1>TLorenz</H1>\n"
 "Simulator element for Lorenz system <br>\n"
 " \\dot{x} = \\sigma ( y-x );<br>\n"
 " \\dot{y} = x (r-z ) - y; <br>\n"
 " \\dot{z} = xy - bz.<br>\n"
 " <br>\n"
 " Inputs 0-2: add to x, y, z <br>\n"
 " <br>\n"
 " Outputs <b>x</b> variable - other can be get by TIinputAny<br>\n"
 " <br>\n"
 "Have 6 parameters: 3 from system, <b>sigma, r ,b </b>, <br>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br>"
 "First 3 may can be changed at any time.";

TClassInfo TLorenz::class_info = {
  CLASS_ID_TLorenz, "TLorenz", TLorenz::create,
  &TMiso::class_info, helpstr, clpElem };

static const int IPARM = efNoRunChange;
static const int XPARM = efNoDial | efNoSave ;

TDataInfo TLorenz::tlorenz_d_i[25] = {
// tp   subtp  l    dx dy   dw  dh fl  min  max hv dyn name        descr  list_d
 { dtpDial, 0, 0,   0,  0, 600,300, 0, 0.0, 0.0, 0, 0, "dia", "", "" },
 { dtpInt,  0, 0,  20, 10,  60, 20, 8, 0.0, 0.0, 0, 0, "ord", "Order", "" }, 
 { dtpStr,  0,60, 100, 10, 380, 20, 2, 0.0, 0.0, 0, 0, "descr", "Description", "" },
// system params [3-8]
 { dtpLabel,0, 0,  20, 40,  90, 20, 0, 0.0, 0.0, 0, 0, "l_sigma", "", "\\sigma" }, 
 { dtpDbl,  0, 0,  20, 60, 100, 20, 0, 0.0, -1., 0, 0, "sigma", "\\sigma", "" }, 
 { dtpLabel,0, 0,  20, 90, 100, 20, 0, 0.0, 0.0, 0, 0, "l_r", "", "r" },
 { dtpDbl,  0, 0,  20,110, 100, 20, 0, 0.0, -1., 0, 0, "r", "r", "" },
 { dtpLabel,0, 0,  20,140, 100, 20, 0, 0.0, 0.0, 0, 0, "l_b", "", "b" },
 { dtpDbl,  0, 0,  20,160, 100, 20, 0, 0.0, -1., 0, 0, "b", "b", "" },
// initial values [9-14]
 { dtpLabel,0, 0, 200, 40,  90, 20, 0, 0.0, 0.0, 0, 0, "l_x_0", "", "x_0" }, 
 { dtpDbl,  0, 0, 200, 60, 100, 20, IPARM, 0.0, -1., 0, 0, "x_0", "x_0", "" }, 
 { dtpLabel,0, 0, 200, 90,  90, 20, 0, 0.0, 0.0, 0, 0, "l_y_0", "", "y_0" }, 
 { dtpDbl,  0, 0, 200,110, 100, 20, IPARM, 0.0, -1., 0, 0, "y_0", "y_0", "" }, 
 { dtpLabel,0, 0, 200,140,  90, 20, 0, 0.0, 0.0, 0, 0, "l_z_0", "", "z_0" }, 
 { dtpDbl,  0, 0, 200,160, 100, 20, IPARM, 0.0, -1., 0, 0, "z_0", "z_0", "" }, 
// variables [15-17]
 { dtpDbl,  0, 0, 160, 60, 100, 20, XPARM, 0.0, -1., 0, 0, "x", "x", "" }, 
 { dtpDbl,  0, 0, 160, 60, 100, 20, XPARM, 0.0, -1., 0, 0, "y", "y", "" }, 
 { dtpDbl,  0, 0, 160, 60, 100, 20, XPARM, 0.0, -1., 0, 0, "z", "z", "" }, 
// buttons [18-24]
 { dtpButton,     0,   0,   20, 250,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  250, 250,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  490, 250,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TLorenz::TLorenz( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( sigma, "\\sigma" ),
	PRM_INIT( r, "r" ),
	PRM_INIT( b, "b" ),
	PRM_INIT( x_0, "x_0" ),
	PRM_INIT( y_0, "y_0" ),
	PRM_INIT( z_0, "z_0" ),
	PRM_INIT( x, "x" ),
	PRM_INIT( y, "y" ),
	PRM_INIT( z, "z" )
{
  int i;
  sigma = 10.0; r = 28.0; b = 8.0/3.0; // Well-known Lorenz params
  x_0 = 3.05; y_0 = 1.58; z_0 = 16.62; 
  x = y = z = 0;

  d_i = tlorenz_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr; 

  ptrs[4] = &sigma;  ptrs[6] = &r;     ptrs[8] = &b;  
  ptrs[10] = &x_0;   ptrs[12] = &y_0;  ptrs[14] = &z_0; 
  ptrs[15] = &x;     ptrs[16] = &y;    ptrs[17] = &z; 
  // from TMiso
  ptrs[21] = links;
  ptrs[22] = &vis_x; ptrs[23] = &vis_y;

  PRMI(sigma).setDescr( "\\sigma" );
  PRMI(r).setDescr( "r" );
  PRMI(b).setDescr( "b" );
  PRMI(x_0).setDescr( "Initial value of x" );
  PRMI(y_0).setDescr( "Initial value of y_0" );
  PRMI(z_0).setDescr( "Initial value of z" );
  PRMI(x).setDescr( "x" );
  PRMI(y).setDescr( "y" );
  PRMI(z).setDescr( "z" );
}

TLorenz::~TLorenz()
{
}

TDataSet* TLorenz::create( TDataSet* apar )
{
  return new TLorenz( apar );
}

int TLorenz::getClassId(void) const
{
  return CLASS_ID_TLorenz;
}


const TClassInfo* TLorenz::getClassInfo(void) const
{
  return &class_info;
}

const char *TLorenz::getHelp(void) const
{
  return helpstr;
}

const char** TLorenz::getIcon(void) const
{
  return icon;
}

double TLorenz::f( const double*  u , double /*t*/ )
{
  double xn, yn, zn;
  xn = x + tdt * ( sigma * (y-x) ) + u[0];
  yn = y + tdt * ( x * (r-z) - y ) + u[1];
  zn = z + tdt * ( x*y - b*z )     + u[2];
  x = xn; y = yn; z = zn;
  return x; 
}

int TLorenz::preRun( int run_tp, int an, int anx, int any, double atdt )
{
  int rc =  TMiso::preRun( run_tp, an, anx, any, atdt );
  return rc;
}

int TLorenz::postRun( int good )
{
  return TMiso::postRun( good );
}


int TLorenz::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  x = x_0; y = y_0 ; z = z_0;
  return rc;
}

int TLorenz::registered = reg();

int TLorenz::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TLorenz::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
" 	c #060905",
".	c #1C1E1C",
"+	c #2B2D2A",
"@	c #393B38",
"#	c #494B48",
"$	c #575956",
"%	c #656764",
"&	c #767875",
"*	c #858784",
"=	c #949693",
"-	c #A7A9A6",
";	c #B6B8B5",
">	c #C6C8C5",
",	c #D7D9D6",
"'	c #E7E9E6",
")	c #FDFFFC",
/* pixels */
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))''''')))))))))))",
")))))))))))))),,'',,',,'))))))))",
"))))))))))))','>;;>>-->'>'))))))",
"))))))))))))',,-========,;')))))",
")))))))))))',>=%&&*-*&&&&,;,))))",
")))))))))))>=-&*&$@@@#$%&$>>,)))",
")))))))'',>>**&$@+#&*#@@%&&>>,))",
"))))))>)-;;%$@.+.$$$%$*@@&&=,,))",
"))))),'=;%@@.##++@%),>&*@$*&>',)",
")))''>=*#@#@##++@@*>>)'&%$*=-,')",
"))''--&@###;>=$++.&-)))&-%=-;>')",
")),,>*#$#*)));$+ .*,)))%*$==;;')",
"),);;%$%&')))>&. .%,))>$%$*=-,')",
")'',=&=*>)))),$...$$-=$*@$*&>',)",
"))'>=*==;))))-$@++@==**+#*&*,,')",
")),,-****,''>$##$$@@%%+#%&&,>,))",
"))',=&&*&*-=$%%$=#%%*$&%*%;>,)))",
")')>,&&%=*=**#$-;%**-***%,;,')))",
"),);,;&%%&*&%%*>=>*===*=,;''))))",
")),)>;'=**==*=,-,;>>>;;,>'')))))",
"))),)'=>)>;;,'=),',>,,'>')))))))",
")))),'),-----;)'),,'''')))))))))",
")))))''''')))'''))))))))))))))))",
"))))))))))''')))))))))))))))))))"
};

// end of tlorenz.cpp

