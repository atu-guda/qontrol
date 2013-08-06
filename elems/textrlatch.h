/***************************************************************************
                          textrlatch.h  -  description
                             -------------------
    begin                : Mon Aug 28 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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
   /** constructor */
   explicit TExtrLatch( TDataSet *aparent );
   /** empty destructor */
   virtual ~TExtrLatch();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** reimplemented from TMiso to reset */
   virtual int do_startLoop( int acnx, int acny );
   
   /** type of extremum */
   PRM_LIST1( type, efNRC, "Type", "Type of catching extremum", "",
       "Max\nMin\nMax||\nAverage\nAmplitude" );
   PRM_SWITCH1( useLocal, efNRC, "Local extr", "Catch local extremums", "sep=col" );
   PRM_SWITCH1( useFirst, efNRC, "Only first", "Catch only first local extremum", "");
   PRM_SWITCH1( outT, efNRC, "Output T", "Output time of catched extremum, not value", "" );
   PRM_SWITCH1( useReset, efNRC, " u[1] - reset", "Use u[1] as reset signal", "" );
   PRM_INT1( wasExtr, efInner,  "wasExtr", "flag: extremum detected", "" );
   PRM_INT1( isStart, efInner, "isStart", "Was start", "");
   /** start time */
   PRM_DOUBLE1( tStart, efNRC, "Time start", "Time start", "sep=col");
   /** fuzzy level for local extr */
   PRM_DOUBLE1( fuzzy, efNRC, "Fuzzy level", "Fuzzu level for local extremum catcher", "" );
   /** holders for extr levels and old values */
   PRM_DOUBLE1( u_max, efInner, "u_max", "u_max", "");
   PRM_DOUBLE1( t_max, efInner, "t_max", "t_max", "");
   PRM_DOUBLE1( u_min, efInner, "u_min", "u_min", "");
   PRM_DOUBLE1( t_min, efInner, "t_min", "t_min", "");
   PRM_DOUBLE1( u_abs, efInner, "u_abs", "u_abs", "");
   PRM_DOUBLE1( t_abs, efInner, "t_abs", "t_abs", "");
   PRM_DOUBLE1( u_ex,  efInner, "u_ex",  "u_ex" , "");
   PRM_DOUBLE1( t_ex,  efInner, "t_ex",  "t_ex" , "");
   double  u_old, u_old2, t_old;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


