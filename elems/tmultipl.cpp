/***************************************************************************
                          tmultipl.cpp  -  description
                             -------------------
    begin                : Fri Aug 25 2000
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

#include "tmultipl.h"

const char* TMultipl::helpstr = "<H1>TMultipl</H1>\n"
 "Simple multiplicator: <br>\n"
 "it multimplies all enabled inputs and parametr <b>a</b>\n"
 "Have 5 parameters: <b>on0 .. on3, a</b>, each can be changed at any time.";

TClassInfo TMultipl::class_info = {
  CLASS_ID_TMultipl, "TMultipl", TMultipl::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TMultipl::TMultipl( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( on0, "On 0" ),
	 PRM_INIT( on1, "On 1" ),
	 PRM_INIT( on2, "On 2" ),
	 PRM_INIT( on3, "On 3" ),
	 PRM_INIT( a,    "a" )
{
  on0 = on1 = 1; on2 = on3 = 0; a = 1;
  
  PRMI(on0).setDescr( "Use input 0" );
  PRMI(on1).setDescr( "Use input 1" );
  PRMI(on2).setDescr( "Use input 2" );
  PRMI(on3).setDescr( "Use input 3" );
  PRMI(a).setDescr( "Common factor" );
}

TMultipl::~TMultipl()
{
}

TDataSet* TMultipl::create( TDataSet* apar )
{
  return new TMultipl( apar );
}

int TMultipl::getClassId(void) const 
{
  return CLASS_ID_TMultipl;
}


const TClassInfo* TMultipl::getClassInfo(void) const
{
  return &class_info;
}

const char *TMultipl::getHelp(void) const
{
  return helpstr;
}

const char** TMultipl::getIcon(void) const
{
  return icon;
}

double TMultipl::f( double /* t */ )
{
  double v = a; 
  if( on0 )
    v *= *in_so[0];
  if( on1 )
    v *= *in_so[1];
  if( on2 )
    v *= *in_so[2];
  if( on3 )
    v *= *in_so[3];
  return v; 
}

int TMultipl::registered = reg();

int TMultipl::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char*  TMultipl::icon[] = {
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
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeegeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeegggeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeegggeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeegggeeeeeeeee",
"eeeeeeeeeeeeeegggeeegggeeeggeeee",
"eeeeeeeeeeeeeeggggeeeggeeggggeee",
"eeeeeeeeeeeeeegggggeeggeggggeeee",
"eeeeeeeeeeeeeeeeegggggggggeeeeee",
"eeeeeeeeeeeeeeeeeeegggggeeeeeeee",
"eeeeeeeeeeeeeeeeeeggggggeeeeeeee",
"eeeeeeeeeeeeeeeegggeggggggggeeee",
"eeeeeeeeeeeeeeegggeeeggegggggeee",
"eeeeeeeeeeeeeeggggeegggeeggggeee",
"eeeeeeeeeeeeeegggeeegggeeeeggeee",
"eeeeeeeeeeeeeeeeeeeeggggeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeggggeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeggggeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeggeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
};

// end of tmultipl.cpp



