# -------------------------------------------------
# Project created by QtCreator 2009-02-14T15:30:46
# -------------------------------------------------
QT += opengl + xml
TEMPLATE = app
SOURCES += \
	src/XFLR5Application.cpp \
	src/miarex/BodyGridDlg.cpp \
	src/miarex/BodyScaleDlg.cpp \
	src/miarex/BodyTableDelegate.cpp \
	src/miarex/BodyTransDlg.cpp \
	src/miarex/CtrlTableDelegate.cpp \
	src/miarex/GLCreateLists.cpp \
	src/miarex/GLCreateBodyLists.cpp \
	src/miarex/GL3dWingDlg.cpp \
	src/miarex/GL3DScales.cpp \
	src/miarex/GL3dBodyDlg.cpp \
	src/miarex/LLTAnalysisDlg.cpp \
	src/miarex/LLTAnalysis.cpp \
	src/miarex/ImportWingDlg.cpp \
	src/miarex/InertiaDlg.cpp \
	src/miarex/Miarex.cpp \
	src/miarex/ManageBodiesDlg.cpp \
	src/miarex/ManageUFOsDlg.cpp \
	src/miarex/PlaneDlg.cpp \
	src/miarex/PanelAnalysisDlg.cpp \
	src/miarex/StabPolarDlg.cpp \
	src/miarex/StabViewDlg.cpp \
	src/miarex/UFOTableDelegate.cpp \
	src/miarex/WAdvancedDlg.cpp \
	src/miarex/WingDelegate.cpp \
	src/miarex/WingScaleDlg.cpp \
	src/miarex/WPolarDlg.cpp \
	src/xdirect/BatchDlg.cpp \
	src/xdirect/BatchThreadDlg.cpp \
	src/xdirect/XFoilTask.cpp \
	src/xdirect/CAddDlg.cpp \
	src/xdirect/FoilCoordDlg.cpp \
	src/xdirect/FoilGeomDlg.cpp \
	src/xdirect/FoilPolarDlg.cpp \
	src/xdirect/FlapDlg.cpp \
	src/xdirect/InterpolateFoilsDlg.cpp \
	src/xdirect/LEDlg.cpp \
	src/xdirect/ManageFoilsDlg.cpp \
	src/xdirect/NacaFoilDlg.cpp \
	src/xdirect/ReListDlg.cpp \
	src/xdirect/TEGapDlg.cpp \
	src/xdirect/TwoDPanelDlg.cpp \
	src/xdirect/XDirectStyleDlg.cpp \
	src/xdirect/XFoil.cpp \
	src/xdirect/XFoilAnalysisDlg.cpp \
	src/xdirect/XFoilAdvancedDlg.cpp \
	src/xdirect/XDirect.cpp \
	src/xdirect/EditPlrDlg.cpp \
	src/objects/ArcBall.cpp \
	src/objects/Body.cpp \
	src/objects/CVector.cpp \
	src/objects/Foil.cpp \
	src/objects/Frame.cpp \
	src/objects/OpPoint.cpp \
	src/objects/Panel.cpp \
	src/objects/Plane.cpp \
	src/objects/Polar.cpp \
	src/objects/POpp.cpp \
	src/objects/Quaternion.cpp \
	src/objects/Sf.cpp \
	src/objects/Spline.cpp \
	src/objects/Surface.cpp \
	src/objects/WPolar.cpp \
	src/objects/WOpp.cpp \
	src/objects/Wing.cpp \
	src/misc/SaveOptionsDlg.cpp \
	src/misc/ProgressDlg.cpp \
	src/misc/ModDlg.cpp \
	src/misc/PolarFilterDlg.cpp \
	src/misc/TranslatorDlg.cpp \
	src/misc/UnitsDlg.cpp \
	src/misc/RenameDlg.cpp \
	src/misc/LinePickerDlg.cpp \
	src/misc/LineDelegate.cpp \
	src/misc/LineCbBox.cpp \
	src/misc/LineButton.cpp \
	src/misc/FloatEditDelegate.cpp \
	src/misc/DisplaySettingsDlg.cpp \
	src/misc/ColorButton.cpp \
	src/misc/FloatEdit.cpp \
	src/misc/AboutQ5.cpp \
	src/misc/NewNameDlg.cpp \
	src/misc/ObjectPropsDlg.cpp \
	src/misc/W3dPrefsDlg.cpp \
	src/graph/QGraph.cpp \
	src/graph/GraphWidget.cpp \
	src/graph/GraphDlg.cpp \
	src/graph/Graph.cpp \
	src/graph/Curve.cpp \
	src/xinverse/FoilSelectionDlg.cpp \
	src/xinverse/PertDlg.cpp \
	src/xinverse/XInverse.cpp \
	src/xinverse/InverseOptionsDlg.cpp \
	src/design/FoilTableDelegate.cpp \
	src/design/AFoilGridDlg.cpp \
	src/design/LECircleDlg.cpp \
	src/design/AFoil.cpp \
	src/design/SplineCtrlsDlg.cpp \
	src/design/AFoilTableDlg.cpp \
    src/globals.cpp \
    src/main.cpp \
    src/mainframe.cpp \
    src/objects/NURBSSurface.cpp \
    src/misc/GLLightDlg.cpp \
    src/threedwidget.cpp \
    src/twodwidget.cpp \
    src/miarex/NURBSDomDoc.cpp

HEADERS += \
	src/XFLR5Application.h \
	src/miarex/Miarex.h \
	src/miarex/GLCreateLists.h \
	src/miarex/GLCreateBodyLists.h \
	src/miarex/WingScaleDlg.h \
	src/miarex/CtrlTableDelegate.h \
	src/miarex/ManageBodiesDlg.h \
	src/miarex/WPolarDlg.h \
	src/miarex/WingDelegate.h \
	src/miarex/PlaneDlg.h \
	src/miarex/PanelAnalysisDlg.h \
	src/miarex/ManageUFOsDlg.h \
	src/miarex/UFOTableDelegate.h \
	src/miarex/StabViewDlg.h \
	src/miarex/InertiaDlg.h \
	src/miarex/StabPolarDlg.h \
	src/miarex/LLTAnalysisDlg.h \
	src/miarex/LLTAnalysis.h \
	src/miarex/ImportWingDlg.h \
	src/miarex/BodyGridDlg.h \
	src/miarex/BodyTableDelegate.h \
	src/miarex/BodyScaleDlg.h \
	src/miarex/WAdvancedDlg.h \
	src/miarex/GL3dWingDlg.h \
	src/miarex/BodyTransDlg.h \
	src/miarex/GL3DScales.h \
	src/miarex/GL3dBodyDlg.h \
	src/xdirect/XFoil.h \
	src/xdirect/XFoilAnalysisDlg.h \
	src/xdirect/XFoilAdvancedDlg.h \
	src/xdirect/XDirect.h \
	src/xdirect/TwoDPanelDlg.h \
	src/xdirect/TEGapDlg.h \
	src/xdirect/InterpolateFoilsDlg.h \
	src/xdirect/FoilGeomDlg.h \
	src/xdirect/FoilCoordDlg.h \
	src/xdirect/ReListDlg.h \
	src/xdirect/XDirectStyleDlg.h \
	src/xdirect/ManageFoilsDlg.h \
	src/xdirect/NacaFoilDlg.h \
	src/xdirect/LEDlg.h \
	src/xdirect/FoilPolarDlg.h \
	src/xdirect/FlapDlg.h \
	src/xdirect/CAddDlg.h \
	src/xdirect/BatchDlg.h \
	src/xdirect/BatchThreadDlg.h \
	src/xdirect/XFoilTask.h \
	src/xdirect/EditPlrDlg.h \
	src/xinverse/XInverse.h \
	src/xinverse/InverseOptionsDlg.h \
	src/xinverse/FoilSelectionDlg.h \
	src/xinverse/PertDlg.h \
	src/objects/WPolar.h \
	src/objects/WOpp.h \
	src/objects/Wing.h \
	src/objects/Surface.h \
	src/objects/Spline.h \
	src/objects/Sf.h \
	src/objects/OpPoint.h \
	src/objects/Quaternion.h \
	src/objects/POpp.h \
	src/objects/Polar.h \
	src/objects/CVector.h \
	src/objects/Plane.h \
	src/objects/CRectangle.h \
	src/objects/Panel.h \
	src/objects/Frame.h \
	src/objects/Foil.h \
	src/objects/Body.h \
	src/objects/ArcBall.h \
	src/misc/SaveOptionsDlg.h \
	src/misc/ModDlg.h \
	src/misc/PolarFilterDlg.h \
	src/misc/TranslatorDlg.h \
	src/misc/UnitsDlg.h \
	src/misc/RenameDlg.h \
	src/misc/LinePickerDlg.h \
	src/misc/LineDelegate.h \
	src/misc/FloatEditDelegate.h \
	src/misc/DisplaySettingsDlg.h \
	src/misc/W3dPrefsDlg.h \
	src/misc/ColorButton.h \
	src/misc/LineCbBox.h \
	src/misc/LineButton.h \
	src/misc/AboutQ5.h \
	src/misc/FloatEdit.h \
	src/misc/ProgressDlg.h \
	src/misc/NewNameDlg.h \
	src/misc/ObjectPropsDlg.h \
	src/graph/GraphWidget.h \
	src/graph/Graph.h \
	src/graph/GraphDlg.h \
	src/graph/Curve.h \
	src/graph/QGraph.h \
	src/design/AFoil.h \
	src/design/AFoilGridDlg.h \
	src/design/LECircleDlg.h \
	src/design/SplineCtrlsDlg.h \
	src/design/FoilTableDelegate.h \
	src/design/AFoilTableDlg.h \
    src/params.h \
    src/globals.h \
    src/mainframe.h \
    src/objects/NURBSSurface.h \
    src/misc/GLLightDlg.h \
    src/twodwidget.h \
    src/threedwidget.h \
    src/miarex/NURBSDomDoc.h

TRANSLATIONS = translations/xflr5v6.ts \
    translations/xflr5v6_de.ts \
    translations/xflr5v6_fr.ts \
    translations/xflr5v6_ja.ts

RESOURCES += xflr5.qrc

win32 { 
    TARGET = XFLR5
    RC_FILE = win/xflr5.rc
}

unix { 
    TARGET = xflr5
    
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    
    # MAKE INSTALL
    INSTALLS += target
#    target.path = $$BINDIR
}

macx { 
    CONFIG(release, debug|release) { 
        OBJECTS_DIR = ./build/release
        MOC_DIR = ./build/release
        RCC_DIR = ./build/release
        UI_HEADERS_DIR = ./build/release
    }
    CONFIG(debug, debug|release) { 
        OBJECTS_DIR = ./build/debug
        MOC_DIR = ./build/debug
        RCC_DIR = ./build/debug
        UI_HEADERS_DIR = ./build/debug
    }
    TARGET = XFLR5
    TEMPLATE = app
    CONFIG += i386 \
        ppc
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
    OTHER_FILES += mac/Info.plist
    LIBS += -framework \
        CoreFoundation
    QMAKE_INFO_PLIST = mac/Info.plist
    ICON = mac/xflr5.icns
}
OTHER_FILES += doc/ReleaseNotes.txt


#QMAKE_CFLAGS+=-pg
#QMAKE_CXXFLAGS+=-pg
#QMAKE_LFLAGS+=-pg

