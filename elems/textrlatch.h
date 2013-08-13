/***************************************************************************
                          textrlatch.h  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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

#ifndef TEXTRLATCH_H
#define TEXTRLATCH_H

#include <tmiso.h>



/**detector of extremums, average and amplitude 
  *@author atu
  */

class TExtrLatch : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TExtrLatch);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to reset */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** type of extremum */
   PRM_LIST( type, efNRC, "Type", "Type of catching extremum", "",
       "Max\nMin\nMax||\nAverage\nAmplitude" );
   PRM_SWITCH( useLocal, efNRC, "Local extr", "Catch local extremums", "sep=col" );
   PRM_SWITCH( useFirst, efNRC, "Only first", "Catch only first local extremum", "");
   PRM_SWITCH( outT, efNRC, "Output T", "Output time of catched extremum, not value", "" );
   PRM_SWITCH( useReset, efNRC, " u[1] - reset", "Use u[1] as reset signal", "" );
   PRM_INT( wasExtr, efInner,  "wasExtr", "flag: extremum detected", "" );
   PRM_INT( isStart, efInner, "isStart", "Was start", "");
   /** start time */
   PRM_DOUBLE( tStart, efNRC, "Time start", "Time start", "sep=col");
   /** fuzzy level for local extr */
   PRM_DOUBLE( fuzzy, efNRC, "Fuzzy level", "Fuzzu level for local extremum catcher", "" );
   /** holders for extr levels and old values */
   PRM_DOUBLE( u_max, efInner, "u_max", "u_max", "");
   PRM_DOUBLE( t_max, efInner, "t_max", "t_max", "");
   PRM_DOUBLE( u_min, efInner, "u_min", "u_min", "");
   PRM_DOUBLE( t_min, efInner, "t_min", "t_min", "");
   PRM_DOUBLE( u_abs, efInner, "u_abs", "u_abs", "");
   PRM_DOUBLE( t_abs, efInner, "t_abs", "t_abs", "");
   PRM_DOUBLE( u_ex,  efInner, "u_ex",  "u_ex" , "");
   PRM_DOUBLE( t_ex,  efInner, "t_ex",  "t_ex" , "");
   double  u_old, u_old2, t_old;
   DCL_DEFAULT_STATIC;
};

#endif


