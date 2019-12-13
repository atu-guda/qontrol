// =====================================================================================
//
//       Filename:  datatableview.h
//
//    Description: class DataTableView declaration (QTableView + mouse, keyboard ...)
//
//        Version:  1.0
//        Created:  2019-12-14 00:15:40
//
//         Author:  Anton Guda (atu), atu@nmetau.edu.ua
//      Copyright:  GPLv2+ 2019-2019 by atu
//   Organization:  NMetAU.ITS
//
// =====================================================================================
#ifndef _DATATABLEVIEW_H
#define _DATATABLEVIEW_H

#include <QTableView>

class DataTableView : public QTableView {
   Q_OBJECT
  public:
   explicit DataTableView( QWidget *parent = nullptr );
  public Q_SLOTS:
   virtual void findData();
   virtual void copySel();
  protected:
   virtual void keyPressEvent( QKeyEvent *ke ) override;
   virtual void mousePressEvent( QMouseEvent *me ) override;
   QMenu* createPopupMenu();
};


#endif

