/* This file is part of CeB.
 * Copyright (C) 2005  Guillaume Denry
 *
 * CeB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * CeB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CeB; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "dialog_about.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

DialogAbout::DialogAbout(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("About CeB");
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	// Label
	labelMain = new QLabel("<b>CeB</b> <i>Alpha</i> by Garou/Mtp<br>Le Client de chat en <b><span style=\"color: brown\">Bois</span></b> Massif<br>qui dechire le slip<br>tellement qu'il est multi-platforme sa race");
	mainLayout->addWidget(labelMain);
	labelMain->setAlignment(Qt::AlignCenter);

	// Separator
	QFrame *line = new QFrame;
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	mainLayout->addWidget(line);

	// Button
	QHBoxLayout *buttonLayout = new QHBoxLayout(0);

	buttonClose = new QPushButton("&OK");
	QSizePolicy policy = buttonClose->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Minimum);
	buttonClose->setSizePolicy(policy);
	connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));

	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
	buttonLayout->addItem(spacer);
	buttonLayout->addWidget(buttonClose);

	mainLayout->addLayout(buttonLayout);
	mainLayout->setMargin(4);

	resize(0, 100);
}
