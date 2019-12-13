// =====================================================================================
//
//       Filename:  datatableview.cpp
//
//    Description: class DataTableView implementetion (QTableView + mouse, keyboard ...)
//        Created:  2019-12-14 00:25:45
//
//         Author:  Anton Guda (atu), atu@nmetau.edu.ua
//      Copyright:  GPLv2+ 2019-2019 by atu
//   Organization:  NMetAU.ITS
//
// =====================================================================================

#include <QtWidgets>

#include "miscfun.h"
#include "datatableview.h"

DataTableView::DataTableView( QWidget *parent )
  : QTableView( parent )
{
}

void DataTableView::findData() // slot
{
  QMessageBox::information( this, QSL("Find"), QSL("DataTableView::findData() called") );
}

void DataTableView::copySel() // slot
{
  QString s;
  int old_row = -1;
  auto indexes = selectionModel()->selection().indexes();

  for( const auto& index : indexes ) {
    if( ! index.isValid() ) {
      continue;
    }
    if( old_row != -1 && old_row != index.row() ) {
      s += '\n';
    }
    s += index.data().toString() + '\t' ;
    old_row = index.row();
  }
  setClipboardStr( s );
}

void DataTableView::keyPressEvent( QKeyEvent *ke )
{
  auto k = ke->key();
  auto st = ke->modifiers();
  constexpr decltype(k) Sh = 0x40000000;
  constexpr decltype(k) Ct = 0x80000000;

  if( st & Qt::ShiftModifier ) {
      k |= Sh;
  }
  if( st & Qt::ControlModifier ) {
      k |= Ct;
  }

  bool my = false;
  switch( k ) {
    case Qt::Key_C | Ct:
      copySel();
      break;
    case Qt::Key_F | Ct:
      findData();
      break;
    default:
      break;
  }

  if( !my ) {
    QTableView::keyPressEvent( ke );
  }
}

void DataTableView::mousePressEvent( QMouseEvent *me )
{
  auto mouse_x = me->x();
  auto mouse_y = me->y();
  auto btn = me->button();

  bool my = false;
  switch( btn ) {
    case Qt::LeftButton:
      break;
    case Qt::RightButton:
      {
        QMenu *menu = createPopupMenu();
        menu->exec( mapToGlobal( QPoint( mouse_x, mouse_y ) ) );
        delete menu;
      }
      my = true;
      break;
    case Qt::MidButton:
      break;
    default:
      break;
  };

  if( !my ) {
    QTableView::mousePressEvent( me );
  }
}

QMenu* DataTableView::createPopupMenu()
{
  auto menu = new QMenu( this );

  QAction *act;

  act = menu->addAction( QSL("&Copy (Ctrl-C)") );
  connect( act, &QAction::triggered, this, &DataTableView::copySel );
  menu->addSeparator();

  return menu;
}

