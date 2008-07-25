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

#include "dialog_system.h"

DialogSystem *DialogSystem::_instance = 0;

DialogSystem::DialogSystem(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("System logs"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    firstShow = true;

    textEditMain = new QTextEdit;
    mainLayout->addWidget(textEditMain);
    textEditMain->setReadOnly(true);

    // Separator
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Hide button
    QHBoxLayout *buttonLayout = new QHBoxLayout(0);

    buttonHide = new QPushButton(tr("&Hide"));
    QSizePolicy policy = buttonHide->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    buttonHide->setSizePolicy(policy);
    connect(buttonHide, SIGNAL(clicked()), this, SLOT(close()));

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonLayout->addItem(spacer);
    buttonLayout->addWidget(buttonHide);

    mainLayout->addLayout(buttonLayout);
    mainLayout->setMargin(4);

    resize(400, 200);
}

DialogSystem::~DialogSystem()
{
    _instance = 0;
}

void DialogSystem::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (firstShow)
        firstShow = false;
    else
    {
        // Restore parameters
        resize(savedWidth, savedHeight);
        move(savedLeft, savedTop);
    }
}

void DialogSystem::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);

    // Save parameters
    savedWidth = width();
    savedHeight = height();
    savedLeft = x();
    savedTop = y();

    emit hideSystemDialog();
}

void DialogSystem::sendInfo(const QString &log)
{
    textEditMain->setTextColor(QColor(0, 0, 200));
    textEditMain->append(log);
}

void DialogSystem::sendError(const QString &log)
{
    textEditMain->setTextColor(QColor(200, 0, 0));
    textEditMain->append(log);
}
void DialogSystem::sendWarning(const QString &log)
{
    textEditMain->setTextColor(Qt::magenta);
    textEditMain->append(log);
}

void DialogSystem::sendSuccess(const QString &log)
{
    textEditMain->setTextColor(QColor(0, 200, 0));
    textEditMain->append(log);
}

void DialogSystem::info(const QString &log)
{
    if (_instance)
        _instance->sendInfo(log);
}

void DialogSystem::error(const QString &log)
{
    if (_instance)
        _instance->sendError(log);
}
void DialogSystem::warning(const QString &log)
{
    if (_instance)
        _instance->sendWarning(log);
}
void DialogSystem::success(const QString &log)
{
    if (_instance)
        _instance->sendSuccess(log);
}

void DialogSystem::init(QWidget *parent)
{
    if (!_instance)
        _instance = new DialogSystem(parent);
}

DialogSystem *DialogSystem::instance()
{
    return _instance;
}
