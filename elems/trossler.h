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
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** called before each inner param loop. Unused param - -1 */
   virtual int startLoop( int acnx, int acny );
 protected:
   /** main computation function */
   virtual double f( double t );
   /** main system parameters */
   PRM_DOUBLE1( a, 0, "a", "Parameter a", "" );
   PRM_DOUBLE1( b, 0, "b", "Parameter b", "" );
   PRM_DOUBLE1( c, 0, "c", "Parameter c", "" );
   /** start x, y, x values */
   PRM_DOUBLE1( x_0, efNRC, "x_0", "Initial x value", "sep=col" );
   PRM_DOUBLE1( y_0, efNRC, "y_0", "Initial y value", "" );
   PRM_DOUBLE1( z_0, efNRC, "z_0", "Initial z value", "" );
   /** variable of state */
   PRM_DOUBLE1( x, efInner, "x", "x value", "" );
   PRM_DOUBLE1( y, efInner, "y", "y value", "" );
   PRM_DOUBLE1( z, efInner, "z", "z value", "" );
   /** values to store some model vars */	  
   double tt;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif
