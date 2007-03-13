/*
    This file is part of Max.

    Max is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    Max is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Max; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
    
#ifndef DIALOG_BASIC_H
#define DIALOG_BASIC_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class DialogBasic : public QDialog
{
    Q_OBJECT

protected:
    QVBoxLayout *mainLayout;
    QHBoxLayout *bottomLayout;
    QPushButton *bOk;
    QPushButton *bCancel;

public:
    DialogBasic(QWidget *parent = 0);
	DialogBasic(bool isCloseWindow, QWidget *parent = 0);

protected slots:
    virtual void checkValues();

private:
	void init(bool isCloseWindow);
};

#endif // DIALOG_BASIC_H
