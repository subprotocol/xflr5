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
	~TwoDWidget();
	void UpdateView();


protected:
	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event );
	void resizeEvent ( QResizeEvent * event );
	void paintEvent(QPaintEvent *event);


private:
//	int m_iView;
	QRect m_rCltRect;          // The client window rectangle
	void * m_pXDirect, *m_pMiarex, *m_pAFoil;
	void *m_pMainFrame;

	QCursor m_hcArrow;
	QCursor m_hcCross;
	QCursor m_hcMove;
};

#endif
