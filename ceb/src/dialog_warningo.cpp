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

#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDesktopWidget>

#include "dialog_warningo.h"
#include "profile_manager.h"

DialogWarningo::DialogWarningo(const QString &title, const QString &message, QWidget *parent): QDialog(parent, 
	Qt::SplashScreen | Qt::WindowStaysOnTopHint)
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_Disabled, true);
	timer.setInterval(profile.warningoLifeTime);
	connect(&timer, SIGNAL(timeout()), this, SLOT(endOfTimer()));
	QLabel *label = new QLabel(message);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);	
	QGroupBox *groupBox = new QGroupBox(title);	
	QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);
	groupBoxLayout->addWidget(label);
	mainLayout->addWidget(groupBox);
	mainLayout->setMargin(0);
	adjustSize();
}

void DialogWarningo::endOfTimer()
{
	close();
}

void DialogWarningo::showEvent(QShowEvent * event)
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	QDesktopWidget desktopWidget;
	QRect desktopGeometry = desktopWidget.availableGeometry();
	switch(profile.warningoLocation)
	{
	case Profile::WarningoLocation_TopLeft:
		move(desktopGeometry.left(), desktopGeometry.top());
		break;
	case Profile::WarningoLocation_TopRight:
		move(desktopGeometry.right() - width(), desktopGeometry.top());
		break;
	case Profile::WarningoLocation_BottomLeft:
		move(0, desktopGeometry.bottom() - height());
		break;
	case Profile::WarningoLocation_BottomRight:
		move(desktopGeometry.right() - width(), desktopGeometry.bottom() - height());
		break;
	default:;
	}

	QDialog::showEvent(event);
	timer.start();
}
