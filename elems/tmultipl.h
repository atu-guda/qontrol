#pragma once
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
   virtual double f() noexcept override;

   /** common coefficient */
   PRM_PARAMD( a,   0, "&a", "Common factor", "sep=col\ndef=1" );

   PRM_INPUT( in_0, 0, "in_{&0}", "First input",  "def=1\nsep=block" );
   PRM_INPUT( in_1, 0, "in_{&1}", "Second input", "def=1\nsep=col" );
   PRM_INPUT( in_2, 0, "in_{&2}", "Third input",  "def=1\nsep=col" );
   PRM_INPUT( in_3, 0, "in_{&3}", "Fourth input", "def=1\nsep=col" );

   Q_CLASSINFO( "nameHintBase",  "mul_" );
   DCL_DEFAULT_STATIC;
};



