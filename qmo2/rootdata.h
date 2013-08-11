/***************************************************************************
                          rootdata.h  -  description lcass TRootData
                             -------------------
    begin                : Thu Apr 20 16:55:07 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _ROOTDATA_H
#define _ROOTDATA_H

#include <datacont.h>

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
  /** return ptr to static class_info, must be implemented in each class */
  virtual const TClassInfo* getClassInfo(void) const;
  /** returns help string */
  virtual const char* getHelp(void) const;
 protected:
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


