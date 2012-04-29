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
#include "dialog_session_config.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

void DialogSessionConfig::init()
{
    _sessionConfigWidget = new SessionConfigWidget;

    mainLayout->insertWidget(0, _sessionConfigWidget);

    resize(300, 150);
}

DialogSessionConfig::DialogSessionConfig(const SessionConfig &config, QWidget *parent) : DialogBasic(parent)
{
    init();
    _sessionConfigWidget->init(config);
}

void DialogSessionConfig::checkValues()
{
    if (_sessionConfigWidget->check())
        accept();
}

void DialogSessionConfig::get(SessionConfig &config)
{
    _sessionConfigWidget->get(config);
}
