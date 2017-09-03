#pragma once
/***************************************************************************
                          tmiso.h  - base for scheme elements description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2017 by atu
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
   Q_INVOKABLE virtual double getDouble( int /*idx*/ = 0 ) const override { return out0; }
   Q_INVOKABLE virtual QString textVisual() const override;

   /** external computation function + in/out */
   double fun( IterType itype ) noexcept;
   //* to call directly - only for some pure types
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 );

   /** check, if element have given visual coords */
   bool isAtCoord( int ax, int ay ) const
     { return (vis_x == ax && vis_y == ay ); }
   virtual void fillComplModelForParams( QStandardItemModel *mdl ) const;
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

   PRM_STRING(       descr, efNRC,  "description", "Object description", "max=128\nncol=-1");
   PRM_INT(            ord, efOld,  "Order", "obsoleted", "" );
   PRM_INT(          vis_x, efOld,  "Visual x", "X-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_INT(          vis_y, efOld,  "Visual y", "Y-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_PARAMD(   out0_init, efNRC,  "Init value", "Initial value of output", "def=0.0\nsep=block" );
   PRM_DOUBLE(        out0, efRO | efNoSave, "Output", "Main output", "sep=col" );
   PRM_SWITCH( noCalcStart,  efOld, "No calc at start", "Disable out0 at startLoop ", ""  );

   // these parameters is moed to ElemParams, remove after conversion
   PRM_SWITCH( locked     ,  efOld,          "Locked", "Bypass out0_init to output"              , "sep=col"  );
   PRM_SWITCH( ignored    ,  efOld,         "Ignored", "Ignore element while run"                , ""  );
   PRM_SWITCH( onlyFirst  ,  efOld,      "only First", "Process element only at first iteration" , "" );
   PRM_SWITCH( onlyLast   ,  efOld,       "only Last", "Process element only at last iteration"  , "" );
   PRM_SWITCH( flip       ,  efOld,      "flip image", "flip left-right element icon"            , "sep=col" );
   PRM_SWITCH( noIcon     ,  efOld,         "no Icon", "don't show element icon"                 , "" );
   PRM_SWITCH( showBaloon ,  efOld,     "show Baloon", "Show baloon on scheme with out0"         , "" );

   ElemParams *eprm = nullptr;
   // cached vars, set in TMiso::do_startLoop
   int ca_noCalcStart = 0, ca_locked = 0, ca_ignored = 0, ca_onlyFirst = 0, ca_onlyLast = 0;

   DCL_DEFAULT_STATIC;
};

typedef TMiso* PTMiso;

