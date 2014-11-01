/***************************************************************************
                          addelem.h  -  description
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _ADDELEMDIA_H
#define _ADDELEMDIA_H

#include <QtWidgets>
#include "dataset.h"

struct addElemInfo {
  QString type; // class type
  QString name; // element name
  int order;
};

class AddElemDialog : public QDialog {
 Q_OBJECT
  public:
   AddElemDialog( addElemInfo *a_aei, TDataSet *a_pobj, QWidget *aparent,
       const QString& allowed_types = QString() );
  public slots:
   virtual void accept();
  protected:
   void setupUi();
   addElemInfo *aei;
   QString allowed;
   TDataSet *pobj; // parent object - to ask allowed types. my not be 0
   QLineEdit *ed_name;
   QLineEdit *ed_order;
   QListWidget *lw;
};

#endif

