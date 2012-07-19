/***************************************************************************
                          tlinear.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
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

#include "tlinear.h"

const char* TLinear::helpstr = "<H1>TLinear</H1>\n"
 "Linear summator: <br>\n"
 "<b>x = a0*u0 + a1*u1 + a2*u2 + a3*u3 + b</b><br>\n"
 "Have 5 parameters: <b>a0 .. a3, b</b>, each can be changed at any time.";

TClassInfo TLinear::class_info = {
  CLASS_ID_TLinear, "TLinear", TLinear::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TLinear::TLinear( TDataSet* aparent )
        :TMiso( aparent )
{
  a0 = 1; a1 = -1; a2 = a3 = 0; b = 0;
}

TLinear::~TLinear()
{
}

TDataSet* TLinear::create( TDataSet* apar )
{
  return new TLinear( apar );
}


const TClassInfo* TLinear::getClassInfo(void) const
{
  return &class_info;
}

const char *TLinear::getHelp(void) const
{
  return helpstr;
}

const char** TLinear::getIcon(void) const
{
  return icon;
}

double TLinear::f( double /* t */ )
{
  return *in_so[0]*a0 + *in_so[1]*a1 + *in_so[2]*a2 + *in_so[3]*a3 + b;
}

int TLinear::registered = reg();

int TLinear::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TLinear::icon[] = {
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
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeee``````eeeeeeeeeeeeeeee",
"eeeeeeee``eeeeee``eeeeeeeeeeeeee",
"eeeeeee`eeeeeeeeee`eeeeeeeeeeeee",
"eeeeee```eeeeeeee```eeeeeeeeeeee",
"eeeee`e```eeeeee```e`eeeeeeeeeee",
"eeeee`ee```eeee```ee`eeeeeeeeeee",
"eeee`eeee```ee```eeee`eeeeeeeeee",
"eeee`eeeee``````eeeee`ee``eeeeee",
"eeee`eeeeee````eeeeee`eeee``eeee",
"`````eeeeee````eeeeee````````eee",
"eeee`eeeee``````eeeee`eeee``eeee",
"eeee`eeee````````eeee`ee``eeeeee",
"eeeee`ee``````````ee`eeeeeeeeeee",
"eeeee`e````````````e`eeeeeeeeeee",
"eeeeee``````````````eeeeeeeeeeee",
"eeeeeee````````````eeeeeeeeeeeee",
"eeeeeeee``````````eeeeeeeeeeeeee",
"eeeeeeeeee``````eeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
};


