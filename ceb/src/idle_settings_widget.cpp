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

#include "idle_settings_widget.h"

IdleSettingsWidget::IdleSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
	setupUi(this);
}

void IdleSettingsWidget::applyProfile(const Profile &profile)
{
	checkBoxAway->setChecked(profile.idleAway);
	spinBoxAway->setValue(profile.idleAwayTimeout);

	// Set bypass expressions list
	foreach (const QString &expr, profile.idleAwayBypassExpressions)
	  {
	    QListWidgetItem *item = new QListWidgetItem(expr, listWidgetExpressions);
	    
	    item->setFlags(item->flags() | Qt::ItemIsEditable);
	  }

	checkBoxQuit->setChecked(profile.idleQuit);
	spinBoxQuit->setValue(profile.idleQuitTimeout);
}

void IdleSettingsWidget::feedProfile(Profile &profile)
{
	profile.idleAway = checkBoxAway->isChecked();
	profile.idleAwayTimeout = spinBoxAway->value();
	
	// Get bypass expressions
	profile.idleAwayBypassExpressions.clear();
	for (int i = 0; i < listWidgetExpressions->count(); ++i)
	  profile.idleAwayBypassExpressions << listWidgetExpressions->item(i)->text();

	profile.idleQuit = checkBoxQuit->isChecked();
	profile.idleQuitTimeout = spinBoxQuit->value();
}

void IdleSettingsWidget::on_checkBoxAway_stateChanged(int state)
{
	spinBoxAway->setEnabled(state == Qt::Checked);
	labelAway->setEnabled(state == Qt::Checked);
}

void IdleSettingsWidget::on_checkBoxQuit_stateChanged(int state)
{
	spinBoxQuit->setEnabled(state == Qt::Checked);
	labelQuit->setEnabled(state == Qt::Checked);
}

void IdleSettingsWidget::on_pushButtonAddExpr_clicked()
{
	QListWidgetItem *item = new QListWidgetItem("", listWidgetExpressions);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	listWidgetExpressions->setCurrentItem(item);
	listWidgetExpressions->editItem(item);
}

void IdleSettingsWidget::on_pushButtonRemoveExpr_clicked()
{
	QListWidgetItem *item = listWidgetExpressions->currentItem();
	if (item)
	  delete item;
}
