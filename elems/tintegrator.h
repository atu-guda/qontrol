/***************************************************************************
                          tintegrator.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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

#ifndef TINTEGRATOR_H
#define TINTEGRATOR_H

#include <tmiso.h>


/**realistic integrator with reset, discharge and floating base
  *@author atu
  */

class TIntegrator : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TIntegrator( TDataSet *aparent );
   /** empty destructor */
   virtual ~TIntegrator();
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

   PRM_DOUBLE1( ki, 0, "k_i",  "Factor before integral", "" );
   PRM_DOUBLE1( s_val, 0, "Start", "Start value", "" );
   PRM_DOUBLE1( dis, 0, "Disc coeff", "Discharde coeff (if enabled)", "" );
   PRM_DOUBLE1( vmin, 0, "Min limit", "Limit min value", "" );
   PRM_DOUBLE1( vmax, 0, "Max limit", "Limit max value", "" );
   PRM_DOUBLE1( v, efInner, "v", "Current value", "" );
   /** flags */
   PRM_SWITCH1( useMin,   efNRC, "use Min value", "limit output to minimum value", "sep=col" );
   PRM_SWITCH1( useMax,   efNRC, "use Max value", "limit output to maximum value", "" );
   PRM_SWITCH1( useReset, efNRC, "u[1] is Reset", "Use u[1] as Reset signal", "" );
   PRM_SWITCH1( useBase,  efNRC, "u[3] is Base",  "Use u[3] as base signal", "" );
   PRM_SWITCH1( useAdd,   efNRC, "add base", "Add base to level", "" );
   PRM_SWITCH1( useDis,   efNRC, "use Discharge", "Use discharge coefficient", "sep=col" );
   PRM_SWITCH1( useHold,  efNRC, "Hold", "Hold output 1 tick after reset", "" );
   PRM_SWITCH1( useAver,  efNRC, "Calc Aver", "Calculate average value", "" );
   PRM_SWITCH1( useSqIn,  efNRC, "x^2 on input", "Calculate square on input", "");
   PRM_SWITCH1( useSqrOut,efNRC, "sqrt on output", "Calculate square root on output", "");
   double v_old;
   double t_rst;

   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};


#endif


