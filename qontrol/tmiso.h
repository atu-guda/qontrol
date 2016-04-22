#pragma once
/***************************************************************************
                          tmiso.h  -  description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2016 by atu
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
   void preCalc();
 protected:
   /** main computation function
    * \param t current time
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

   PRM_STRING( descr, efNRC, "description", "Object description", "max=128\nncol=-1");
   PRM_INT( ord, efOld, "Order", "obsoleted", "" );
   /** visual coordinates */
   PRM_INT( vis_x, efRODial | efNRC, "Visual x",
       "X-coordinate of element in scheme", "min=0\nmax=64\nsep=block" );
   PRM_INT( vis_y, efRODial | efNRC, "Visual y",
       "Y-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_PARAMD( out0_init, efNRC, "Init value", "Initial value of output", "def=0.0" );
   PRM_SWITCH( noCalcStart, efNRC, "No calc at start", "Disable out0 at startLoop ", ""  );

   PRM_SWITCH( locked, efNRC, "Locked", "Bypass out0_init to output", "sep=col"  );
   PRM_SWITCH( ignored, efNRC, "Ignored", "Ignore element while run", ""  );
   PRM_SWITCH( onlyFirst, efNRC, "only First", "Process element only at first iteration", "" );
   PRM_SWITCH( onlyLast, efNRC , "only Last", "Process element only at last iteration", "" );
   PRM_SWITCH( flip, efNRC, "flip image", "flip left-right element icon", "sep=col" );
   PRM_SWITCH( noIcon, efNRC, "no Icon", "don't show element icon", "sep=blockend" );

   PRM_DOUBLE( out0, efInner, "Output", "Main output", "" );

   DCL_DEFAULT_STATIC;

};

typedef TMiso* PTMiso;

