/***************************************************************************
                          datawidget.h  - widgets for DataDialog
                             -------------------
    begin                : 2012.03.27
    copyright            : GPL (C) 2012 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _DATAWIDGET_H
#define _DATAWIDGET_H

#include <QtGui>
#include <QVariant>
#include <QString>

#include "dataset.h"
#include "qcolorbtn.h"

class DataWidget;



/** Abstract class for any widgets for data editing in DataDialog...
 * Must be used as co-parent with QWidget 
 * */
class DataWidget : public QWidget {
 public: 
  DataWidget( HolderData &d, QWidget *parent = 0 );
  virtual bool set() = 0;
  virtual bool get() const = 0;
  QVariant::Type getTp() const { return ho.getTp(); }
  static DataWidget* create( HolderData & /*ho*/, QWidget * /*parent*/ )
    { return 0;};
  // virtual void fix() = 0;
  // virtual bool check() = 0;
 protected:
  HolderData &ho;
};

/** properties of DataWidget */
struct DataWidgetProp {
  DataWidget* (*creator)( HolderData &h, QWidget *parent );
  const char* eprop; // edit props
};

typedef QMap<QString,DataWidgetProp> DwPropMap;

class DummyDataWidget:  public DataWidget {
 public:
  DummyDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QLabel *lbl;
};


class StringDataWidget:  public DataWidget {
 public:
  StringDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QLineEdit *le;
};

class StringMLDataWidget: public DataWidget {
 public:
  StringMLDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QTextEdit *te;
};


class IntDataWidget: public DataWidget {
 public:
  IntDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QLineEdit *le;
};

class IntSpinDataWidget: public DataWidget {
 public:
  IntSpinDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QSpinBox *sb;
};

class SwitchDataWidget: public DataWidget {
 public:
  SwitchDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QCheckBox *cb;
};


class ListDataWidget: public DataWidget {
 public:
  ListDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QComboBox *cb;
};

class DoubleDataWidget: public DataWidget {
 public:
  DoubleDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QLineEdit *le;
};


class DoubleSpinDataWidget: public DataWidget {
 public:
  DoubleSpinDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QDoubleSpinBox *sb;
};


class ColorDataWidget: public DataWidget {
 public:
  ColorDataWidget( HolderData &h, QWidget *parent = 0 );
  virtual bool set();
  virtual bool get() const;
  static DataWidget* create( HolderData &h, QWidget *parent  );
  virtual QSize	minimumSizeHint() const;
  virtual QSize	sizeHint() const;
 protected:
  static int reg();
  static int registered;
  QColorBtn *cb;
};

// --------------------------- Factory ----------------------


/** creator of DataWidget and database of its properties 
 * */
class FactoryDataWidget {
  public:
   static FactoryDataWidget& theFactory();
   DataWidget* createDataWidget( HolderData &ho, QWidget *parent  ) const;
   QString findForHolder( const HolderData &ho, int *lev = 0 ) const;
   bool registerWidgetType( const QString &wname, const DataWidgetProp &prop );
   bool unregisterWidgetType( const QString &wname );

  private:
   FactoryDataWidget();
   FactoryDataWidget( const FactoryDataWidget& r ) = delete;
   FactoryDataWidget& operator=( const FactoryDataWidget& r ) = delete;

   DwPropMap propMap;
};

// ======================= DataDialog ===========================
//* simple class for dialog with DataWidgets

typedef QMap<QString,DataWidget*> DaWiMap;

class DataDialog : public QDialog {
  public:
   DataDialog( TDataSet &a_ds, QWidget *parent = 0 );
   int getAll();
   int setAll();
  public slots:
   virtual void accept();
  protected: 
   virtual int createWidgets();
   TDataSet &ds;
   DaWiMap dwm;
};


#endif  // _DATAWIDGET_H

