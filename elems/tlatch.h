/***************************************************************************
                          tlatch.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#ifndef TLATCH_H
#define TLATCH_H

#include <tmiso.h>

/**Remembers given value by signal
  *@author atu
  */

class TLatch : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TLatch( TDataSet *aparent );
   /** empty destructor */
   virtual ~TLatch();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny );
   
   /** type of */
   PRM_LIST1( type, efNRC, "Type", "Latch type", "", "Time\nSignal u[1]");
   /** time to latch */
   PRM_DOUBLE1( t0, efNRC, "Time", "Time to latch" , "sep=col" );
   /** start value */
   PRM_DOUBLE1( v_st, efNRC, "Start val", "Start value", "" );
   /** current value */
   PRM_DOUBLE1( v, efInner, "v", "Current value", "" );
   /** old input  */
   double u_old;
   /** flags, state */
   int usePulse, useFirst, useAdd, wasLatch;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


