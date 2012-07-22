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
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented from TMiso to reset integrators and diffs */
   virtual int do_startLoop( int acnx, int acny );
   
   /** coefficients  */
   PRM_DOUBLE1( kd2, 0, "k_d2", "Coefficient in second derivation", "");
   PRM_DOUBLE1( kd1, 0, "k_d1", "Coefficient in first derivation" , "");
   PRM_DOUBLE1( kp,  0, "k_p" , "Coefficient in proportional part", "sep=col");
   PRM_DOUBLE1( ki1, 0, "k_i1", "Coefficient in first intergator" , "sep=col");
   PRM_DOUBLE1( ki2, 0, "k_i2", "Coefficient in second intergator", "");
   /** should output be devided to t */
   PRM_SWITCH1( aver, efNRC, "Average", "Calculate average value", "sep=col" );
   /** accumulators and old values */
   double vi1, vi2, u_old, u_old2, tdt2;
   /** flag for special handling initial steps */
   int start;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


