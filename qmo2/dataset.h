/***************************************************************************
                          dataset.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#ifndef _DATASET_H
#define _DATASET_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QStringList>
#include <QVariant>
#include <QMap>
#include <QtXml>
typedef QMap<QString,QString> QSSMap;

#include "defs.h"


class HolderData;
class   TDataSet;
typedef HolderData* PHolderData;
typedef TDataSet* PTDataSet;
typedef const TDataSet* CPTDataSet;
typedef HolderData* (*PFHolderData)( 
    const QString& a_name, TDataSet* a_pa, 
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
  clpData = 16      //* simple data
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
};

//* arguments for std ctor and creater - full form
#define ARGS_CTOR const QString &obj_name, TDataSet *a_parent, \
         int a_flags = 0, const QString &a_v_name = QString(),  \
	 const QString &a_descr = QString(), const QString &a_extra = QString() 

//* arguments for std ctor and creater - short form w/o def values
#define ARGS_CTOR_MIN const QString &obj_name, TDataSet *a_parent, \
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
  virtual bool set( const QVariant & x ) override; \
  virtual QVariant get() const override; \
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
    {  #clname,  clname::create,  helpstr, clp };

// class_info definition under alias (for data holders HolderInt->int)
#define STD_CLASSINFO_ALIAS(clname,clp,alias) \
  TClassInfo clname::class_info =  \
    {  #alias,  clname::create,  helpstr, clp };


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
  LinkBad       // link target not found
};

enum allow_type {
  allowNone = 0,
  allowObject = 1,
  allowParam = 2,
  allowBoth = 3
};


// -------------------------- HOLDERS ------------------------------------

/** Abstract holder for simple data types */
class HolderData : public QObject {
  Q_OBJECT
 public: 
  DCL_CTOR(HolderData);
  virtual ~HolderData();
  DCL_CREATE;
  QVariant::Type getTp() const { return tp; }
  /** return number of elems: none for simple data */
  virtual int getNumObj() const { return 0; };
  int isDyn() const { return dyn; }
  /** is holded value is object of given type or child */
  virtual bool isObject( const QString &cl_name = QString() ) const;
  void setFlags( int a_flags ) { flags = a_flags; }
  int getFlags() const { return flags; }
  TDataSet* getParent() const { return par; }
  // see DCL_STD_INF, DCL_STD_GETSET for childs
  virtual const TClassInfo* getClassInfo() const = 0;
  virtual void post_set() = 0;
  virtual bool getData( const QString &nm, int *da ) const;
  virtual bool getData( const QString &nm, double *da ) const;
  virtual bool getData( const QString &nm, QVariant &da ) const;
  virtual bool getData( const QString &nm, QString &da ) const;
  virtual bool setData( const QString &nm, const QVariant &da );
  virtual QDomElement toDom( QDomDocument &dd ) const;
 public slots: 
  /** returns full name of object: aaa.bbb.cc  */ // TODO: up to HolderData
  QString getFullName() const;
  void setParm( const QString &name, const QString &value );
  QString getParm( const QString &name ) const;
  void setElems( const QString &els ); 
  virtual QString getType() const = 0;
  virtual const char* getHelp() const  = 0;
  virtual void reset_dfl() = 0; // reset to default value ("def" parm). No TMiso reset()!
  virtual bool set( const QVariant & x ) = 0;
  virtual QVariant get() const = 0;
  virtual QString toString() const = 0;
  virtual bool fromString( const QString &s ) = 0;
 protected:
  void extraToParm();

  int dyn = 0; //* flag: is created dynamicaly i.e. can be deleted
  int flags;   //* use bitset of _ELEM_FLAGS: efRO, efNoRunChange, ...
  QVariant::Type tp = QVariant::Invalid;
  TDataSet *par; // keep it?
  QSSMap parms;
  DCL_DEFAULT_STATIC;
};


/** Holder of int values */
class HolderInt : public HolderData {
  Q_OBJECT
 public: 
  DCL_CTOR(HolderInt);
  virtual ~HolderInt();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual QString getType() const override; 
  STD_CONVERSIONS(int);
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
  virtual QString getType() const override;
  int operator=( int a ) { v=a; post_set(); return v; }
 protected: 
  DCL_DEFAULT_STATIC;
};

#define PRM_SWITCH( name, flags, vname, descr, extra ) \
 HolderSwitch name = { #name, this, flags, vname, descr, extra  } ; 


/** Holder of int values as List (ComboBox) */
class HolderList : public HolderInt {
  Q_OBJECT
 public:
  // non-standard ctor, as have elems. TODO: global enum/strings for elems
  HolderList( const QString &obj_name, TDataSet *a_parent,
     int a_flags = 0, const QString &v_name = QString(),
     const QString &a_descr = QString(),
     const QString &a_extra  = QString(), const QString &a_elems = QString() );
  virtual ~HolderList();
  DCL_CREATE; // now bad: no info for strings
  DCL_STD_INF;
  // most functions from HolderInt
  virtual void post_set() override;
  virtual QString getType() const;
  int operator=( int a ) { v=a; post_set(); return v; }
 protected: 
  DCL_DEFAULT_STATIC;
};


#define PRM_LIST( name, flags, vname, descr, extra, elems ) \
 HolderList name ={ #name, this, flags, vname, descr, extra, elems }; 

/** Holder of double values */
class HolderDouble : public HolderData {
  Q_OBJECT
 public: 
  DCL_CTOR(HolderDouble);
  virtual ~HolderDouble();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual QString getType() const override; 
  STD_CONVERSIONS(double);
 protected:
  double v;
  DCL_DEFAULT_STATIC;
};

#define PRM_DOUBLE( name, flags, vname, descr, extra ) \
 HolderDouble name = { #name, this, flags, vname, descr, extra  } ; 
 
 

/** Holder of QString values */
class HolderString : public HolderData {
  Q_OBJECT
 public: 
  DCL_CTOR(HolderString);
  virtual ~HolderString();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual QString getType() const override; 
  STD_CONVERSIONS(QString);
  const QString& cval() const { return v; };
 protected:
  QString v;
  DCL_DEFAULT_STATIC;
};

#define PRM_STRING( name, flags, vname, descr, extra ) \
 HolderString name = { #name, this, flags, vname, descr, extra  } ; 



/** Holder of QColor values */
class HolderColor : public HolderData {
  Q_OBJECT
 public: 
  DCL_CTOR(HolderColor);
  virtual ~HolderColor();
  DCL_CREATE;
  DCL_STD_INF;
  DCL_STD_GETSET;
  virtual QString getType() const override; 
  STD_CONVERSIONS(QColor);
 protected:
  QColor v;
  DCL_DEFAULT_STATIC;
};

#define PRM_COLOR( name, flags, vname, descr, extra ) \
 HolderColor name = { #name, this, flags, vname, descr, extra  } ; 


// ####################################################################

/** base class for all non-trivial data objects */
class TDataSet : public HolderData {
  Q_OBJECT
 signals: 
   void sigStructChanged();
 public:
   DCL_CTOR(TDataSet);
   virtual ~TDataSet() override;
   DCL_CREATE;
   DCL_STD_INF;
   DCL_STD_GETSET;
   virtual QString getType() const override; 
   virtual bool isObject( const QString &cl_name = QString() ) const override;
   /** return number of elems */
   virtual int getNumObj() const override { return children().size(); };
   /** return flags of allow adding */
   int getAllowAdd() const { return allow_add; }
   /** returns list of registerd elems names */
   QStringList elemNames() const;
   /** returns holder by number - for QModel... */
   HolderData* getElem( int i ) const;
   /** find holder for object by name */
   HolderData* getElem( const QString &oname ) const;
   /** index of holder, if my, -1 - if not */
   int indexOfHolder( HolderData *ho ) const;
   /** return state */
   virtual int getState() const { return state; }
   /** returns modified flag */
   int getModified() const { return modified; }
   /** set modified flag */
   void setModified() { modified |= 1; }
   /** drop modified flag */
   void setUnModified() { modified = 0; }
   /** return ptr to elem by name with optional type check */
   // use only by QMo2Doc while creating/loading 
   HolderData* getObj( const QString &ename, const QString &cl_name = QString() );   
  
   virtual bool getData( const QString &nm, QVariant &da ) const override;
   virtual bool getData( const QString &nm, int *da ) const override;
   virtual bool getData( const QString &nm, double *da ) const override;
   virtual bool getData( const QString &nm, QString &da ) const override;
   virtual bool setData( const QString &nm, const QVariant &da ) override;

   /** corrects data, if ni==-1 -- all elements -- now empty, see setData */
   virtual int checkData( int ni );
   /** add new object and it's description (new)*/
   virtual HolderData* add_obj( const QString &cl_name, const QString &ob_name );
   /** add new param and it's description */
   virtual HolderData* add_param( const QString &tp_name, const QString &ob_name );
   /** is given type of subelement valid for this object */
   virtual int isValidType( const QString &cl_name ) const;
   /** is this class child of given or the same by name */
   bool isChildOf( const QString &cname ) const;
   void dumpStruct() const;
   QDomElement toDom( QDomDocument &dd ) const;
   bool fromDom( QDomElement &de, QString &errstr );
   void check_guard() const;
   /** reaction to add/remove of subobjects: call do_structChanged */
   void structChanged();
   // special part - TODO: remove or ifdef in separate lib
   /** returns pointer to given parameter, checking if valid 
    * valid names:
    * elmname = elmname.out0 
    * elmname.parmname 
    * :parmname - only local param? 
    * parmname - try new, w/o ':' 
    * lt - ptr: store link type, 
    * lev - level of recursion, not for user */
   virtual const double* getDoublePtr( const QString &nm, ltype_t *lt = 0, int lev = 0 ) const;
 public slots:
   /** create object with params as string */
   bool add_obj_param( const QString &cl_name, const QString &ob_name, const QString &params );
   /** delete given object by name */
   int del_obj( const QString &ob_name );
 protected:
   /** gets pointer to parameter, near to getDoublePrmPtr 
    * for param mod only - no descend  */
   double* getDoublePrmPtr( const QString &nm, int *flg );
   /** do real actions after structure changed */
   virtual void do_structChanged();
 protected:
   /** guard value: debug */
   static const int guard_val = 7442428;
   int guard = guard_val;
   /** state: 0-bad, 1-constructed, 2-run; */
   int state = stateGood; 
   /** allowing object add /remove for some classes 1-add obj, 2-add params */
   int allow_add = 0;
   /** flag: is modified: 0:no, 1-yes, 2-yes(auto) */
   int modified = 0;
   DCL_DEFAULT_STATIC;
};

// ---------------------------------------------------------------------

/** creator of TDataSet childs by name or id (legacy)
 * */
class ElemFactory {
 typedef QMap<QString,const TClassInfo*> MapStrClass;
  public:
   static ElemFactory& theFactory();
   HolderData* createElem( const QString &a_type, ARGS_CTOR ) const;
   bool registerElemType( const TClassInfo *cl_inf );
   // bool unregisterElemType( const QString &a_type );
   QStringList allTypeNames() const { return str_class.keys(); } // TODO: criterion
   const QStringList& allParamTypes() const { return param_names; } 
   const TClassInfo* getInfo( const QString &a_type ) const;
   bool isChildOf( const QString &cl, const QString &par_cl );

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


