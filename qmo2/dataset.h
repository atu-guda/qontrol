/***************************************************************************
                          dataset.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _DATASET_H
#define _DATASET_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QIcon>
#include <QStringList>
#include <QVariant>
#include <QMap>
#include <QAbstractItemModel>
#include <QtXml>
typedef QMap<QString,QString> QSSMap;

#include "defs.h"


class HolderData;
class   TDataSet;
class InputSimple;
class InputParam;
typedef HolderData* PHolderData;
typedef TDataSet* PTDataSet;
typedef const TDataSet* CPTDataSet;
typedef HolderData* (*PFHolderData)(
    const QString& a_name, HolderData* a_pa,
    int a_flags, const QString &a_v_name,
    const QString& a_descr, const QString& a_extra );
typedef PTDataSet (*PFDataSet)( PTDataSet aparent );
class   TDataContainer;
// typedef QVector<HolderData*> QHoVect;

/** properties of class -- bitfield */
enum ClassProps {
  clpElem = 1,      //* element of cheme
  clpPure = 2,      //* dont know - may by for stateless or abstract
  clpContainer = 4, //* can contain elements
  clpSpecial = 8,   //* have special meaning for structue
  clpData = 16,     //* simple data
  clpArray = 32,     //* array
  clpInput = 64,     //* link to other
  clpParamInput = 128, //* link to other + name of inner field
  clpObsolete = 256  //* obsolesed class, should not writed
};


/** describes class and it's creator
   used for class registration
*/
struct TClassInfo {
  /** class name */
  const char *className;
  /** ptr to static fun for creating instanses */
  PFHolderData creator;
  /** ptr to help string */
  const char *helpstr;
  /** properties of class (not ClassProps, as use | ) */
  int props;
  /** pointer to class metaboject */
  const QMetaObject *meta;
};

//* arguments for std ctor and creater - full form
#define ARGS_CTOR const QString &obj_name, HolderData *a_parent, \
         int a_flags = 0, const QString &a_v_name = QString(),  \
         const QString &a_descr = QString(), const QString &a_extra = QString()

//* arguments for std ctor and creater - short form w/o def values
#define ARGS_CTOR_MIN const QString &obj_name, HolderData *a_parent, \
         int a_flags, const QString &a_v_name,  \
         const QString &a_descr, const QString &a_extra

//* arguments for std ctor and creater - only names
#define ARGS_CTOR_NAMES obj_name, a_parent, \
         a_flags, a_v_name,  \
         a_descr, a_extra

//* declare common ctor
#define DCL_CTOR(clname) \
  clname( ARGS_CTOR );

//* and header for ctor _definition_
#define CTOR(clname,par_class) \
  clname::clname( ARGS_CTOR_MIN ) \
  : par_class( ARGS_CTOR_NAMES )

//* declare creator function
#define DCL_CREATE \
  static HolderData* create( ARGS_CTOR );

//* declare common info functions
#define DCL_STD_INF \
  virtual const TClassInfo* getClassInfo() const override; \
  virtual const char* getHelp() const override;

//* declare common data manipulation functions
#define DCL_STD_GETSET \
  virtual void reset_dfl() override; \
  virtual bool set( const QVariant & x, int idx = 0 ) override; \
  virtual QVariant get( int idx = 0 ) const override; \
  virtual void post_set() override; \
  virtual QString toString() const override; \
  virtual bool fromString( const QString &s ) override;

// default actions in cpp file to register class in factory
#define REGISTER_CLASS(clname) \
 int clname::registered = reg(); \
 int clname::reg() { return EFACT.registerElemType( &class_info  ); }

// definition in cpp file of common functions
#define DEFAULT_FUNCS(clname) \
 HolderData* clname::create( ARGS_CTOR_MIN ) \
     { clname *t = new clname( ARGS_CTOR_NAMES ); \
       t->dyn=1; t->reset_dfl(); return t; } \
 const TClassInfo* clname::getClassInfo() const {  return &class_info; } \
 const char *clname::getHelp() const {  return helpstr; }

// both of above
#define DEFAULT_FUNCS_REG(clname) DEFAULT_FUNCS(clname) REGISTER_CLASS(clname)

// declarations in class: common static values
#define DCL_DEFAULT_STATIC \
  static TClassInfo class_info; \
  static int registered; \
  static int reg(); \
  static const char* helpstr;

// standard class_info definition
#define STD_CLASSINFO(clname,clp) \
  TClassInfo clname::class_info =  \
    {  #clname,  clname::create,  helpstr, clp, &staticMetaObject };

// class_info definition under alias (for data holders HolderInt->int)
#define STD_CLASSINFO_ALIAS(clname,clp,alias) \
  TClassInfo clname::class_info =  \
    {  #alias,  clname::create,  helpstr, clp, &staticMetaObject };


// define in class common converions to target type
// need for usage class objects as pure data
#define STD_CONVERSIONS(targ_type) \
  operator targ_type() const { return v; } \
  operator targ_type&()  { return v; } \
  const targ_type* caddr() const { return &v; } \
  targ_type* addr() { return &v; } \
  targ_type operator=( targ_type a ) { return v=a; }


/** types of link - moved from tmiso.h */
enum ltype_t {
  LinkNone = 0, // not linked
  LinkElm,      // linked to element
  LinkSpec,     // linked to special name, like ':prm1', ':t'
  LinkBad       // link source or target not found
};


// -------------------------- HOLDERS ------------------------------------

/** Abstract holder for simple data types */
class HolderData : public QAbstractItemModel {
  Q_OBJECT
 public:
  DCL_CTOR(HolderData);
  virtual ~HolderData();
  DCL_CREATE;

  // QAbstractItemModel part
  virtual int columnCount( const QModelIndex &par = QModelIndex() ) const override;
  virtual int rowCount( const QModelIndex &par = QModelIndex() ) const override;
  virtual QVariant data( const QModelIndex &idx,
                     int role = Qt::DisplayRole ) const override;
  // virtual bool hasChildren( const QModelIndex &par = QModelIndex() ) const;
  virtual QModelIndex index( int row, int column,
                      const QModelIndex &par = QModelIndex() ) const override;
  virtual QModelIndex parent( const QModelIndex &idx ) const override;
  // helper for "data", but for "this", not index
  virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const;

  QVariant::Type getTp() const { return tp; }
  int isDyn() const { return dyn; }
   /**  returns false for simple data holders
    * returns true if this object is base or save of given type.
    * if cl_name is empty, return true in >= TDataSet */
  Q_INVOKABLE bool isObject( const QString &cl_name = QString() ) const;
  // void setFlags( int a_flags ) { flags = a_flags; }
  void setImmutable() { flags |= efImmutable; }
  Q_INVOKABLE int getFlags() const { return flags; }
  HolderData* getParent() const { return par; } // no Q_INVOKABLE: need reg HolderData
  // return ptr to ancessor of given type
  template<typename T> T* getAncestorT() const {
    HolderData *pa = par;
    while( pa ) {
      T* pt = qobject_cast<T*>( pa );
      if( pt ) {
        return pt;
      }
      pa = pa->getParent();
    }
    return nullptr;
  }
  // see DCL_STD_INF, DCL_STD_GETSET for childs
  virtual const TClassInfo* getClassInfo() const = 0;
  /** returns list of registerd (exists) clild elems names */
  Q_INVOKABLE QStringList elemNames() const;
  /** returns holder by number */
  HolderData* getElem( int i ) const;
  /** find holder for object by name */ // TODO: +full.name.elm
  HolderData* getElem( const QString &oname ) const;
  /** find holder for object by insex, safely cast to type T */
  template<typename T> T getElemT( int idx ) const
    {  return qobject_cast<T>( getElem( idx ) );  }
  /** find holder for object by name, safely cast to type T */
  template<typename T> T getElemT( const QString &oname ) const
    { return qobject_cast<T>( getElem( oname ) ); }
  /** index of holder, if my, -1 - if not */
  int indexOfHolder( const HolderData *ho ) const;

  /** returns modified flag */
  int getModified() const { return modified; }
  /** set modified flag */
  void setModified() { modified |= 1; }
  /** drop modified flag */
  void setUnModified() { modified = 0; }
  virtual void post_set() = 0;
  virtual bool getData( const QString &nm, int *da, bool er = true ) const;
  virtual bool getData( const QString &nm, double *da, bool er = true ) const;
  virtual bool getData( const QString &nm, QVariant &da, bool er = true ) const;
  virtual bool getData( const QString &nm, QString &da, bool er = true ) const;
  // the same but with default and direct return
  Q_INVOKABLE int     getDataD( const QString &nm, int dfl ) const;
  Q_INVOKABLE double  getDataD( const QString &nm, double dfl ) const;
  Q_INVOKABLE QString getDataD( const QString &nm, const QString &dfl ) const;
  Q_INVOKABLE QColor  getDataD( const QString &nm, const QColor &dfl ) const;

  virtual bool setData( const QString &nm, const QVariant &da );
  //* get data from some of parent
  bool getUpData( const QString &nm, int *da ) const;
  bool getUpData( const QString &nm, double *da ) const;
  bool getUpData( const QString &nm, QString &da ) const;
  bool getUpData( const QString &nm, QVariant &da ) const;

  /** corrects data, if ni==-1 -- all elements -- now empty, see setData */
  virtual int checkData( int ni );
  /** add new object and it's description (new)*/
  virtual HolderData* add_obj( const QString &cl_name, const QString &ob_name );
  /** type-cast interface to add_obj */
  template <typename T>
    T* addObj( const QString &ob_name ) {
      return qobject_cast<T*>( add_obj( T::staticMetaObject.className(), ob_name ) );
    }
  /** add new param and it's description */
  virtual HolderData* add_param( const QString &tp_name, const QString &ob_name );
  /** is given type of subelement valid for this object */
  virtual int isValidType( const QString &cl_name ) const;
  void dumpStruct() const;
  /** initiates reactions to structure change: common: to root */
  void reportStructChanged();
  /** temporary suspend reaction to struct changes */
  void suspendHandleStructChange() { updSuspended = true; }
  /** resume reaction to struct changes */
  void resumeHandleStructChange() { updSuspended = false; handleStructChanged(); }
  //* return icon for visualization
  virtual QIcon getIcon() const;


  virtual QDomElement toDom( QDomDocument &dd ) const;
  Q_INVOKABLE QString allowTypes() const { return allowed_types; }
  /** is this class child of given or the same by name */
  Q_INVOKABLE bool isChildOf( const QString &cname ) const;
  //* make real work for getType()
  virtual QString getTypeV() const = 0;
  //* returns this object index in parent or -1 on no [my] parent
  Q_INVOKABLE int getMyIndexInParent() const;
  Q_INVOKABLE QString childName( int n ) const
    { return ( n<size() && n >= 0 ) ? children().at( n )->objectName() : ""; }
  //* return strings for given enum
  Q_INVOKABLE QStringList getEnumStrings( const QString &enum_name ) const;
 signals:
   void sigStructChanged();
 public slots:
  /** returns full name of object: aaa.bbb.cc  */
  QString getFullName() const;
  void setParm( const QString &name, const QString &value );
  QString getParm( const QString &name ) const;
  bool setDatas( const QString datas ); //* data sep: newline
  QString getType() const { return getTypeV(); };
  virtual const char* getHelp() const  = 0;
  virtual void reset_dfl() = 0; // reset to default value ("def" parm). No TMiso reset()!
  virtual bool set( const QVariant & x, int idx = 0 ) = 0;
  virtual QVariant get( int idx = 0 ) const = 0;
  virtual QString toString() const = 0;
  virtual bool fromString( const QString &s ) = 0;
  int size() const { return children().size(); }
  virtual int arrSize() const { return 1; }
  int getState() const { return state; }
  /** create object with params as string */
  bool add_obj_datas( const QString &cl_name, const QString &ob_name, const QString &datas );
  /** delete given object by name, returns 0 - error, !=0 = ok */
  int del_obj( const QString &ob_name );
  //* rename object (if created dynamicaly)
  int rename_obj( const QString &ob_name, const QString &new_name );
  // void check_guard() const;
  int getActiveIdx() const { return active_idx; }
  void setActiveIdx( int i );
  void setActiveElem( const QString &nm );
  HolderData* getActiveElem() const { return getElem( active_idx ); };
 public:
  template<typename T> T getActiveElemT() const
      {
        return qobject_cast<T>( getActiveElem() );
      }
  /** reaction to add/remove/relink of subobjects: call do_structChanged */
  void handleStructChanged();
 protected:
  void extraToParm();
  /** do real actions after structure changed */
  virtual void do_structChanged();

  /** guard value: debug */
  // static const int guard_val = 7442428;
  // int guard = guard_val;
  int dyn = 0; //* flag: is created dynamicaly i.e. can be deleted
  int flags;   //* use bitset of _ELEM_FLAGS: efRO, efNoRunChange, ...
  QVariant::Type tp = QVariant::Invalid;
  HolderData *par; // keep it?
  /** state: 0-bad, 1-constructed, 2-run; */
  int state = stateGood;
  /** flag: is modified: 0:no, 1-yes, 2-yes(auto) */
  int modified = 0;
  /** flag: suspend reaction to structure update: use only in mass changes */
  bool updSuspended = false;
  //* active element index
  int active_idx = -1;
  //* flag: show active/inactive state in Qt::CheckStateRole
  bool show_active = false;
  QSSMap parms;
  QString allowed_types = ""; // separator=','
  DCL_DEFAULT_STATIC;
};

// ----------------------------- HolderValue ---------------------------------
/** Parent of all value holders */
class HolderValue : public HolderData {
  Q_OBJECT
 public:
  DCL_CTOR(HolderValue);
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET; // just for working create (need for register)
  virtual QString getTypeV() const override;
 protected:
  DCL_DEFAULT_STATIC;
};



/** Holder of int values */
class HolderInt : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderInt);
  virtual ~HolderInt();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  STD_CONVERSIONS(int);
  virtual QString getTypeV() const override;
 protected:
  int v;
  DCL_DEFAULT_STATIC;
};

#define PRM_INT( name, flags, vname, descr, extra ) \
 HolderInt name = { #name, this, flags, vname, descr, extra  } ;

/** Holder of int values as Switch */
class HolderSwitch : public HolderInt {
  Q_OBJECT
 public:
  DCL_CTOR(HolderSwitch);
  virtual ~HolderSwitch();
  DCL_CREATE;
  DCL_STD_INF;
  // most functions from HolderInt
  virtual void post_set() override;
  int operator=( int a ) { v=a; post_set(); return v; }
  virtual QString getTypeV() const override;
 protected:
  DCL_DEFAULT_STATIC;
};

#define PRM_SWITCH( name, flags, vname, descr, extra ) \
 HolderSwitch name = { #name, this, flags, vname, descr, extra  } ;


/** Holder of int values as List (ComboBox) */
class HolderList : public HolderInt {
  Q_OBJECT
 public:
  DCL_CTOR(HolderList);
  virtual ~HolderList();
  DCL_CREATE;
  DCL_STD_INF;
  // most functions from HolderInt
  virtual void post_set() override;
  int operator=( int a ) { v=a; post_set(); return v; }
  virtual QString getTypeV() const;
 protected:
  DCL_DEFAULT_STATIC;
};


#define PRM_LIST( name, flags, vname, descr, extra ) \
 HolderList name ={ #name, this, flags, vname, descr, extra };

/** Holder of double values */
class HolderDouble : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderDouble);
  virtual ~HolderDouble();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  STD_CONVERSIONS(double);
  virtual QString getTypeV() const override;
 protected:
  double v;
  DCL_DEFAULT_STATIC;
};

#define PRM_DOUBLE( name, flags, vname, descr, extra ) \
 HolderDouble name = { #name, this, flags, vname, descr, extra  } ;



/** Holder of QString values */
class HolderString : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderString);
  virtual ~HolderString();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  STD_CONVERSIONS(QString);
  virtual QString getTypeV() const override;
  const QString& cval() const { return v; };
 protected:
  QString v;
  DCL_DEFAULT_STATIC;
};

#define PRM_STRING( name, flags, vname, descr, extra ) \
 HolderString name = { #name, this, flags, vname, descr, extra  } ;



/** Holder of QColor values */
class HolderColor : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderColor);
  virtual ~HolderColor();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual QString getTypeV() const override;
  STD_CONVERSIONS(QColor);
 protected:
  QColor v;
  DCL_DEFAULT_STATIC;
};

#define PRM_COLOR( name, flags, vname, descr, extra ) \
 HolderColor name = { #name, this, flags, vname, descr, extra  } ;

// ----------------------------------------------------------------
/** Holder: array of int */
class HolderIntArray : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderIntArray);
  virtual ~HolderIntArray();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual int arrSize() const override { return v.size(); }
  virtual QString getTypeV() const override;
  STD_CONVERSIONS(std::vector<int>);
  int operator[](int i) const { return v[i]; };
  int& operator[](int i) { return v[i]; };
 protected:
  std::vector<int> v;
  DCL_DEFAULT_STATIC;
};

#define PRM_INT_ARR( name, flags, vname, descr, extra ) \
 HolderIntArray name = { #name, this, flags, vname, descr, extra  } ;

// ----------------------------------------------------------------
/** Holder: array of double */
class HolderDoubleArray : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderDoubleArray);
  virtual ~HolderDoubleArray();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual int arrSize() const override { return v.size(); }
  virtual QString getTypeV() const override;
  STD_CONVERSIONS(std::vector<double>);
  double operator[](int i) const { return v[i]; };
  double& operator[](int i) { return v[i]; };
 protected:
  std::vector<double> v;
  DCL_DEFAULT_STATIC;
};

#define PRM_DOUBLE_ARR( name, flags, vname, descr, extra ) \
 HolderDoubleArray name = { #name, this, flags, vname, descr, extra  } ;

// ----------------------------------------------------------------
/** Holder: array of QString */
class HolderStringArray : public HolderValue {
  Q_OBJECT
 public:
  DCL_CTOR(HolderStringArray);
  virtual ~HolderStringArray();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual int arrSize() const override { return v.size(); }
  virtual QString getTypeV() const override;
  STD_CONVERSIONS(QStringList);
  QString operator[](int i) const { return v[i]; };
  QString& operator[](int i) { return v[i]; };
 protected:
  QStringList v;
  DCL_DEFAULT_STATIC;
};

#define PRM_STRING_ARR( name, flags, vname, descr, extra ) \
 HolderStringArray name = { #name, this, flags, vname, descr, extra  } ;



// ####################################################################

/** base class for all non-trivial data objects */
class TDataSet : public HolderData {
  Q_OBJECT
  friend class InputSimple; // for register
  friend class InputParam; //  same
 public:
   DCL_CTOR(TDataSet);
   virtual ~TDataSet() override;
   DCL_CREATE;
   DCL_STD_INF;
   DCL_STD_GETSET;

   virtual QString getTypeV() const override;

   QDomElement toDom( QDomDocument &dd ) const;
   bool fromDom( QDomElement &de, QString &errstr );
   // special part - TODO: remove or ifdef in separate lib
   /** returns pointer to given parameter, checking if valid
    * valid names:
    * elmname = elmname.out0
    * elmname.parmname
    * :parmname - only local param?
    * parmname - try new, w/o ':'
    * lt - ptr: store link type,
    * targ - ptr_pre: strore ptr to source TDataSet,
    * lev - level of recursion, not for user */
   virtual const double* getDoublePtr( const QString &nm, ltype_t *lt = nullptr,
        const TDataSet **src_ob = nullptr, int lev = 0 ) const;
   //* transmit this requues to parent, untill scheme detected, where work done
   // via getDoublePtr
   virtual const double* getSchemeDoublePtr( const QString &nm, ltype_t *lt = nullptr,
        const TDataSet **src_ob = nullptr, int lev = 0 ) const;
   /** return number of inputs */
   int inputsCount() const { return inputs.size(); };
   /** returns input by number */
   InputSimple* getInput( int n ) const;
 protected:
   /** gets pointer to parameter, near to getDoublePrmPtr
    * for param mod only - no descend  */
   double* getDoublePrmPtr( const QString &nm, int *flg );
   /** register input (call by ctor) in inputs */
   void registerInput( InputSimple *inp );
   /** unregister input (call by dtor) from inputs */
   void unregisterInput( InputSimple *inp );
 protected:
   /** place for inputs */
   QVector<InputSimple*> inputs;
   /** place for parametric inputs */
   DCL_DEFAULT_STATIC;
};

// ---------------------------------------------------------------------
/** Abstract-alike special holder link, parent for all inputs */
class InputAbstract : public TDataSet {
  Q_OBJECT
 public:
  DCL_CTOR(InputAbstract);
  virtual ~InputAbstract();  // must be abstract, but in this case cannot register
  DCL_CREATE;
  DCL_STD_INF;
  virtual void post_set() override;
  //* return ptr to TDataSet, which holds element or nullptr;
  const TDataSet* getSourceObj() const { return src_obj; };
  //* returns type of link
  ltype_t getLinkType() const { return linkType; };
 protected:
  /** do real actions after structure changed */
  virtual void do_structChanged();
  /** find and set link to source or fake source */
  virtual void set_link();

  PRM_STRING( source, efNoRunChange, "Source", "Address of signal source", "max=128\nprops=STRING,SIMPLE,LINK"  );
  PRM_STRING( label,  efNoRunChange, "Label", "Label to display on structure", "max=64"  );
  PRM_INT( x_shift, 0, "X shift", "Shift on x-part of link represenration", "sep=col" );
  PRM_INT( y_shift, 0, "Y shift", "Shift on y-part of link represenration", "" );
  PRM_INT( line_w,  0, "Line width", "Line width on scheme", "def=1\nmin=0\nmax=20" );
  PRM_COLOR( line_color,  0, "Line color", "Line color on scheme", "def=black" );
  PRM_SWITCH( onlyLabel, 0, "only Label", "draw only label of link on scheme", "" );
  static const double fake_in;
  static const double one_in;
  const double *p = &fake_in;
  const TDataSet *src_obj = nullptr;
  ltype_t linkType = LinkBad;
  DCL_DEFAULT_STATIC;
};

// ---------------------------------------------------------------------
/** Special holder link, have QString, but can be used as const double */
class InputSimple : public InputAbstract {
  Q_OBJECT
 public:
  DCL_CTOR(InputSimple);
  virtual ~InputSimple();
  DCL_CREATE;
  DCL_STD_INF;
  virtual void post_set() override;
  // less operators for double: const only
  operator double() const { return *p; };
  const double* caddr() const { return p; };
 protected:
  /** find and set link to source or fake source */
  virtual void set_link() override;

  DCL_DEFAULT_STATIC;
};

#define PRM_INPUT( name, flags, vname, descr, extra ) \
  InputSimple name = { #name, this, flags, vname, descr, extra  } ;

// ----------------------------------------------------------------
/** Special holder link - paramitric input,
 * like simple, but with local param target */
class InputParam : public InputAbstract {
  Q_OBJECT
 public:
  DCL_CTOR(InputParam);
  virtual ~InputParam();
  DCL_CREATE;
  DCL_STD_INF;
  virtual void post_set() override;
  operator double() const { return *p; };
  const double* caddr() const { return p; };
  double* targ_addr() const { return targ; };
  int getOnlyFirst() const { return onlyFirst; }
  int getTargetFlag() const { return target_flag; }
 protected:
  /** find and set link to  from (fake)  source to (fake) target */
  virtual void set_link() override;

  PRM_STRING( tparam, efNoRunChange, "Param", "Name of param target", "max=128\nprops=STRING,SIMPLE,INNERLINK\nsep=block"  );
  PRM_SWITCH( onlyFirst, 0, "only First", "apply only at start of run", "" );

  double fake_target = 0;
  double *targ = &fake_target;
  int target_flag = 0;

  DCL_DEFAULT_STATIC;
};

// no auto init

// ----------------------------------------------------------------

/** creator of TDataSet childs by name
 * */
class ElemFactory {
 typedef QMap<QString,const TClassInfo*> MapStrClass;
  public:
   static ElemFactory& theFactory();
   HolderData* createElem( const QString &a_type, ARGS_CTOR ) const;
   bool registerElemType( const TClassInfo *cl_inf );
   // bool unregisterElemType( const QString &a_type );
   QStringList allTypeNames() const { return str_class.keys(); } // TODO: criterion
   QStringList goodTypeNames( const QString & allows,
                          bool no_obj = false, bool no_param = false ) const;
   const QStringList& allParamTypes() const { return param_names; }
   const TClassInfo* getInfo( const QString &a_type ) const;
   bool isChildOf( const QString &cl, const QString &par_cl ) const;

  private:
   ElemFactory();
   ElemFactory( const ElemFactory& r ) = delete;
   ElemFactory( ElemFactory &&r ) = delete;
   ElemFactory& operator=( const ElemFactory& r ) = delete;
   ElemFactory& operator=( ElemFactory&& r ) = delete;

   MapStrClass str_class;
   QStringList param_names;
};

#define EFACT ElemFactory::theFactory()

#endif  // _DATASET_H


