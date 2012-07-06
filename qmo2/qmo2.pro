QT += core gui xml
TEMPLATE = app
TARGET = qmo2x
CONFIG += release warn_on qt thread 
QMAKE_CXXFLAGS += -std=c++11
#CONFIG += debug warn_on qt thread 

RESOURCES = qmo2x.qrc

INCLUDEPATH += .. ../elems 

LIBS += -lgsl -lgslcblas -lm

SOURCES += \
  datacont.cpp \
  dataset.cpp \
  elemfact.cpp \
  main.cpp \
  miscfun.cpp \
  mo2settdlg.cpp \
  qcolorbtn.cpp \
  qdoubletable.cpp \
  qgraphview.cpp \
  qmo2doc.cpp \
  qmo2view.cpp \
  qmo2win.cpp \
  qoutview.cpp \
  qplotview.cpp \
  qrunview.cpp \
  qstatusmodel.cpp \
  qstructview.cpp \
  rootdata.cpp \
  tmiso.cpp \
  tmodel.cpp \
  datawidget.cpp \
  ../elems/tadjgen.cpp \
  ../elems/tcorralalysis.cpp \
  ../elems/tcounter.cpp \
  ../elems/tcriterion.cpp \
  ../elems/tdelay.cpp \
  ../elems/textrlatch.cpp \
  ../elems/tfourier.cpp \
  ../elems/tfriction.cpp \
  ../elems/tfuncmisc.cpp \
  ../elems/tfuncpoly.cpp \
  ../elems/tfunctrans.cpp \
  ../elems/tgraph.cpp \
  ../elems/thyst.cpp \
  ../elems/tinputany.cpp \
  ../elems/tintegrator.cpp \
  ../elems/tlatch.cpp \
  ../elems/tlinear.cpp \
  ../elems/tlinp1q0.cpp \
  ../elems/tlogic.cpp \
  ../elems/tlorenz.cpp \
  ../elems/trossler.cpp \
  ../elems/tmultipl.cpp \
  ../elems/toutarr.cpp \
  ../elems/toutputany.cpp \
  ../elems/tpid.cpp \
  ../elems/trand.cpp \
  ../elems/trandtrigg.cpp \
  ../elems/tsource.cpp \
  ../elems/tsourcelin.cpp \
  ../elems/tswitch.cpp \
  ../elems/ttrigger.cpp \
  ../elems/tvibro.cpp 

HEADERS += \
  datacont.h \
  dataset.h \
  defs.h \
  miscfun.h \
  mo2settdlg.h \
  myelems1.h \
  qcolorbtn.h \
  qdoubletable.h \
  qgraphview.h \
  qmo2doc.h \
  qmo2view.h \
  qmo2win.h \
  qoutview.h \
  qplotview.h \
  qrunview.h \
  qstatusmodel.h \
  qstructview.h \
  rootdata.h \
  tmiso.h \
  tmodel.h \ 
  datawidget.h \
  ../elems/tadjgen.h \
  ../elems/tcorralalysis.h \
  ../elems/tcounter.h \
  ../elems/tcriterion.h \
  ../elems/tdelay.h \
  ../elems/textrlatch.h \
  ../elems/tfourier.h \
  ../elems/tfriction.h \
  ../elems/tfuncmisc.h \
  ../elems/tfuncpoly.h \
  ../elems/tfunctrans.h \
  ../elems/tgraph.h \
  ../elems/thyst.h \
  ../elems/tinputany.h \
  ../elems/tintegrator.h \
  ../elems/tlatch.h \
  ../elems/tlinear.h \
  ../elems/tlinp1q0.h \
  ../elems/tlogic.h \
  ../elems/tlorenz.h \
  ../elems/trossler.h \
  ../elems/tmultipl.h \
  ../elems/toutarr.h \
  ../elems/toutputany.h \
  ../elems/tpid.h \
  ../elems/trand.h \
  ../elems/trandtrigg.h \
  ../elems/tsource.h \
  ../elems/tsourcelin.h \
  ../elems/tswitch.h \
  ../elems/ttrigger.h \
  ../elems/tvibro.h


