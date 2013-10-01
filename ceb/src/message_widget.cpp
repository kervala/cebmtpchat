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
#include <filter_widget.h>

#include "message_widget.h"
#include "profile.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

MessageWidget::MessageWidget(Session *session, QWidget *parent) : SessionWidget(session, parent)
{
    init();

    connect(_session, SIGNAL(newToken(const Token&)),
            this, SLOT(newTokenFromSession(const Token&)));

    _firstShow = true;
}

void MessageWidget::showEvent(QShowEvent *)
{
    if (_firstShow)
    {
        _firstShow = false;
        focusLastItem();
    }
}

void MessageWidget::init()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(2);

    QSplitter *splitterMain = new QSplitter;
    mainLayout->addWidget(splitterMain);
    splitterMain->setOrientation(Qt::Vertical);

    QWidget *widgetMessages = new QWidget;
    splitterMain->addWidget(widgetMessages);

    QSizePolicy sizePolicy = widgetMessages->sizePolicy();
    sizePolicy.setVerticalStretch(1);
    widgetMessages->setSizePolicy(sizePolicy);

    QVBoxLayout *messagesLayout = new QVBoxLayout(widgetMessages);
    messagesLayout->setMargin(0);

    // Filter widget
    FilterWidget *filterWidget = new FilterWidget;
    messagesLayout->addWidget(filterWidget);

    _treeViewMessages = new QTreeView;
    messagesLayout->addWidget(_treeViewMessages);
    _sortModel = new GenericSortModel(this);
    _sortModel->setSourceModel(_messageModel = new MessageModel(_session->myMessages()));
    _treeViewMessages->setModel(_sortModel);
    filterWidget->setTreeView(_treeViewMessages);
    _treeViewMessages->setRootIsDecorated(false);
    _treeViewMessages->setAlternatingRowColors(true);
    _treeViewMessages->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeViewMessages->header()->setSortIndicatorShown(true);
#ifndef USE_QT5
    _treeViewMessages->header()->setClickable(true);
#else
    _treeViewMessages->header()->setSectionsClickable(true);
#endif
    _treeViewMessages->header()->setStretchLastSection(false);
    _treeViewMessages->resizeColumnToContents(0);
    _treeViewMessages->resizeColumnToContents(2);
    _treeViewMessages->resizeColumnToContents(3);
    _treeViewMessages->header()->resizeSection(1, 400);
    _treeViewMessages->header()->setSortIndicator(0, Qt::DescendingOrder);
    _sortModel->sort(0, Qt::DescendingOrder);

    connect(_treeViewMessages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(currentMessageChanged(const QModelIndex &, const QModelIndex &)));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setMargin(0);
    messagesLayout->addLayout(bottomLayout);

    QPushButton *pushButtonRefresh = new QPushButton(tr("&Refresh"));
    connect(pushButtonRefresh, SIGNAL(clicked()), this, SLOT(refreshMessages()));
    bottomLayout->addWidget(pushButtonRefresh);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    bottomLayout->addItem(spacer);

    QPushButton *pushButtonRemove = new QPushButton(tr("Re&move"));
    pushButtonRemove->setIcon(QIcon(":/images/remove.png"));
    bottomLayout->addWidget(pushButtonRemove);
    connect(pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeSelectedMessage()));

    _textEditMessage = new UrlTextEdit;
    _textEditMessage->setReadOnly(true);
    splitterMain->addWidget(_textEditMessage);
    splitterMain->setCollapsible(0, false);
    splitterMain->setCollapsible(1, false);

    QList<int> list;
    list.append(0);
    list.append(50);
    splitterMain->setSizes(list);
}

void MessageWidget::currentMessageChanged(const QModelIndex & current, const QModelIndex &)
{
    if (!current.isValid())
        return;
    QModelIndex currentSource = _sortModel->mapToSource(current);
    const MessageItem &message = _messageModel->myMessages()[currentSource.row()];
//    _textEditMessage->addNewLine(message.message(), Profile::instance().textSkin().textFont().font(), QColor(0, 0, 0));
    _textEditMessage->setText(message.message());
}

void MessageWidget::removeSelectedMessage()
{
    if (!_session->isLogged())
        return;

    // Get all selected ID
    QModelIndexList list = _treeViewMessages->selectionModel()->selectedIndexes();
    QList<int> indexes;
    foreach (const QModelIndex &index, list)
    {
        QModelIndex sourceIndex = _sortModel->mapToSource(index);
        int i = sourceIndex.row();
        if (indexes.indexOf(i) < 0)
            indexes << i;
    }

    if (!indexes.count())
        return;

    qSort(indexes.begin(), indexes.end());

    int low = indexes[0];
    int pred = low;
    int offset = 0;
    for (int i = 1; i < indexes.count(); i++)
    {
        if (indexes[i] == pred + 1)
            pred++;
        else
        {
            // Remove the old range
            if (pred == low) // Single
                _session->sendCommand("clearmsg " + QString::number(low + 1 - offset));
            else // Range
                _session->sendCommand("clearmsg " + QString::number(low + 1 - offset) + " " +
                                      QString::number(pred + 1 - offset));
            offset += pred - low + 1;
            low = indexes[i];
            pred = low;
        }
    }
    // Remove the old range
    if (pred == low) // Single
        _session->sendCommand("clearmsg " + QString::number(low + 1 - offset));
    else // Range
        _session->sendCommand("clearmsg " + QString::number(low + 1 - offset) + " " +
                              QString::number(pred + 1 - offset));
}

void MessageWidget::newTokenFromSession(const Token &token)
{
    switch(token.type())
    {
    case Token::MessageEnd:
    case Token::NoMessage:
    case Token::MessageReceived:
    case Token::AllMessagesCleared:
    case Token::MessageCleared:
    case Token::MessagesCleared:
        _messageModel->setMyMessages(_session->myMessages());
        _messageModel->refreshDatas();
        if (token.type() == Token::MessageEnd)
            focusLastItem();
        break;
    default:;
    }
}

void MessageWidget::refreshMessages()
{
    if (!_session->isLogged())
        return;

    _session->requestTicket(TokenFactory::Command_ShowMsg);
    _session->sendCommand("showmsg");
}

QString MessageWidget::widgetCaption() const
{
    return "Messages";
}

void MessageWidget::focusLastItem()
{
    if (_sortModel->rowCount())
    {
        QModelIndex lastIndex = _sortModel->mapFromSource(_messageModel->index(_messageModel->rowCount() - 1, 0));

        _treeViewMessages->selectionModel()->select(lastIndex,
                                                    QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        _treeViewMessages->setCurrentIndex(lastIndex);
    }
}
