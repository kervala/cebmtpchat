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

#include <QMessageBox>
#include <QVBoxLayout>
#include <QSpacerItem>

#include "session_config_widget.h"

SessionConfigWidget::SessionConfigWidget(QWidget *parent, bool showRemoveButton) : QWidget(parent)
{
	setupUi(this);

	// Remove button
	pushButtonRemove->setVisible(showRemoveButton);

	if (showRemoveButton)
		connect(pushButtonRemove, SIGNAL(clicked()),
				this, SIGNAL(removeMe()));

	lineEditName->setFocus();
}

void SessionConfigWidget::init(const SessionConfig &config)
{
	lineEditName->setText(config.name());
	lineEditAddress->setText(config.address());
	lineEditPort->setText(QString::number(config.port()));
	lineEditLogin->setText(config.login());
	lineEditPassword->setText(config.password());
	lineEditConfirmPassword->setText(config.password());
	lineEditDescription->setText(config.description());
	checkBoxAutoConnect->setChecked(config.autoconnect());
	checkBoxBackupServers->setChecked(config.manageBackupServers());
	checkBoxFurtiveMode->setChecked(config.furtiveMode());

	m_oldName = config.name();
	lineEditName->setFocus();
}

bool SessionConfigWidget::check()
{
	// Name
	if (lineEditName->text().length() == 0)
	{
		lineEditName->setFocus();
		QMessageBox::critical(0, tr("Error"), tr("Empty name is forbidden"));
		return false;
	}

	// Address
	if (lineEditAddress->text().length() == 0)
	{
		lineEditAddress->setFocus();
		QMessageBox::critical(0, tr("Error"), tr("Empty address is forbidden"));
		return false;
	}

	// Port
	if (lineEditPort->text().length() == 0)
	{
		lineEditPort->setFocus();
		QMessageBox::critical(0, tr("Error"), tr("A port number must be specified"));
		return false;
	}

	// Password
	if (!checkBoxFurtiveMode->isChecked() &&
		lineEditPassword->text() != lineEditConfirmPassword->text())
	{
		lineEditPort->setFocus();
		QMessageBox::critical(0, tr("Error"), tr("Confirmation password is not equal to password"));
		return false;
	}

	return true;
}

void SessionConfigWidget::get(SessionConfig &config)
{
	// Get values
	config.setName(lineEditName->text());
	config.setAddress(lineEditAddress->text());
	config.setPort(lineEditPort->text().toInt());
	if (!checkBoxFurtiveMode->isChecked())
	{
		config.setLogin(lineEditLogin->text());
		config.setPassword(lineEditPassword->text());
	}
	else
	{
		config.setLogin("");
		config.setPassword("");
	}
	config.setDescription(lineEditDescription->text());
	config.setFurtiveMode(checkBoxFurtiveMode->isChecked());
	config.setAutoconnect(checkBoxAutoConnect->isChecked());
	config.setManageBackupServers(checkBoxBackupServers->isChecked());
}

void SessionConfigWidget::get(SessionConfig &config, QString &oldName)
{
	get(config);
	oldName = m_oldName;
}

void SessionConfigWidget::on_checkBoxFurtiveMode_toggled(bool state)
{
	lineEditLogin->setEnabled(!state);
	lineEditPassword->setEnabled(!state);
	lineEditConfirmPassword->setEnabled(!state);
}
