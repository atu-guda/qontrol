/***************************************************************************
                          thyst.h  -  description
                             -------------------
    begin                : Sat Aug 26 2000
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

#ifndef THYST_H
#define THYST_H

#include <tmiso.h>

#define CLASS_ID_THyst 1011

static const char* const thyst_list = 
     "s+alpha*d\n"     // 0
     "alpha*d\n"       // 1
     "Thetta(d)\n"     // 2 
     "sign(d)\n"       // 3 
     "tanh(alpha*d)\n" // 4
     "tanh(s+alpha*d)"; // 5

/**basic hysteresis simulation
  *@author atu
  */
class THyst : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit THyst( TDataSet *aparent );
   /** empty destructor */
   virtual ~THyst();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** reimplemented from TMiso to set init state */
   virtual int do_startLoop( int acnx, int acny );
   void fixState(void);
 protected:
   /** type of hysteresis */
   PRM_LIST1( type, efNoRunChange, "Type",  "Type of hysteresis", "", thyst_list );
   /** width of hysteresis  */
   PRM_DOUBLE1( x0, 0, "x_0",  "x0 - width if hysteresis", "min=0\nsep=col" );
   /** slope of hysteresis  */
   PRM_DOUBLE1( alpha, 0, "\\alpha", "Hysteresis slope", ""  );
   /** output scale */
   PRM_DOUBLE1( a, 0, "a scale", "output scale", "sep=col" );
   /** output shift */
   PRM_DOUBLE1( b, 0, "b shift", "output shift", ""  );
   /** state d */
   PRM_DOUBLE1( d, efInner, "d", "inner state 'd'", "" );
   /** state s */
   PRM_DOUBLE1( s, efInner, "s", "inner state 's'", "" );
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif
