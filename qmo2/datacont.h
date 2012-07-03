/***************************************************************************
                          datacont.h  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/



#ifndef _DATACONT_H
#define _DATACONT_H

#include "dataset.h"

#define CLASS_ID_TDataContainer 3

/** this class allows insertion any type of objects
  *@author atu
  */

class TDataContainer : public TDataSet  {
  Q_OBJECT
 public:
  /** allocates d_i for each instance */
  explicit TDataContainer( TDataSet* aparent );
  virtual ~TDataContainer();
  /** class id */
  virtual int getClassId(void) const;
  /** class name - for check & human purpose */
  virtual const char* getClassName(void) const;
  /** creates instance */
  static TDataSet* create( TDataSet* apar );
  /** return ptr to static class_info, must be implemented in each class */
  virtual const TClassInfo* getClassInfo(void) const;
  /** return ptr to static class_info, static version */
  static const TClassInfo* getStaticClassInfo(void) 
      { return &class_info; };
  /** returns help string */
  virtual const char* getHelp(void) const;
 protected: 
  /** class decription */
  static TClassInfo class_info; 
  /** static help str */
  static const char *helpstr;
  /** autoregister */
  static int registered;
  static int reg();
  
};

#endif // _DATACONT_H
