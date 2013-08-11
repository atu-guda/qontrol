/***************************************************************************
                          tfuncpoly.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TFUNCPOLY_H
#define TFUNCPOLY_H

#include <tmiso.h>

static const char* const tfuncpoly_list = 
     "a*y+g\n"                                 // 0
     "a*y^2+b*y+g\n"                           // 1 
     "a*y^3+b*y^2+c*y+g\n"                     // 2
     "a*u0^2+b*u0*u1+c*u1^2+g\n"               // 3
     "a*sqrt(y)+g\n"                           // 4
     "hypot(a*u0,b*u1)+g\n"                    // 5
     "a*u0^2+..+d*u3^2+g\n"                    // 6
     "Vibro Ampl(omega); A=c0 B=Ome\n"         // 7
     "a*(u1-u0^2)^2+b*(1-u0)^2\n"              // 8
     "1-exp(-a*((u0^2+u1^2-1)^2-b*u0-c*u1))\n" // 9
     "a*(1+b*y)+g\n"                           // 10
     "a*(b*y+c*abs(y))+g"                      // 11
;


/**polinomial functions
  *@author atu
  */

class TFuncPoly : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFuncPoly( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFuncPoly();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** type of function */
   PRM_LIST1( type, 0, "Type", "Function type", "", tfuncpoly_list );
   /** coefficients and shift */
   PRM_DOUBLE1( a,  0, "a", "Parameter a", "sep=col" );
   PRM_DOUBLE1( b,  0, "b", "Parameter b", "" );
   PRM_DOUBLE1( c,  0, "c", "Parameter c", "" );
   PRM_DOUBLE1( d,  0, "d", "Parameter d", "sep=col" );
   PRM_DOUBLE1( e,  0, "e", "Parameter e", "" );
   PRM_DOUBLE1( g,  0, "g", "Parameter g", "" );
   PRM_DOUBLE1( x0, 0, "x0", "Input shift: y = u[0] - u[1] - x0;", "" );
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


