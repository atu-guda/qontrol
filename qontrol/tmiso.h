#pragma once
/***************************************************************************
                          tmiso.h  -  description
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

// aux element params: TODO: move here from TMiso
class ElemParams : public TDataSet  {
  Q_OBJECT
 public:
   DCL_CTOR(ElemParams);
   // virtual ~ElemParams() override;
   DCL_CREATE;
   DCL_STD_INF;

 public: // SIC, this is a structure!
   PRM_INT(          vis_x, efNRC,         "Visual x", "X-coordinate of element in scheme", "min=0\nmax=1024" );
   PRM_INT(          vis_y, efNRC,         "Visual y", "Y-coordinate of element in scheme", "min=0\nmax=1024" );
   PRM_SWITCH( noCalcStart, efNRC, "No calc at start", "Disable out0 at startLoop ", ""  );

   PRM_SWITCH(      locked, efNRC,           "Locked", "Bypass out0_init to output"              , "sep=col"  );
   PRM_SWITCH(     ignored, efNRC,          "Ignored", "Ignore element while run"                , ""  );
   PRM_SWITCH(   onlyFirst, efNRC,       "only First", "Process element only at first iteration" , "" );
   PRM_SWITCH(    onlyLast, efNRC,        "only Last", "Process element only at last iteration"  , "" );
   PRM_SWITCH(        flip, efNRC,       "flip image", "flip left-right element icon"            , "sep=col" );
   PRM_SWITCH(      noIcon, efNRC,          "no Icon", "don't show element icon"                 , "" );
   PRM_SWITCH(  showBaloon, efNRC,      "show Baloon", "Show baloon on scheme with out0"         , "" );

   PRM_STRING(   baloonStr, efNRC,    "Baloon string", "Baloon format string", "sep=block\nmax=256\nncol=-1");

   DCL_DEFAULT_STATIC;
};

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

   PRM_STRING(       descr, efNRC, "description", "Object description", "max=128\nncol=-1");
   PRM_INT(            ord, efOld, "Order", "obsoleted", "" );
   PRM_INT(          vis_x, efNoDial | efNRC, "Visual x", "X-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_INT(          vis_y, efNoDial | efNRC, "Visual y", "Y-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_PARAMD(   out0_init, efNRC,       "Init value", "Initial value of output", "def=0.0\nsep=block" );
   PRM_DOUBLE(        out0, efRO | efNoSave, "Output", "Main output", "" );
   PRM_SWITCH( noCalcStart,  efNRC, "No calc at start", "Disable out0 at startLoop ", ""  );

   PRM_SWITCH( locked     ,  efNRC,          "Locked", "Bypass out0_init to output"              , "sep=col"  );
   PRM_SWITCH( ignored    ,  efNRC,         "Ignored", "Ignore element while run"                , ""  );
   PRM_SWITCH( onlyFirst  ,  efNRC,      "only First", "Process element only at first iteration" , "" );
   PRM_SWITCH( onlyLast   ,  efNRC,       "only Last", "Process element only at last iteration"  , "" );
   PRM_SWITCH( flip       ,  efNRC,      "flip image", "flip left-right element icon"            , "sep=col" );
   PRM_SWITCH( noIcon     ,  efNRC,         "no Icon", "don't show element icon"                 , "" );
   PRM_SWITCH( showBaloon ,  efNRC,     "show Baloon", "Show baloon on scheme with out0"         , "" );

   ElemParams *eprm = nullptr;

   DCL_DEFAULT_STATIC;
};

typedef TMiso* PTMiso;

