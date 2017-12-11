#pragma once
/***************************************************************************
                          linkedobj.h - object with link capabilities
                             -------------------
    begin                : 2015.10.24
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include "dataset.h"

class InputAbstract; class InputSimple; class ParamDouble;
class TModel; class Scheme; class Simulation;

constexpr double TDT_MIN = 1e-30; // minimal tau

/** iteration type */
enum IterType {
  IterMid   = 0,   // not-first and not-last iteration
  IterFirst = 1,   // first iteration in i inner loop
  IterLast  = 2,   // last iteration in inner loop
  IterNo    = 3,   // before or after run
};

struct RunInfo {
  int run_tp;
  long N;
  long nx;
  long ny;
  int fakeRT;
  double tdt;
  double T;
  double t_0;
  const double *p_t_model;
  TModel *model;
  Simulation *sim;
  Scheme *sch;
};

/** base class for all  objects, capable to double-type links*/
class LinkedObj : public TDataSet {
  Q_OBJECT
 public:
   DCL_CTOR(LinkedObj);
   // virtual ~LinkedObj() override;
   DCL_CREATE;
   DCL_STD_INF;

   /** returns pointer to given parameter, checking if valid
    * valid values:
    * 1.23e-5 - direct double value -- NO, not here (see set_link)
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
    * for parameter mod only - no descend (now only in TModel param mirror)  */
   double* getDoublePrmPtr( const QString &nm, int *flg );
   bool isIgnored() const; // self or parents...
   virtual void do_structChanged() override;

   /** will be called before all runs -- good place for allocs
    *
    * \param run_tp current run type: 0-simple,
    *      1 - 1d param loop, 2 - 2d param loop
    * \param an number of steps in modeling
    * \param anx number of inner param loop
    * \param any number of outer param loop
    * \param adt time step  - will be a \b tdt in elememt
    * non-virtual: adjusted by do_preRun
    * */
   int preRun( const RunInfo *rinf_ );
   int postRun( int good ); //* will be called after all actions -- call do_postRun
   int startLoop( long acnx, long acny ); //* called before each inner param loop -- call do_startLoop
   int endLoop(); //* will be called after each inner loop -- call do_endLoop

   Q_INVOKABLE int getN_Inputs() const { return inps.size(); }
   Q_INVOKABLE int getN_SimpleInputs() const { return inps_s.size(); }
   Q_INVOKABLE int getN_ActiveInputs() const { return inps_a.size(); }
   Q_INVOKABLE int getN_ActiveParmInputs() const { return inps_ap.size(); }

   Q_INVOKABLE QStringList getInputsNames() const;
   Q_INVOKABLE QString lsInputs() const;

   void readInputs() noexcept;
   void readAllInputs() noexcept;
   Q_INVOKABLE virtual void iterateSources( int dn = 1 );
 protected:

   double ct { 0.0 };
   double ctdt { 1.0 };
   const RunInfo *rinf = nullptr;
   const double *p_t_model = nullptr;

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
   IterType iter_c = IterNo;//* Current iteration type: to propagate to subschemes...

   /** place of customization of preRun, return: !=0 = Ok */
   virtual int do_preRun();
   /** will be called after all actions from posrtRun  -- good place for deallocs */
   virtual int do_postRun( int good );
   /** called before each inner param loop from startLoop */
   virtual int do_startLoop( long acnx, long acny );
   virtual int post_startLoop(); // for near-to ready actions
   /** will be called after each inner loop: called from endLoop */
   virtual int do_endLoop();

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
  // virtual ~InputAbstract();
  virtual void reset_dfl() override; // += handle "def" value sto "source" parameter
  virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
  DCL_CREATE;
  DCL_STD_INF;
  Q_INVOKABLE virtual double getDouble( int /*idx*/ = 0 ) const override { return out0; }
  Q_INVOKABLE virtual bool fromString( const QString &s ) override; // special handling non XML
  QString prepTextVisual( bool isLong ) const;
  Q_INVOKABLE virtual QString textVisual() const override;
  //* return ptr to LinkedObj, which holds element or nullptr;
  const LinkedObj* getSourceObj() const noexcept { return src_obj; }
  void readInput() noexcept { out0 = *p * factor + shift; post_readInput(); } ;
  void setInput( double v ) noexcept { out0 = v; } ; // to use while f_d();
  int  getLinkType() const noexcept { return linkType; }
  int  getOnlyFirst() const noexcept { return onlyFirst; }
  /** find and set link to source or fake source */
  virtual void set_link();

  // less operators for double: const only
  operator double() const noexcept { return out0; }
  const double* caddr() const noexcept { return p; }
  Q_INVOKABLE double vc() const noexcept { return out0; } // synonims
  double cval() const noexcept { return out0; }

  Q_INVOKABLE virtual int iterateSource( int dn = 1 ); // return new number or -1000000

 protected:
  // virtual void do_post_set() override;
  /** do real actions after structure changed */
  virtual void do_structChanged() override;
  virtual void post_readInput() {};

  PRM_STRING(      source, efNRC, "Source", "Address of signal source", "max=128\nprops=STRING,SIMPLE,LINK\ncmpl=in"  );
  PRM_SWITCH(   onlyFirst, efNRC, "only First", "apply only at start of run", "" );
  PRM_DOUBLE(      factor,     0, "factor", "Input factor", "def=1" );
  PRM_DOUBLE(       shift,     0, "shift",  "Input shift",  "def=0" );
  PRM_STRING(       label, efNRC, "Label", "Label to display on structure", "max=64"  );
  PRM_INT(        x_shift,     0, "X shift", "Shift on x-part of link representation", "sep=col" );
  PRM_INT(        y_shift,     0, "Y shift", "Shift on y-part of link representation", "" );
  PRM_INT(         line_w,     0, "Line width", "Line width on scheme", "def=1\nmin=0\nmax=20" );
  PRM_COLOR(   line_color,     0, "Line color", "Line color on scheme", "def=black" );
  PRM_SWITCH(   onlyLabel,     0, "only Label", "draw only link label on scheme", "" );
  PRM_SWITCH( fixedSource,     0, "fixed source", "do not change source name with iterate", "" );

  PRM_INT(       linkType, efInner | efRO, "Link type", "Describes link type", "def=3" ); // def=LinkBad
  PRM_STRING(  srcObjName, efInner | efRO, "Source object", "Name of the source object", ""  );

  PRM_DOUBLE(        out0, efInner, "Output", "Main output", "" );

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
  // virtual ~InputSimple() override;
  DCL_CREATE;
  DCL_STD_INF;
  /** find and set link to source (external or direct_in) */
  // virtual void set_link() override;
 protected:
  // virtual void do_post_set() override;

  PRM_INT( channel, efNRC, "Channel", "Channel number of this input", "sep=block" );
  PRM_INT( subchannel, efNRC, "Subchannel", "Subchannel number of this input", "sep=col" );

  Q_CLASSINFO( "nameHintBase",  "in_" );
  DCL_DEFAULT_STATIC;
};

#define PRM_INPUT( name, flags, vname, descr, extra ) \
  InputSimple name = { #name, this, flags, vname, descr, extra  } ;

// ---------------------------------------------------------------------
/** Special input: to be used as logic value */
class InputLogic : public InputSimple {
  Q_OBJECT
 public:
   enum InputLogicType {
     itLevel  = 0,
     itRise,
     itFall,
     itBoth,
     itShmitt,
     itShmittRise
   };
   Q_ENUMS(InputLogicType);
   Q_CLASSINFO( "enum_InputLogicType_0", "Level" );       // itLevel
   Q_CLASSINFO( "enum_InputLogicType_1", "Rise" );        // itRise
   Q_CLASSINFO( "enum_InputLogicType_2", "Fall" );        // itFall
   Q_CLASSINFO( "enum_InputLogicType_3", "Both" );        // itBoth
   Q_CLASSINFO( "enum_InputLogicType_4", "Shmitt" );      // itShmitt
   Q_CLASSINFO( "enum_InputLogicType_5", "ShmittRise" );  // itShmittRise
  DCL_CTOR(InputLogic);
  // virtual ~InputLogic() override;
  DCL_CREATE;
  DCL_STD_INF;
  bool lval() const { return ll; };
  operator bool() const { return ll; }
 protected:
  virtual int do_startLoop( long /*acnx*/, long /*acny*/ ) override
  {
    old_out0 = out0; sll = false; post_readInput(); return 1;
  }
  virtual void post_readInput() override;

  PRM_LIST(     type,   efNRC,   "&Type",        "Function type", "enum=InputLogicType\nsep=block" );
  PRM_DOUBLE(     l0,       0, "level 0", "level of '0' if need", "def=0.01\nsep=col" );
  PRM_DOUBLE(     l1,       0, "level 1", "level of '1' if need", "def=0.1" );
  PRM_SWITCH( inv_in,   efNRC,     "Not",        "Inverse input", ""  );

  PRM_INT(        ll, efInner,      "ll",           "current logic level", "" );
  PRM_DOUBLE(    llv, efInner,     "llv", "current logic level in double", "" );

  double old_out0 { 0.0 };
  bool sll = false; // saved logic level, = ll except itShmittRise, where like itShmitt

  Q_CLASSINFO( "nameHintBase",  "in_" );
  DCL_DEFAULT_STATIC;
};

#define PRM_LOGICIN( name, flags, vname, descr, extra ) \
  InputLogic name = { #name, this, flags, vname, descr, extra  } ;


// ---------------------------------------------------------------------
/** Special input - paramertic / value, can be used as double */
class ParamDouble : public InputAbstract {
  Q_OBJECT
 public:
  DCL_CTOR(ParamDouble);
  // virtual ~ParamDouble() override;
  DCL_CREATE;
  DCL_STD_INF;
  virtual QIcon getIcon() const override { return QIcon(); } // empty icon: save space
  operator double&() { return out0; }
  double& operator=( double rhs ) { out0 = rhs; return out0; }
  Q_INVOKABLE bool isFixparmNeed() const { return need_fixparm; }
  // virtual void set_link() override;
 protected:
  // virtual void do_post_set() override;

  PRM_STRING( tparam, efOld, "Param", "old param names - remove after cnv", ""  );
  bool need_fixparm = false;

  Q_CLASSINFO( "nameHintBase",  "p_" );
  DCL_DEFAULT_STATIC;
};

#define PRM_PARAMD( name, flags, vname, descr, extra ) \
  ParamDouble name = { #name, this, flags, vname, descr, extra  } ;



// ----------------------------------------------------------------



