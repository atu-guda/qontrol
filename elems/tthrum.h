/***************************************************************************
                          tthrum.h  -  description
                             -------------------
    begin                : 2013.08.17
    copyright            : (C) 2013-2013 by atu
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
 protected:
   /** main computation function */
   virtual double f( double t ) override;

   PRM_INT( p_int, 0, "Int", "integer value", "min=-2\nmax=12\ndef=1" );
   PRM_SWITCH( p_sw, 0, "switch", "simple switch", "def=1" );
   PRM_LIST( p_list, 0, "LIst", "ListElement", "def=2", "First\nSecond\n3-rd\nFourth\n..." );
   PRM_INT( p_intsp, 0, "Int_sp", "spin test", "props=INT,SPIN\nmin=-2\nmax=12\ndef=1" );
   PRM_DOUBLE( p_dou,  0, "Double", "factor in input 0", "def=0.123\nmin=1e-10\nsep=col" );
   PRM_DOUBLE( p_dous,  0, "Double S", "Spin test", "props=DOUBLE,SPIN\ndef=-5.\nmin=10\nmax=100\nstep=0.5" );
   PRM_DOUBLE( p_doun, efNoRunChange, "Double N", "test noRunChange", "def=1.1\nmax=1e10" );
   PRM_COLOR( p_col, 0, "Color", "Color value", "def=silver" );
   PRM_STRING( p_str, 0, "String", "some string", "sep=col\nncol=2\ndef=\"a d f\"" );
   // PRM_STRING( p_strn, 0, "ML String", "Large string", "sep=block\nprops=STRING,MLINE\nncol=-1" );
   PRM_INT_ARR( a_int, 0, "int[]", "int array", "N=5\ndef=1\ndefs=0 2 4 \nsep=block" );
   
   DCL_DEFAULT_STATIC;
};

#endif
