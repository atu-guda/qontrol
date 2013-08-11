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

/** iteration type */
enum IterType {
  IterMid = 0,     // not-first and not-last iteration
  IterFirst = 1,   // first iteration in i inner loop
  IterLast = 2     // last iteration in inner loop
};

class TModel;

/** \class TElmLink  
 * aux class for get/set TMiso links and flags 
*/

class TElmLink : public TDataSet {
  Q_OBJECT
 friend class TMiso;
 public:
   /** constructor */
   explicit TElmLink( TDataSet *apar );
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** names of elements, which outputs will be used as inputs */
   PRM_STRING1( inps0, efNoRunChange, "Input 0", "Name of source for input 0", "max=32" );
   PRM_STRING1( inps1, efNoRunChange, "Input 1", "Name of source for input 1", "max=32" );
   PRM_STRING1( inps2, efNoRunChange, "Input 2", "Name of source for input 2", "max=32" );
   PRM_STRING1( inps3, efNoRunChange, "Input 3", "Name of source for input 3", "max=32" );
   PRM_SWITCH1( noauto, efInner, "No Auto", "Unused", "" );
   PRM_SWITCH1( locked, efNoRunChange, "Locked", "Bypass u[0] to output", "sep=col"  );
   PRM_SWITCH1( onlyFirst, efNoRunChange, "only First", "Process element only at first iteration", ""); 
   PRM_SWITCH1( onlyLast, efNoRunChange , "only Last", "Process element only at last iteration", ""); 
   PRM_SWITCH1( flip, efNoRunChange, "flip image", "flip left-right element icon", "sep=col");
   PRM_SWITCH1( noIcon, efNoRunChange, "no Icon", "don't show element icon", "");
   /** names of elms, which outputs will be used as parm inputs */
   PRM_STRING1( pinps0, efNoRunChange, "Parm. input 0", "Name of source for parametric input 0", "max=32\nsep=block"  );
   PRM_STRING1( pinps1, efNoRunChange, "Parm. input 1", "Name of source for parametric input 1", "max=32"  );
   PRM_STRING1( pinps2, efNoRunChange, "Parm. input 2", "Name of source for parametric input 2", "max=32"  );
   PRM_STRING1( pinps3, efNoRunChange, "Parm. input 3", "Name of source for parametric input 3", "max=32"  );
   /** names of inner params, to be modifyed */ 
   PRM_STRING1( pnames0, efNoRunChange, "Parm. name 0", "Name of inner parameter 0", "max=32\nsep=col" );
   PRM_STRING1( pnames1, efNoRunChange, "Parm. name 1", "Name of inner parameter 1", "max=32"  );
   PRM_STRING1( pnames2, efNoRunChange, "Parm. name 2", "Name of inner parameter 2", "max=32"  );
   PRM_STRING1( pnames3, efNoRunChange, "Parm. name 3", "Name of inner parameter 3", "max=32"  );
   /** inner flags for params: 1-only start of loop, ..? */
   PRM_SWITCH1( pflags0, efNoRunChange, "only First 0", "Change param 0 only at start", "sep=col" );
   PRM_SWITCH1( pflags1, efNoRunChange, "only First 1", "Change param 1 only at start", ""  );
   PRM_SWITCH1( pflags2, efNoRunChange, "only First 2", "Change param 2 only at start", ""  );
   PRM_SWITCH1( pflags3, efNoRunChange, "only First 3", "Change param 3 only at start", ""  );
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr; 
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
   explicit TMiso( TDataSet *aparent );
   virtual ~TMiso();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const = 0;
   /** returns help string */
   virtual const char* getHelp(void) const;

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
   virtual double f( double t ) = 0;
   /** place of customization of preRun, return: 0 = Ok */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** will be called after all actions from posrtRun  -- good place for deallocs */
   virtual int do_postRun( int good );
   /** called before each inner param loop from startLoop */
   virtual int do_startLoop( int acnx, int acny );
   /** will be called after each inner loop: called frop endLoop */
   virtual int do_endLoop(void);
   /** fill links to sources */
   int fillLinks();
   /** proceed parameters modification before first iteration 
    * returns number of modified params */
   int modifyPrmsPre();
   /** proceed parameters modification in every iteration before fun()
    * returns number of modified params */
   int modifyPrms();
   /** description on element */
   PRM_STRING1( descr, efNoRunChange, "description", 
       "Object description", "max=128\nncol=-1");
   /** order, in which elements will be used and holder */
   PRM_INT1( ord, efRODial | efNoRunChange, "Order", 
       "Order, in which element will be processed", "min=0\nsep=block" );
   /** visual coordinates */
   PRM_INT1( vis_x, efRODial | efNoRunChange, "Visual x", 
       "X-coordinate of element in scheme", "min=0\nmax=64\nsep=col" );
   PRM_INT1( vis_y, efRODial | efNoRunChange, "Visual y", 
       "Y-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_DOUBLE1( out0_init, efNoRunChange, "Init output", 
       "Initial value of output", "sep=col" );
   PRM_DOUBLE1( out0, efInner, "Output", 
       "Main output", "" );
   /** pointer to link data */
   TElmLink *links;
   PRM_OBJ1( links, 0, "object links", "Object links description", "sep=blockend" );
   /** time step -- setted by preRun */
   double tdt; 
   /** number of iteration per loop -- setted by PreRun */
   int model_nn;
   /** pointer to model-owner of this element, same as parent only 
    * between preRun -- postRun, elseware-0 */
   TModel *model;
   /** fake source */
   double fake_so = 0;
   /** fake param target */
   double fake_prm = 0;
   /** pointers to inputs */
   const double* in_so[4] = { &fake_so, &fake_so, &fake_so, &fake_so };
   /** pointers to param inputs */
   const double* inp_so[4] = { &fake_so, &fake_so, &fake_so, &fake_so };
   /** pointers to params */
   double* inp_prm[4] = { &fake_so, &fake_so, &fake_so, &fake_so };
   /** target param flags */
   int prm_flg[4] = { -1, -1, -1, -1 };
   /** maximum (index+1) in parameters - may be gaps */
   int max_prm = 0;
   /** parameters modified during run flag */
   int prm_mod = 0;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;

}; 

typedef TMiso* PTMiso;

#endif
