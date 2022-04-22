#pragma once
/***************************************************************************
                          simul.h  -  description
                             -------------------
    begin                : 2013.10.05
    copyright            : (C) 2013-2022 by atu
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


#include "linkedobj.h"


/** Description of simulation
  *@author atu
  */

class Simulation : public LinkedObj  {
  Q_OBJECT
 public:
  DCL_CTOR(Simulation);
  // virtual ~Simulation() override;
  DCL_CREATE;
  DCL_STD_INF;

  enum RunType {
    runSingle = 0, runLoop, run2DLoop
  };
  Q_ENUM( RunType );
  Q_CLASSINFO( "enum_RunType_0", "Single" );      // runSingle
  Q_CLASSINFO( "enum_RunType_1", "1D loop" );     // runLoop
  Q_CLASSINFO( "enum_RunType_2", "2D loop" );     // run2DLoop

  enum SeedType {
    everyRun = 0, startLoop, start2DLoop
  };
  Q_ENUM( SeedType );
  Q_CLASSINFO( "enum_SeedType_0", "Every Run" );               // everyRun
  Q_CLASSINFO( "enum_SeedType_1", "On start on 1D loop" );     // startLoop
  Q_CLASSINFO( "enum_SeedType_2", "On start on 2D loop" );     // start2DLoop


  //virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
  virtual QIcon getIcon() const override;


 protected:
  // simulation parameters, borrowed/renamed from TModel
  PRM_LIST(     runType,      efNRC, "&Run type",  "type of simulation run", "enum=RunType" );
  PRM_DOUBLE(         T,      efNRC, "&T", "Full Run Time", "min=0\nmax=1e300\ndef=100" );
  PRM_DOUBLE(       t_0,      efNRC, "t_0", "Start time", "def=0" );
  PRM_DOUBLE(     T_brk,      efNRC, "T_{brk}", "Break time", "min=0\nmax=1e300\ndef=1e100" );
  PRM_LONG(           N,      efNRC, "&N", "Number of steps in one run", "min=1\nmax=200000000\nsep=col\ndef=10000"  );
  PRM_DOUBLE(       tdt,  efNRC | efRODial | efNoSave, "\\tau", "step time", "def=0.01" );
  PRM_INT(        n_pre,       efNRC, "N pre", "Number of calculations at startLoop ", "def=5" );
  PRM_SWITCH(    syncRT,       efNRC, "Sync RT", "Real and model time synchronization ", "sep=col" );
  PRM_DOUBLE(      io_t,       efNRC, "I/O time", "Time per each I/O action", "def=0.1\nmin=1e-4" );
  PRM_SWITCH(    fakeRT,       efNRC, "Fake RT",  "Real time is set to model time", "" );

  /** Loop parameter values */
  PRM_LONG(          N1,       efNRC, "N&1",
       "Number of inner parametric loops iterations",
       "min=1\nmax=10000\nsep=block\ndef=1" );
  PRM_DOUBLE(     prm0s,       efNRC, "prm0s", "Initial prm0 value", "" );
  PRM_DOUBLE(     prm0d,       efNRC, "prm0+=", "Parameter 0 delta", "" );
  PRM_STRING(  prm0_map,       efNRC, "Map to(0)", "Change parameter with prm0", "max=128\ncmpl=in" );
  PRM_LONG(          N2,       efNRC, "N&2",
       "Number of outer parametric loops iterations",
       "min=1\nmax=10000\nsep=col\ndef=1\nsep=col" );
  PRM_DOUBLE(     prm1s,       efNRC, "prm1s", "Initial prm1 value", "" );
  PRM_DOUBLE(     prm1d,       efNRC, "prm1+=", "Parameter 1 delta", "" );
  PRM_STRING(  prm1_map,       efNRC, "Map to(1)", "Change parameter with prm1", "max=128\ncmpl=in" );
  PRM_DOUBLE(     prm2s,     efInner, "prm2s", "Initial prm2 value", ""  );
  PRM_DOUBLE(     prm3s,     efInner, "prm3s", "Initial prm3 value", ""  );
  PRM_SWITCH(saveParams,       efNRC, "Save params", "Save mapped parameters during run, and restore after", "def=1\nsep=col" );
  PRM_SWITCH( autoStart,       efNRC, "auto start",  "Start simulation without key awaiting", "" );
  PRM_INT(         seed,       efNRC, "Seed", "Seed for random generator" , "min=-1\ndef=RND" );
  PRM_LIST(    seedType,       efNRC, "Seed type",  "type of seeding", "enum=SeedType" );
  PRM_STRING( askParams,       efNRC, "Ask params:", "Ask this parameters before run: var1,var2", "ncol=-1\nsep=block" );
  //* -------- script params -------------------------------
  PRM_SWITCH( useScripts,      efNRC, "use &scripts", "Turn on all script usage", "def=0\nsep=block" );
  PRM_SWITCH( initEng,         efNRC, "Init engine", "Init engine before run", "def=1" );
  PRM_SWITCH( execModelScript, efNRC, "Exec model script", "Execute model script before run", "def=1" );
  PRM_STRING( scriptPreRun,    efNRC, "preRun",  "Execute this before all loops", "sep=col" );
  PRM_STRING( scriptPostRun,   efNRC, "postRun", "Execute this after all loops", "" );
  PRM_SWITCH( brkOnZero,       efNRC, "Break if rc ==0", "Break execution, if one of pre scripts returns 0", "def=0" );
  PRM_STRING( scriptStartLoop, efNRC, "startLoop",  "Execute this is star of loop (il1, il2)", "sep=col" );
  PRM_STRING( scriptEndLoop,   efNRC, "endLoop", "Execute this after loop (il1, il2)", "sep=blockend" );
  // -------- TODO: input/output channels -------
  /** description */
  PRM_STRING( descr, 0, "Description", "Simulation description",
      "props=STRING,MLINE\nncol=-1\nsep=tabend\ntabname=Parameters");
  //* helper values to simplify usage
  PRM_LONG( n1_eff, efInner, "n1_eff", "Effective number of iteration in inner loop" , "" );
  PRM_LONG( n2_eff, efInner, "n2_eff", "Effective number of iteration in outer loop" , "" );
  PRM_LONG( n_tot,  efInner, "n_tot", "Total number of iterations" , "" );

  virtual void do_post_set() override;

  Q_CLASSINFO( "nameHintBase",  "sim" );
  DCL_DEFAULT_STATIC;

};

// ------------------ container of Simulations --------------

class ContSimul : public TDataSet {
   Q_OBJECT
  public:
   DCL_CTOR(ContSimul);
   virtual ~ContSimul() override;
   DCL_CREATE;
   DCL_STD_INF;
  private:
   DCL_DEFAULT_STATIC;
};


