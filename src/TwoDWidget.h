#ifndef TwoDWidget_H
#define TwoDWidget_H

#include <QWidget>
#include "Params.h"


class TwoDWidget : public QWidget
{
    Q_OBJECT

	friend class MainFrame;
	friend class QXDirect;
public:
	TwoDWidget(QWidget *parent = 0);
//	void UpdateView();


protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event );
	void resizeEvent ( QResizeEvent * event );
	void paintEvent(QPaintEvent *event);
	void mouseDoubleClickEvent ( QMouseEvent * event );
 
private:
//	QRect m_rCltRect;          // The client window rectangle
	void * m_pXDirect, *m_pMiarex, *m_pAFoil, *m_pXInverse;
	void *m_pMainFrame;

};

#endif
