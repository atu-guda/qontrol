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

/**connects output to one of {u_0-u_7} inputs, depend of u_s value
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
   PRM_DOUBLE( level_0,efNRC, "Level 0", "Level to switch from in_0 to in_1", "def=0" );
   PRM_DOUBLE( step,   efNRC, "Step",   "Step between levels", "def=1\nmin=1e-20" );
   PRM_INT(    n_max,  efNRC, "Max n",   "Maximum level number", "def=7\nmin=1\nmax=7" );

   PRM_DOUBLE( n_cur,  efInner, "Current n",   "Current input number (double)", "" );

   PRM_INPUT( in_0, 0, "in_0", "Input 0",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Input 1",  "" );
   PRM_INPUT( in_2, 0, "in_2", "Input 2",  "sep=col" );
   PRM_INPUT( in_3, 0, "in_1", "Input 3",  "" );
   PRM_INPUT( in_4, 0, "in_4", "Input 4",  "sep=col" );
   PRM_INPUT( in_5, 0, "in_5", "Input 5",  "" );
   PRM_INPUT( in_6, 0, "in_6", "Input 6",  "sep=col" );
   PRM_INPUT( in_7, 0, "in_7", "Input 7",  "" );
   PRM_INPUT( in_s, 0, "in_s", "switch input", "sep=col" );


   DCL_DEFAULT_STATIC;
};

#endif

