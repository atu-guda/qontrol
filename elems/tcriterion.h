/***************************************************************************
                          tcriterion.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#ifndef TCRITERION_H
#define TCRITERION_H

#include <tmiso.h>


#define CLASS_ID_TCriterion 1019

/**miscellaneous criterions
  *@author atu
  */

class TCriterion : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TCriterion( TDataSet *aparent );
   /** empty destructor */
   virtual ~TCriterion();
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
   /** reimplemented from TMiso to init state */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( double t );
 protected:
   /** type of criterion */
   PRM_LIST1( type, efNRC, "Type", "Type of criterion", "",  
       "|x|<a\n|x|>a\nx<a\nx>a\na<x<b"  );
   /** coefficients  */
   PRM_DOUBLE1( a, 0, "a", "level 'a'", "" );
   PRM_DOUBLE1( b, 0, "b", "level 'b'", "" );
   PRM_DOUBLE1( t0, 0, "t0",  "Start time", "" );
   /** flags and state */
   PRM_SWITCH1( useA, efNRC, "u[3] is a", "Use u[3] as 'a' value", "sep=col" );
   PRM_SWITCH1( useT0, efNRC, "t0", "Dont work before given time (t0)", "" );
   PRM_SWITCH1( useEnable, efNRC, "u[2] is Enable", "Use u[2] signal as Enable", "" );
   PRM_SWITCH1( useLock, efNRC, "Lock", "Lock after first front", "sep=col" );
   PRM_SWITCH1( usePulse, efNRC, "Pulse Output", "output is pulse +1, 0, -1", "" );
   PRM_INT1( st, efInner, "state", "current state", "");
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


