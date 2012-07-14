/***************************************************************************
                          dataset.h  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#ifndef _DATASET_H
#define _DATASET_H

#include <iosfwd>
#include <vector>
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
class   TDataSet;
typedef TDataSet* PTDataSet;
typedef const TDataSet* CPTDataSet;
typedef PTDataSet (*PFDataSet)( PTDataSet aparent );
class   TRootData;
class   TDataContainer;

/** properties of class -- bitfield */
enum ClassProps {
  clpElem = 1,  clpPure = 2, clpContainer = 4,
  clpSpecial = 8
};

/** describes class and it's creator
  used for class registration (my be many classes for one array of structs)
  TRootData must handle it
*/
struct TClassInfo {
  /** uniq class id; 0 -- end of list */
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

// -------------------------- HOLDERS ------------------------------------

/** Abstract holder for simple data types */
class HolderData : public QObject {
  Q_OBJECT
 public: 
  HolderData( const QString &obj_name, const QString &v_name = QString(), 
              QObject *a_parent = 0, int a_flags = 0,
	      const QString &a_descr = QString(), 
	      const QString &a_extra = QString() );
  virtual ~HolderData() {}; // place to delete auto data
  void* getPtr() const { return ptr; } ; // horror here !!!
  QVariant::Type getTp() const { return tp; };
  int getOldTp() const { return old_tp; };
  int getOldSubTp() const { return old_subtp; };
  void setFlags( int a_flags ) { flags = a_flags; };
  int getFlags() const { return flags; };
  void setMinMax( double a_min, double a_max ) { v_min = a_min; v_max = a_max; };
  double getMin() const { return v_min; }
  double getMax() const { return v_max; }
  void setParm( const QString &name, const QString &value );
  QString getParm( const QString &name ) const;
  void extraToParm();
  const QString& targetName() const { return target_name; };
  // tmp: to remove, use only set/getParm
  void setVisName( const QString &av_name );
  QString getVisName() const;
  void setDescr( const QString &a_descr );
  QString getDescr() const;
  void setProps( const QString &a_prop );
  QString getProps() const ;
  void setElems( const QString &els ); 
  const QStringList& getElems() const { return elems; }; 
  virtual bool set( const QVariant & x ) = 0;
  virtual QVariant get() const = 0;
  virtual void post_set() = 0;
  virtual QString toString() const = 0;
  virtual bool fromString( const QString &s ) = 0;
  virtual QString getType() const = 0;
  virtual bool toOldStream( std::ostream &os ) const = 0;
  // virtual bool fromOldStream( std::istream *os ) const = 0;
  virtual QDomElement toDom( QDomDocument &dd ) const;
  // TODO: to/from XML
  // TODO: metadata in/out
 protected:
  int old_tp, old_subtp, dyn;
  int flags; //* use bitset of _ELEM_FLAGS: efRO, efNoRunChange, ...
  double v_min, v_max; // double as most common type, v_max = max_len
  QVariant::Type tp;
  void *ptr;
  QString target_name;
  QStringList elems;
  QSSMap parms;
  // test for auto params inclusion TODO: remove!!!
  // constexpr static const char* xxx_test1 = "Test1";
  // const char* const xxx_test2[4] = { "T2", R"(nice\nstring)", "T2e", nullptr };
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
    const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
    const QString &a_descr = QString(),
    const QString &a_extra  = QString() );
  virtual ~HolderInt();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual bool toOldStream( std::ostream &os ) const;
  // virtual bool fromOldStream( std::istream *os ) const;
  virtual QString getType() const;
 protected:
  int *val;
};

#define PRM_INT( name, flags ) \
 int name; \
 HolderInt __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 

#define PRM_INT1( name, flags, vname, descr, extra ) \
 int name; \
 HolderInt __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 

/** Holder of int values as Switch */
class HolderSwitch : public HolderInt {
  Q_OBJECT
 public: 
  HolderSwitch( int *p, const QString &obj_name,  // if p==0 - autocreate 
    const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
    const QString &a_descr = QString(),
    const QString &a_extra  = QString() );
  virtual ~HolderSwitch();
  virtual void post_set();
  virtual QString getType() const;
};

#define PRM_SWITCH( name, flags ) \
 int name; \
 HolderSwitch __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 

#define PRM_SWITCH1( name, flags, vname, descr, extra ) \
 int name; \
 HolderSwitch __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 


/** Holder of int values as List (ComboBox) */
class HolderList : public HolderInt {
  Q_OBJECT
 public: 
  HolderList( int *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString(), const QString &a_elems = QString() );
  virtual ~HolderList();
  // virtual void post_set();
  virtual QString getType() const;
 private:
};

#define PRM_LIST( name, flags ) \
 int name; \
 HolderList __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 


// TODO: elems
#define PRM_LIST1( name, flags, vname, descr, extra, elems ) \
 int name; \
 HolderList __HO_##name ={ & name, #name, vname, this, flags, descr, extra, elems }; 

/** Holder of double values */
class HolderDouble : public HolderData {
  Q_OBJECT
 public: 
  HolderDouble( double *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderDouble();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual bool toOldStream( std::ostream &os ) const;
  // virtual bool fromOldStream( std::istream *os ) const;
  virtual QString getType() const;
 protected:
  double *val;
};

#define PRM_DOUBLE( name, flags ) \
 double name; \
 HolderDouble __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 

#define PRM_DOUBLE1( name, flags, vname, descr, extra ) \
 double name; \
 HolderDouble __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 
 

#define PRM_DOUBLEx( ptr, name, flags, vname, descr, extra ) \
 HolderDouble __HO_##name ={  ptr, #name, vname, this, flags, descr, extra  } ; 
 

/** Holder of QString values */
class HolderString : public HolderData {
  Q_OBJECT
 public: 
  HolderString( QString *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderString();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual bool toOldStream( std::ostream &os ) const;
  // virtual bool fromOldStream( std::istream *os ) const;
  virtual QString getType() const;
 protected:
  QString *val;
};

#define PRM_STRING( name, flags ) \
 QString name; \
 HolderString __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 

#define PRM_STRING1( name, flags, vname, descr, extra ) \
 QString name; \
 HolderString __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 

/** Holder of QString fixed arrays */
class HolderStringArr : public HolderData {
  Q_OBJECT
 public: 
  HolderStringArr( QString *p, int an, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderStringArr();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual bool toOldStream( std::ostream &os ) const;
  // virtual bool fromOldStream( std::istream *os ) const;
  virtual QString getType() const;
 protected:
  int n;
  QString *val;
};

#define PRM_STRINGARR( name, n, flags, vname, descr, extra ) \
 QString name[n]; \
 HolderStringArr __HO_##name ={  & name, n, #name, vname, this, flags, descr, extra  } ; 


/** Holder of QColor values */
class HolderColor : public HolderData {
  Q_OBJECT
 public: 
  HolderColor( QColor *p, const QString &obj_name,  // if p==0 - autocreate 
     const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
     const QString &a_descr = QString(),
     const QString &a_extra  = QString() );
  virtual ~HolderColor();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual bool toOldStream( std::ostream &os ) const;
  // virtual bool fromOldStream( std::istream *os ) const;
  virtual QString getType() const;
 protected:
  QColor *val;
};

#define PRM_COLOR( name, flags ) \
 QColor name; \
 HolderColor __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 

#define PRM_COLOR1( name, flags, vname, descr, extra ) \
 QColor name; \
 HolderColor __HO_##name ={  & name, #name, vname, this, flags, descr, extra  } ; 

/** Holder of objects ??? TODO: ?? combine with TDataSet? or proxy */
class HolderObj : public HolderData {
  Q_OBJECT
 public: 
  HolderObj( TDataSet *p, const QString &obj_name,  // NO autocreate !
    const QString &v_name = QString(), QObject *a_parent = 0, int a_flags = 0,
    const QString &a_descr = QString(),
    const QString &a_extra  = QString() );
  virtual ~HolderObj();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual void post_set();
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
  virtual bool toOldStream( std::ostream &os ) const;
  // virtual bool fromOldStream( std::istream *os ) const;
  virtual QString getType() const;
  TDataSet* getObj() { return obj; } // XXX: may be horror here!!
  virtual QDomElement toDom( QDomDocument &dd ) const;
 protected:
  TDataSet *obj;
};

#define PRM_OBJ( name, flags ) \
 HolderObj __HO_##name ; \
 static const int __PRM_FLAGS_##name = flags ; 


#define PRM_OBJ1( name, flags, vname, descr, extra ) \
 HolderObj __HO_##name ={ name, #name, vname, this, flags, descr, extra  } ; 

// ----===============************** OLD part ----------------------------

/** describes each element of class
*/
struct TDataInfo {
  /** type: dtpXXX: 0-End; 1-Int; 2-Dou; 3-Str;.... 10-Obj, 11-Fun ... */
  int tp;           
  /** subtype(dtpsXXX): int: 0-Int; 1-Switch; 2-list;...  */
  int subtp;      
  /** max length for string, num of choices for list */
  int max_len;
  /** coords for dialog  */
  int dlg_x, dlg_y, dlg_w, dlg_h; 
  /** flags: efMustRead, efNoRunChange, efNoDial */
  int flags;     
  /**  max & min values for int and double values if min<=max */
  double v_min, v_max;
  /** hash value for name - filled by TSetData::initHash  */
  int hval;
  /** flag: created dynamicaly, not static element */
  int dyna;
  /**  name for access & i/o operations */
  char name[MAX_NAMELEN];
  /** description of element; - for help ?? and label string  */
  const char* descr;
  /** string of list(1.2) elements \n  - delimiter   */
  const char* listdata;
  /** read from stream */
  int read( std::istream *is );
  /** save to stream -- simple format one value -- one line.
      Using simple strict syntax: @ val1 val2 ...  or @ "val" */
  int save( std::ostream *os ) const;
};

typedef TDataInfo *PTDataInfo;
typedef const TDataInfo *CPTDataInfo;

/** base class for all objects */
class TDataSet : public QObject {
  Q_OBJECT
 public:
   /** default constructor */
   explicit TDataSet( TDataSet *aparent );
   /** destructor -- deallocates all subelements */
   virtual ~TDataSet();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** new way to create objects */
   TDataSet* createObj( const QString &cl_name, const QString &nm, TDataSet* apar );
   /** class id  - for compat */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   const char* getClassName(void) const;
   /** fills dst with full name of object: .aaa.bbb.cc  */
   QString getFullName() const;
   /** returns ptr to help string */
   virtual const char* getHelp(void) const;
   /** return state */
   virtual int getState(void) const { return state; };
   /** returns modified flag */
   int getModified(void) const { return modified; };
   /** set modified flag */
   void setModified(void) { modified |= 1; };
   /** return nelm */
   virtual int getN(void) const;
   /** return ptr to elem by name -- HORROR! TODO make protected? */
   // use only by QMo2Doc while cheating/loading 
   virtual void* getObj( const QString &ename );   
   // /** return ptr to Object elem by name  */
   // virtual const TDataSet* getObjPtr( const QString &nm ) const;
   /** find holder for object */
   HolderData* getHolder( const QString &oname ) const;
   /** data index or -1 if not found  */
   virtual int getDataIdx( const char *nm ) const;

   /** new functions to read datas */
   int getData( const QString &nm, QVariant &da ) const;
   int getData( const QString &nm, int *da ) const;
   int getData( const QString &nm, double *da ) const;
   int getData( const QString &nm, QString &da ) const;

   /** new function to store datas: single: auto convert to QVariant */
   int setData( const QString &nm, const QVariant &da );
   
   /** corrects data, if ni==-1 -- all elements -- now empty, see setData */
   virtual int checkData( int ni );
   /** save header, all elements and trail to stream */
   virtual int saveDatasOld( std::ostream &os );
   /** load data to all elements */
   virtual int loadDatas( std::istream *is );
   /** add new object and it's description (new)*/
   virtual void* add_obj( const QString &cl_name, const QString &ob_name );
   /** delete given object by num in ptrs */
   virtual int del_obj( int n_ptrs );
   /** is given type of subelement valid for this object */
   virtual int isValidType( int a_tp, int a_subtp ) const;
   /** return ptr to static class_info, must be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** is this class child of given or the same by name */
   virtual int isChildOf( const char *cname );
   /** is this class child of given or the same by index */
   virtual int isChildOf( int cid );
   /** new part - iface a-la Holder FIXME: implement it */
   virtual bool set( const QVariant & x );
   virtual QVariant get() const;
   virtual void post_set();
   virtual QString toString() const;
   virtual bool fromString( const QString &s );
   void dumpStruct() const;
   QDomElement toDom( QDomDocument &dd ) const;
   void check_guard() const;
   // special part - TODO: remove or ifdef in separate lib
   /** returns pointer to given parameter, checking if valid 
    * valid names:
    * elmname = elmname.out0 
    * elmname.parmname 
    * :parmname - only local param? 
    * parmname - try new, w/o ':' */
   virtual const double* getDoublePtr( const QString &nm ) const;
 protected:
   /** gets pointer to parameter, near to getDoublePrmPtr 
    * for param mod only - no descend  */
   double* getDoublePrmPtr( const QString &nm, int *flg );
   /** count nelm, fills hval in d_i  */
   int initHash(void);
   /** parse & assing one elem. returns: 0-ok 1-comment 2-end; <0-error */
   int processElem( std::istream *is );
 protected:
   /** guard value */
   int guard;
   static const int guard_val = 7442428;
   /** parent may be 0 */
   TDataSet *parent;
   /** current number of elements */
   int nelm;  
   /** state: 0-bad, 1-constructed, 2-run; */
   int state; 
   /** allowing object add /remove for some classes */
   int allow_add;
   /** flag: was d_i dynamicaly allocaled */
   int d_i_alloc;
   /** flag: is modified: 0:no, 1-yes, 2-yes(auto) */
   int modified;
   /** pointers to real data - filled by constructor or loadData if need */
   std::vector<pvoid> ptrs;
   /** common ptr to data descriptions */
   TDataInfo *d_i;
   /** each class have own data descriptors */
   static TDataInfo dataset_d_i[1]; 
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
 typedef QMap<int,QString> MapIdClass;
  public:
   static ElemFactory& theFactory();
   TDataSet* createElem( const QString &a_type, 
       const QString &ob_name, TDataSet *parent  ) const;
   TDataSet* createElem( int t_id, 
       const QString &ob_name, TDataSet *parent  ) const; // legacy
   bool registerElemType( const TClassInfo *cl_info );
   bool unregisterElemType( const QString &a_type );
   QStringList allTypeNames() const { return str_class.keys() ; };
   const TClassInfo* getInfo( const QString &a_type ) const;
   const TClassInfo* getInfo( int t_id ) const;

  private:
   ElemFactory();
   ElemFactory( const ElemFactory& r ) = delete;
   ElemFactory& operator=( const ElemFactory& r ) = delete;

   MapStrClass str_class;
   MapIdClass  id_class;
};


#endif  // _DATASET_H


