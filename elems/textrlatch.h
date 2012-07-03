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



#define CLASS_ID_TExtrLatch 1010

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
   /** reimplemented from TMiso to reset */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** type of extremum */
   PRM_LIST( type, efNoRunChange );
   PRM_SWITCH( useLocal, efNoRunChange );
   PRM_SWITCH( useFirst, efNoRunChange );
   PRM_SWITCH( outT, efNoRunChange );
   PRM_SWITCH( useReset, efNoRunChange );
   PRM_INT( wasExtr, efInner );
   PRM_INT( isStart, efInner );
   /** start time */
   PRM_DOUBLE( tStart, efNoRunChange );
   /** fuzzy level for local extr */
   PRM_DOUBLE( fuzzy, efNoRunChange );
   /** holders for extr levels and old values */
   PRM_DOUBLE( u_max, efInner );
   PRM_DOUBLE( t_max, efInner );
   PRM_DOUBLE( u_min, efInner );
   PRM_DOUBLE( t_min, efInner );
   PRM_DOUBLE( u_abs, efInner );
   PRM_DOUBLE( t_abs, efInner );
   PRM_DOUBLE( u_ex,  efInner );
   PRM_DOUBLE( t_ex,  efInner );
   double  u_old, u_old2, t_old;
   /** data descriptors -- with base class elements */ 
   static TDataInfo textrlatch_d_i[20];
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


