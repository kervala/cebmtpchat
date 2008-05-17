#include <QHBoxLayout>

#include "search_widget.h"

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent),
                                              _textWidget(0)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QPushButton *closeButton = new QPushButton;
    mainLayout->addWidget(closeButton);
    closeButton->setIcon(QIcon(":/images/clear.png"));
    QSize size = closeButton->maximumSize();
    size.setWidth(32);
    closeButton->setMaximumSize(size);
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(hideMe()));

    _lineEditSearch = new QLineEdit;
    mainLayout->addWidget(_lineEditSearch);

    connect(_lineEditSearch, SIGNAL(textChanged(const QString &)),
            this, SLOT(lineEditTextChanged(const QString &)));

    _previousButton = new QPushButton(tr("&Previous"));
    mainLayout->addWidget(_previousButton);
    _previousButton->setIcon(QIcon(":/images/up.png"));
    connect(_previousButton, SIGNAL(clicked()), this, SLOT(searchForPrevious()));

    _nextButton = new QPushButton(tr("&Next"));
    mainLayout->addWidget(_nextButton);
    _nextButton->setIcon(QIcon(":/images/down.png"));
    connect(_nextButton, SIGNAL(clicked()), this, SLOT(searchForNext()));

    _toolButtonOptions = new QToolButton;
    _toolButtonOptions->setText(tr("Options"));
    mainLayout->addWidget(_toolButtonOptions);
    _toolButtonOptions->setAutoRaise(true);
}

void SearchWidget::setTextWidget(QTextBrowser *textWidget)
{
    _textWidget = textWidget;
}

void SearchWidget::afterShow()
{
    _lineEditSearch->setFocus();
    _lineEditSearch->selectAll();
}

void SearchWidget::lineEditTextChanged(const QString &text)
{
    if (!_textWidget)
        return;

    QTextCursor cursor = _textWidget->textCursor();
    cursor.movePosition(QTextCursor::End);
    _textWidget->setTextCursor(cursor);
    if (text == "")
    {
        QPalette palette = QLineEdit().palette();
        _lineEditSearch->setPalette(palette);
    } else if (!_textWidget->find(text, QTextDocument::FindBackward))
    {
        QPalette palette = _lineEditSearch->palette();
        palette.setColor(QPalette::Base, Qt::red);
        _lineEditSearch->setPalette(palette);
    } else
    {
        QPalette palette = _lineEditSearch->palette();
        palette.setColor(QPalette::Base, Qt::green);
        _lineEditSearch->setPalette(palette);
    }
}

void SearchWidget::searchForPrevious()
{
    if (!_textWidget->find(_lineEditSearch->text(), QTextDocument::FindBackward))
    {
        QTextCursor cursor = _textWidget->textCursor();
        cursor.movePosition(QTextCursor::End);
        _textWidget->setTextCursor(cursor);
        _textWidget->find(_lineEditSearch->text(), QTextDocument::FindBackward);
    }
}

void SearchWidget::searchForNext()
{
    if (!_textWidget->find(_lineEditSearch->text()))
    {
        QTextCursor cursor = _textWidget->textCursor();
        cursor.movePosition(QTextCursor::Start);
        _textWidget->setTextCursor(cursor);
        _textWidget->find(_lineEditSearch->text());
    }
}
