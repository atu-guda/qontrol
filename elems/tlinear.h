/***************************************************************************
                          tlinear.h  -  description
                             -------------------
    begin                : Tue Aug 1 2000
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

#ifndef TLINEAR_H
#define TLINEAR_H

#include "tmiso.h"

/** Simple element, which summing it's inputs with given coefficients
  *@author atu
  */

class TLinear : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TLinear( TDataSet *aparent );
   /** empty destructor */
   virtual ~TLinear();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** coefficients on input and shift */
   PRM_DOUBLE1( a0, 0, "a0", "factor in input 0", "" );
   PRM_DOUBLE1( a1, 0, "a1", "factor in input 1", "" );
   PRM_DOUBLE1( a2, 0, "a2", "factor in input 2", "sep=col" );
   PRM_DOUBLE1( a3, 0, "a3", "factor in input 3", "" );
   PRM_DOUBLE1( b,  0, "b", "static shift", "sep=col" );
   PRM_INT( tst1, 117, 0, "test 1", "test variable", "" ); // remove me!
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif
