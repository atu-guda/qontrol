/***************************************************************************
                          addelem.h  -  description
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _ADDELEMDIA_H
#define _ADDELEMDIA_H

#include <QtWidgets>
#include "dataset.h"

// ---------------------------- SelectTypeDialog --------------------------------------

class SelectTypeDialog : public QDialog {
 Q_OBJECT
  public:
   SelectTypeDialog( HolderData *a_pobj, QWidget *aparent,
       const QString& allowed_types = QString() );
   QString getSelType() const { return sel_type; }
   static QString getType( HolderData *pobj, QWidget *aparent,
       const QString& allowed_types = QString() );
   static QString getTypeAndName( HolderData *pobj, QWidget *aparent, QString &nm,
       const QString& allowed_types = QString() );
   int countTypes() const { return cl_names.size(); }
   QString getTypeN( int n ) const
     { return  n<cl_names.size() ? cl_names[n] : QString(); }
  public slots:
   virtual void accept();
  protected:
   void setupUi();
   QString allowed;
   QString sel_type;
   QStringList cl_names;
   HolderData *pobj; // parent object - to ask allowed types. my not be 0
   QListWidget *lw;
};

#endif

