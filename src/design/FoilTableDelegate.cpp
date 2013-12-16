/****************************************************************************

	FoilTableDelegate Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include <QApplication>

#include "FoilTableDelegate.h"
#include "AFoil.h"
#include "../globals.h"
#include "../mainframe.h"

void *FoilTableDelegate::s_pAFoil;


FoilTableDelegate::FoilTableDelegate(QObject *parent)
 : QItemDelegate(parent)
{
}


QWidget *FoilTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	return NULL;//No edition possible - display only

	if(index.column()==0)
	{
		QLineEdit *editor = new QLineEdit(parent);
		editor->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		return editor;
	}
	else
	{
		DoubleEdit *editor = new DoubleEdit(parent);
		editor->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		editor->SetPrecision(m_Precision[index.column()]);
		return editor;
	}

	return NULL;
}


bool FoilTableDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
									const QModelIndex &index)
{
	if(index.column()<12) return false;
	// make sure that the item is checkable
	Qt::ItemFlags flags = model->flags(index);
	if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
		return false;
	// make sure that we have a check state
	QVariant value = index.data(Qt::CheckStateRole);
	if (!value.isValid())
		return false;

	// make sure that we have the right event type
	if (event->type() == QEvent::MouseButtonRelease)
	{
		const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
		QRect checkRect = QStyle::alignedRect(option.direction,
											  Qt::AlignCenter,
											  option.rect.size(),
											  QRect(option.rect.x() + textMargin, option.rect.y(),
													option.rect.width() - (2 * textMargin), option.rect.height()));


		if (!checkRect.contains(((QMouseEvent*)event)->pos())) return false;
	}
	else if (event->type() == QEvent::KeyPress)
	{
		if (   ((QKeyEvent*)event)->key() != Qt::Key_Space
			&& ((QKeyEvent*)event)->key() != Qt::Key_Select)
			return false;
	}
	else
	{
		return false;
	}

	Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);

	QAFoil *pAFoil = (QAFoil*)s_pAFoil;
	
	bool bSuccess = model->setData(index, state, Qt::CheckStateRole);
	if(bSuccess) pAFoil->FoilVisibleClicked(index);
		
	return bSuccess;
}


void FoilTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QString strong;
	QStyleOptionViewItem myOption = option;
	QAFoil *pAFoil = (QAFoil*)s_pAFoil;
	int NFoils = pAFoil->m_poaFoil->size();

	if(index.row()> NFoils)
	{
		strong=" ";
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
	else if(index.column()==0)
	{
		myOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
		strong = index.model()->data(index, Qt::DisplayRole).toString();
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
	else if(index.column()==5)
	{
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
		strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toInt());
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
	else if(index.column()==6 || index.column()==9)
	{
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
		strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble(), 0,'f',m_Precision[index.column()]);
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
	else if(index.column()==12 || index.column()==13 || index.column()==14)
	{
		QVariant value = index.data(Qt::CheckStateRole);
		Qt::CheckState state;
		if(value.toInt()==0)  state = Qt::Unchecked;
		else                  state = Qt::Checked;
		drawCheck(painter, myOption, myOption.rect, state);
//		drawFocus(painter, myOption, myOption.rect);
	}
	else if(index.column()==15)
	{
		QColor color;
		int style, width;

		if(index.row()==0)
		{
			color = pAFoil->m_pSF->m_FoilColor;
			style = pAFoil->m_pSF->m_FoilStyle;
			width = pAFoil->m_pSF->m_FoilWidth;
		}
		else
		{
			Foil *pFoil = (Foil*)pAFoil->m_poaFoil->at(index.row()-1);
			color = pFoil->m_FoilColor;
			style = pFoil->m_nFoilStyle;
			width = pFoil->m_nFoilWidth;			
		}
		QRect r = option.rect;
		r = pAFoil->m_pctrlFoilTable->visualRect(index);;

		painter->save();
	
		QPen LinePen(color);
		LinePen.setStyle(GetStyle(style));
		LinePen.setWidth(width);
		painter->setPen(LinePen);
		painter->drawLine(r.left()+5, r.top()+r.height()/2, r.right()-5, r.top()+r.height()/2);

		painter->restore();

//		drawFocus(painter, myOption, myOption.rect);
	}
	else
	{
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
		strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble()*100.0, 0,'f', m_Precision[index.column()]);
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
}



void FoilTableDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
								  const QRect &, Qt::CheckState state) const
{
//	QItemDelegate::drawCheck(painter, option, checkRect, state);

	QFontMetrics fm(MainFrame::s_TextFont);
	int h23 = (int)((double)fm.height()*3./5.);//pixels
	int h3 = (int)((double)fm.height()/3.5);//pixels
	double h4 = (double)fm.height()/5.0;

	QPoint center = option.rect.center();

	QRect sR2 = QRect(center.x()-h23, center.y()-h23, 2*h23, 2*h23);
	QRect sR3 = QRect(center.x()-h3, center.y()-h3, 2*h3, 2*h3);

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(QApplication::palette().button());
	painter->setBackgroundMode(Qt::TransparentMode);

	QPen checkPen;

	QColor drawClr = QApplication::palette().color(QPalette::Button);
	QColor textClr = QApplication::palette().color(QPalette::ButtonText);

	checkPen.setColor(drawClr);
	checkPen.setWidth(1);
	painter->setPen(checkPen);

	painter->drawRoundedRect(sR2, h4, h4);

	checkPen.setColor(textClr);
	checkPen.setWidth(2);
	painter->setPen(checkPen);

	if(state==Qt::Checked)
	{
		painter->drawLine(sR3.left(), sR3.bottom(), sR3.right(), sR3.top());
		painter->drawLine(sR3.left(), sR3.top(),    sR3.right(), sR3.bottom());
	}
	else
	{

	}
	painter->restore();
}


void FoilTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if(index.column()==0)
	{
		QString strong = index.model()->data(index, Qt::EditRole).toString();
		QLineEdit *lineEdit = (QLineEdit*)editor;
		lineEdit->setText(strong);
	}
	else
	{
		double value = index.model()->data(index, Qt::EditRole).toDouble();
        DoubleEdit *pDE = static_cast<DoubleEdit*>(editor);
        pDE->SetValue(value);
	}
}


void FoilTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if(index.column()==0)
	{
		QString strong;
		QLineEdit *pLineEdit = static_cast<QLineEdit*>(editor);
		strong = pLineEdit->text();
		model->setData(index, strong, Qt::EditRole);
	}
	else
	{
        DoubleEdit *pDE = static_cast<DoubleEdit*>(editor);
        double value = pDE->Value()/100.0;
		model->setData(index, value, Qt::EditRole);
	}
}


void FoilTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

