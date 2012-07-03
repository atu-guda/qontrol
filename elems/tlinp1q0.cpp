/***************************************************************************
                          tlinp1q0.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
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

#include "tmodel.h"
#include "tlinp1q0.h"

const char* TLinP1Q0::helpstr = "<H1>TLinP1Q0</H1>\n"
 "Integrate (linear if f(x)==x) diffefentional equation: <br>\n"
 "<b> dx/dt = a * f( ku * u(t) - x ) </b><br>\n"
 "Parameter <b>ku</b> -- amplification, can be changed at any time.<br>\n"
 "Parameter <b>a</b> -- frequency, can be changed at any time.<br>\n"
 " <b>a = 1 / tau; </B> "
 "where <B> tau </B> -- time constant.<br>\n"
 "Parametr: <b>fx</b> -- name of element, calculating f(x),"
 " if none - assumed f(x) = x. <br>\n"
 "Linear transfer function: <br>\n"
 "<B>W = ku / ( tau * p + 1 ) </B>.<br>\n"
 "If u(t) = theta(t) than x(t) = ku * ( 1 - exp(-t/tau) ), and<br>\n"
 " x(tau) = 0.6321 * ku \n"
 ;

TClassInfo TLinP1Q0::class_info = {
  CLASS_ID_TLinP1Q0, "TLinP1Q0", TLinP1Q0::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TLinP1Q0::tlinp1q0_d_i[17] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 350, 0,  0.0, 0.0, 0, 0, "ldp1q0_dia", "", "Dialog for TLinP1Q0"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,  100,  70, 150,  20, 0,  0.0, 0.0, 0, 0, "l_eqn",   "", "<B>dx/dt = a*f(ku*u(t)-x)</B>"},
 { dtpLabel,      0,   0,   40, 110,  50,  20, 0,  0.0, 0.0, 0, 0, "l_a", "", "a"},
 { dtpDou,        0,   0,   30, 130, 120,  20, 0,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpLabel,      0,   0,   40, 160,  50,  20, 0,  0.0, 0.0, 0, 0, "l_ku", "", "ku"},
 { dtpDou,        0,   0,   30, 180, 120,  20, 0,  -1e300, 1e300, 0, 0, "ku", "ku", ""},
 { dtpLabel,      0,   0,  190, 110, 150,  20, 0,  0.0, 0.0, 0, 0, "l_fx", "", "Function f(x) element"},
 { dtpStr,        0,  32,  190, 130, 150,  20, 0,  0.0, 0.0, 0, 0, "fx", "f(x) name", ""},
 { dtpButton,     0,   0,   20, 290,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 290,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 290,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TLinP1Q0::TLinP1Q0( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( a, "a" ),
	PRM_INIT( ku, "k_u" ),
	PRM_INIT( fx, "elem name" )
{
  int i;
  a = 1.0;  ku = 1.0; 
  // fx = QString(); 
  elnu_fx = -1;
  x_old = 0;
  d_i = tlinp1q0_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[5] = &a; 
  ptrs[7] = &ku; 
  ptrs[9] = &fx;
  // from TMiso 
  ptrs[13] = links;
  ptrs[14] = &vis_x; ptrs[15] = &vis_y;

  PRMI(a).setDescr( "Frequency in dx/dt = a*f(ku*u(t)-x)" );
  PRMI(ku).setDescr( "Amplification " );
  PRMI(fx).setDescr( "Function f(x) element name" );
  PRMI(fx).setMinMax( 0, MAX_NAMELEN );
  // TODO: element name or DROP it!
}

TLinP1Q0::~TLinP1Q0()
{
}

TDataSet* TLinP1Q0::create( TDataSet* apar )
{
  return new TLinP1Q0( apar );
}

int TLinP1Q0::getClassId(void) const 
{
  return CLASS_ID_TLinP1Q0;
}

const char* TLinP1Q0::getClassName(void) const 
{
  return "TLinP1Q0";
}

const TClassInfo* TLinP1Q0::getClassInfo(void) const
{
  return &class_info;
}

const char *TLinP1Q0::getHelp(void) const
{
  return helpstr;
}

const char** TLinP1Q0::getIcon(void) const
{
  return icon;
}

int TLinP1Q0::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int rc = TMiso::preRun( run_tp, an, anx, any, adt );
  elnu_fx = -1;
  elnu_fx = model->oname2elnu( fx.toLocal8Bit().constData() );
  return rc;
}

int TLinP1Q0::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  x_old = 0;
  return rc;
}

double TLinP1Q0::f( const double* u, double t )
{
  double x, f, uu[4];
  if( elnu_fx >= 0 ) {
    uu[0] = x_old; uu[1] = uu[2] = uu[3] = 0;
    f = model->runOneElem( elnu_fx, uu, t );
  } else {
    f = x_old;
  };
  // TODO: check this for stability
  x = x_old + a * tdt * ( ku * u[0] - f );
  x_old = x;
  return x;
}

int TLinP1Q0::registered = reg();

int TLinP1Q0::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TLinP1Q0::icon[] = {
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
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllll`lllllllllllllll",
"lllllllllllllll``lllllllllllllll",
"llllllllllllllll`lllllllllllllll",
"llllllllllllllll`lllllllllllllll",
"lllllllllllllll```llllllllllllll",
"llllllllllllllllllllllllllllllll",
"ll````````````````````````````ll",
"llllllllllllllllllllllllllllllll",
"lllllll````lllll`llllll`llllllll",
"lllllll`lll`llll`lllll``llllllll",
"lllllll`lll`ll`````llll`llllllll",
"lllllll````lllll`llllll`llllllll",
"lllllll`llllllll`lllll```lllllll",
"lllllll`llllllllllllllllllllllll",
"l`lllllllllllgggggggggggggggglll",
"l`llllllllggglllllllllllllllllll",
"l`lllllgggllllllllllllllllllllll",
"l`lllggllkllllllllllllllllllllll",
"l`llgglllkllllllllllllllllllllll",
"l`lggllllkllllllllllllllllllllll",
"l`gglllllkllllllllllllllllllllll",
"lgg```````````````````````````ll",
"llllllllllllllllllllllllllllllll"
};


// end of tlinp1q0.cpp


