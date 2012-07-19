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
  &TMiso::class_info, helpstr, clpElem };


TOutputAny::TOutputAny( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( name, "Sink name" ),
	 PRM_INIT( useEnable, "u[1] is Enable" )
{
  type = -1; 
  useEnable = 0; ne = -1; pel = 0;

  PRMI(name).setDescr( "Name of sink to output" ); // TODO: checial field
  PRMI(name).setMinMax( 0, 80 );
  PRMI(useEnable).setDescr( "Use u[1] as enable" );
}

TOutputAny::~TOutputAny()
{
}

TDataSet* TOutputAny::create( TDataSet* apar )
{
  return new TOutputAny( apar );
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

// TODO: fix! NO: remove this object!
int TOutputAny::do_preRun( int /*run_tp*/, int /*an*/, 
                           int /*anx*/, int /*any*/, double /*adt*/ )
{
  int k, l;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN], tname[MAX_INPUTLEN];
  TDataSet *cob, *nob;
  lastname = "";
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
  return 0;
}

double TOutputAny::f( double /* t */ )
{
  if( useEnable && *in_so[1] < 0.1 )
    return *in_so[0];
  switch( type ) {
    // case 0: pel->setDataID( ne, *in_so[0], 1 ); break; TODO: drop at all
    case 1: model->setVar( ne, *in_so[0] ); break;
    default: ;
  };
  return *in_so[0];
}

int TOutputAny::registered = reg();

int TOutputAny::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
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


