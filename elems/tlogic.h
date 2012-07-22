/***************************************************************************
                          tlogic.h  -  description
                             -------------------
    begin                : Mon Sep 4 2000
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

#ifndef TLOGIC_H
#define TLOGIC_H

#include <tmiso.h>

static const char* const tlogic_list = 
 "AND\n"  // 0 
 "OR\n"   // 1
 "XOR\n"  // 2
 "u[0]\n" // 3
 "0\n"    // 4
 "1"      // 5
;

#define CLASS_ID_TLogic 1013

/**simple logical operation: AND, OR, XOR
  *@author atu
  */

class TLogic : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TLogic( TDataSet* aparent );
   /** empty destructor */
   virtual ~TLogic();
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
   /** type of logic element */
   PRM_LIST1( type, efNRC, "Type", "Type of logic", "", tlogic_list );
   /** level of 1  */
   PRM_DOUBLE1( level, 0, "level of 1", "Minimal level of '1' signal on input", "" );
   /** misc flags */
   PRM_SWITCH1( useNInp0, efNRC, "Inverse u[0]", "Inverse input u[0]", "sep=col" );
   PRM_SWITCH1( useNInp1, efNRC, "Inverse u[1]", "Inverse input u[1]", ""  );
   PRM_SWITCH1( useNInp2, efNRC, "Inverse u[2]", "Inverse input u[2]", ""  );
   PRM_SWITCH1( useNInp3, efNRC, "Inverse u[3]", "Inverse input u[3]", ""  );
   PRM_SWITCH1( useNOut, efNRC,  "Inverse out" , "Inverse input output", "sep=col"  );
   PRM_SWITCH1( useMinus, efNRC, "Negative out", "Negative output is -1, not 0", "" );
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


