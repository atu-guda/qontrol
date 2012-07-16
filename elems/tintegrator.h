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


#define CLASS_ID_TIntegrator 1017

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
   /** class id */
   virtual int getClassId(void) const ;
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
   PRM_DOUBLE( ki, 0 );
   PRM_DOUBLE( s_val, 0 );
   PRM_DOUBLE( dis, 0 );
   PRM_DOUBLE( vmin, 0 );
   PRM_DOUBLE( vmax, 0 );
   PRM_DOUBLE( v, efInner );
   /** flags */
   PRM_SWITCH( useMin, efNoRunChange );
   PRM_SWITCH( useMax, efNoRunChange );
   PRM_SWITCH( useReset, efNoRunChange );
   PRM_SWITCH( useBase, efNoRunChange );
   PRM_SWITCH( useAdd, efNoRunChange );
   PRM_SWITCH( useDis, efNoRunChange );
   PRM_SWITCH( useHold, efNoRunChange );
   PRM_SWITCH( useAver, efNoRunChange );
   PRM_SWITCH( useSqIn, efNoRunChange );
   PRM_SWITCH( useSqrOut, efNoRunChange );
   double v_old;
   double t_rst;

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


