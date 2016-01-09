/***************************************************************************
                          linkedobj.h - object with link capabilities
                             -------------------
    begin                : 2015.10.24
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _LINKEDOBJ_H
#define _LINKEDOBJ_H

#include "dataset.h"

class InputAbstract; class InputSimple; class ParamDouble;

/** base class for all  objects, capable to double-type links*/
class LinkedObj : public TDataSet {
  Q_OBJECT
 public:
   DCL_CTOR(LinkedObj);
   virtual ~LinkedObj() override;
   DCL_CREATE;
   DCL_STD_INF;

   /** returns pointer to given parameter, checking if valid
    * valid values:
    * 1.23e-5 - direct double value
    * elmname = elmname.out0
    * elmname.parmname
    * parmname
    * lt - ptr: store link type,
    * targ - ptr_pre: store ptr to source LinkedObj,
    * lev - level of recursion, not for user */
   virtual const double* getDoublePtr( const QString &nm, int *lt = nullptr,
        const LinkedObj **src_ob = nullptr, int lev = 0 ) const;
   //* transmit this request to parent, until scheme detected, where work done
   // via getDoublePtr
   virtual const double* getSchemeDoublePtr( const QString &nm, int *lt = nullptr,
        const LinkedObj **src_ob = nullptr, int lev = 0 ) const;
   /** gets pointer to parameter, near to getDoublePtr
    * for parameter mod only - no descend  */
   double* getDoublePrmPtr( const QString &nm, int *flg );
   bool isIgnored() const; // self or parents...
   virtual void do_structChanged() override;

   Q_INVOKABLE int getN_Inputs() const { return inps.size(); }
   Q_INVOKABLE int getN_SimpleInputs() const { return inps_s.size(); }
   Q_INVOKABLE int getN_ActiveInputs() const { return inps_a.size(); }
   Q_INVOKABLE int getN_ActiveParmInputs() const { return inps_ap.size(); }

   virtual void readInputs();
   virtual void readAllInputs();
 protected:

   //* ptrs to all inputs: filled by do_structChanged
   QList<InputAbstract*> inps;
   //* ptrs to active inputs: LinkElm or LinkSpec
   QList<InputAbstract*> inps_a;
   //* ptrs to simple inputs:
   QList<InputSimple*> inps_s;
   //* ptrs to active param inputs: (onlyOnce Counted)
   QList<ParamDouble*> inps_ap;

   bool needReadInputsRecurse = false;
   int prm_mod = 0; //* parameters modified during run flag
   int prm_will_mod = 0; //** parameters modification will require fixing, prm_mod - indicator

   DCL_DEFAULT_STATIC;
};

/** types of link */
enum ltype_t {
  LinkNone = 0, // not linked or direct value
  LinkElm,      // linked to element
  LinkSpec,     // linked to special name, like 'prm1', 't'
  LinkBad       // link source or target not found
};

// ---------------------------------------------------------------------
/** Abstract-alike special holder link, parent for all inputs */
class InputAbstract : public LinkedObj {
  Q_OBJECT
 public:
  DCL_CTOR(InputAbstract);
  virtual ~InputAbstract();  // must be abstract, but in this case cannot register
  virtual void reset_dfl() override;
  virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
  DCL_CREATE;
  DCL_STD_INF;
  //* return ptr to LinkedObj, which holds element or nullptr;
  const LinkedObj* getSourceObj() const { return src_obj; };
  void readInput() { out0 = *p; };
  int  getLinkType() const { return linkType; }
  int  getOnlyFirst() const { return onlyFirst; }
  /** find and set link to source or fake source */
  virtual void set_link();

  // less operators for double: const only
  operator double() const { return out0; };
  const double* caddr() const { return p; };
  Q_INVOKABLE double vc() const { return out0; } // synonims
  double cval() const { return out0; }

 protected:
  virtual void do_post_set() override;
  /** do real actions after structure changed */
  virtual void do_structChanged() override;

  PRM_STRING( source, efNoRunChange, "Source", "Address of signal source", "max=128\nprops=STRING,SIMPLE,LINK\ncmpl=in"  );
  PRM_SWITCH( onlyFirst, 0, "only First", "apply only at start of run", "" );
  PRM_STRING( label,  efNoRunChange, "Label", "Label to display on structure", "max=64"  );
  PRM_INT( x_shift, 0, "X shift", "Shift on x-part of link representation", "sep=col" );
  PRM_INT( y_shift, 0, "Y shift", "Shift on y-part of link representation", "" );
  PRM_INT( line_w,  0, "Line width", "Line width on scheme", "def=1\nmin=0\nmax=20" );
  PRM_COLOR( line_color,  0, "Line color", "Line color on scheme", "def=black" );
  PRM_SWITCH( onlyLabel, 0, "only Label", "draw only link label on scheme", "" );

  PRM_INT( linkType,  efInner | efRO, "Link type", "Describes link type", "def=3" ); // def=LinkBad
  PRM_STRING( srcObjName, efInner | efRO, "Source object", "Name of the source object", ""  );

  PRM_DOUBLE( out0, efInner, "Output", "Main output", "" );

  double direct_in = 0;
  const double *p = &direct_in;
  const LinkedObj *src_obj = nullptr;

  Q_CLASSINFO( "nameHintBase",  "inx_" );
  DCL_DEFAULT_STATIC;
};

// ---------------------------------------------------------------------
/** Special holder link, have QString, but can be used as const double */
class InputSimple : public InputAbstract {
  Q_OBJECT
 public:
  DCL_CTOR(InputSimple);
  virtual ~InputSimple() override;
  DCL_CREATE;
  DCL_STD_INF;
  /** find and set link to source (external or direct_in) */
  virtual void set_link() override;
 protected:
  virtual void do_post_set() override;

  PRM_INT( channel, efNRC, "Channel", "Channel number of this input", "sep=block" );
  PRM_INT( subchannel, efNRC, "Subchannel", "Subchannel number of this input", "sep=col" );

  Q_CLASSINFO( "nameHintBase",  "in_" );
  DCL_DEFAULT_STATIC;
};

#define PRM_INPUT( name, flags, vname, descr, extra ) \
  InputSimple name = { #name, this, flags, vname, descr, extra  } ;

// ---------------------------------------------------------------------
/** Special input - paramertic / value, can be used as double */
class ParamDouble : public InputAbstract {
  Q_OBJECT
 public:
  DCL_CTOR(ParamDouble);
  virtual ~ParamDouble() override;
  DCL_CREATE;
  DCL_STD_INF;
  virtual QIcon getIcon() const override { return QIcon(); }; // empty icon: save space
  operator double&() { return out0; };
  double& operator=( double rhs ) { out0 = rhs; return out0; }
  Q_INVOKABLE bool isFixparmNeed() const { return need_fixparm; };
  /** find and set link to values */
  virtual void set_link() override;
 protected:
  virtual void do_post_set() override;

  PRM_STRING( tparam, efOld, "Param", "old param names - remove after cnv", ""  );
  bool need_fixparm = false;

  Q_CLASSINFO( "nameHintBase",  "p_" );
  DCL_DEFAULT_STATIC;
};

#define PRM_PARAMD( name, flags, vname, descr, extra ) \
  ParamDouble name = { #name, this, flags, vname, descr, extra  } ;



// ----------------------------------------------------------------

#endif // _LINKEDOBJ_H


