#ifndef DIALOG_SHORTCUT_H
#define DIALOG_SHORTCUT_H

#include <dialog_basic.h>

class DialogShortcut : public DialogBasic
{
	Q_OBJECT

public:
	DialogShortcut(QWidget *parent = 0) : DialogBasic(parent) {}
};

#endif // DIALOG_SHORTCUT_H
