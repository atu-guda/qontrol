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
   QString inps[4]; 
   HolderString ho_inps0, ho_inps1, ho_inps2, ho_inps3;
   /** flag: no autotomatic processing */
   int noauto;
   HolderSwitch ho_noauto;
   /** flag: locked state: x = u[0] */
   int locked; 
   HolderSwitch ho_locked;
   /** flag: call f(u,t) only first time */
   int onlyFirst; 
   HolderSwitch ho_onlyFirst;
   /** flag: call f(u,t) only last time */
   int onlyLast; 
   HolderSwitch ho_onlyLast;
   /** flag: flip left-right element icon */
   int flip;
   HolderSwitch ho_flip;
   /** flag: dont show icon */
   int noIcon;
   HolderSwitch ho_noIcon;
   /** names of elms, which outputs will be used as parm inputs */
   QString pinps[4]; 
   HolderString ho_pinps0, ho_pinps1, ho_pinps2, ho_pinps3;
   /** names of inner params, to be modifyed */ 
   QString pnames[4]; 
   HolderString ho_pnames0, ho_pnames1, ho_pnames2, ho_pnames3;
   /** inner flags for params: 1-only start of loop, ..? */
   int pflags[4]; 
   HolderSwitch ho_pflags0, ho_pflags1, ho_pflags2, ho_pflags3; 
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
   int ord; 
   HolderInt ho_ord;
   /** description on element */
   QString descr;
   HolderString ho_descr;
   /** visual coordinates */
   int vis_x, vis_y;
   HolderInt ho_vis_x, ho_vis_y;
   /** time step -- setted by preRun */
   double tdt; 
   /** number of iteration per loop -- setted by PreRun */
   int model_nn;
   /** pointer to model-owner of this element, same as parent only 
    * between preRun -- postRun, elseware-0 */
   TModel *model;
   /** pointer to link data */
   TElmLink *links;
   HolderObj ho_links;
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
