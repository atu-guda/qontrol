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
   PRM_DOUBLE1( sigma, 0, "\\sigma", "\\sigma parameter", "" );
   PRM_DOUBLE1( r, 0, "r", "r parameter", "" );
   PRM_DOUBLE1( b, 0, "b", "b parameter", "" );
   /** start x, y, x values */
   PRM_DOUBLE1( x_0, efNRC, "x_0", "Initial value of x", "sep=col" );
   PRM_DOUBLE1( y_0, efNRC, "y_0", "Initial value of y", "" );
   PRM_DOUBLE1( z_0, efNRC, "z_0", "Initial value of z", "" );
   /** state variables */
   PRM_DOUBLE1( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE1( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE1( z, efInner, "z", "z current value", ""  );
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
