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

#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = 0);
    virtual ~SearchWidget();

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
    QMenu *_menuOptions;
    QTextDocument::FindFlags _findFlags;

private slots:
    void lineEditTextChanged(const QString &text);
    void searchForPrevious();
    void searchForNext();
    void caseSensitiveActionTriggered(bool checked);
    void completeWordsActionTriggered(bool checked);
};

#endif
