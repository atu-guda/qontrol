/***************************************************************************
                          tmaxnum.h  -  find maximum input
                             -------------------
    begin                : 2015.03.02
    copyright            : (C) 2015-2015 by atu
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

#ifndef TMAXNUM_H
#define TMAXNUM_H

#include <tmiso.h>

/** Selects maximum (min) of {u_0-u_7} inputs, and output Number on maximim
  *@author atu
  */

class TMaxNum : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TMaxNum);
   DCL_CREATE;
   DCL_STD_INF;

   enum ExtrType {
      et_MAX = 0, et_MIN, et_MAXMOD, et_MINMOD
   };
   Q_ENUMS(ExtrType);
   Q_CLASSINFO( "enum_ExtrType_0", "MAX"       );   // et_MAX
   Q_CLASSINFO( "enum_ExtrType_1", "MIN"       );   // et_MIN
   Q_CLASSINFO( "enum_ExtrType_2", "MAX||"     );   // et_MAXMOD
   Q_CLASSINFO( "enum_ExtrType_3", "MIN||"     );   // et_MINMOD

 protected:
   virtual double f( double t ) override;

   PRM_LIST(   type,   efNRC,    "Type",  "Type of extremum", "enum=ExtrType" );
   PRM_INT(    n_in,   efNRC, "N_{in}",   "Number of inputs", "def=8\nmin=1\nmax=8" );
   PRM_INT(   n_min,   efNRC, "N_{min}",  "Minimum number", "def=0\nmin=0\nmax=7\nsep=col" );
   PRM_INT(   n_max,   efNRC, "N_{max}",  "Miximum number", "def=7\nmin=1\nmax=7" );

   PRM_DOUBLE(  im,  efInner, "im",   "Index on maximum input", "" );
   PRM_DOUBLE(  vm,  efInner, "vm",   "Value of maximum input", "" );

   // if not exist - used current
   PRM_DOUBLE(  im_prev,  efInner, "im_prev",   "Index of previous input", "" );
   PRM_DOUBLE(  im_next,  efInner, "im_next",   "Index of next input", "" );
   PRM_DOUBLE(  vm_prev,  efInner, "vm_prev",   "Value of previous input", "" );
   PRM_DOUBLE(  vm_next,  efInner, "vm_next",   "Value of next input", "" );

   PRM_INPUT( in_0, 0, "in_0", "Input 0",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Input 1",  "" );
   PRM_INPUT( in_2, 0, "in_2", "Input 2",  "sep=col" );
   PRM_INPUT( in_3, 0, "in_1", "Input 3",  "" );
   PRM_INPUT( in_4, 0, "in_4", "Input 4",  "sep=col" );
   PRM_INPUT( in_5, 0, "in_5", "Input 5",  "" );
   PRM_INPUT( in_6, 0, "in_6", "Input 6",  "sep=col" );
   PRM_INPUT( in_7, 0, "in_7", "Input 7",  "" );

   static constexpr const int m_n_in = 8; // change in changed number of inputs ^

   InputSimple *ins[m_n_in];

   Q_CLASSINFO( "nameHintBase",  "maxn_" );
   DCL_DEFAULT_STATIC;
};

#endif

