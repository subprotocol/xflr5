/****************************************************************************

	FoilTableDelegate Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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


#include <QtGui>
#include "FoilTableDelegate.h"
#include "AFoil.h"

void *FoilTableDelegate::s_pAFoil;


FoilTableDelegate::FoilTableDelegate(QObject *parent)
 : QItemDelegate(parent)
{
}


QWidget *FoilTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	if(index.column()==0)
	{
		QLineEdit *editor = new QLineEdit(parent);
		editor->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		return editor;
	}
	if(index.column()==12)
	{
/*		QCheckBox *editor = new QCheckBox(parent);
//		editor->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		return editor;*/
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


void FoilTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if(index.column()==0)
	{
		QString strong = index.model()->data(index, Qt::EditRole).toString();
		QLineEdit *lineEdit = (QLineEdit*)editor;
		lineEdit->setText(strong);
	}
	if(index.column()==12)
	{
/*		QVariant value = index.data(Qt::CheckStateRole);
		Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);

		QCheckBox *checkBox = (QCheckBox*)editor;
		checkBox->setChecked(state == Qt::Checked);
//qDebug() << "Setting editor data " << (state == Qt::Checked);*/
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
	else if(index.column()==12)
	{
/*
		QCheckBox *checkBox = (QCheckBox*)editor;
		if(checkBox->isChecked()) model->setData(index, Qt::Checked, Qt::CheckStateRole);
		else                      model->setData(index, Qt::Unchecked, Qt::CheckStateRole);
*/
	}
	else
	{
		FloatEdit *floatEdit = static_cast<FloatEdit*>(editor);
		double value = floatEdit->GetValue()/100.0;
		model->setData(index, value, Qt::EditRole);
	}
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
	else if(index.column()==12 )
	{
		QVariant value = index.data(Qt::CheckStateRole);
		int val = value.toInt();
		Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
//qDebug() << val << state << index.row();// <<Qt::Checked<<Qt::Unchecked;
		drawCheck(painter, myOption, myOption.rect, state);
		drawFocus(painter, myOption, myOption.rect);
	}
	else
	{
		myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
		strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble()*100.0,0,'f', m_Precision[index.column()]);
		drawDisplay(painter, myOption, myOption.rect, strong);
		drawFocus(painter, myOption, myOption.rect);
	}
}


void FoilTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}


void FoilTableDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &, Qt::CheckState state) const
{
//qDebug() << 	"drawCheck";
	const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

	QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
										  check(option, option.rect, Qt::Checked).size(),
										  QRect(option.rect.x() + textMargin, option.rect.y(),
												option.rect.width() - (textMargin * 2), option.rect.height()));
	QItemDelegate::drawCheck(painter, option, checkRect, state);
}


bool FoilTableDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
						 const QModelIndex &index)
{
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
		QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
											  check(option, option.rect, Qt::Checked).size(),
											  QRect(option.rect.x() + textMargin, option.rect.y(),
													option.rect.width() - (2 * textMargin), option.rect.height()));

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

	Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
						? Qt::Unchecked : Qt::Checked);
	return model->setData(index, state, Qt::CheckStateRole);
}
