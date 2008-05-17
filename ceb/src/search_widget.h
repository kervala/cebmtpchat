#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QToolButton>

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = 0);

    void setTextWidget(QTextBrowser *textWidget);

    void afterShow();

signals:
    void hideMe();

private:
    QLineEdit *_lineEditSearch;
    QPushButton *_previousButton;
    QPushButton *_nextButton;
    QTextBrowser *_textWidget;
    QToolButton *_toolButtonOptions;
    QColor _standardBaseColor;

private slots:
    void lineEditTextChanged(const QString &text);
    void searchForPrevious();
    void searchForNext();
};

#endif
