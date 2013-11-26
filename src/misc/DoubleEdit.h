#ifndef DOUBLEEDIT_H
#define DOUBLEEDIT_H

#include <QDoubleValidator>
#include <QLineEdit>
#include <QKeyEvent>


class DoubleEdit : public QLineEdit
{
public:
    DoubleEdit(QWidget *pParent=NULL);
	DoubleEdit(double val, int decimals=2);

	~DoubleEdit() {delete m_pDV;}

	void focusOutEvent ( QFocusEvent * event );
	void keyPressEvent(QKeyEvent *event);


	double Value(){return m_Value;}
	void SetValue(double val);


	void SetValueNoFormat(double val);

	void FormatValue();
	double ReadValue();
	void SetMin(double min) {m_pDV->setBottom(min);}
	void SetMax(double max) {m_pDV->setTop(max);}


	void SetPrecision(int decimals) {m_pDV->setDecimals(decimals);}

	void setNotation(QDoubleValidator::Notation notation) {m_pDV->setNotation(notation);}

	int precision(){return m_pDV->decimals();}

public:
	QDoubleValidator *m_pDV;
	double m_Value;//we need to store a full precision value, irrespective of the display
};

#endif // DOUBLEEDIT_H
