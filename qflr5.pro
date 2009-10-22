# -------------------------------------------------
# Project created by QtCreator 2009-02-14T15:30:46
# -------------------------------------------------
QT += opengl
TARGET = QFLR5
TEMPLATE = app
QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
CONFIG += x86 \
    ppc
SOURCES += MainFrame.cpp \
    XDirect/XFoil.cpp \
    XDirect/XFoilAnalysisDlg.cpp \
    XDirect/XFoilAdvancedDlg.cpp \
    XDirect/XDirect.cpp \
    XDirect/TwoDPanelDlg.cpp \
    XDirect/InterpolateFoilsDlg.cpp \
    XDirect/FoilPolarDlg.cpp \
    XDirect/FoilGeomDlg.cpp \
    XDirect/TEGapDlg.cpp \
    XDirect/ReListDlg.cpp \
    XDirect/NacaFoilDlg.cpp \
    XDirect/LEDlg.cpp \
    XDirect/FoilCoordDlg.cpp \
    XDirect/FlapDlg.cpp \
    XDirect/CAddDlg.cpp \
    XDirect/BatchDlg.cpp \
    Objects/WPolar.cpp \
    Objects/WOpp.cpp \
    Objects/Wing.cpp \
    Objects/Surface.cpp \
    Objects/Spline.cpp \
    Objects/Panel.cpp \
    Objects/Sf.cpp \
    Objects/Quaternion.cpp \
    Objects/OpPoint.cpp \
    Objects/Frame.cpp \
    Objects/POpp.cpp \
    Objects/Polar.cpp \
    Objects/CVector.cpp \
    Objects/Plane.cpp \
    Objects/Pf.cpp \
    Objects/CRectangle.cpp \
    Objects/Foil.cpp \
    Objects/Body.cpp \
    Misc/UnitsDlg.cpp \
    Misc/RenameDlg.cpp \
    Misc/LinePickerDlg.cpp \
    Misc/LineDelegate.cpp \
    Misc/LineCbBox.cpp \
    Misc/LineButton.cpp \
    Misc/FloatEditDelegate.cpp \
    Misc/DisplaySettingsDlg.cpp \
    Misc/ColorButton.cpp \
    Misc/FloatEdit.cpp \
    Misc/AboutQ5.cpp \
    Miarex/WPolarDlg.cpp \
    Miarex/WingDelegate.cpp \
    Miarex/VLMAnalysisDlg.cpp \
    Miarex/PlaneDlg.cpp \
    Miarex/PanelAnalysisDlg.cpp \
    Miarex/LLTAnalysisDlg.cpp \
    Miarex/ImportWingDlg.cpp \
    Miarex/GLLightDlg.cpp \
    Miarex/CpScaleDlg.cpp \
    Miarex/BodyTableDelegate.cpp \
    Miarex/Miarex.cpp \
    Miarex/ArcBall.cpp \
    Miarex/BodyGridDlg.cpp \
    Miarex/BodyScaleDlg.cpp \
    Graph/QGraph.cpp \
    Graph/GraphWidget.cpp \
    Graph/GraphDlg.cpp \
    Graph/Graph.cpp \
    Graph/Curve.cpp \
    Design/AFoil.cpp \
    Main.cpp \
    Globals.cpp \
    TwoDWidget.cpp \
    Miarex/GL3dBodyDlg.cpp \
    Design/FoilTableDelegate.cpp \
    XInverse/XInverse.cpp \
    Design/AFoilGridDlg.cpp \
    Miarex/GL3DScales.cpp \
    GLWidget.cpp \
    XInverse/InverseOptionsDlg.cpp \
    Miarex/ManageBodiesDlg.cpp \
    Misc/EditPlrDlg.cpp \
    Miarex/W3dPrefsDlg.cpp \
    Miarex/CtrlPolarDlg.cpp \
    Miarex/CtrlTableDelegate.cpp \
    Design/SplineCtrlsDlg.cpp \
    XDirect/XDirectStyleDlg.cpp \
    Miarex/WingScaleDlg.cpp \
    XInverse/FoilSelectionDlg.cpp \
    Miarex/BodyTransDlg.cpp \
    XInverse/PertDlg.cpp \
    Misc/SaveOptionsDlg.cpp \
    Miarex/WAdvancedDlg.cpp \
    Miarex/GL3dWingDlg.cpp \
    Misc/ProgressDlg.cpp \
    Design/LECircleDlg.cpp \
    Misc/ModDlg.cpp \
    XDirect/ManageFoilsDlg.cpp \
    Miarex/ManageUFOsDlg.cpp \
    Miarex/UFOTableDelegate.cpp \
    Misc/PolarFilterDlg.cpp \
    Misc/TranslatorDlg.cpp \
    QFLR5Application.cpp \
    Miarex/InertiaDlg.cpp
HEADERS += MainFrame.h \
    XDirect/XFoil.h \
    XDirect/XFoilAnalysisDlg.h \
    XDirect/XFoilAdvancedDlg.h \
    XDirect/XDirect.h \
    XDirect/TwoDPanelDlg.h \
    XDirect/TEGapDlg.h \
    XDirect/InterpolateFoilsDlg.h \
    XDirect/FoilGeomDlg.h \
    XDirect/FoilCoordDlg.h \
    XDirect/ReListDlg.h \
    XDirect/NacaFoilDlg.h \
    XDirect/LEDlg.h \
    XDirect/FoilPolarDlg.h \
    XDirect/FlapDlg.h \
    XDirect/CAddDlg.h \
    XDirect/BatchDlg.h \
    Objects/WPolar.h \
    Objects/WOpp.h \
    Objects/Wing.h \
    Objects/Surface.h \
    Objects/Spline.h \
    Objects/Sf.h \
    Objects/OpPoint.h \
    Objects/Quaternion.h \
    Objects/POpp.h \
    Objects/Polar.h \
    Objects/CVector.h \
    Objects/Plane.h \
    Objects/Pf.h \
    Objects/CRectangle.h \
    Objects/Panel.h \
    Objects/Frame.h \
    Objects/Foil.h \
    Objects/Body.h \
    Misc/UnitsDlg.h \
    Misc/RenameDlg.h \
    Misc/LinePickerDlg.h \
    Misc/LineDelegate.h \
    Misc/FloatEditDelegate.h \
    Misc/DisplaySettingsDlg.h \
    Misc/ColorButton.h \
    Misc/LineCbBox.h \
    Misc/LineButton.h \
    Misc/FloatEdit.h \
    Misc/AboutQ5.h \
    Miarex/WPolarDlg.h \
    Miarex/WingDelegate.h \
    Miarex/VLMAnalysisDlg.h \
    Miarex/PlaneDlg.h \
    Miarex/PanelAnalysisDlg.h \
    Miarex/LLTAnalysisDlg.h \
    Miarex/ImportWingDlg.h \
    Miarex/GLLightDlg.h \
    Miarex/CpScaleDlg.h \
    Miarex/Miarex.h \
    Miarex/ArcBall.h \
    Miarex/BodyGridDlg.h \
    Miarex/BodyTableDelegate.h \
    Miarex/BodyScaleDlg.h \
    Graph/GraphWidget.h \
    Graph/Graph.h \
    Graph/GraphDlg.h \
    Graph/Curve.h \
    Graph/QGraph.h \
    Design/AFoil.h \
    Globals.h \
    TwoDWidget.h \
    Miarex/GL3dBodyDlg.h \
    Design/FoilTableDelegate.h \
    XInverse/XInverse.h \
    Design/AFoilGridDlg.h \
    Miarex/GL3DScales.h \
    GLWidget.h \
    XInverse/InverseOptionsDlg.h \
    Miarex/ManageBodiesDlg.h \
    Misc/EditPlrDlg.h \
    Miarex/W3dPrefsDlg.h \
    Miarex/CtrlPolarDlg.h \
    Miarex/CtrlTableDelegate.h \
    Design/SplineCtrlsDlg.h \
    XDirect/XDirectStyleDlg.h \
    Miarex/WingScaleDlg.h \
    XInverse/FoilSelectionDlg.h \
    Miarex/BodyTransDlg.h \
    XInverse/PertDlg.h \
    Misc/SaveOptionsDlg.h \
    Miarex/WAdvancedDlg.h \
    Miarex/GL3dWingDlg.h \
    Misc/ProgressDlg.h \
    Params.h \
    Design/LECircleDlg.h \
    Misc/ModDlg.h \
    XDirect/ManageFoilsDlg.h \
    Miarex/ManageUFOsDlg.h \
    Miarex/UFOTableDelegate.h \
    Misc/PolarFilterDlg.h \
    Misc/TranslatorDlg.h \
    QFLR5Application.h \
    Miarex/InertiaDlg.h
TRANSLATIONS = qflr5.ts \
    qflr5_de.ts \
    qflr5_fr.ts
win32:RC_FILE = res/QFLR5.rc
RESOURCES += qflr5.qrc
FORMS += 
unix { 
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    
    # MAKE INSTALL
    INSTALLS += target
    target.path = $$BINDIR
}
mac { 
    QMAKE_INFO_PLIST = mac/Info.plist
    ICON = mac/qflr5.icns
}
OTHER_FILES += mac/Info.plist
