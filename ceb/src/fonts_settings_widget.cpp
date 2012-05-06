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

#include "common.h"
#include "profile.h"
#include "fonts_settings_widget.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

FontsSettingsWidget::FontsSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
    setupUi(this);

    refreshListWidgetItems();
    fillFontWidgets(listWidget->currentRow());
}

void FontsSettingsWidget::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if (!current)
        return;

    fillFontWidgets(listWidget->row(current));
}

void FontsSettingsWidget::fillFontWidgets(int row)
{
    const TextSkin &textSkin = Profile::instance().textSkin();

    const TextSkin::MtpFont &mtpFont = getMtpFontByRow(row);

    QFont font = mtpFont.font();
    labelFont->setFont(font);
    labelFont->setText(font.family() + ", " + QString::number(font.pointSize()));

    checkBoxForceFont->setChecked(mtpFont.isForcedFont());

    QColor color = mtpFont.color();
    QPalette palette = lineEditForcedColor->palette();
    palette.setColor(QPalette::Base, color);
    lineEditForcedColor->setPalette(palette);
    checkBoxForceColor->setChecked(mtpFont.isForcedColor());

    // Colors
    color = textSkin.backgroundColor();
    palette = lineEditBackgroundColor->palette();
    palette.setColor(QPalette::Base, color);
    lineEditBackgroundColor->setPalette(palette);
    checkBoxForceBackgroundColor->setChecked(textSkin.isForcedBackgroundColor());

    color = textSkin.awayBackgroundColor();
    palette = lineEditAwayBackgroundColor->palette();
    palette.setColor(QPalette::Base, color);
    lineEditAwayBackgroundColor->setPalette(palette);
    checkBoxForceAwayBackgroundColor->setChecked(textSkin.isForcedAwayBackgroundColor());
}

void FontsSettingsWidget::on_checkBoxForceFont_toggled(bool b)
{
    TextSkin &textSkin = Profile::instance().textSkin();

    switch (listWidget->currentRow())
    {
    case outputRow:
        if (b)
            textSkin.textFont().setFont(labelFont->font());
        else
            textSkin.textFont().resetFont();
        break;
    case inputRow:
        if (b)
            textSkin.inputTextFont().setFont(labelFont->font());
        else
            textSkin.inputTextFont().resetFont();
        break;
    case whoRow:
        if (b)
            textSkin.whoTextFont().setFont(labelFont->font());
        else
            textSkin.whoTextFont().resetFont();
        break;
    case topicRow:
        if (b)
            textSkin.topicTextFont().setFont(labelFont->font());
        else
            textSkin.topicTextFont().resetFont();
        break;
    case timeStampRow:
        if (b)
            textSkin.timeStampFont().setFont(labelFont->font());
        else
            textSkin.timeStampFont().resetFont();
        break;
    default:;
    }

    refreshListWidgetItems();
}

void FontsSettingsWidget::on_checkBoxForceColor_toggled(bool b)
{
    TextSkin &textSkin = Profile::instance().textSkin();

    TextSkin::MtpFont *mtpFont;
    switch (listWidget->currentRow())
    {
    case outputRow: mtpFont = &textSkin.textFont(); break;
    case inputRow: mtpFont = &textSkin.inputTextFont(); break;
    case whoRow: mtpFont = &textSkin.whoTextFont(); break;
    case topicRow: mtpFont = &textSkin.topicTextFont(); break;
    case timeStampRow: mtpFont = &textSkin.timeStampFont(); break;
    default: return;
    }

    if (b)
    {
        QPalette palette = lineEditForcedColor->palette();
        QColor color = palette.color(QPalette::Base);
        mtpFont->setColor(color);
    }
    else
        mtpFont->resetColor();

    refreshListWidgetItems();
}

void FontsSettingsWidget::on_pushButtonForcedFont_clicked()
{
    TextSkin &textSkin = Profile::instance().textSkin();
    bool ok;

    QFont font = QFontDialog::getFont(&ok, labelFont->font(), this);
    if (ok)
    {
        labelFont->setFont(font);
        labelFont->setText(font.family() + ", " + QString::number(font.pointSize()));

        switch (listWidget->currentRow())
        {
        case outputRow: textSkin.textFont().setFont(font); break;
        case inputRow: textSkin.inputTextFont().setFont(font); break;
        case whoRow: textSkin.whoTextFont().setFont(font); break;
        case topicRow: textSkin.topicTextFont().setFont(font); break;
        case timeStampRow: textSkin.timeStampFont().setFont(font); break;
        default: return;
        }
    }
}

void FontsSettingsWidget::on_pushButtonForcedColor_clicked()
{
    TextSkin &textSkin = Profile::instance().textSkin();
    QPalette palette = lineEditForcedColor->palette();

    QColor color = QColorDialog::getColor(palette.color(QPalette::Base));
    if (color.isValid())
    {
        palette.setColor(QPalette::Base, color);
        lineEditForcedColor->setPalette(palette);
        switch (listWidget->currentRow())
        {
        case outputRow: textSkin.textFont().setColor(color); break;
        case inputRow: textSkin.inputTextFont().setColor(color); break;
        case whoRow: textSkin.whoTextFont().setColor(color); break;
        case topicRow: textSkin.topicTextFont().setColor(color); break;
        case timeStampRow: textSkin.timeStampFont().setColor(color); break;
        default: return;
        }
    }
}

void FontsSettingsWidget::on_checkBoxForceBackgroundColor_toggled(bool b)
{
    TextSkin &textSkin = Profile::instance().textSkin();
    if (b)
    {
        QPalette palette = lineEditBackgroundColor->palette();
        QColor color = palette.color(QPalette::Base);
        textSkin.setBackgroundColor(color);
    }
    else
        textSkin.resetBackgroundColor();
}

void FontsSettingsWidget::on_checkBoxForceAwayBackgroundColor_toggled(bool b)
{
    TextSkin &textSkin = Profile::instance().textSkin();
    if (b)
    {
        QPalette palette = lineEditAwayBackgroundColor->palette();
        QColor color = palette.color(QPalette::Base);
        textSkin.setAwayBackgroundColor(color);
    }
    else
        textSkin.resetAwayBackgroundColor();
}

void FontsSettingsWidget::on_pushButtonBackgroundColor_clicked()
{
    TextSkin &textSkin = Profile::instance().textSkin();
    QPalette palette = lineEditBackgroundColor->palette();

    QColor color = QColorDialog::getColor(palette.color(QPalette::Base));
    if (color.isValid())
    {
        palette.setColor(QPalette::Base, color);
        lineEditBackgroundColor->setPalette(palette);
        textSkin.setBackgroundColor(color);
    }
}

void FontsSettingsWidget::on_pushButtonAwayBackgroundColor_clicked()
{
    TextSkin &textSkin = Profile::instance().textSkin();
    QPalette palette = lineEditAwayBackgroundColor->palette();

    QColor color = QColorDialog::getColor(palette.color(QPalette::Base));
    if (color.isValid())
    {
        palette.setColor(QPalette::Base, color);
        lineEditAwayBackgroundColor->setPalette(palette);
        textSkin.setAwayBackgroundColor(color);
    }
}

void FontsSettingsWidget::refreshListWidgetItems()
{
    for (int row = 0; row < listWidget->count(); ++row)
    {
        QListWidgetItem *item = listWidget->item(row);
        const TextSkin::MtpFont &mtpFont = getMtpFontByRow(row);
        QFont itemFont = item->font();
        itemFont.setBold(mtpFont.isForcedFont() || mtpFont.isForcedColor());
        item->setFont(itemFont);
    }
}

TextSkin::MtpFont &FontsSettingsWidget::getMtpFontByRow(int row)
{
    TextSkin &textSkin = Profile::instance().textSkin();
    switch (row)
    {
    case outputRow: return textSkin.textFont();
    case inputRow: return textSkin.inputTextFont();
    case whoRow: return textSkin.whoTextFont();
    case topicRow: return textSkin.topicTextFont();
    case timeStampRow: return textSkin.timeStampFont();
    default: return textSkin.textFont();
    }
}
