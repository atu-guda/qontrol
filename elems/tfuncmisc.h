/***************************************************************************
                          tfuncmisc.h  -  description
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

#ifndef TFUNCMISC_H
#define TFUNCMISC_H

#include <tmiso.h>


#define CLASS_ID_TFuncMisc 1007

static const char* const tfuncmisc_list = 
     "a*deadLine(y,b)+g\n"           // 0
     "a*limitLine(y,b)+g\n"          // 1 
     "a*deadLimitLine(y,b,c,d)+g\n"  // 2
     "a*sign(y)+g\n"                 // 3
     "a*Thetta(y)+g\n"               // 4
     "a*diod(y)+g\n"                 // 5
     "a*threeState(y,b)+g\n"         // 6
     "a*triangle(y,b)+g\n"           // 7
     "a*rectLine(y,b,c)+g\n"         // 8
     "((y>0)?a:b)*y+g\n"             // 9
     "a*u0/(u1+b)+g\n"  	     // 10
     "a*int(y*b)+g\n" 		     // 11
     "a*frac(y*b)+g\n" 		     // 12
     "a*asin(abs(sin(y*b)))+g\n"     // 13
     "a*erf(y*b)+g\n"                // 14
     "a*min(u0,u1)+g\n"              // 15
     "a*max(u0,u1)+g"                // 16
;

/**miscelenios nonlinear functions
  *@author atu
  */

class TFuncMisc : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFuncMisc( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFuncMisc();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** type of function */
   PRM_LIST1( type, 0, "Type", "Function type", "", tfuncmisc_list );
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


