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

#include <QFileDialog>
#include <QHeaderView>

#include "profile_manager.h"
#include "shortcuts_settings_widget.h"

ShortcutsSettingsWidget::ShortcutsSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
	setupUi(this);

	treeView->setModel(_model = new ActionsModel(this));

	connect(treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
			this, SLOT(treeActionFocusedChanged(const QModelIndex &, const QModelIndex &)));
	treeView->header()->resizeSection(0, 295);

	// Focus the first line
	treeView->selectionModel()->setCurrentIndex(_model->index(0, 0),
												QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void ShortcutsSettingsWidget::applyProfile(const Profile &profile)
{
/*	if (profile.linksCustomBrowser.isEmpty())
		radioButtonDefault->setChecked(true);
	else
	{
		radioButtonForce->setChecked(true);
		lineEditCustomBrowser->setText(profile.linksCustomBrowser);
		}*/
}

void ShortcutsSettingsWidget::feedProfile(Profile &profile)
{
/*	if (radioButtonDefault->isChecked())
		profile.linksCustomBrowser = "";
	else
	profile.linksCustomBrowser = lineEditCustomBrowser->text();*/
}

void ShortcutsSettingsWidget::treeActionFocusedChanged(const QModelIndex &current, const QModelIndex &previous)
{	
	Profile &profile = *ProfileManager::instance().currentProfile();
	ActionManager &actionManager = profile.actionManager;
	const Action &action = actionManager.actions()[current.row()];
	pushButtonChange->setText(action.keySequence().toString());

	if (action.keySequence().isEmpty())
		radioButtonNone->setChecked(true);
	else if (action.keySequence() == action.defaultKeySequence())
		radioButtonDefault->setChecked(true);
	else
		radioButtonCustom->setChecked(true);
}
