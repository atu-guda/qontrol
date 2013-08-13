/***************************************************************************
                          trossler.h  -  description
                             -------------------
    begin                : Thu May 03 2012
    copyright            : (C) 2012 by atu
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

#ifndef TROSSLER_H
#define TROSSLER_H

#include "tmiso.h"

/**This element simulates Rossler system
  * @author atu
  */

class TRossler : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRossler);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** main system parameters */
   PRM_DOUBLE( a, 0, "a", "Parameter a", "" );
   PRM_DOUBLE( b, 0, "b", "Parameter b", "" );
   PRM_DOUBLE( c, 0, "c", "Parameter c", "" );
   /** start x, y, x values */
   PRM_DOUBLE( x_0, efNRC, "x_0", "Initial x value", "sep=col" );
   PRM_DOUBLE( y_0, efNRC, "y_0", "Initial y value", "" );
   PRM_DOUBLE( z_0, efNRC, "z_0", "Initial z value", "" );
   /** variable of state */
   PRM_DOUBLE( x, efInner, "x", "x value", "" );
   PRM_DOUBLE( y, efInner, "y", "y value", "" );
   PRM_DOUBLE( z, efInner, "z", "z value", "" );
   /** values to store some model vars */	  
   double tt;
   
   DCL_DEFAULT_STATIC;
};

#endif
