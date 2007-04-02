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
	connect(_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
			this, SLOT(actionRowChanged(const QModelIndex &, const QModelIndex &)));

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
	ActionManager &actionManager = ProfileManager::instance().currentProfile()->actionManager;
	const Action &action = actionManager.actions()[current.row()];
	if (action.keySequence().isEmpty())
		pushButtonChange->setText(tr("None"));
	else
		pushButtonChange->setText(action.keySequence().toString());

	if (action.keySequence().isEmpty())
		radioButtonNone->setChecked(true);
	else if (action.keySequence() == action.defaultKeySequence())
		radioButtonDefault->setChecked(true);
	else
		radioButtonCustom->setChecked(true);

	labelDefault->setText(tr("Default key: %1").arg(action.defaultKeySequence().isEmpty() ? tr("None") : action.defaultKeySequence().toString()));
}

void ShortcutsSettingsWidget::on_pushButtonChange_clicked()
{
	qDebug("Lancement de la boite de raccourci");
}

void ShortcutsSettingsWidget::on_radioButtonNone_toggled(bool checked)
{
	if (checked)
	{
		QModelIndex index = treeView->selectionModel()->currentIndex();
		_model->setData(_model->index(index.row(), ActionsModel::columnShortcut), QKeySequence().toString());
	}
}

void ShortcutsSettingsWidget::on_radioButtonDefault_toggled(bool checked)
{
	if (checked)
	{
		ActionManager &actionManager = ProfileManager::instance().currentProfile()->actionManager;
		
		QModelIndex index = treeView->selectionModel()->currentIndex();
		const Action &action = actionManager.actions()[index.row()];
		_model->setData(_model->index(index.row(), ActionsModel::columnShortcut), action.defaultKeySequence().toString());
	}
}

void ShortcutsSettingsWidget::on_radioButtonCustom_toggled(bool checked)
{
	if (checked)
	{
		qDebug("Custom checked");
	}
}

void ShortcutsSettingsWidget::actionRowChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	if (topLeft.row() == treeView->selectionModel()->currentIndex().row())
	{
		ActionManager &actionManager = ProfileManager::instance().currentProfile()->actionManager;
		const Action &action = actionManager.actions()[topLeft.row()];
		if (action.keySequence().isEmpty())
			pushButtonChange->setText(tr("None"));
		else
			pushButtonChange->setText(action.keySequence().toString());
	}
}
