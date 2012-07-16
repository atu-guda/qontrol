/***************************************************************************
                          tpid.h  -  description
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

#ifndef TPID_H
#define TPID_H

#include <tmiso.h>

#define CLASS_ID_TPid 1004

/**combines integrators, differentiators and proportional 
  *@author atu
  */

class TPid : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TPid( TDataSet* aparent );
   /** empty destructor */
   virtual ~TPid();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to reset integrators and diffs */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( double t );
 protected:
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** coefficients  */
   PRM_DOUBLE( kd2, 0 );
   PRM_DOUBLE( kd1, 0 );
   PRM_DOUBLE( kp, 0 );
   PRM_DOUBLE( ki1, 0 );
   PRM_DOUBLE( ki2, 0 );
   /** should output be devided to t */
   PRM_SWITCH( aver, efNoRunChange );
   /** accumulators and old values */
   double vi1, vi2, u_old, u_old2, tdt2;
   /** flag for special handling initial steps */
   int start;
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


