# -------------------------------------------------
# Project created by QtCreator 2009-02-14T15:30:46
# -------------------------------------------------
QT += opengl
TARGET = QFLR5
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
    XDirect/ManageFoilsDlg.cpp \
    XDirect/FoilCoordDlg.cpp \
    XDirect/FlapDlg.cpp \
    XDirect/CAddDlg.cpp \
    XDirect/BatchDlg.cpp \
    XDirect/XDirectStyleDlg.cpp \
    XInverse/XInverse.cpp \
    XInverse/InverseOptionsDlg.cpp \
    XInverse/FoilSelectionDlg.cpp \
    XInverse/PertDlg.cpp \
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
    Misc/EditPlrDlg.cpp \
    Misc/ModDlg.cpp \
    Misc/PolarFilterDlg.cpp \
    Misc/TranslatorDlg.cpp \
    Misc/SaveOptionsDlg.cpp \
    Misc/ProgressDlg.cpp \
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
    Miarex/GL3dBodyDlg.cpp \
    Miarex/ManageBodiesDlg.cpp \
    Miarex/W3dPrefsDlg.cpp \
    Miarex/CtrlPolarDlg.cpp \
    Miarex/CtrlTableDelegate.cpp \
    Miarex/WingScaleDlg.cpp \
    Miarex/BodyTransDlg.cpp \
    Miarex/ManageUFOsDlg.cpp \
    Miarex/UFOTableDelegate.cpp \
    Miarex/GL3DScales.cpp \
    Miarex/WAdvancedDlg.cpp \
    Miarex/GL3dWingDlg.cpp \
    Miarex/InertiaDlg.cpp \
    Design/FoilTableDelegate.cpp \
    Design/AFoilGridDlg.cpp \
    Design/SplineCtrlsDlg.cpp \
    Design/AFoil.cpp \
    Design/LECircleDlg.cpp \
    Graph/QGraph.cpp \
    Graph/GraphWidget.cpp \
    Graph/GraphDlg.cpp \
    Graph/Graph.cpp \
    Graph/Curve.cpp \
    Main.cpp \
    Globals.cpp \
    TwoDWidget.cpp \
    GLWidget.cpp \
    QFLR5Application.cpp
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
    XDirect/XDirectStyleDlg.h \
    XDirect/ManageFoilsDlg.h \
    XDirect/BatchDlg.h \
    XInverse/XInverse.h \
    XInverse/InverseOptionsDlg.h \
    XInverse/FoilSelectionDlg.h \
    XInverse/PertDlg.h \
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
    Misc/EditPlrDlg.h \
    Misc/SaveOptionsDlg.h \
    Misc/ProgressDlg.h \
    Misc/PolarFilterDlg.h \
    Misc/TranslatorDlg.h \
    Misc/ModDlg.h \
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
    Miarex/W3dPrefsDlg.h \
    Miarex/CtrlPolarDlg.h \
    Miarex/CtrlTableDelegate.h \
    Miarex/GL3dBodyDlg.h \
    Miarex/GL3DScales.h \
    Miarex/ManageBodiesDlg.h \
    Miarex/WingScaleDlg.h \
    Miarex/BodyTransDlg.h \
    Miarex/WAdvancedDlg.h \
    Miarex/GL3dWingDlg.h \
    Miarex/ManageUFOsDlg.h \
    Miarex/UFOTableDelegate.h \
    Miarex/InertiaDlg.h \
    Graph/GraphWidget.h \
    Graph/Graph.h \
    Graph/GraphDlg.h \
    Graph/Curve.h \
    Graph/QGraph.h \
    Design/AFoil.h \
    Design/FoilTableDelegate.h \
    Design/SplineCtrlsDlg.h \
    Design/AFoilGridDlg.h \
    Design/LECircleDlg.h \
    Globals.h \
    TwoDWidget.h \
    GLWidget.h \
    Params.h \
    QFLR5Application.h
TRANSLATIONS = Translations/qflr5.ts \
    Translations/qflr5_de.ts \
    Translations/qflr5_fr.ts
win32:RC_FILE = res/QFLR5.rc
RESOURCES += qflr5.qrc
FORMS += 
unix { 
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /home/windsoarer/Qt/rpmbuild
    BINDIR = $$PREFIX/BUILDROOT/QFLR5-4.04-4.x86_64/usr/local/bin
    DATADIR = $$PREFIX/BUILDROOT/DynaLay-4.04-4.x86_64/usr/local/share
    
    # MAKE INSTALL
    INSTALLS += target
    target.path = $$BINDIR
}
mac { 
    TEMPLATE = app
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
    CONFIG += x86 \
        ppc
    OTHER_FILES += mac/Info.plist
    LIBS += -framework \
        CoreFoundation
    QMAKE_INFO_PLIST = mac/Info.plist
    ICON = mac/qflr5.icns
}
OTHER_FILES += ReleaseNotes.txt

#QMAKE_CFLAGS+=-pg
#QMAKE_CXXFLAGS+=-pg
#QMAKE_LFLAGS+=-pg
