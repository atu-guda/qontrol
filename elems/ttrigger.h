/***************************************************************************
                          ttrigger.h  -  description
                             -------------------
    begin                : Sun Sep 3 2000
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

#ifndef TTRIGGER_H
#define TTRIGGER_H

#include <tmiso.h>

#define CLASS_ID_TTrigger 1012

/**different kinds if triggers
  *@author atu
  */

class TTrigger : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TTrigger( TDataSet* aparent );
   /** empty destructor */
   virtual ~TTrigger();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
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
   virtual double f( const double *u, double t );
 protected:
   /** type of trigger */
   PRM_LIST( type, efNoRunChange );
   /**  current state */
   PRM_INT( cst, efInner );
   /** level of 0  */ 
   PRM_DOUBLE( level0, 0 );
   /** level of 1 (shmidt) */
   PRM_DOUBLE( level1, 0 );
   /** autoreset time */
   PRM_DOUBLE( t0, 0 );
   /** elapsed time */
   PRM_DOUBLE( et, efInner );
   /** old u[2] value */
   double u2_old;
   /** set to 1 on start */
   PRM_SWITCH( useInit1, efNoRunChange );
   /** use u[3] as enable */
   PRM_SWITCH( useEnable, efNoRunChange );
   /** pulse output */
   PRM_SWITCH( usePulse, efNoRunChange );
   /** use -1 as negative output */
   PRM_SWITCH( useMinus, efNoRunChange );
   /** autoreset after t0 */
   PRM_SWITCH( useT0, efNoRunChange );
   /** data descriptors -- with base class elements */ 
   static TDataInfo ttrigger_d_i[23];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};



#endif


