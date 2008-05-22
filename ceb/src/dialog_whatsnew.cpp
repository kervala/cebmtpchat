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

#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "dialog_whatsnew.h"

DialogWhatsNew::DialogWhatsNew(QWidget *parent) : DialogBasic(true, parent)
{
    _textBrowser = new QTextBrowser;
    mainLayout->insertWidget(0, _textBrowser);

    QDir dir(QApplication::applicationDirPath());
    QUrl url = QUrl::fromLocalFile(dir.absoluteFilePath("whatsnew.html"));
    _textBrowser->setSource(url);
    resize(400, 250);
}
