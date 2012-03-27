/***************************************************************************
                          tmiso.h  -  description
                             -------------------
    begin                : Mon Jul 24 2000
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

/** \file tmiso.h 
  *@author atu
*/


#ifndef TMISO_H
#define TMISO_H

#include "dataset.h"

#define CLASS_ID_TElmLink 999
#define CLASS_ID_TMiso 1000

class TModel;

/** \class TElmLink  
 * aux class for get/set TMiso links and flags 
*/

class TElmLink : public TDataSet { //@ c,{},(380,320),"Object links"
  Q_OBJECT
 friend class TMiso;
 public:
   /** constructor */
   explicit TElmLink( TDataSet *apar );
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** names of elements, which outputs will be used as inputs */
   PRM_STRING( inps0, efNoRunChange );
   PRM_STRING( inps1, efNoRunChange );
   PRM_STRING( inps2, efNoRunChange );
   PRM_STRING( inps3, efNoRunChange );
   /** flag: no autotomatic processing */
   PRM_SWITCH( noauto, efNoRunChange );
   /** flag: locked state: x = u[0] */
   PRM_SWITCH( locked, efNoRunChange );
   /** flag: call f(u,t) only first time */
   PRM_SWITCH( onlyFirst, efNoRunChange ); 
   /** flag: call f(u,t) only last time */
   PRM_SWITCH( onlyLast, efNoRunChange ); 
   /** flag: flip left-right element icon */
   PRM_SWITCH( flip, efNoRunChange );
   /** flag: dont show icon */
   PRM_SWITCH( noIcon, efNoRunChange );
   /** names of elms, which outputs will be used as parm inputs */
   PRM_STRING( pinps0, efNoRunChange );
   PRM_STRING( pinps1, efNoRunChange );
   PRM_STRING( pinps2, efNoRunChange );
   PRM_STRING( pinps3, efNoRunChange );
   /** names of inner params, to be modifyed */ 
   PRM_STRING( pnames0, efNoRunChange );
   PRM_STRING( pnames1, efNoRunChange );
   PRM_STRING( pnames2, efNoRunChange );
   PRM_STRING( pnames3, efNoRunChange );
   /** inner flags for params: 1-only start of loop, ..? */
   PRM_SWITCH( pflags0, efNoRunChange );
   PRM_SWITCH( pflags1, efNoRunChange );
   PRM_SWITCH( pflags2, efNoRunChange );
   PRM_SWITCH( pflags3, efNoRunChange );
   /** data descriptors */ 
   static TDataInfo telmlink_d_i[40];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr; 
}; 
typedef TElmLink* PTElmLink;
typedef const TElmLink* CPTElmLink;

// ------------------------- LinkInfo ----------------------------

/** types of link */
enum ltype_t {
  LinkNone = 0, // not linked
  LinkElm,      // linked to element 
  LinkSpec,     // linked to special name, like ':prm1', ':t'
  LinkBad       // link target not found
};

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
   /** constructor */ 
   explicit TMiso( TDataSet *aparent );
   /** destructor */
   virtual ~TMiso();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual int getClassId(void) const = 0;
   /** class name - for check and human purpose */
   virtual const char* getClassName(void) const = 0;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const = 0;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;

   /** main computation function 
    * 
    * \param u array of input signals, for now 4 elems in it
    * \param t current time
    * \return output value of element
    * */
   virtual double f( const double *u, double t ) = 0;
   
   /** will be called before any action -- good place for allocs 
    *
    * \param run_tp current run type: 0-simple, 
    *      1 - 1d param loop, 2 - 2d param loop
    * \param an number of steps in modeling
    * \param anx number of inner param loop 
    * \param any number of outer param loop 
    * \param adt time step  - will be a \b tdt in elememt
    * */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** will be called after all actions -- good place for deallocs */
   virtual int postRun( int good );
   /** called before each inner param loop. */
   virtual int startLoop( int acnx, int acny );
   /** will be called after each inner loop  */
   virtual int endLoop(void);
 protected:
   /** order, in which elements will be used and holder */
   PRM_INT( ord, efRODial | efNoRunChange );
   /** description on element */
   PRM_STRING( descr, efNoRunChange );
   /** visual coordinates */
   PRM_INT( vis_x, efNoDial | efNoRunChange );
   PRM_INT( vis_y, efNoDial | efNoRunChange );
   /** time step -- setted by preRun */
   double tdt; 
   /** number of iteration per loop -- setted by PreRun */
   int model_nn;
   /** pointer to model-owner of this element, same as parent only 
    * between preRun -- postRun, elseware-0 */
   TModel *model;
   /** pointer to link data */
   TElmLink *links;
   PRM_OBJ( links, 0 );
   /** data descriptors -- empty -- never be used */ 
   static TDataInfo tmiso_d_i[2];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];

}; 

typedef TMiso* PTMiso;

#endif
