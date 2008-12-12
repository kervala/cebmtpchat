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

#include <QVBoxLayout>

#include "system_widget.h"

SystemWidget *SystemWidget::_instance = 0;

SystemWidget::SystemWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(2);

    _textEditMain = new QTextEdit;
    mainLayout->addWidget(_textEditMain);
    _textEditMain->setReadOnly(true);
}

SystemWidget::~SystemWidget()
{
    _instance = 0;
}

void SystemWidget::sendInfo(const QString &log)
{
    _textEditMain->setTextColor(QColor(0, 0, 200));
    _textEditMain->append(log);
}

void SystemWidget::sendError(const QString &log)
{
    _textEditMain->setTextColor(QColor(200, 0, 0));
    _textEditMain->append(log);
}
void SystemWidget::sendWarning(const QString &log)
{
    _textEditMain->setTextColor(Qt::magenta);
    _textEditMain->append(log);
}

void SystemWidget::sendSuccess(const QString &log)
{
    _textEditMain->setTextColor(QColor(0, 200, 0));
    _textEditMain->append(log);
}

void SystemWidget::info(const QString &log)
{
    instance()->sendInfo(log);
}

void SystemWidget::error(const QString &log)
{
    instance()->sendError(log);
}

void SystemWidget::warning(const QString &log)
{
    instance()->sendWarning(log);
}

void SystemWidget::success(const QString &log)
{
    instance()->sendSuccess(log);
}

SystemWidget *SystemWidget::instance()
{
    if (!_instance)
        _instance = new SystemWidget;

    return _instance;
}
