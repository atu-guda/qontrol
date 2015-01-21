/***************************************************************************
                          tswitch.h  -  description
                             -------------------
    begin                : Sun Nov 19 2000
    copyright            : (C) 2000-2015 by atu
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

#ifndef TSWITCH_H
#define TSWITCH_H

#include <tmiso.h>

/**connects output to one of {u0,u1,u2} inputs, depend of u3 value
  *@author atu
  */

class TSwitch : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSwitch);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;

   /** levels  */
   PRM_DOUBLE( level1, 0, "Level 1", "Level to switch from in_0 to in_1", "def=1" );
   PRM_DOUBLE( level2, 0, "Level 2", "Level to switch from in_1 to in_2", "def=2" );

   PRM_INPUT( in_0, 0, "input 0", "Input 0",  "sep=block" );
   PRM_INPUT( in_1, 0, "input 1", "Input 1",  "sep=col" );
   PRM_INPUT( in_2, 0, "input 2", "Input 2",  "sep=col" );
   PRM_INPUT( in_3, 0, "input 3", "switch input", "sep=col" );


   DCL_DEFAULT_STATIC;
};

#endif

