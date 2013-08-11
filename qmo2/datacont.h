/***************************************************************************
                          datacont.h  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/



#ifndef _DATACONT_H
#define _DATACONT_H

#include "dataset.h"

/** this class allows insertion any type of objects
  *@author atu
  */

class TDataContainer : public TDataSet  {
  Q_OBJECT
 public:
  explicit TDataContainer( TDataSet* aparent );
  virtual ~TDataContainer();
  /** creates instance */
  static TDataSet* create( TDataSet* apar );
  /** return ptr to static class_info, must be implemented in each class */
  virtual const TClassInfo* getClassInfo(void) const;
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
