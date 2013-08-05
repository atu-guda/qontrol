/***************************************************************************
                          dataset.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#ifndef _DATASET_H
#define _DATASET_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>
#include <QtXml>
typedef QMap<QString,QString> QSSMap;

#include "defs.h"
#define MAX_DESCRLEN 80
#define MAX_LISTLEN  256
#define CLASS_ID_TDataSet 1


class QColor;
class HolderData;
class   TDataSet;
typedef TDataSet* PTDataSet;
typedef const TDataSet* CPTDataSet;
typedef PTDataSet (*PFDataSet)( PTDataSet aparent );
class   TDataContainer;
typedef QMap<QString,HolderData*> QSHoMap;
typedef QVector<HolderData*> QHoVect;

/** properties of class -- bitfield */
enum ClassProps {
  clpElem = 1,  clpPure = 2, clpContainer = 4,
  clpSpecial = 8
};

/** describes class and it's creator
  used for class registration
*/
struct TClassInfo {
  /** uniq class id; -- TODO: obsoleted */
  int id;
  /** class name */
  const char *className;
  /** ptr to static fun for creating instanses */
  PFDataSet creator;
  /** ptr to parents TClassInfo or 0 for TDataSet TODO: drop use QMetaObject*/
  const TClassInfo* parent_class;
  /** ptr to help string */
  const char *helpstr;
  /** properties of class (not ClassProps, as use | ) */
  int props;
};

/** types of link - moved from tmiso.h */
enum ltype_t {
  LinkNone = 0, // not linked
  LinkElm,      // linked to element 
  LinkSpec,     // linked to special name, like ':prm1', ':t'
  LinkBad       // link target not found
};

enum allow_type {
  allowObject = 1,
  allowParam = 2
};

typedef HolderData* (*PFHolderData)( const QString &obj_name, const QString &v_name, 
              TDataSet *a_parent, int a_flags, const QString &a_descr, 
	      const QString &a_extra);

/** structure to create Holders dynamicaly whith simple targets */
struct HolderInfo {
  const char *name; // name of type, to holder itself (or "obj"?)
  PFHolderData creator;
};


// -------------------------- HOLDERS ------------------------------------

/** Abstract holder for simple data types */
class HolderData : public QObject {
  Q_OBJECT
 public: 
  HolderData( const QString &obj_name, const QString &v_name = QString(), 
              TDataSet *a_parent = 0, int a_flags = 0,
	      const QString &a_descr = QString(), 
	      const QString &a_extra = QString() );
  virtual ~HolderData();
  void* getPtr() const { return ptr; }  // horror here !!!
  QVariant::Type getTp() const { return tp; }
  int isDyn() const { return dyn; }
  /** is holded value is object of given type or child */
  virtual bool isObject( const QString &cl_name = QString() ) const;
  void setFlags( int a_flags ) { flags = a_flags; }
  int getFlags() const { return flags; }
  double getMin() const { return v_min; }
  double getMax() const { return v_max; }
  void setParm( const QString &name, const QString &value );
  QString getParm( const QString &name ) const;
  void extraToParm();
  const QString& targetName() const { return target_name; }
  TDataSet* getParent() const { return par; }
  void setElems( const QString &els ); 
  const QStringList& getElems() const { return elems; }
  virtual bool set( const QVariant & x ) = 0;
  virtual QVariant get() const = 0;
  virtual void post_set() = 0;
  virtual QString toString() const = 0;
  virtual bool fromString( const QString &s ) = 0;
  virtual QString getType() const = 0;
  virtual QDomElement toDom( QDomDocument &dd ) const;
 protected:
  int dyn = 0; // flag: is created dynamicaly
  int flags; //* use bitset of _ELEM_FLAGS: efRO, efNoRunChange, ...
  double v_min = DMIN; // double as most common type, v_max = max_len
  double v_max = DMAX;
  QVariant::Type tp = QVariant::Invalid;
  void *ptr = nullptr;
  TDataSet *par;
  QString target_name;
  QStringList elems;
  QSSMap parms;
};

#define PRM_INIT( name, descr ) \
  __HO_##name( & name, #name, descr, this, __PRM_FLAGS_##name  )
#define POBJ_INIT( name, descr ) \
  __HO_##name( name, #name, descr, this, __PRM_FLAGS_##name  )
#define PRMI( name ) __HO_##name


/** Holder of int values */
class HolderInt : public HolderData {
  Q_OBJECT
 public: 
  HolderInt( int *p, const QString &obj_name,  // if p==0 - autocreate 
    const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
    const QString &a_descr = QString(),
    const QString &a_extra  = QString() );
  virtual ~HolderInt();
  static HolderData* createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra);
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual QString getType() const;
 protected:
  int *val;
  /** holder decription + autoregister */
  static HolderInfo holder_info;
  static int registered;
  static int reg();
};

#define PRM_INT1( name, flags, vname, descr, extra ) \
 int name; \
 HolderInt __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 

/** Holder of int values as Switch */
class HolderSwitch : public HolderInt {
  Q_OBJECT
 public: 
  HolderSwitch( int *p, const QString &obj_name,  // if p==0 - autocreate 
    const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
    const QString &a_descr = QString(),
    const QString &a_extra  = QString() );
  virtual ~HolderSwitch();
  static HolderData* createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra);
  virtual void post_set();
  virtual QString getType() const;
 protected: 
  /** holder decription + autoregister */
  static HolderInfo holder_info;
  static int registered;
  static int reg();
};

#define PRM_SWITCH1( name, flags, vname, descr, extra ) \
 int name; \
 HolderSwitch __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 


/** Holder of int values as List (ComboBox) */
class HolderList : public HolderInt {
  Q_OBJECT
 public: 
  HolderList( int *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString(), const QString &a_elems = QString() );
  virtual ~HolderList();
  static HolderData* createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra);
  // virtual void post_set();
  virtual QString getType() const;
 protected: 
  /** holder decription + autoregister */
  static HolderInfo holder_info;
  static int registered;
  static int reg();
};


#define PRM_LIST1( name, flags, vname, descr, extra, elems ) \
 int name; \
 HolderList __HO_##name ={ & name, #name, vname, this, flags, descr, extra, elems }; 

/** Holder of double values */
class HolderDouble : public HolderData {
  Q_OBJECT
 public: 
  HolderDouble( double *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderDouble();
  static HolderData* createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra);
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual QString getType() const;
 protected:
  double *val;
  /** holder decription + autoregister */
  static HolderInfo holder_info;
  static int registered;
  static int reg();
};

#define PRM_DOUBLE1( name, flags, vname, descr, extra ) \
 double name; \
 HolderDouble __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 
 
// to make alias to existent
#define PRM_DOUBLEx( ptr, name, flags, vname, descr, extra ) \
 HolderDouble __HO_##name ={  ptr, #name, vname, this, flags, descr, extra  } ; 
 

/** Holder of QString values */
class HolderString : public HolderData {
  Q_OBJECT
 public: 
  HolderString( QString *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderString();
  static HolderData* createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra);
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual QString getType() const;
 protected:
  QString *val;
  /** holder decription + autoregister */
  static HolderInfo holder_info;
  static int registered;
  static int reg();
};

#define PRM_STRING1( name, flags, vname, descr, extra ) \
 QString name; \
 HolderString __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 



/** Holder of QColor values */
class HolderColor : public HolderData {
  Q_OBJECT
 public: 
  HolderColor( QColor *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderColor();
  static HolderData* createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra);
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual QString getType() const;
 protected:
  /** holder decription + autoregister */
  static HolderInfo holder_info;
  static int registered;
  static int reg();
  QColor *val;
};

#define PRM_COLOR1( name, flags, vname, descr, extra ) \
 QColor name; \
 HolderColor __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 

/** Holder of objects */
class HolderObj : public HolderData {
  Q_OBJECT
 public: 
  HolderObj( TDataSet *p, const QString &obj_name,  // NO autocreate !
    const QString &v_name = QString(), TDataSet *a_parent = 0, int a_flags = 0,
    const QString &a_descr = QString(),
    const QString &a_extra  = QString() );
  virtual ~HolderObj();
  virtual bool isObject( const QString &cl_name = QString() ) const;
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual QString getType() const;
  TDataSet* getObj() { return obj; } // XXX: may be horror here!!
  virtual QDomElement toDom( QDomDocument &dd ) const;
 protected:
  TDataSet *obj;
  /** TODO??? holder decription + autoregister */
  //static HolderInfo holder_info;
  //static int registered;
  //static int reg();
};


#define PRM_OBJ1( name, flags, vname, descr, extra ) \
 HolderObj __HO_##name ={ name, #name, vname, this, flags, descr, extra  } ; 

// ####################################################################

/** base class for all objects */
class TDataSet : public QObject {
  Q_OBJECT
   friend class HolderData;
   friend class HolderObj;
 signals: 
   void sigStructChanged();
 public:
   /** default constructor */
   explicit TDataSet( TDataSet *aparent );
   /** destructor -- deallocates all subelements */
   virtual ~TDataSet();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class name - for check & human purpose */
   const char* getClassName(void) const;
   /** parent is TDataSet, not QObject */
   TDataSet* getParent() { return par; }
   /** return ptr from parent holders to me or 0 */
   HolderObj* myHolder() const { return holderOfMe; }
   /** fills dst with full name of object: .aaa.bbb.cc  */
   QString getFullName() const;
   /** return number of registerd elems = number of holders */
   int getNumObj() const;
   /** return flags of allow adding */
   int getAllowAdd() const { return allow_add; }
   /** returns list of registerd elems names - not holders */
   QStringList elemNames() const;
   /** returns list of holders */
   QVector<HolderData*> holders() const;
   /** returns holder by number - for QModel... */
   HolderData* getHolder( int i ) const;
   /** find holder for object by name */
   HolderData* getHolder( const QString &oname ) const;
   /** index of holder, if my, -1 - if not */
   int indexOfHolder( HolderData *ho ) const;
   /** returns ptr to help string */
   virtual const char* getHelp(void) const;
   /** return state */
   virtual int getState() const { return state; }
   /** returns modified flag */
   int getModified() const { return modified; }
   /** set modified flag */
   void setModified() { modified |= 1; }
   /** drop modified flag */
   void setUnModified() { modified = 0; }
   /** return ptr to elem by name */
   // use only by QMo2Doc while creating/loading 
   TDataSet* getObj( const QString &ename, const QString &cl_name = QString() );   

   /** new functions to read datas */
   int getData( const QString &nm, QVariant &da ) const;
   int getData( const QString &nm, int *da ) const;
   int getData( const QString &nm, double *da ) const;
   int getData( const QString &nm, QString &da ) const;

   /** new function to store datas: single: auto convert to QVariant */
   int setData( const QString &nm, const QVariant &da );
   
   /** corrects data, if ni==-1 -- all elements -- now empty, see setData */
   virtual int checkData( int ni );
   /** add new object and it's description (new)*/
   virtual TDataSet* add_obj( const QString &cl_name, const QString &ob_name );
   /** add new param and it's description */
   virtual HolderData* add_param( const QString &tp_name, const QString &ob_name );
   /** delete given object by name */
   int del_obj( const QString &ob_name );
   /** is given type of subelement valid for this object */
   virtual int isValidType( const QString &cl_name ) const;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; }
   /** is this class child of given or the same by name */
   bool isChildOf( const QString &cname );
   /** new part - iface a-la Holder */
   virtual bool set( const QVariant & x );
   virtual QVariant get() const;
   virtual void post_set();
   // to XML representation
   virtual QString toString() const;
   // from XML representation
   virtual bool fromString( const QString &s );
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
 protected:
   /** gets pointer to parameter, near to getDoublePrmPtr 
    * for param mod only - no descend  */
   double* getDoublePrmPtr( const QString &nm, int *flg );
   /** register new holder */
   bool registerHolder( HolderData *ho );
   /** unregister holder */
   bool unregisterHolder( HolderData *ho );
   /** do real actions after structure changed */
   virtual void do_structChanged();
 protected:
   /** guard value: debug */
   static const int guard_val = 7442428;
   int guard = guard_val;
   /** map of holders* */
   QSHoMap ho_map;
   /** QVector of holders* */
   QHoVect ho_vect;
   /** parent may be 0 */
   TDataSet *par;
   /** Holder in parent to me. May be 0. */
   HolderObj *holderOfMe = nullptr;
   /** state: 0-bad, 1-constructed, 2-run; */
   int state = stateGood; 
   /** allowing object add /remove for some classes 1-add obj, 2-add params */
   int allow_add = 0;
   /** flag: is modified: 0:no, 1-yes, 2-yes(auto) */
   int modified = 0;
   /** class decription */
   static TClassInfo class_info;
   /** help string */
   static const char* helpstr; 
};

// ---------------------------------------------------------------------

/** creator of TDataSet childs by name or id (legacy)
 * */
class ElemFactory {
 typedef QMap<QString,const TClassInfo*> MapStrClass;
 typedef QMap<QString,const HolderInfo*> MapStrHolder;
  public:
   static ElemFactory& theFactory();
   TDataSet* createElem( const QString &a_type, 
       const QString &ob_name, TDataSet *parent  ) const;
   HolderData* createParam( const QString &a_type, 
       const QString &ob_name, TDataSet *parent  ) const;
   bool registerElemType( const TClassInfo *cl_inf );
   bool unregisterElemType( const QString &a_type );
   bool registerSimpleType( const HolderInfo *ho_inf );
   QStringList allTypeNames() const { return str_class.keys(); }
   QStringList allParamTypes() const { return str_holder.keys(); }
   const TClassInfo* getInfo( const QString &a_type ) const;
   bool isChildOf( const QString &cl, const QString &par_cl );

  private:
   ElemFactory();
   ElemFactory( const ElemFactory& r ) = delete;
   ElemFactory( const ElemFactory &&r ) = delete;
   ElemFactory& operator=( const ElemFactory& r ) = delete;

   MapStrClass str_class;
   MapStrHolder str_holder;
};

#define EFACT ElemFactory::theFactory() 

#endif  // _DATASET_H


