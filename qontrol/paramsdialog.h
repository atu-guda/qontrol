#pragma once
/***************************************************************************
     paramsdialog.h - dialog to ask params before run
                             -------------------
    begin                : 2016.07.04
    copyright            : GPL (C) 2012-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>
#include "askedparam.h"



class ParamsDialog : public QDialog {
 Q_OBJECT
  public:
   ParamsDialog( QList<AskedParam> &_plist, QWidget *_parent );
  public Q_SLOTS:
   virtual void accept() override;
  protected:
   void setupUi();
   QList<AskedParam> &plist;
   QMap<QString,QLineEdit*> eds;
};


