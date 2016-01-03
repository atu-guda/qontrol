/***************************************************************************
                          tmultipl.h  -  description
                             -------------------
    begin                : Fri Aug 25 2000
    copyright            : (C) 2000-2016 by atu
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

#ifndef TMULTIPL_H
#define TMULTIPL_H

#include <tmiso.h>

/**simple multiplicator
  *@author atu
  */

class TMultipl : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TMultipl);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;

   /** common coefficient */
   PRM_DOUBLE( a,    0, "a", "Common factor", "sep=col\ndef=1" );
   /** switches -- is this input on */
   PRM_SWITCH( on0, 0, "On 0", "Use input 0", "def=1\nsep=block" );
   PRM_SWITCH( on1, 0, "On 1", "Use input 1", "def=1\nsep=col" );
   PRM_SWITCH( on2, 0, "On 2", "Use input 2", "sep=col" );
   PRM_SWITCH( on3, 0, "On 3", "Use input 3", "sep=col" );

   PRM_INPUT( in_0, 0, "in_0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "in_2", "Third input",  "sep=col" );
   PRM_INPUT( in_3, 0, "in_3", "Fourth input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "mul_" );
   DCL_DEFAULT_STATIC;
};

#endif


