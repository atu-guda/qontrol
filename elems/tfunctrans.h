/***************************************************************************
                          tfunctrans.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#ifndef TFUNCTRANS_H
#define TFUNCTRANS_H

#include <tmiso.h>

static const char* const tfunctrans_list = 
     "a*sin(b*y)+g\n"                // 0
     "a*sign(sin(b*y)+c)+g\n"        // 1 
     "a*tanh(by)+g\n"                // 2
     "a*atan2(u0,u1(x))+g\n"         // 3
     "a*exp(b*y)+g\n"                // 4
     "a*exp(-b*y^2)+g\n"             // 5
     "a*Wave(b*y)+g\n"               // 6
     "a*Mhat(b*y)+g\n"               // 7
     "a*ln(b*y)+g\n"                 // 8
     "a*y*exp(-b*y)+g\n"             // 9
     "a*y*exp(-b*y^2)+g\n"           // 10
     "a*sin^2(b*y)+g\n"              // 11
     "a*cos(b*y)+g\n"                // 12
     "a*cos^2(b*y)+g\n"              // 13
     "a*tan(b*y)+g\n"                // 14
     "a*exp(-b*y*y)*(1-c*sin^2(dy))+g\n" // 15
     "a*(1-exp(-b*y*y))*(1-c*sin^2(dy))+g" // 16
;

#define CLASS_ID_TFuncTrans 1006

/**trigonometric, exponetial and wavelet functions
  *@author atu
  */

class TFuncTrans : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFuncTrans( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFuncTrans();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;      
   // /** reimplemented from TMiso to ... */
   // virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( double t );
 protected:
   /** type of function */
   PRM_LIST1( type, 0, "Type", "Function type", "", tfunctrans_list );
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
   /** icon */
   static const char* icon[];
   /** autoregister */
   static int registered;
   static int reg();
};

#endif



