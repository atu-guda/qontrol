/***************************************************************************
                          simul.h  -  description
                             -------------------
    begin                : 2013.10.05
    copyright            : (C) 2013-2014 by atu
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

#ifndef SIMUL_H
#define SIMUL_H

#include "dataset.h"
#include "datacont.h"


/** Description of simulation
  *@author atu
  */

class Simulation : public TDataSet  {
  Q_OBJECT
 public:
  DCL_CTOR(Simulation);
  virtual ~Simulation() override;
  DCL_CREATE;
  DCL_STD_INF;

  enum RunType {
    runSingle = 0, runLoop, run2DLoop
  };
  Q_ENUMS(RunType);
  Q_CLASSINFO( "enum_RunType_0", "Single" );      // runSingle
  Q_CLASSINFO( "enum_RunType_1", "1D loop" );     // runLoop
  Q_CLASSINFO( "enum_RunType_2", "2D loop" );     // run2DLoop

  enum SeedType {
    everyRun = 0, startLoop, start2DLoop
  };
  Q_ENUMS(SeedType);
  Q_CLASSINFO( "enum_SeedType_0", "Every Run" );               // everyRun
  Q_CLASSINFO( "enum_SeedType_1", "On start on 1D loop" );     // startLoop
  Q_CLASSINFO( "enum_SeedType_2", "On start on 2D loop" );     // start2DLoop


  //virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
  virtual QIcon getIcon() const override;

  virtual void post_set() override;

 protected:
  // simulation parameters, borrowed/renamed from TModel
  PRM_LIST( runType, efNoRunChange, "Run type",  "type of simulation run", "enum=RunType" );
  PRM_DOUBLE( T, efNoRunChange, "T", "Full Run Time", "min=0\nmax=1e300\ndef=100" );
  PRM_INT( N, efNoRunChange, "N", "Number of steps in one run",
      "min=1\nmax=200000000\nsep=col\ndef=10000"  );
  PRM_SWITCH( syncRT, efNoRunChange, "Sync RT",
      "Real and model time synchronization ", "sep=col" );
  PRM_INT( N1, efNoRunChange, "N1",
       "Number of inner parametric loops iterations",
       "min=1\nmax=10000\nsep=block\ndef=1" );
  /** number of outer parametric loops iterations */
  PRM_INT( N2, efNoRunChange, "N2",
       "Number of outer parametric loops iterations",
       "min=1\nmax=10000\nsep=col\ndef=1" );
  /** number of steps per i/o action */
  PRM_INT( n_iosteps, efNoRunChange, "N IO steps",
      "number of steps per i/o action ",
      "min=1\nmax=100000\nsep=col\ndef=1000" );
  /** Initial parameters values */
  PRM_DOUBLE( prm0s, efNoRunChange, "prm0s", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE( prm1s, efNoRunChange, "prm1s", "Initial prm1 value", "" );
  PRM_DOUBLE( prm2s, efNoRunChange, "prm2s", "Initial prm2 value", ""  );
  PRM_DOUBLE( prm3s, efNoRunChange, "prm3s", "Initial prm3 value", ""  );
  PRM_DOUBLE( prm0d, efNoRunChange, "prm0+=", "Parameter 0 delta", "sep=col" );
  PRM_DOUBLE( prm1d, efNoRunChange, "prm1+=", "Parameter 1 delta", "" );
  PRM_INT( seed, efNoRunChange, "Seed", "Seed for random generator" , "min=-1\ndef=RND" );
  PRM_LIST( seedType, efNoRunChange, "Seed type",  "type of seeding", "enum=SeedType" );
  PRM_SWITCH( autoStart, efNoRunChange, "auto start",
      "Start simulation without key awaiting", "sep=col" );
  // -------- TODO: input/output channels -------
  /** description */
  PRM_STRING( descr, 0, "Description", "Simulation description",
      "props=STRING,MLINE\nncol=-1\nsep=block");
  //* helper values to simplify usage
  PRM_INT( n1_eff, efInner, "n1_eff", "Effective number of iteration in inner loop" , "" );
  PRM_INT( n2_eff, efInner, "n2_eff", "Effective number of iteration in outer loop" , "" );
  PRM_INT( n_tot, efInner, "n_tot", "Total number of iterations" , "" );

  DCL_DEFAULT_STATIC;

};

// ------------------ container of Simulations --------------

class ContSimul : public TDataContainer {
   Q_OBJECT
  public:
   DCL_CTOR(ContSimul);
   virtual ~ContSimul() override;
   DCL_CREATE;
   DCL_STD_INF;
  private:
   DCL_DEFAULT_STATIC;
};

#endif

