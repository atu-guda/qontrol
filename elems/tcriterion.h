/***************************************************************************
                          tcriterion.h  -  description
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

#ifndef TCRITERION_H
#define TCRITERION_H

#include <tmiso.h>


#define CLASS_ID_TCriterion 1019

/**miscellaneous criterions
  *@author atu
  */

class TCriterion : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TCriterion( TDataSet *aparent );
   /** empty destructor */
   virtual ~TCriterion();
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
   virtual double f( const double *u, double t );
 protected:
   /** type of criterion */
   PRM_LIST( type, efNoRunChange );
   /** coefficients  */
   PRM_DOUBLE( a, 0 );
   PRM_DOUBLE( b, 0 );
   PRM_DOUBLE( t0, 0 );
   /** flags and state */
   PRM_SWITCH( useA, efNoRunChange );
   PRM_SWITCH( useT0, efNoRunChange );
   PRM_SWITCH( useEnable, efNoRunChange );
   PRM_SWITCH( useLock, efNoRunChange );
   PRM_SWITCH( usePulse, efNoRunChange );
   PRM_INT( st, efInner );
   /** data descriptors -- with base class elements */ 
   static TDataInfo tcriterion_d_i[23];
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


