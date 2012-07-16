/***************************************************************************
                          trossler.h  -  description
                             -------------------
    begin                : Thu May 03 2012
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

#ifndef TROSSLER_H
#define TROSSLER_H

#include "tmiso.h"

#define CLASS_ID_TRossler 1027

/**This element simulates Rossler system
  * @author atu
  */

class TRossler : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TRossler( TDataSet* aparent );
   /** empty destructor */
   virtual ~TRossler();
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
   /** main computation function */
   virtual double f( double t );
   /** called before each inner param loop. Unused param - -1 */
   virtual int startLoop( int acnx, int acny );
 protected:
   /** main system parameters */
   PRM_DOUBLE( a, 0 );
   PRM_DOUBLE( b, 0 );
   PRM_DOUBLE( c, 0 );
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
