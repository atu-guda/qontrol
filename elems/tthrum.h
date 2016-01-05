/***************************************************************************
                          tthrum.h  -  description
                             -------------------
    begin                : 2013.08.17
    copyright            : (C) 2013-2016 by atu
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

#ifndef TTHRUM_H
#define TTHRUM_H

#include "tmiso.h"

/** dummy element to test interface
  *@author atu
  */

class TThrum : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TThrum);
   DCL_CREATE;
   DCL_STD_INF;

   enum ListType {
     lt_first = 0, lt_second, lt_3rd, lt_fourth, lt_more
   };
   Q_ENUMS(ListType);
   Q_CLASSINFO( "enum_ListType_0", "First" );   // lt_first
   Q_CLASSINFO( "enum_ListType_1", "Second" );  // lt_second
   Q_CLASSINFO( "enum_ListType_2", "3-rd" );    // lt_3rd
   Q_CLASSINFO( "enum_ListType_3", "Fourth" );  // lt_fourth
   Q_CLASSINFO( "enum_ListType_4", "..." );     // lt_more
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   void fixparm();

   PRM_INT( p_int, 0, "Int", "integer value", "min=-2\nmax=12\ndef=1" );
   PRM_SWITCH( p_sw, 0, "switch", "simple switch", "def=1" );
   PRM_LIST( p_list, 0, "List", "ListElement", "def=2\nenum=ListType" );
   PRM_INT( p_intsp, 0, "Int_sp", "spin test", "props=INT,SPIN\nvis_name=<div>int_sp</div>\nmin=-2\nmax=12\ndef=1" );
   PRM_DOUBLE( p_dou,  0, "Double", "factor in input 0", "def=0.123\nmin=1e-10\nsep=col" );
   PRM_DOUBLE( p_dous,  0, "Double S", "Spin test", "props=DOUBLE,SPIN\ndef=-5.\nmin=10\nmax=100\nstep=0.5" );
   PRM_DOUBLE( p_doun, efNRC, "Double N", "test noRunChange", "def=1.1\nmax=1e10" );
   PRM_DOUBLE( doup, 0, "doup", "Test: replace by param", "def=1.1\nmax=42\nfixparm=1" );

   PRM_FONT( p_font, 0, "Font", "Font value", "sep=col\ndef=Sans" );
   PRM_FONT( p_fontd, 0, "Font default", "Default font value", "def=DEFAULT" );
   PRM_FONT( p_fonts, 0, "Font small", "Small font value", "def=SMALL" );
   PRM_FONT( p_fontt, 0, "Font tiny", "Tiny font value", "def=TINY" );

   PRM_COLOR( p_col, 0, "Color", "Color value", "sep=col\ndef=silver" );
   PRM_DATE( p_date, 0, "Date", "Date value", "def=2015.11.20\nmin=1010.10.10" );
   PRM_TIME( p_time, 0, "Time", "Time value", "def=23:59:59.001\nmin=00:13:00.000" );

   PRM_STRING( p_str, 0, "&String", "some string", u8"sep=block\nncol=2\ndef=\"a b c d f\"" );

   PRM_INPUT( in0, 0, "in0", "First input",  "sep=block" );
   PRM_INPUT( in1, 0, "in1", "Second input", "sep=col" );
   PRM_INPUT( in2, 0, "in2", "Third input",  "sep=col" );

   // PRM_STRING( p_strn, 0, "ML String", "Large string", "sep=block\nprops=STRING,MLINE\nncol=-1" );
   PRM_INT_ARR( a_int, 0, "int[]", "int array", "N=5\ndef=1\ndefs=0 2 4 \nsep=tab\ntabname=X1" );
   PRM_DOUBLE_ARR( a_dou, 0, "dou[]", "double array", "N=5\ndef=0.1\ndefs=7 1.2e-5 3e7 \nsep=tabend" );
   PRM_STRING_ARR( a_str, 0, "str[]", "string array", "N=5\ndef=a b c\ndefs=first\x01second x x\x01last" );

   Q_CLASSINFO( "nameHintBase",  "thrum_" );
   DCL_DEFAULT_STATIC;
};

#endif
