/***************************************************************************
                          tlorenz.h  -  description
                             -------------------
    begin                : Fri Mar 09 2012
    copyright            : (C) 2012 by atu
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

#ifndef TLORENZ_H
#define TLORENZ_H

#include "tmiso.h"

#define CLASS_ID_TLorenz 1026

/**This element simulates Loretz system
  * @author atu
  */

class TLorenz : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TLorenz( TDataSet* aparent );
   /** empty destructor */
   virtual ~TLorenz();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;
   /** main computation function */
   virtual double f( const double *u, double t );
   /** will be called before any action -- good place for allocs */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented to delete rnds */
   virtual int postRun( int good );
   /** called before each inner param loop. Unused param - -1 */
   virtual int startLoop( int acnx, int acny );
 protected:
   /** main system parameters */
   PRM_DOUBLE( sigma, 0 );
   PRM_DOUBLE( r, 0 );
   PRM_DOUBLE( b, 0 );
   /** start x, y, x values */
   PRM_DOUBLE( x_0, efNoRunChange );
   PRM_DOUBLE( y_0, efNoRunChange );
   PRM_DOUBLE( z_0, efNoRunChange );
   /** variable of state */
   PRM_DOUBLE( x, efInner );
   PRM_DOUBLE( y, efInner );
   PRM_DOUBLE( z, efInner );
   /** values to store some model vars */	  
   double tt;
   /** data descriptors -- with base class elements */
   static TDataInfo tlorenz_d_i[25]; 
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif
