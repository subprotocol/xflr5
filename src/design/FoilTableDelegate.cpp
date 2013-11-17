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
		FloatEdit *editor = new FloatEdit(parent);
		editor->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		editor->SetPrecision(m_Precision[index.column()]);
		return editor;
	}

	return NULL;
}


void FoilTableDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, 
								  const QRect &, Qt::CheckState state) const
{
	const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

/*	QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
										  check(option, option.rect, Qt::Checked).size(),
										  QRect(option.rect.x() + textMargin, option.rect.y(),
												option.rect.width() - (textMargin * 2), option.rect.height()));*/
	QRect checkRect = QStyle::alignedRect(option.direction,
										  Qt::AlignCenter,
										  option.rect.size(),
										  QRect(option.rect.x() + textMargin, option.rect.y(),
												option.rect.width() - (2 * textMargin), option.rect.height()));

	QItemDelegate::drawCheck(painter, option, checkRect, state);
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
/*		QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
											  check(option, option.rect, Qt::Checked).size(),
											  QRect(option.rect.x() + textMargin, option.rect.y(),
													option.rect.width() - (2 * textMargin), option.rect.height()));*/
		QRect checkRect = QStyle::alignedRect(option.direction,
											  Qt::AlignCenter,
											  option.rect.size(),
											  QRect(option.rect.x() + textMargin, option.rect.y(),
													option.rect.width() - (2 * textMargin), option.rect.height()));

/*
QRect QStyle::alignedRect(Qt::LayoutDirection direction, Qt::Alignment alignment, const QSize & size, const QRect & rectangle) [static]
Returns a new rectangle of the specified size that is aligned to the given rectangle according to the specified alignment and direction.*/

		if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos())) return false;
	}
	else if (event->type() == QEvent::KeyPress)
	{
		if (   static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
			&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
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
//		Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
		Qt::CheckState state;
		if(value.toInt()==0)  state = Qt::Unchecked;
		else                  state = Qt::Checked;
		drawCheck(painter,myOption, myOption.rect, state);
		drawFocus(painter, myOption, myOption.rect);
	}
	else if(index.column()==15)
	{
		QColor color;
		int style, width;
		//get a link to the foil to get its style
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
		QColor ContourColor = Qt::gray;
		
		painter->setBrush(Qt::NoBrush);
		painter->setBackgroundMode(Qt::TransparentMode);
	
		QPen LinePen(color);
		LinePen.setStyle(::GetStyle(style));
		LinePen.setWidth(width);
		painter->setPen(LinePen);
		painter->drawLine(r.left()+5, r.top()+r.height()/2, r.right()-5, r.top()+r.height()/2);
	
		QPen ContourPen(ContourColor);
		painter->setPen(ContourPen);
		r.adjust(0,2,-1,-3);
		painter->drawRoundRect(r,5,40);
		
		drawFocus(painter, myOption, myOption.rect);
	}
	else
	{
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
		strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble()*100.0, 0,'f', m_Precision[index.column()]);
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
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
		FloatEdit *floatEdit = static_cast<FloatEdit*>(editor);
		floatEdit->SetValue(value);
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
		FloatEdit *floatEdit = static_cast<FloatEdit*>(editor);
		double value = floatEdit->Value()/100.0;
		model->setData(index, value, Qt::EditRole);
	}
}


void FoilTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

