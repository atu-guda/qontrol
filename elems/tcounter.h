/***************************************************************************
                          tcounter.h  -  description
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

#ifndef TCOUNTER_H
#define TCOUNTER_H

#include <tmiso.h>

/**Counts input pulses and generates output pulse after n input.
  *@author atu
  */


#define CLASS_ID_TCounter 1014

class TCounter : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TCounter( TDataSet *aparent );
   /** empty destructor */
   virtual ~TCounter();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** reimplemented from TMiso to reset counter */
   virtual int startLoop( int acnx, int acny );
 protected:
   /** main computation function */
   virtual double f( double t );
   /** type of output from counter */
   PRM_LIST1( type, efNRC, "Type", "Type of counters output", "", 
       "level\npulse+\npulse+-\nn" );
   //* n of counts */
   PRM_INT1( n, efNRC, "n", "Number to count", "sep=col" );
   //* current counter */
   PRM_INT1( cn, efInner, "cn", "Current counter value", "" );
   /** use u[1] as reset */
   PRM_SWITCH1( useReset, 0, "u[1] is Reset", "Use u[1] as Reset signal", "" );
   int flip;
   /** old value of input */
   double u_old;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};




#endif
