/***************************************************************************
                          tdelay.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#ifndef TDELAY_H
#define TDELAY_H

#include <tmiso.h>
class TCircBuf;


/**delays signal for given time
  *@author atu
  */

class TDelay : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TDelay( TDataSet *aparent );
   /** empty destructor */
   virtual ~TDelay();
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
   /** calc values & alloc buf */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** deallocates buf */
   virtual int do_postRun( int good );
   /** reimplemented from TMiso to reset array */
   virtual int do_startLoop( int acnx, int acny );
   
   /** max delay */
   PRM_DOUBLE1( mdelay, efNRC, "Max. delay", "Maximum delay", "min=0\nmax=1e20");
   /** current delay */
   PRM_DOUBLE1( cdelay, 0, "Cur. delay", "Current delay", "min=0\nmax=1e20" );

   double v1;
   double v2;
   double u00;
   /** max and current delays indexes ... */
   int imd, icd;
   /** data container */
   TCircBuf *buf;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


