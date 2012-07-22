/***************************************************************************
                          ttrigger.h  -  description
                             -------------------
    begin                : Sun Sep 3 2000
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

#ifndef TTRIGGER_H
#define TTRIGGER_H

#include <tmiso.h>

#define CLASS_ID_TTrigger 1012

static const char *ttrigger_list = 
   "RS\n"             // 0
   "Shmitt(u2)\n"     // 1
   "CountRise(u2)\n"  // 2
   "CountDown(u2)\n"  // 3
   "CountLevel(u2)"   // 4 
;

/**different kinds if triggers
  *@author atu
  */

class TTrigger : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TTrigger( TDataSet* aparent );
   /** empty destructor */
   virtual ~TTrigger();
   /** creator */
   static TDataSet* create( TDataSet* apar );
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
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny );
   
   /** type of trigger */
   PRM_LIST1( type, efNRC, "Type", "Type of trigger", "", ttrigger_list );
   /**  current state */
   PRM_INT1( cst, efInner, "state", "current state", "" );
   /** level of 0  */ 
   PRM_DOUBLE1( level0, 0, "Level of 0", "Level of '0' signal", "" );
   /** level of 1 (shmidt) */
   PRM_DOUBLE1( level1, 0, "Level of 1", "Level of '1' signal (sor shmidt)", ""  );
   /** autoreset time */
   PRM_DOUBLE1( t0, 0, "t0", "Autoreset time (if enabled)", "" );
   /** elapsed time */
   PRM_DOUBLE1( et, efInner, "et", "Elapsed time", "" );
   /** set to 1 on start */
   PRM_SWITCH1( useInit1, efNRC, "Set on start", "Set triggers value to 1 of start", "sep=col" );
   /** use u[3] as enable */
   PRM_SWITCH1( useEnable, efNRC, "u3 is enable", "Use u[3] as enable signal", "" );
   /** pulse output */
   PRM_SWITCH1( usePulse, efNRC, "Pulse output",  "Output is pulse, no level", "sep=col" );
   /** use -1 as negative output */
   PRM_SWITCH1( useMinus, efNRC, "Negative pulse", "Drop pulse is negative, not 0", "" );
   /** autoreset after t0 */
   PRM_SWITCH1( useT0, efNRC, "Autoreset (t0)", "Use autoreset after t0 time elapsed", "");
   /** old u[2] value */
   double u2_old;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};



#endif


