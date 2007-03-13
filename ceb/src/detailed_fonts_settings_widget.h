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

#ifndef DETAILED_FONTS_SETTINGS_WIDGET_H
#define DETAILED_FONTS_SETTINGS_WIDGET_H

#include "my_textedit.h"
#include "settings_widget.h"
#include "mtp_token.h"
#include "mtp_analyzer.h"
#include "ui_detailed_fonts_settings.h"

class DetailedFontsSettingsWidget : public SettingsWidget, private Ui::DetailedFontsSettings
{
	Q_OBJECT

public:
	DetailedFontsSettingsWidget(QWidget *parent = 0);

	void applyProfile(const Profile &) {}
	void feedProfile(Profile &) {}

private:
	MyTextEdit *m_textEdit;
	MtpAnalyzer m_analyzer;
	MtpToken m_currentToken;
	int m_currentArg;

	void refreshListItemsAfterGroupBoxToggle();

private slots:
	void on_listWidgetTokenTypes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_listWidgetTokenArgs_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_groupBoxForcedFont_toggled(bool b);
	void on_groupBoxForcedColor_toggled(bool b);
	void on_pushButtonFont_clicked();
	void on_pushButtonColor_clicked();
};

#endif
