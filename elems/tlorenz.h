/***************************************************************************
                          tlorenz.h  -  description
                             -------------------
    begin                : Fri Mar 09 2012
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

#ifndef TLORENZ_H
#define TLORENZ_H

#include "tmiso.h"

/**This element simulates Loretz system
  * @author atu
  */

class TLorenz : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLorenz);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** main system parameters */
   PRM_DOUBLE( sigma, 0, "\\sigma", "\\sigma parameter", "" );
   PRM_DOUBLE( r, 0, "r", "r parameter", "" );
   PRM_DOUBLE( b, 0, "b", "b parameter", "" );
   /** start x, y, x values */
   PRM_DOUBLE( x_0, efNRC, "x_0", "Initial value of x", "sep=col" );
   PRM_DOUBLE( y_0, efNRC, "y_0", "Initial value of y", "" );
   PRM_DOUBLE( z_0, efNRC, "z_0", "Initial value of z", "" );
   /** state variables */
   PRM_DOUBLE( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE( z, efInner, "z", "z current value", ""  );
   /** values to store some model vars */	  
   double tt;
   
   DCL_DEFAULT_STATIC;
};

#endif
