/***************************************************************************
                          tmiso.h  -  description
                             -------------------
    begin                : Mon Jul 24 2000
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

/** \file tmiso.h
  *@author atu
*/


#ifndef TMISO_H
#define TMISO_H

#include "dataset.h"
#include "inputparams.h"

/** iteration type */
enum IterType {
  IterMid = 0,     // not-first and not-last iteration
  IterFirst = 1,   // first iteration in i inner loop
  IterLast = 2     // last iteration in inner loop
};

class TModel;

/** class TElmLink  - obseleted by InputSimple and Input Param
 * aux class for get/set TMiso links and flags
*/

class TElmLink : public TDataSet {
  Q_OBJECT
 friend class TMiso;
 public:
  DCL_CTOR(TElmLink);
  DCL_CREATE;
  DCL_STD_INF;
 protected:
   /** names of elements, which outputs will be used as inputs */
   PRM_STRING( inps0, efOld, "Input 0", "Name of source for input 0", "max=32" );
   PRM_STRING( inps1, efOld, "Input 1", "Name of source for input 1", "max=32" );
   PRM_STRING( inps2, efOld, "Input 2", "Name of source for input 2", "max=32" );
   PRM_STRING( inps3, efOld, "Input 3", "Name of source for input 3", "max=32" );
   PRM_SWITCH( noauto, efOld, "No Auto", "Unused", "def=0" );
   PRM_SWITCH( locked, efOld, "Locked", "Bypass u[0] to output", "sep=col"  );
   PRM_SWITCH( onlyFirst, efOld, "only First", "Process element only at first iteration", "");
   PRM_SWITCH( onlyLast, efOld , "only Last", "Process element only at last iteration", "");
   PRM_SWITCH( flip, efOld, "flip image", "flip left-right element icon", "sep=col");
   PRM_SWITCH( noIcon, efOld, "no Icon", "don't show element icon", "");
   /** names of elms, which outputs will be used as parm inputs */
   PRM_STRING( pinps0, efOld, "Parm. input 0", "Name of source for parametric input 0", "max=32\nsep=block"  );
   PRM_STRING( pinps1, efOld, "Parm. input 1", "Name of source for parametric input 1", "max=32"  );
   PRM_STRING( pinps2, efOld, "Parm. input 2", "Name of source for parametric input 2", "max=32"  );
   PRM_STRING( pinps3, efOld, "Parm. input 3", "Name of source for parametric input 3", "max=32"  );
   /** names of inner params, to be modifyed */
   PRM_STRING( pnames0, efOld, "Parm. name 0", "Name of inner parameter 0", "max=32\nsep=col" );
   PRM_STRING( pnames1, efOld, "Parm. name 1", "Name of inner parameter 1", "max=32"  );
   PRM_STRING( pnames2, efOld, "Parm. name 2", "Name of inner parameter 2", "max=32"  );
   PRM_STRING( pnames3, efOld, "Parm. name 3", "Name of inner parameter 3", "max=32"  );
   /** inner flags for params: 1-only start of loop, ..? */
   PRM_SWITCH( pflags0, efOld, "only First 0", "Change param 0 only at start", "sep=col" );
   PRM_SWITCH( pflags1, efOld, "only First 1", "Change param 1 only at start", ""  );
   PRM_SWITCH( pflags2, efOld, "only First 2", "Change param 2 only at start", ""  );
   PRM_SWITCH( pflags3, efOld, "only First 3", "Change param 3 only at start", ""  );
   DCL_DEFAULT_STATIC;
};
typedef TElmLink* PTElmLink;
typedef const TElmLink* CPTElmLink;

// ------------------------- LinkInfo ----------------------------

/**
 * \struct LinkInfo tmiso.h
 * filled by TModel::getLinkInfos
 * */
struct LinkInfo {
  ltype_t ltype; /** type in link */
  int elnu;  /** number of linked element or -1; */
  int x, y;  /** coordinates of linked element or -1 */
  int eflip; /** flag: linked element is flipped */
  int pflags; /** flags from inner model array */
  //
  LinkInfo() { reset(); }
  void reset() {
    ltype = LinkNone; elnu = x = y = -1;
    eflip = 0; pflags = 0;
  };
};


// -------------------------- TMiso -----------------------------

/** \class TMiso tmiso.h
 * \breef Pure parent of all model elements.
 *
 */

class TMiso : public TDataSet  {
  Q_OBJECT
 public:
   DCL_CTOR(TMiso);
   virtual ~TMiso();
   DCL_CREATE;
   DCL_STD_INF;
   virtual void post_set() override;

   /** external computation function + in/out */
   double fun( double t, IterType itype );

   /** will be called before any action -- good place for allocs
    *
    * \param run_tp current run type: 0-simple,
    *      1 - 1d param loop, 2 - 2d param loop
    * \param an number of steps in modeling
    * \param anx number of inner param loop
    * \param any number of outer param loop
    * \param adt time step  - will be a \b tdt in elememt
    * non-virtual: adjusted by do_preRun
    * */
   int preRun( int run_tp, int an, int anx, int any, double adt );
   /** will be called after all actions -- call do_postRun */
   int postRun( int good );
   /** called before each inner param loop -- call do_startLoop */
   int startLoop( int acnx, int acny );
   /** will be called after each inner loop -- call do_endLoop */
   int endLoop(void);
 protected:
   /** main computation function
    * \param t current time
    * \return output value of element
    * */
   virtual double f( double t );
   /** place of customization of preRun, return: 0 = Ok */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** will be called after all actions from posrtRun  -- good place for deallocs */
   virtual int do_postRun( int good );
   /** called before each inner param loop from startLoop */
   virtual int do_startLoop( int acnx, int acny );
   /** will be called after each inner loop: called frop endLoop */
   virtual int do_endLoop(void);
   /** description on element */
   PRM_STRING( descr, efNoRunChange, "description",
       "Object description", "max=128\nncol=-1");
   /** order, in which elements will be used and holder */
   PRM_INT( ord, efRODial | efNoRunChange, "Order",
       "Order, in which element will be processed", "min=0\nsep=block" );
   /** visual coordinates */
   PRM_INT( vis_x, efRODial | efNoRunChange, "Visual x",
       "X-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_INT( vis_y, efRODial | efNoRunChange, "Visual y",
       "Y-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_DOUBLE( out0_init, efNoRunChange, "Init value",
       "Initial value of output", "" );
   PRM_SWITCH( locked, efNoRunChange, "Locked", "Bypass u[0] to output", "sep=col"  );
   PRM_SWITCH( flip, efNoRunChange, "flip image", "flip left-right element icon", "");
   PRM_SWITCH( onlyFirst, efNoRunChange, "only First", "Process element only at first iteration", "cep=col");
   PRM_SWITCH( onlyLast, efNoRunChange , "only Last", "Process element only at last iteration", "");
   PRM_SWITCH( noIcon, efNoRunChange, "no Icon", "don't show element icon", "sep=col");

   PRM_DOUBLE( out0, efInner, "Output", "Main output", "" );

   /** pointer to link data */
   TElmLink *links;
   /** */
   InputParams *pis;
   // PRM_OBJ1( links, 0, "object links", "Object links description", "sep=blockend" );
   /** time step -- setted by preRun 0 - special value to detect usage before start */
   double tdt = 0;
   /** number of iteration per loop -- setted by PreRun */
   int model_nn = 0;
   /** pointer to model-owner of this element, same as parent only
    * between preRun -- postRun, elseware-0 TODO: remove */
   TModel *model = nullptr;
   /** parameters modified during run flag */
   int prm_mod = 0;

   DCL_DEFAULT_STATIC;

};

typedef TMiso* PTMiso;

#endif
