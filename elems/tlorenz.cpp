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
  sigma = 10.0; r = 28.0; b = 8.0/3.0; // Well-known Lorenz params
  x_0 = 3.05; y_0 = 1.58; z_0 = 16.62; 
  x = y = z = 0;

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

double TLorenz::f( double /*t*/ )
{
  double xn, yn, zn;
  xn = x + tdt * ( sigma * (y-x) ) + *in_so[0];
  yn = y + tdt * ( x * (r-z) - y ) + *in_so[1];
  zn = z + tdt * ( x*y - b*z )     + *in_so[2];
  x = xn; y = yn; z = zn;
  return x; 
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

