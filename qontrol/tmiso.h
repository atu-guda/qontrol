#pragma once
/***************************************************************************
                          tmiso.h  - base for scheme elements description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2022 by atu
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

/** \file tmiso.h
  *@author atu
*/



#include "linkedobj.h"
#include "elemparams.h"

// -------------------------- TMiso -----------------------------

/** \class TMiso tmiso.h
 * \breef Parent of all scheme elements.
 *
 */

class TMiso : public LinkedObj  {
  Q_OBJECT
 public:
   DCL_CTOR(TMiso);
   // virtual ~TMiso() override;
   DCL_CREATE;
   DCL_STD_INF;
   Q_INVOKABLE virtual QString textVisual() const override;

   /** external computation function + in/out */
   double fun( IterType itype ) noexcept;
   //* to call directly - only for some pure types
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 );

   /** check, if element have given visual coords */
   bool isAtCoord( int ax, int ay ) const;
   virtual void fillComplModelForParams( QStandardItemModel *mdl ) const override;
   virtual void preCalc();
 protected:
   /** main computation function
    * \return output value of element
    * */
   virtual double f() noexcept;
   /** place of customization of preRun, return: !=0 = Ok */
   // virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** will be called after all actions from posrtRun  -- good place for deallocs */
   // virtual int do_postRun( int good ) override;
   /** called before each inner param loop from startLoop */
   virtual int do_startLoop( long acnx, long acny ) override;
   //* to be overrided by real elements
   virtual int miso_startLoop( long acnx, long acny );
   /** will be called after each inner loop: called from endLoop */
   // virtual int do_endLoop() override;
   /** do real actions after structure changed - refills inputs */
   virtual void do_structChanged() override;

   PRM_STRING(       descr, efNRC,  "description", "Object description", "max=128\nncol=-1\nsep=blockend");
   PRM_PARAMD(   out0_init, efNRC,  "Init value", "Initial value of output", "def=0.0\nsep=colend" );

   PRM_DOUBLE(      debug0, efInner, "debug0", "Debug value 0", "" );
   PRM_DOUBLE(      debug1, efInner, "debug1", "Debug value 1", "" );
   PRM_DOUBLE(      debug2, efInner, "debug2", "Debug value 2", "" );
   PRM_DOUBLE(      debug3, efInner, "debug3", "Debug value 3", "" );


   ElemParams *eprm = nullptr;
   // cached vars, set in TMiso::do_startLoop
   int ca_noCalcStart = 0, ca_locked = 0, ca_ignored = 0, ca_onlyFirst = 0, ca_onlyLast = 0;

   DCL_DEFAULT_STATIC;
};

typedef TMiso* PTMiso;

