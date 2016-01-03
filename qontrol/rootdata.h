/***************************************************************************
                          rootdata.h  -  description lcass TRootData
                             -------------------
    begin                : Thu Apr 20 16:55:07 2000
    copyright            : GPL (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _ROOTDATA_H
#define _ROOTDATA_H

#include "dataset.h"

class LaboDoc;
class TModel;

/** class holding other classes descriptions
    so can create all registered class objects
*/
class TRootData : public TDataSet {
  Q_OBJECT
 public:
   DCL_CTOR(TRootData);
   virtual ~TRootData() override;
   DCL_CREATE;
   DCL_STD_INF;
   const LaboDoc* getDoc() const { return doc; };
   void setDoc( const LaboDoc *a_doc ) { doc = a_doc; }
   TModel *getModel() { return model; }
   QString getFilePath() const; // pass to doc, in exists
   QString getFileBase() const; // same
 protected:
   const LaboDoc *doc = nullptr;
   TModel *model = nullptr;
   Q_CLASSINFO( "nameHintBase",  "root" );
   DCL_DEFAULT_STATIC;
};

#endif  // _ROOTDATA_H


