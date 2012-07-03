/***************************************************************************
                          rootdata.h  -  description lcass TRootData
                             -------------------
    begin                : Thu Apr 20 16:55:07 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#ifndef _ROOTDATA_H
#define _ROOTDATA_H

#include <datacont.h>

#define CLASS_ID_TRootData 10

/** class holding other classes descriptions
    so can create all registered class objects
*/
class TRootData : public TDataContainer {
  Q_OBJECT
 public:
   /** constructor - registered def classes */
   explicit TRootData( TDataSet* aparent );
   /** destructor */
   virtual ~TRootData();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** real creating object by it's id */
   virtual TDataSet* createObj( int id, const QString &nm, TDataSet* apar );
   /** class id */
   virtual int getClassId(void) const;
   /** for registering */
   virtual int regClass( const TClassInfo *c_i );
  /** finds class by it's name */
  int findClass( const char *name ) const;
  /** finds class by it's ID */
  int findClass( int a_id ) const;
  /** in addition to TDataSet save as comments %!c all known classes */
  virtual int saveDatas( ostream *os );
  /** return ptr to static class_info, must be implemented in each class */
  virtual const TClassInfo* getClassInfo(void) const;
  /** return ptr to static class_info, static version */
  static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
  /** returns help string */
  virtual const char* getHelp(void) const;
  /** return number of registered classes of given type */
  int getNClasses( int parent_id = -1, int countParent = 0 ) const;
  /** is child-parent pair */
  int isHisParent( int child_id, int parent_id, int countParent = 0 ) const;
  /** returns pointer to TClassInfo by id */
  const TClassInfo* classInfo( int id ) const;
  /** returns pointer to TClassInfo by num */
  const TClassInfo* classInfoByNum( int n ) const;
 protected:
   /** number of registered classes */
   int n_reg;
   /** storage for all registered classes */
   TClassInfo cl_info[MAX_CLASSES];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
  /** autoregister */
  static int registered;
  static int reg();
};
typedef TRootData* PTRootData;

#endif  // _ROOTDATA_H


