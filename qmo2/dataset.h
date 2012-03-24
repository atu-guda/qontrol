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
#include <QVariant>

#include "defs.h"
#define MAX_DESCRLEN 80
#define MAX_LISTLEN  256
#define CLASS_ID_TDataSet 1

using namespace std;

class   TDataSet;
typedef TDataSet* PTDataSet;
typedef const TDataSet* CPTDataSet;
typedef PTDataSet (*PFDataSet)( PTDataSet aparent );
class   TRootData;
class   TDataContainer;

/** describes class and it's creator
  used for class registration (my be many classes for one array of structs)
  TRootData must handle it
*/
struct TClassInfo {
  /** uniq class id; 0 -- end of list */
  int id;
  /** class name */
  char className[MAX_NAMELEN];
  /** ptr to static fun for creating instanses */
  PFDataSet creator;
  /** ptr to parents TClassInfo or 0 for TDataSet */
  const TClassInfo* parent_class;
  /** ptr to help string */
  const char *helpstr;
};

/** Abstract holder for simple data types */
class HolderData : public QObject {
  Q_OBJECT
 public: 
  HolderData( const QString &obj_name, QObject *a_parent = 0 );
  virtual ~HolderData() {}; // place to delete auto data
  void* getPtr() const { return ptr; } ; // horror here !!!
  QVariant::Type getTp() const { return tp; };
  int getOldTp() const { return old_tp; };
  int getOldSubTp() const { return old_subtp; };
  virtual bool set( const QVariant & x ) = 0;
  virtual QVariant get() const = 0;
  virtual QString toString() const = 0;
  virtual bool fromString( const QString &s ) = 0;
  // TODO: to/from XML
 protected:
  int old_tp, old_subtp, dyn;
  int flags; //* use bitset of _ELEM_FLAGS: efRO, efNoRunChange, ...
  QVariant::Type tp;
  void *ptr;
};

/** Holder of int values */
class HolderInt : public HolderData {
  Q_OBJECT
 public: 
  HolderInt( int *p, const QString &obj_name, QObject *a_parent = 0 ); // if p==0 - autocreate 
  virtual ~HolderInt();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
 protected:
  int *val;
};

/** Holder of double values */
class HolderDouble : public HolderData {
  Q_OBJECT
 public: 
  HolderDouble( double *p, const QString &obj_name, QObject *a_parent = 0 ); // if p==0 - autocreate 
  virtual ~HolderDouble();
  virtual bool set( const QVariant & x );
  virtual QVariant get() const;
  virtual QString toString() const;
  virtual bool fromString( const QString &s );
 protected:
  double *val;
};


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
  int read( istream *is );
  /** save to stream -- simple format one value -- one line.
      Using simple strict syntax: @ val1 val2 ...  or @ "val" */
  int save( ostream *os ) const;
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
   /** request for creating object - done by TRootData .. */
   virtual TDataSet* createObj( int id, TDataSet *apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return obj name, or "" if no parent, or 0 -- parent don't know it */
   virtual const char* getName(void) const;
   /** return child name, or 0 -- parent don't know it */
   virtual const char* getChildName( const TDataSet* child ) const;
   /** fills dst with full name[MAX_INPUTLEN] of object: aaa.bbb.cc 
     * @returns: 0 -- ok -1 --bad */
   virtual int getFullName( char *dst ) const;
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
   /** return ptr to elem by idx */
   virtual void* getObj( int ni );
   /** return ptr to elem by name */
   virtual void* getObj( const char *ename );   
   /** return element description or 0 */
   virtual const TDataInfo* getDataInfo( int ni ) const;
   /** data index or -1 if not found  */
   virtual int getDataIdx( const char *nm ) const;

   /** read integer data from element by index, convert if need and allowed */
   virtual int getDataII( int ni, int *da, int allowConv );
   /** read integer data from element by name, convert if need and allowed */
   virtual int getDataSI( const char *nm, int *da, int allowConv );
   /** read double data from element by index, convert if need and allowed */
   virtual int getDataID( int ni, double *da, int allowConv );
   /** read double data from element by name, convert if need and allowed */
   virtual int getDataSD( const char *nm, double *da, int allowConv );
   /** read string data from element by index, convert if need and allowed */
   virtual int getDataIS( int ni, QString *da, int maxlen, int allowConv );
   /** read string data from element by name, convert if need and allowed */
   virtual int getDataSS( const char *nm, QString *da, int maxlen, int allowConv );

   /** store integer data to element by number, convert if need */
   virtual int setDataII( int ni, int da, int allowConv );
   /** store integer data to element by name, convert if need */
   virtual int setDataSI( const char *nm, int da, int allowConv );
   /** store double data to element by number, convert if need */
   virtual int setDataID( int ni, double da, int allowConv );
   /** store double data to element by name, convert if need */
   virtual int setDataSD( const char *nm, double da, int allowConv );
   /** store string data to element by number, convert if need */
   virtual int setDataIS( int ni, const QString *da, int allowConv );
   /** store string data to element by name, convert if need */
   virtual int setDataSS( const char *nm, const QString *da, int allowConv );
   
   /** corrects data, if ni==-1 -- all elements -- now empty, see setData */
   virtual int checkData( int ni );
   /** save header, all elements and trail to stream */
   virtual int saveDatas( ostream *os );
   /** load data to all elements */
   virtual int loadDatas( istream *is );
   /** add new object and it's description */
   virtual int add_obj( const TDataInfo *dai );
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
 protected:
   /** count nelm, fills hval in d_i  */
   int initHash(void);
   /**  save one element */
   int saveData( int ni, ostream *os ) const;
   /** parse & assing one elem. returns: 0-ok 1-comment 2-end; <0-error */
   int processElem( istream *is );
 protected:
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
   vector<pvoid> ptrs;
   /** common ptr to data descriptions */
   TDataInfo *d_i;
   /** each class have own data descriptors */
   static TDataInfo dataset_d_i[1]; 
   /** class decription */
   static TClassInfo class_info;
   /** help string */
   static const char* helpstr; 
};

#endif  // _DATASET_H


