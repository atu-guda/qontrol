/***************************************************************************
                          tcounter.h  -  description
                             -------------------
    begin                : Sun Nov 19 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TCOUNTER_H
#define TCOUNTER_H

#include <tmiso.h>

/**Counts input pulses and generates output pulse after n input.
  *@author atu
  */


class TCounter : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TCounter);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to reset counter */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** type of output from counter */
   PRM_LIST( type, efNRC, "Type", "Type of counters output", "", 
       "level\npulse+\npulse+-\nn" );
   //* n of counts */
   PRM_INT( n, efNRC, "n", "Number to count", "sep=col\ndef=2" );
   //* current counter */
   PRM_INT( cn, efInner, "cn", "Current counter value", "" );
   /** use u[1] as reset */
   PRM_SWITCH( useReset, 0, "use Reset", "Use in_rst as Reset signal", "sep=col" );
   
   PRM_INPUT( in_x,   0, "input", "Input source",  "sep=block" );
   PRM_INPUT( in_rst, 0, "rst input", "Signal to reset counter", "sep=col" );
   
   int flip;
   /** old value of input */
   double u_old = DMAX;
   DCL_DEFAULT_STATIC;
};




#endif
