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

#include "profile.h"
#include "token_info.h"

#include "detailed_fonts_settings_widget.h"

DetailedFontsSettingsWidget::DetailedFontsSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
    setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(frameTextEdit);
    layout->setMargin(0);
    layout->addWidget(_textEdit = new MyTextEdit);

//    TokenFactory tokenFactory;

    // Fill nodes
    for (int i = 0; i < Token::End; ++i)
    {
        QListWidgetItem *newItem = new QListWidgetItem(TokenInfo::tokenToDisplayString((Token::Type) i));
        QFont font = newItem->font();
        if (Profile::instance().textSkin().isForcedToken((Token::Type) i))
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

    _currentTokenType = (Token::Type) listWidgetTokenTypes->row(current);
    labelTokenType->setText(current->text());
    listWidgetTokenArgs->clear();
    int capCount = _tokenFactory.tokenRegexp()[_currentTokenType].arguments().count();
    QListWidgetItem *newItem = new QListWidgetItem(tr("Entire line "));
    listWidgetTokenArgs->addItem(newItem);
    QFont font = newItem->font();
    if (Profile::instance().textSkin().isForcedToken(_currentTokenType, 0))
    {
        font.setBold(true);
        newItem->setFont(font);
    }
    for (int i = 0; i < capCount; ++i)
    {
        QListWidgetItem *newItem = new QListWidgetItem(tr("Argument %1").arg(i + 1));
        QFont font = newItem->font();
        if (Profile::instance().textSkin().isForcedToken(_currentTokenType, i + 1))
        {
            font.setBold(true);
            newItem->setFont(font);
        }
        listWidgetTokenArgs->addItem(newItem);
    }

    lineEditFont->setText(_tokenFactory.tokenRegexp()[_currentTokenType].example());

    // Focus the first
    listWidgetTokenArgs->setCurrentRow(0);
}

void DetailedFontsSettingsWidget::on_listWidgetTokenArgs_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if (!current)
        return;

    _currentArg = listWidgetTokenArgs->row(current);

    const TextSkin &textSkin = Profile::instance().textSkin();

    bool forcedFont = textSkin.isForcedFont(_currentTokenType, _currentArg);
    QFont font = textSkin.tokenFont(_currentTokenType, _currentArg);
    labelFont->setFont(font);
    labelFont->setText(font.family() + ", " + QString::number(font.pointSize()));
    groupBoxForcedFont->setChecked(forcedFont);

    bool forcedColor = textSkin.isForcedColor(_currentTokenType, _currentArg);
    QColor color = textSkin.tokenColor(_currentTokenType, _currentArg);
    QPalette palette = lineEditColor->palette();
    palette.setColor(QPalette::Base, color);
    lineEditColor->setPalette(palette);
    groupBoxForcedColor->setChecked(forcedColor);
}

void DetailedFontsSettingsWidget::on_groupBoxForcedFont_toggled(bool b)
{
    if (!b)
        Profile::instance().textSkin().resetTokenFont(_currentTokenType, _currentArg);
    else
        Profile::instance().textSkin().setTokenFont(_currentTokenType, _currentArg, labelFont->font());

    refreshListItemsAfterGroupBoxToggle();
}

void DetailedFontsSettingsWidget::on_groupBoxForcedColor_toggled(bool b)
{
    if (!b)
        Profile::instance().textSkin().resetTokenColor(_currentTokenType, _currentArg);
    else
    {
        QPalette palette = lineEditColor->palette();
        Profile::instance().textSkin().setTokenColor(_currentTokenType, _currentArg, palette.color(QPalette::Base));
    }

    refreshListItemsAfterGroupBoxToggle();
}

void DetailedFontsSettingsWidget::on_pushButtonFont_clicked()
{
    bool ok;

    QFont font = QFontDialog::getFont(&ok, labelFont->font(), this);
    if (ok)
    {
        labelFont->setFont(font);
        labelFont->setText(font.family() + ", " + QString::number(font.pointSize()));
        Profile::instance().textSkin().setTokenFont(_currentTokenType, _currentArg, font);
    }
}

void DetailedFontsSettingsWidget::on_pushButtonColor_clicked()
{
    QPalette palette = lineEditColor->palette();

    QColor color = QColorDialog::getColor(palette.color(QPalette::Base));
    if (color.isValid())
    {
        palette.setColor(QPalette::Base, color);
        lineEditColor->setPalette(palette);
        Profile::instance().textSkin().setTokenColor(_currentTokenType, _currentArg, color);
    }
}

void DetailedFontsSettingsWidget::refreshListItemsAfterGroupBoxToggle()
{
    QListWidgetItem *item = listWidgetTokenArgs->item(_currentArg);
    QFont font = item->font();
    font.setBold(Profile::instance().textSkin().isForcedToken(_currentTokenType, _currentArg));
    item->setFont(font);

    item = listWidgetTokenTypes->item(_currentTokenType);
    font = item->font();
    font.setBold(Profile::instance().textSkin().isForcedToken(_currentTokenType));
    item->setFont(font);
}
