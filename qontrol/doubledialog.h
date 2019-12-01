#pragma once
/***************************************************************************
     doubledialog.h - dialog to data table representation
                             -------------------
    begin                : 2019.12.01
    copyright            : GPL (C) 2019-2019 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>

struct DatasInfo;
class DoubleTableModel;

class DoubleDialog : public QDialog {
 Q_OBJECT
  public:
   DoubleDialog( const DatasInfo &_di, const QString &_stat_str, QWidget *_parent );
  public Q_SLOTS:
   virtual void btn_clicked( QAbstractButton *button );
   virtual void findData();
   virtual void copySel();
  protected:
   void setupUi();
  //
   const DatasInfo &di;
   const QString &stat_str;

   DoubleTableModel *model = nullptr;
   QTableView *view = nullptr;

   QPushButton *btn_find = nullptr;
   QPushButton *btn_copy = nullptr;
};


