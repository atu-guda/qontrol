#pragma once
/***************************************************************************
                          addelem.h  -  description
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>
#include "dataset.h"

struct AddObjParams {
  QString tp;
  QString name;
  QString descr;
  QString values;
  QString vis_name;
  QString extra;
};

// ---------------------------- SelectTypeDialog --------------------------------------

class SelectTypeDialog : public QDialog {
 Q_OBJECT
  public:
   SelectTypeDialog( HolderData *a_pobj, QWidget *aparent,
       const QString& allowed_types = QString() );
   QString getSelType() const { return sel_type; }
   static QString getType( HolderData *pobj, QWidget *aparent,
       const QString& allowed_types = QString() );
   static bool getTypeAndParams( HolderData *pobj, QWidget *aparent, AddObjParams &prm,
       const QString& allowed_types = QString() );
   static HolderData* askAndCreateObj( HolderData *pobj, QWidget *aparent, AddObjParams &prm,
       const QString& allowed_types = QString() );
   int countTypes() const { return cl_names.size(); }
   QString getTypeN( int n ) const
     { return  n<cl_names.size() ? cl_names[n] : QString(); }
  public Q_SLOTS:
   virtual void accept();
  protected:
   void setupUi();
   QString allowed;
   QString sel_type;
   QStringList cl_names;
   HolderData *pobj; // parent object - to ask allowed types. my not be 0
   QListWidget *lw;
};


