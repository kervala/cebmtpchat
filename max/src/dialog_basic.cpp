/*
    This file is part of Max.

    Max is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    Max is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Max; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "common.h"
#include "dialog_basic.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>

DialogBasic::DialogBasic(QWidget *parent): QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	init(false);
}

DialogBasic::DialogBasic(bool isCloseWindow, QWidget *parent) : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	init(isCloseWindow);
}

void DialogBasic::checkValues()
{
    accept();
}

void DialogBasic::init(bool isCloseWindow)
{
	mainLayout = new QVBoxLayout(this);

	QFrame *line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	mainLayout->addWidget(line);

	// Bottom
	bottomLayout = new QHBoxLayout();
	mainLayout->addLayout(bottomLayout);

	// Ok button
	bOk = new QPushButton(tr("&Ok"), this);
	QSizePolicy policy = bOk->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Minimum);
	policy.setVerticalPolicy(QSizePolicy::Fixed);
	bOk->setSizePolicy(policy);

	if (isCloseWindow)
		bOk->setText(tr("&Close"));
	else
	{
		// Cancel button
		bCancel = new QPushButton(tr("&Cancel"), this);
		policy = bCancel->sizePolicy();
		policy.setHorizontalPolicy(QSizePolicy::Minimum);
		policy.setVerticalPolicy(QSizePolicy::Fixed);
		bCancel->setSizePolicy(policy);
	}

	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding,
										  QSizePolicy::Fixed);
	bottomLayout->addItem(spacer);
	bottomLayout->addWidget(bOk);
	if (!isCloseWindow)
		bottomLayout->addWidget(bCancel);

	resize(500, 350);
	mainLayout->setMargin(2);

	connect(bOk, SIGNAL(clicked()), this, SLOT(checkValues()));
	if (!isCloseWindow)
		connect(bCancel, SIGNAL(clicked()), this, SLOT(reject()));
}
