/***************************************************************************
                          dataset.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2015 by atu
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


class QStandardItemModel;
class QStandardItem;
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

enum ModificationBits {
  modifManual = 1,
  modifAuto = 2,
  modifMask = 3
};

//                               none  man  auto both extra......
const char *const modificationChar[] = { "", "+", "#", "*", "?", ".", ",", "X" };


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
  virtual void do_post_set() override; \
  virtual QString toString() const override; \
  virtual bool fromString( const QString &s ) override;

// default actions in cpp file to register class in factory
#define REGISTER_CLASS(clname) \
 int clname::registered = reg(); \
 int clname::reg() { return EFACT.registerObjType( &class_info  ); }

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
  targ_type vc() const { return v; } \
  const targ_type& cval() const { return v; } \
  targ_type& val() { return v; } \
  const targ_type* caddr() const { return &v; } \
  targ_type* addr() { return &v; } \
  targ_type operator=( targ_type a ) { return v=a; }

// operator= for pure data holders: copy inner part
#define INNER_ASSIGN(clname) \
  clname& operator=( const clname &r ) { v = r.v; return *this; }




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
  QString getStateStr() const;
  int isDyn() const { return dyn; }
   /**  returns false for simple data holders
    * returns true if this object is base or save of given type.
    * if cl_name is empty, return true in >= TDataSet */
  Q_INVOKABLE bool isObject( const QString &cl_name = QString() ) const;
  // count number of elements of given type, optionaly with named started with nm_start
  Q_INVOKABLE int countObjsOfType( const QString &tp, const QString &nm_start = QString() ) const;
  Q_INVOKABLE QString hintName( const QString &tp, const QString &nm_start = QString() ) const;
  void addFlags( int a_flags ) { flags |= a_flags; }
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
  /** returns list of registerd (exists) child elems names */
  Q_INVOKABLE QStringList elemNames() const;
  /** returns list of registerd child + type */
  Q_INVOKABLE QString ls() const;
  /** returns list of registerd functions + signatures */
  Q_INVOKABLE QString lsf() const;
  /** returns holder by number */
  HolderData* getObj( int i ) const;
  /** find holder for object by name */ // TODO: +full.name.elm
  HolderData* getObj( const QString &oname ) const;
  /** find holder for object by index, safely cast to type T */
  template<typename T> T getObjT( int idx ) const
    {  return qobject_cast<T>( getObj( idx ) );  }
  /** find holder for object by name, safely cast to type T */
  template<typename T> T getObjT( const QString &oname ) const
    { return qobject_cast<T>( getObj( oname ) ); }
  /** find element of ancessor by name and type T */
  template<typename T> T getObjOfAncessorT( const QString &oname ) const
    {
      HolderData *pa = par;
      while( pa ) {
        T el = pa->getObjT<T>( oname );
        if( el ) {
          return el;
        }
        pa = pa->getParent();
      }
      return nullptr;
    }
  /** index of holder, if my, -1 - if not */
  int indexOfHolder( const HolderData *ho ) const;
  //* reset object and all (sub)children, to local actions - do_reset();
  Q_INVOKABLE void reset();

  int getModified() const { return modified & modifMask; } //* returns modified flag
  void setModified();/** set modified flag : and to parents*/
  void setUnModified(); //* drop modified flag: and from childs
  void post_set();
  virtual bool getData( const QString &nm, int *da, bool er = true ) const;
  virtual bool getData( const QString &nm, double *da, bool er = true ) const;
  virtual bool getData( const QString &nm, QVariant &da, bool er = true ) const;
  virtual bool getData( const QString &nm, QString &da, bool er = true ) const;
  // the same but with default and direct return
  Q_INVOKABLE int     getDataD( const QString &nm, int dfl ) const;
  Q_INVOKABLE double  getDataD( const QString &nm, double dfl ) const;
  Q_INVOKABLE QString getDataD( const QString &nm, const QString &dfl ) const;
  Q_INVOKABLE QColor  getDataD( const QString &nm, const QColor &dfl ) const;

  using QAbstractItemModel::setData;
  virtual bool setData( const QString &nm, const QVariant &da );
  Q_INVOKABLE bool setD( const QString &nm, int da );
  Q_INVOKABLE bool setD( const QString &nm, double da );
  Q_INVOKABLE bool setD( const QString &nm, const QString &da );
  //* get data from some of parent
  bool getUpData( const QString &nm, int *da ) const;
  bool getUpData( const QString &nm, double *da ) const;
  bool getUpData( const QString &nm, QString &da ) const;
  bool getUpData( const QString &nm, QVariant &da ) const;

  /** add new object and it's description (new)*/
  virtual HolderData* addObjP( const QString &cl_name, const QString &ob_name, bool ignoreMod = false );
  /** type-cast interface to addObj */
  template <typename T>
    T* addObjT( const QString &ob_name, bool ignoreMod = false  ) {
      return qobject_cast<T*>( addObjP( T::staticMetaObject.className(), ob_name, ignoreMod ) );
    }
  Q_INVOKABLE bool addObj( const QString &cl_name, const QString &ob_name );
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
  bool fromDom( QDomElement &de, QString &errstr );
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

  /** returns full name of object: aaa.bbb.cc  */
  Q_INVOKABLE QString getFullName() const;
  Q_INVOKABLE void setParm( const QString &name, const QString &value );
  Q_INVOKABLE QString getParm( const QString &name ) const;
  Q_INVOKABLE bool setDatas( const QString &datas ); //* data sep: newline
  Q_INVOKABLE QString getType() const { return getTypeV(); };
  virtual const char* getHelp() const  = 0;
  Q_INVOKABLE virtual void reset_dfl() = 0; // reset to default value ("def" parm). No TMiso reset()!
  Q_INVOKABLE virtual bool set( const QVariant & x, int idx = 0 ) = 0;
  Q_INVOKABLE virtual QVariant get( int idx = 0 ) const = 0;
  Q_INVOKABLE virtual QString toString() const = 0;
  Q_INVOKABLE virtual bool fromString( const QString &s ) = 0;
  Q_INVOKABLE int size() const { return children().size(); }
  Q_INVOKABLE virtual int arrSize() const { return 1; }
  Q_INVOKABLE int getState() const { return state; }
  /** create object with params as string */
  Q_INVOKABLE bool addObjDatas( const QString &cl_name, const QString &ob_name, const QString &datas );
  /** delete given object by name, returns 0 - error, !=0 = ok */
  Q_INVOKABLE int delObj( const QString &ob_name, bool ignoreMod = false  );
  Q_INVOKABLE int delAllDyn();
  Q_INVOKABLE bool cloneObj( const QString &old_name, const QString &new_name );
  //* rename object (if created dynamicaly)
  Q_INVOKABLE int renameObj( const QString &ob_name, const QString &new_name );
  // void check_guard() const;
  Q_INVOKABLE int getActiveIdx() const { return active_idx; }
  Q_INVOKABLE void setActiveIdx( int i );
  Q_INVOKABLE bool setActiveObj( const QString &nm );
  HolderData* getActiveObj() const { return getObj( active_idx ); };
  template<typename T> T getActiveObjT() const
      {
        return qobject_cast<T>( getActiveObj() );
      }
  /** reaction to add/remove/relink of subobjects: call do_structChanged */
  void handleStructChanged();
  void extraToParm();

  // functions to work with substructure
  HolderData* addObjToSubP( const QString &subname, const QString &tp, const QString &ob_name );
  Q_INVOKABLE bool addObjToSub( const QString &subname, const QString &tp, const QString &ob_name );
  template <typename T>
    T* addObjToSubT( const QString &subname, const QString &ob_name ) {
      return qobject_cast<T*>( addObjToSubP( subname, T::staticMetaObject.className(), ob_name ) );
    }
  Q_INVOKABLE bool delObjFromSub( const QString &subname, const QString &ob_name );
  Q_INVOKABLE bool cloneObjInSub( const QString &subname, const QString &old_name, const QString &new_name );
  Q_INVOKABLE bool renameObjInSub( const QString &subname, const QString &old_name, const QString &new_name );

  //* return model for completer
  QAbstractItemModel *getComplModel( const QString &targ, QObject *mdl_par ) const;
  //* fill model for orinary inputs: now: pass to Model/Scheme
  virtual void fillComplModelForInputs( QStandardItemModel *mdl ) const;
  //* fill model for params
  virtual void fillComplModelForParams( QStandardItemModel *mdl ) const;
  //* fill model for outputs: pass to model
  virtual void fillComplModelForOuts( QStandardItemModel *mdl ) const;
  //* fill items for input: workhorse for fillComplModelForInputs
  int fillComplForInputs( QStandardItem *it0, const QString &prefix = QString()  ) const;
 signals:
   void sigStructChanged();
 protected:
  virtual void do_post_set() = 0;
  virtual void do_reset() {}; //* adjustable reset function
  /** do real actions after structure changed */
  virtual void do_structChanged();
  //* real part of fromDom */
  bool fromDom_real( QDomElement &de, QString &errstr );

  int dyn = 0; //* flag: is created dynamically i.e. can be deleted
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
  Q_CLASSINFO( "nameHintBase",  "holder_" );
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
  Q_CLASSINFO( "nameHintBase",  "data_" );
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
  INNER_ASSIGN(HolderInt);
  virtual QString getTypeV() const override;
 protected:
  int v;
  Q_CLASSINFO( "nameHintBase",  "iv_" );
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
  int operator=( int a ) { v=a; post_set(); return v; }
  virtual QString getTypeV() const override;
 protected:
  virtual void do_post_set() override;
  Q_CLASSINFO( "nameHintBase",  "sw_" );
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
  int operator=( int a ) { v=a; post_set(); return v; }
  virtual QString getTypeV() const;
 protected:
  virtual void do_post_set() override;
  Q_CLASSINFO( "nameHintBase",  "list_" );
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
  INNER_ASSIGN(HolderDouble);
  virtual QString getTypeV() const override;
 protected:
  double v;
  Q_CLASSINFO( "nameHintBase",  "dv_" );
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
  INNER_ASSIGN(HolderString);
  virtual QString getTypeV() const override;
  std::string toStdString() const { return v.toStdString(); }
  bool isEmpty() const { return v.isEmpty(); }
  const char* c_str() const { return v.toStdString().c_str(); } // danger
 protected:
  QString v;
  Q_CLASSINFO( "nameHintBase",  "str_" );
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
  INNER_ASSIGN(HolderColor);
  int toInt() const { return (int)( v.rgb() ); };
  int red() const { return v.red(); }
  int green() const { return v.green(); }
  int blue() const { return v.blue(); }
  double redF() const { return v.redF(); }
  double greenF() const { return v.greenF(); }
  double blueF() const { return v.blueF(); }
 protected:
  QColor v;
  Q_CLASSINFO( "nameHintBase",  "color_" );
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
  INNER_ASSIGN(HolderIntArray);
  int operator[](int i) const { return v[i]; };
  int& operator[](int i) { return v[i]; };
 protected:
  std::vector<int> v;
  Q_CLASSINFO( "nameHintBase",  "ia_" );
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
  INNER_ASSIGN(HolderDoubleArray);
  double operator[](int i) const { return v[i]; };
  double& operator[](int i) { return v[i]; };
 protected:
  std::vector<double> v;
  Q_CLASSINFO( "nameHintBase",  "da_" );
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
  INNER_ASSIGN(HolderStringArray);
  QString operator[](int i) const { return v[i]; };
  QString& operator[](int i) { return v[i]; };
 protected:
  QStringList v;
  Q_CLASSINFO( "nameHintBase",  "sa_" );
  DCL_DEFAULT_STATIC;
};

#define PRM_STRING_ARR( name, flags, vname, descr, extra ) \
 HolderStringArray name = { #name, this, flags, vname, descr, extra  } ;



// ####################################################################

/** base class for all non-trivial data objects */
class TDataSet : public HolderData {
  Q_OBJECT
 public:
   DCL_CTOR(TDataSet);
   virtual ~TDataSet() override;
   DCL_CREATE;
   DCL_STD_INF;
   DCL_STD_GETSET;

   virtual QString getTypeV() const override;

   QDomElement toDom( QDomDocument &dd ) const override;
 protected:
   Q_CLASSINFO( "nameHintBase",  "obj_" );
   DCL_DEFAULT_STATIC;
};

// no auto init

// ----------------------------------------------------------------

/** creator of TDataSet childs by name
 * */
class ObjFactory {
 typedef QMap<QString,const TClassInfo*> MapStrClass;
  public:
   static ObjFactory& theFactory();
   HolderData* createObj( const QString &a_type, ARGS_CTOR ) const;
   bool registerObjType( const TClassInfo *cl_inf );
   // bool unregisterObjType( const QString &a_type );
   QStringList allTypeNames() const { return str_class.keys(); } // TODO: criterion
   QStringList goodTypeNames( const QString & allows,
                          const QString &hidden_types = QString() ) const;
   const QStringList& allParamTypes() const { return param_names; }
   const TClassInfo* getInfo( const QString &a_type ) const;
   bool isChildOf( const QString &cl, const QString &par_cl ) const;

  private:
   ObjFactory();
   ObjFactory( const ObjFactory& r ) = delete;
   ObjFactory( ObjFactory &&r ) = delete;
   ObjFactory& operator=( const ObjFactory& r ) = delete;
   ObjFactory& operator=( ObjFactory&& r ) = delete;

   MapStrClass str_class;
   QStringList param_names;
};

#define EFACT ObjFactory::theFactory()


#endif  // _DATASET_H


