/***************************************************************************
                          trossler.cpp  -  description
                             -------------------
    begin                : Thu May 03 2012
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
#include "trossler.h"

const char* TRossler::helpstr = "<H1>TRossler</H1>\n"
 "Simulator element for Rossler system <br>\n"
 " \\dot{x} = -y -x );   <br>\n"
 " \\dot{y} = x + a * y; <br>\n"
 " \\dot{z} = b + z * (x-c)<br>\n"
 " <br>\n"
 " Inputs 0-2: add to x, y, z <br>\n"
 " <br>\n"
 " Outputs <b>x</b> variable - other can be get by TIinputAny<br>\n"
 " <br>\n"
 "Have 6 parameters: 3 from system, <b>a, b ,c </b>, <br>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br>"
 "First 3 may can be changed at any time.";

TClassInfo TRossler::class_info = {
  CLASS_ID_TRossler, "TRossler", TRossler::create,
  &TMiso::class_info, helpstr, clpElem };

static const int IPARM = efNoRunChange;
static const int XPARM = efNoDial | efNoSave ;

TDataInfo TRossler::trossler_d_i[25] = {
// tp   subtp  l    dx dy   dw  dh fl  min  max hv dyn name        descr  list_d
 { dtpDial, 0, 0,   0,  0, 600,300, 0, 0.0, 0.0, 0, 0, "dia", "", "" },
 { dtpInt,  0, 0,  20, 10,  60, 20, 8, 0.0, 0.0, 0, 0, "ord", "Order", "" }, 
 { dtpStr,  0,60, 100, 10, 380, 20, 2, 0.0, 0.0, 0, 0, "descr", "Description", "" },
// system params [3-8]
 { dtpLabel,0, 0,  20, 40,  90, 20, 0, 0.0, 0.0, 0, 0, "l_a", "", "a" }, 
 { dtpDbl,  0, 0,  20, 60, 100, 20, 0, 0.0, -1., 0, 0, "a", "a", "" }, 
 { dtpLabel,0, 0,  20, 90, 100, 20, 0, 0.0, 0.0, 0, 0, "l_b", "", "b" },
 { dtpDbl,  0, 0,  20,110, 100, 20, 0, 0.0, -1., 0, 0, "b", "b", "" },
 { dtpLabel,0, 0,  20,140, 100, 20, 0, 0.0, 0.0, 0, 0, "l_c", "", "c" },
 { dtpDbl,  0, 0,  20,160, 100, 20, 0, 0.0, -1., 0, 0, "c", "c", "" },
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


TRossler::TRossler( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( a, "a" ),
	PRM_INIT( b, "b" ),
	PRM_INIT( c, "c" ),
	PRM_INIT( x_0, "x_0" ),
	PRM_INIT( y_0, "y_0" ),
	PRM_INIT( z_0, "z_0" ),
	PRM_INIT( x, "x" ),
	PRM_INIT( y, "y" ),
	PRM_INIT( z, "z" )
{
  int i;
  a = 0.25; b = 1.0; c = 5.58; 
  x_0 = 0.1; y_0 = 0.0; z_0 = 0.0; 
  x = y = z = 0;

  d_i = trossler_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr; 

  ptrs[4] = &a;      ptrs[6] = &b;     ptrs[8] = &c;  
  ptrs[10] = &x_0;   ptrs[12] = &y_0;  ptrs[14] = &z_0; 
  ptrs[15] = &x;     ptrs[16] = &y;    ptrs[17] = &z; 
  // from TMiso
  ptrs[21] = links;
  ptrs[22] = &vis_x; ptrs[23] = &vis_y;

  PRMI(a).setDescr( "a" );
  PRMI(b).setDescr( "b" );
  PRMI(c).setDescr( "c" );
  PRMI(x_0).setDescr( "Initial value of x" );
  PRMI(y_0).setDescr( "Initial value of y_0" );
  PRMI(z_0).setDescr( "Initial value of z" );
  PRMI(x).setDescr( "x" );
  PRMI(y).setDescr( "y" );
  PRMI(z).setDescr( "z" );
}

TRossler::~TRossler()
{
}

TDataSet* TRossler::create( TDataSet* apar )
{
  return new TRossler( apar );
}

int TRossler::getClassId(void) const
{
  return CLASS_ID_TRossler;
}

const char* TRossler::getClassName(void) const
{
  return "TRossler";
}

const TClassInfo* TRossler::getClassInfo(void) const
{
  return &class_info;
}

const char *TRossler::getHelp(void) const
{
  return helpstr;
}

const char** TRossler::getIcon(void) const
{
  return icon;
}

double TRossler::f( const double*  u , double /*t*/ )
{
  double xn, yn, zn;
  xn = x + tdt * ( -y - z )      + u[0];
  yn = y + tdt * ( x  + a*y )    + u[1];
  zn = z + tdt * ( b + z*(x-c) ) + u[2];
  x = xn; y = yn; z = zn;
  return x; 
}

int TRossler::preRun( int run_tp, int an, int anx, int any, double atdt )
{
  int rc =  TMiso::preRun( run_tp, an, anx, any, atdt );
  return rc;
}

int TRossler::postRun( int good )
{
  return TMiso::postRun( good );
}


int TRossler::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  x = x_0; y = y_0 ; z = z_0;
  return rc;
}

int TRossler::registered = reg();

int TRossler::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TRossler::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
" 	c #121212",
".	c #252525",
"+	c #323232",
"@	c #424242",
"#	c #515151",
"$	c #616161",
"%	c #6E6E6E",
"&	c #7F7F7F",
"*	c #8E8E8E",
"=	c #9D9D9D",
"-	c #ACACAC",
";	c #BBBBBB",
">	c #CACACA",
",	c #DBDBDB",
"'	c #EAEAEA",
")	c #FEFEFE",
/* pixels */
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))),=-,)))",
"))))))))))))))))))))))))>%%%$'))",
")))))))))))))))))))))))'%*,,-*))",
")))))))))))))))))))))))*&''',*,)",
"))))))))))))))))))))))>$,'>;;*>)",
"))))))))))))))))))))))$;';>''=-)",
")))))))))))))))))))))=&';>',;-*)",
")))))))))),>;=****=-;$>>>,---=%)",
"))))))';*%%$##@@##@@+.+@%##%=&#'",
"))))'*$%$#+.+@####@+ .#;,>*%###'",
")))*%&%#+@$*->;;;-=*>;%@=,)>;%+'",
")'&&*$#@&->,=#$&=->,,))>@=,),*@'",
")=&;$$#=>'>#&',,,,,'),))=&>),&$)",
"'%>=*=%,''&&),')))),),)'&&,);@*)",
",&>===%>',&$,',))))',''*#;)>* ;)",
"'%>-**@*>)>%$;,,,,>>>*#%>';*.#')",
")**;%##%=->>-%$%=;=##$=,>=%.+=))",
"),&%&%%#@%=->>;>;;>>,>*$@++.%)))",
"))>*&*%$$@@#$%***&&&%$#@@@@$>)))",
")))'-=%%%$$$##@@#@#@@@@#@%='))))",
")))))'>-&%$####$$$#@##%*=>))))))",
"))))))))'>>;-===**==-;,'))))))))",
"))))))))))))))''''')))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))",
"))))))))))))))))))))))))))))))))"
};

// end of trossler.cpp

