#ifndef MO2SETTDLG_H
#define MO2SETTDLG_H

#include <QDialog>

class QGridLayout;
class QPushButton;
class QCheckBox;

#include "qmo2win.h"

class Mo2SettDlg : public QDialog
{
  Q_OBJECT

  public:
    Mo2SettDlg( Mo2Settings &se, QWidget* parent = 0 );
    ~Mo2SettDlg();
  protected:
    Mo2Settings &pse;
    Mo2Settings ts;
    QPushButton* btnMainFont;
    QPushButton* btnSmallFont;
    QPushButton* btnPlotFont;
    QPushButton* btnStructFont;
    QCheckBox* chkMaximize;
    QPushButton* btnOK;
    QPushButton* btnCancel;

  public slots:
    virtual void mainFontSlot();
    virtual void smallFontSlot();
    virtual void plotFontSlot();
    virtual void structFontSlot();

  protected:
    QGridLayout* lay;

  protected slots:
    virtual void languageChange();
    virtual void accept();

};

#endif // MO2SETTDLG_H
