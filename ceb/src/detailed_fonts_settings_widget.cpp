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

#include <QTreeWidgetItem>
#include <QFontDialog>
#include <QColorDialog>

#include "profile_manager.h"
#include "mtp_token_info.h"
#include "detailed_fonts_settings_widget.h"
#include "mtp_token_info.h"

DetailedFontsSettingsWidget::DetailedFontsSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
    setupUi(this);

    Profile &profile = *ProfileManager::instance().currentProfile();
    QVBoxLayout *layout = new QVBoxLayout(frameTextEdit);
    layout->setMargin(0);
    layout->addWidget(m_textEdit = new MyTextEdit);

    MtpAnalyzer analyzer;

    // Fill nodes
    for (int i = 0; i < Token_End; ++i)
    {
        QListWidgetItem *newItem = new QListWidgetItem(MtpTokenInfo::tokenToDisplayString((MtpToken) i));
        QFont font = newItem->font();
        if (profile.textSkin().isForcedToken((MtpToken) i))
        {
            font.setBold(true);
            newItem->setFont(font);
        }

        listWidgetTokenTypes->addItem(newItem);
    }

    listWidgetTokenTypes->setCurrentRow(0);
}

void DetailedFontsSettingsWidget::on_listWidgetTokenTypes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if (!current)
        return;

    Profile &profile = *ProfileManager::instance().currentProfile();
    m_currentToken = (MtpToken) listWidgetTokenTypes->row(current);
    labelTokenType->setText(current->text());
    listWidgetTokenArgs->clear();
    int capCount = m_analyzer.tokenRegexp()[m_currentToken].arguments().count();
    QListWidgetItem *newItem = new QListWidgetItem(tr("Entire line "));
    listWidgetTokenArgs->addItem(newItem);
    QFont font = newItem->font();
    if (profile.textSkin().isForcedToken(m_currentToken, 0))
    {
        font.setBold(true);
        newItem->setFont(font);
    }
    for (int i = 0; i < capCount; ++i)
    {
        QListWidgetItem *newItem = new QListWidgetItem(tr("Argument ") + QString::number(i + 1));
        QFont font = newItem->font();
        if (profile.textSkin().isForcedToken(m_currentToken, i + 1))
        {
            font.setBold(true);
            newItem->setFont(font);
        }
        listWidgetTokenArgs->addItem(newItem);
    }

    lineEditFont->setText(m_analyzer.tokenRegexp()[m_currentToken].example());

    // Focus the first
    listWidgetTokenArgs->setCurrentRow(0);
}

void DetailedFontsSettingsWidget::on_listWidgetTokenArgs_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if (!current)
        return;

    Profile &profile = *ProfileManager::instance().currentProfile();

    m_currentArg = listWidgetTokenArgs->row(current);

    const TextSkin &textSkin = profile.textSkin();

    bool forcedFont = textSkin.isForcedFont(m_currentToken, m_currentArg);
    QFont font = textSkin.tokenFont(m_currentToken, m_currentArg);
    labelFont->setFont(font);
    labelFont->setText(font.family() + ", " + QString::number(font.pointSize()));
    groupBoxForcedFont->setChecked(forcedFont);

    bool forcedColor = textSkin.isForcedColor(m_currentToken, m_currentArg);
    QColor color = textSkin.tokenColor(m_currentToken, m_currentArg);
    QPalette palette = lineEditColor->palette();
    palette.setColor(QPalette::Base, color);
    lineEditColor->setPalette(palette);
    groupBoxForcedColor->setChecked(forcedColor);
}

void DetailedFontsSettingsWidget::on_groupBoxForcedFont_toggled(bool b)
{
    Profile &profile = *ProfileManager::instance().currentProfile();
    if (!b)
        profile.textSkin().resetTokenFont(m_currentToken, m_currentArg);
    else
        profile.textSkin().setTokenFont(m_currentToken, m_currentArg, labelFont->font());

    refreshListItemsAfterGroupBoxToggle();
}

void DetailedFontsSettingsWidget::on_groupBoxForcedColor_toggled(bool b)
{
    Profile &profile = *ProfileManager::instance().currentProfile();
    if (!b)
        profile.textSkin().resetTokenColor(m_currentToken, m_currentArg);
    else
    {
        QPalette palette = lineEditColor->palette();
        profile.textSkin().setTokenColor(m_currentToken, m_currentArg, palette.color(QPalette::Base));
    }

    refreshListItemsAfterGroupBoxToggle();
}

void DetailedFontsSettingsWidget::on_pushButtonFont_clicked()
{
    Profile &profile = *ProfileManager::instance().currentProfile();
    bool ok;

    QFont font = QFontDialog::getFont(&ok, labelFont->font(), this);
    if (ok)
    {
        labelFont->setFont(font);
        labelFont->setText(font.family() + ", " + QString::number(font.pointSize()));
        profile.textSkin().setTokenFont(m_currentToken, m_currentArg, font);
    }
}

void DetailedFontsSettingsWidget::on_pushButtonColor_clicked()
{
    Profile &profile = *ProfileManager::instance().currentProfile();

    QPalette palette = lineEditColor->palette();

    QColor color = QColorDialog::getColor(palette.color(QPalette::Base));
    if (color.isValid())
    {
        palette.setColor(QPalette::Base, color);
        lineEditColor->setPalette(palette);
        profile.textSkin().setTokenColor(m_currentToken, m_currentArg, color);
    }
}

void DetailedFontsSettingsWidget::refreshListItemsAfterGroupBoxToggle()
{
    Profile &profile = *ProfileManager::instance().currentProfile();
    QListWidgetItem *item = listWidgetTokenArgs->item(m_currentArg);
    QFont font = item->font();
    font.setBold(profile.textSkin().isForcedToken(m_currentToken, m_currentArg));
    item->setFont(font);

    item = listWidgetTokenTypes->item(m_currentToken);
    font = item->font();
    font.setBold(profile.textSkin().isForcedToken(m_currentToken));
    item->setFont(font);
}
