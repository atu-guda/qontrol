/***************************************************************************
                          tlatch.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#ifndef TLATCH_H
#define TLATCH_H

#include <tmiso.h>

#define CLASS_ID_TLatch 1020

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
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to init state */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( double t );
 protected:
   /** type of */
   PRM_LIST( type, efNoRunChange );
   /** time to latch */
   PRM_DOUBLE( t0, efNoRunChange );
   /** start value */
   PRM_DOUBLE( v_st, efNoRunChange );
   /** current value */
   PRM_DOUBLE( v, efInner );
   /** old input  */
   double u_old;
   /** flags, state */
   int usePulse, useFirst, useAdd, wasLatch;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


