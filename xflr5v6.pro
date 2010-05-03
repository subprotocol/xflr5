# -------------------------------------------------
# Project created by QtCreator 2009-02-14T15:30:46
# -------------------------------------------------
QT += opengl
TEMPLATE = app
SOURCES += src/MainFrame.cpp \
    src/Main.cpp \
    src/Globals.cpp \
    src/TwoDWidget.cpp \
    src/GLWidget.cpp \
    src/QFLR5Application.cpp \
    src/Miarex/Miarex.cpp \
    src/Miarex/GLCreateLists.cpp \
    src/Miarex/WPolarDlg.cpp \
    src/Miarex/WingDelegate.cpp \
    src/Miarex/VLMAnalysisDlg.cpp \
    src/Miarex/PlaneDlg.cpp \
    src/Miarex/PanelAnalysisDlg.cpp \
    src/Miarex/LLTAnalysisDlg.cpp \
    src/Miarex/ImportWingDlg.cpp \
    src/Miarex/GLLightDlg.cpp \
    src/Miarex/CpScaleDlg.cpp \
    src/Miarex/BodyTableDelegate.cpp \
    src/Miarex/ArcBall.cpp \
    src/Miarex/BodyGridDlg.cpp \
    src/Miarex/BodyScaleDlg.cpp \
    src/Miarex/StabViewDlg.cpp \
    src/Miarex/InertiaDlg.cpp \
    src/Miarex/StabPolarDlg.cpp \
    src/Miarex/StabAnalysisDlg.cpp \
    src/Miarex/ManualInertiaDlg.cpp \
    src/Miarex/GL3dBodyDlg.cpp \
    src/Miarex/ManageBodiesDlg.cpp \
    src/Miarex/W3dPrefsDlg.cpp \
    src/Miarex/CtrlPolarDlg.cpp \
    src/Miarex/CtrlTableDelegate.cpp \
    src/Miarex/WingScaleDlg.cpp \
    src/Miarex/BodyTransDlg.cpp \
    src/Miarex/WAdvancedDlg.cpp \
    src/Miarex/GL3dWingDlg.cpp \
    src/Miarex/ManageUFOsDlg.cpp \
    src/Miarex/UFOTableDelegate.cpp \
    src/Miarex/GL3DScales.cpp \
    src/XDirect/XFoil.cpp \
    src/XDirect/XFoilAnalysisDlg.cpp \
    src/XDirect/XFoilAdvancedDlg.cpp \
    src/XDirect/XDirect.cpp \
    src/XDirect/TwoDPanelDlg.cpp \
    src/XDirect/InterpolateFoilsDlg.cpp \
    src/XDirect/FoilPolarDlg.cpp \
    src/XDirect/FoilGeomDlg.cpp \
    src/XDirect/TEGapDlg.cpp \
    src/XDirect/ReListDlg.cpp \
    src/XDirect/NacaFoilDlg.cpp \
    src/XDirect/LEDlg.cpp \
    src/XDirect/FoilCoordDlg.cpp \
    src/XDirect/FlapDlg.cpp \
    src/XDirect/CAddDlg.cpp \
    src/XDirect/BatchDlg.cpp \
    src/XDirect/XDirectStyleDlg.cpp \
    src/XDirect/ManageFoilsDlg.cpp \
    src/Objects/WPolar.cpp \
    src/Objects/WOpp.cpp \
    src/Objects/Wing.cpp \
    src/Objects/Surface.cpp \
    src/Objects/Spline.cpp \
    src/Objects/Panel.cpp \
    src/Objects/Sf.cpp \
    src/Objects/Quaternion.cpp \
    src/Objects/OpPoint.cpp \
    src/Objects/Frame.cpp \
    src/Objects/POpp.cpp \
    src/Objects/Polar.cpp \
    src/Objects/CVector.cpp \
    src/Objects/Plane.cpp \
    src/Objects/Pf.cpp \
    src/Objects/CRectangle.cpp \
    src/Objects/Foil.cpp \
    src/Objects/Body.cpp \
    src/Misc/SaveOptionsDlg.cpp \
    src/Misc/ProgressDlg.cpp \
    src/Misc/ModDlg.cpp \
    src/Misc/EditPlrDlg.cpp \
    src/Misc/PolarFilterDlg.cpp \
    src/Misc/TranslatorDlg.cpp \
    src/Misc/UnitsDlg.cpp \
    src/Misc/RenameDlg.cpp \
    src/Misc/LinePickerDlg.cpp \
    src/Misc/LineDelegate.cpp \
    src/Misc/LineCbBox.cpp \
    src/Misc/LineButton.cpp \
    src/Misc/FloatEditDelegate.cpp \
    src/Misc/DisplaySettingsDlg.cpp \
    src/Misc/ColorButton.cpp \
    src/Misc/FloatEdit.cpp \
    src/Misc/AboutQ5.cpp \
    src/Graph/QGraph.cpp \
    src/Graph/GraphWidget.cpp \
    src/Graph/GraphDlg.cpp \
    src/Graph/Graph.cpp \
    src/Graph/Curve.cpp \
    src/XInverse/FoilSelectionDlg.cpp \
    src/XInverse/PertDlg.cpp \
    src/XInverse/XInverse.cpp \
    src/XInverse/InverseOptionsDlg.cpp \
    src/Design/FoilTableDelegate.cpp \
    src/Design/AFoilGridDlg.cpp \
    src/Design/LECircleDlg.cpp \
    src/Design/AFoil.cpp \
    src/Design/SplineCtrlsDlg.cpp 

HEADERS += src/MainFrame.h \
    src/Params.h \
    src/QFLR5Application.h \
    src/Globals.h \
    src/TwoDWidget.h \
    src/GLWidget.h \
    src/Miarex/Miarex.h \
    src/Miarex/W3dPrefsDlg.h \
    src/Miarex/GLCreateLists.h \
    src/Miarex/WingScaleDlg.h \
    src/Miarex/CtrlPolarDlg.h \
    src/Miarex/CtrlTableDelegate.h \
    src/Miarex/ManageBodiesDlg.h \
    src/Miarex/WPolarDlg.h \
    src/Miarex/WingDelegate.h \
    src/Miarex/VLMAnalysisDlg.h \
    src/Miarex/PlaneDlg.h \
    src/Miarex/PanelAnalysisDlg.h \
    src/Miarex/ManageUFOsDlg.h \
    src/Miarex/UFOTableDelegate.h \
    src/Miarex/StabViewDlg.h \
    src/Miarex/InertiaDlg.h \
    src/Miarex/StabPolarDlg.h \
    src/Miarex/StabAnalysisDlg.h \
    src/Miarex/ManualInertiaDlg.h \
    src/Miarex/LLTAnalysisDlg.h \
    src/Miarex/ImportWingDlg.h \
    src/Miarex/GLLightDlg.h \
    src/Miarex/CpScaleDlg.h \
    src/Miarex/ArcBall.h \
    src/Miarex/BodyGridDlg.h \
    src/Miarex/BodyTableDelegate.h \
    src/Miarex/BodyScaleDlg.h \
    src/Miarex/WAdvancedDlg.h \
    src/Miarex/GL3dWingDlg.h \
    src/Miarex/BodyTransDlg.h \
    src/Miarex/GL3DScales.h \
    src/Miarex/GL3dBodyDlg.h \
    src/XDirect/XFoil.h \
    src/XDirect/XFoilAnalysisDlg.h \
    src/XDirect/XFoilAdvancedDlg.h \
    src/XDirect/XDirect.h \
    src/XDirect/TwoDPanelDlg.h \
    src/XDirect/TEGapDlg.h \
    src/XDirect/InterpolateFoilsDlg.h \
    src/XDirect/FoilGeomDlg.h \
    src/XDirect/FoilCoordDlg.h \
    src/XDirect/ReListDlg.h \
    src/XDirect/XDirectStyleDlg.h \
    src/XDirect/ManageFoilsDlg.h \
    src/XDirect/NacaFoilDlg.h \
    src/XDirect/LEDlg.h \
    src/XDirect/FoilPolarDlg.h \
    src/XDirect/FlapDlg.h \
    src/XDirect/CAddDlg.h \
    src/XDirect/BatchDlg.h \
    src/XInverse/XInverse.h \
    src/XInverse/InverseOptionsDlg.h \
    src/XInverse/FoilSelectionDlg.h \
    src/XInverse/PertDlg.h \
    src/Objects/WPolar.h \
    src/Objects/WOpp.h \
    src/Objects/Wing.h \
    src/Objects/Surface.h \
    src/Objects/Spline.h \
    src/Objects/Sf.h \
    src/Objects/OpPoint.h \
    src/Objects/Quaternion.h \
    src/Objects/POpp.h \
    src/Objects/Polar.h \
    src/Objects/CVector.h \
    src/Objects/Plane.h \
    src/Objects/Pf.h \
    src/Objects/CRectangle.h \
    src/Objects/Panel.h \
    src/Objects/Frame.h \
    src/Objects/Foil.h \
    src/Objects/Body.h \
    src/Misc/SaveOptionsDlg.h \
    src/Misc/ModDlg.h \
    src/Misc/PolarFilterDlg.h \
    src/Misc/TranslatorDlg.h \
    src/Misc/UnitsDlg.h \
    src/Misc/RenameDlg.h \
    src/Misc/LinePickerDlg.h \
    src/Misc/LineDelegate.h \
    src/Misc/FloatEditDelegate.h \
    src/Misc/DisplaySettingsDlg.h \
    src/Misc/ColorButton.h \
    src/Misc/LineCbBox.h \
    src/Misc/LineButton.h \
    src/Misc/EditPlrDlg.h \
    src/Misc/AboutQ5.h \
    src/Misc/FloatEdit.h \
    src/Misc/ProgressDlg.h \
    src/Graph/GraphWidget.h \
    src/Graph/Graph.h \
    src/Graph/GraphDlg.h \
    src/Graph/Curve.h \
    src/Graph/QGraph.h \
    src/Design/AFoil.h \
    src/Design/AFoilGridDlg.h \
    src/Design/LECircleDlg.h \
    src/Design/SplineCtrlsDlg.h \
    src/Design/FoilTableDelegate.h
TRANSLATIONS = translations/xflr5.ts \
    translations/xflr5_de.ts \
    translations/xflr5_fr.ts
win32 { 
    TARGET = XFLR6
    RC_FILE = win/xflr5.rc
}
RESOURCES += xflr5.qrc
unix { 
    TARGET = xflr6
    
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    
    # MAKE INSTALL
    INSTALLS += target
}

# target.path = $$BINDIR
macx {
    TARGET = XFLR5
    TEMPLATE = app
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
    CONFIG += x86 \
        ppc
    OTHER_FILES += mac/Info.plist
    LIBS += -framework \
        CoreFoundation
    QMAKE_INFO_PLIST = mac/Info.plist
    ICON = mac/xflr5.icns
}
OTHER_FILES += doc/ReleaseNotes.txt

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg
