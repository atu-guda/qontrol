/***************************************************************************
                          tswitch.h  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#ifndef TSWITCH_H
#define TSWITCH_H

#include <tmiso.h>

/**connects output to one of {u0,u1,u2} inputs, depend of u3 value
  *@author atu
  */

#define CLASS_ID_TSwitch 1022

class TSwitch : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TSwitch( TDataSet* aparent );
   /** empty destructor */
   virtual ~TSwitch();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** main computation function */
   virtual double f( double t );
 protected:
   /** levels  */
   PRM_DOUBLE1( level1, 0, "Level 1", "Level to switch from u[0] to u[1]", "" );
   PRM_DOUBLE1( level2, 0, "Level 2", "Level to switch from u[1] to u[2]", "" );
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif

