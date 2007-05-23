#ifndef DIALOG_SHORTCUT_H
#define DIALOG_SHORTCUT_H

#include <QPushButton>

#include <dialog_basic.h>

class DialogShortcut : public DialogBasic
{
	Q_OBJECT

public:
	DialogShortcut(QWidget *parent = 0);

	void init(const QKeySequence &keySequence);

	const QKeySequence &keySequence() const { return _keySequence; }

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	QPushButton *_pushButtonChange;
	QKeySequence _keySequence;

	void refreshButton();
};

#endif // DIALOG_SHORTCUT_H
