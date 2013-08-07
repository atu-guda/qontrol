/***************************************************************************
                          tadjgen.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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

#ifndef TADJGEN_H
#define TADJGEN_H

#include <tmiso.h>


/**Controlled on frequency generator sqare pulses
  *@author atu
  */

class TAdjGen : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TAdjGen( TDataSet *aparent );
   /** empty destructor */
   virtual ~TAdjGen();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; }
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** reimplemented from TMiso to reset */
   virtual int do_startLoop( int acnx, int acny );
   
   /** type of averaging, */
   PRM_LIST1( type, efNoRunChange, "Type", "Type of generator", "", 
       "Default\nMAI\nDual(u0,u3)" );
   /** misc flags */
   PRM_SWITCH1( useReset, efNoRunChange, "u[1] is Reset", "Use u[1] as Reset signal", "" );
   PRM_SWITCH1( useLock, efNoRunChange, "u[2] is Lock", "Use u[2] as LOck signal", "" );
   PRM_SWITCH1( outStrobe, efNoRunChange, "Output Strobe", "Output only at switching moments", "" );
   PRM_SWITCH1( useZero, efNoRunChange, "0 is neg. out", "Negative output is 0, not -1", "" );
   PRM_SWITCH1( useSignStrobe, efNoRunChange, "Signed strobe", "Output +1 - switch to ON, -1 - OFF", "" );
   PRM_SWITCH1( usePlusStrobe, efNoRunChange, "Only + strobe", "Output obly positive strobe", "" );
   PRM_SWITCH1( useF, efNoRunChange, "input F", "input is F, not omega", "sep=col" );
   PRM_INT1( currOut, efInner, "Current out", "Current INT output", "");
   PRM_DOUBLE1( tick, efInner, "Tick", "1 means generator ticks now", "" );
   /** koeff to F-omega conversion if useF */
   PRM_DOUBLE1( omega_0, 0, "\\omega_0", "Base frequency if flag 'input F' is set", "" );
   PRM_DOUBLE1( k_omega, 0, "k_\\omega", "Frequency scale if flag 'input F' is set", "");
   /**  accumulators, ... */
   PRM_DOUBLE1( ctt, efInner, "ctt", "Accumulator 0", "" );
   PRM_DOUBLE1( ig, efInner,  "ig",  "Accumulator 1", "" );
   PRM_DOUBLE1( ig2, efInner, "ig2", "Accumulator 2", "" );
   /** real tick: exported to double tick */
   int real_tick;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};


#endif

