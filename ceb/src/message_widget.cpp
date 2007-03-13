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

#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>

#include <filter_widget.h>

#include "message_widget.h"
#include "profile_manager.h"

MessageWidget::MessageWidget(Session *session, QWidget *parent) : SessionWidget(session, parent)
{
	init();

	connect(m_session, SIGNAL(newToken(const TokenEvent&)),
		this, SLOT(newTokenFromSession(const TokenEvent&)));

	m_firstShow = true;
}

void MessageWidget::showEvent(QShowEvent *)
{
	if (m_firstShow)
	{
		m_firstShow = false;
		focusLastItem();
	}
}

void MessageWidget::init()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(2);

	splitterMain = new QSplitter;
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

	treeViewMessages = new QTreeView;

	messagesLayout->addWidget(treeViewMessages);
	m_sortModel = new GenericSortModel(this);
	m_sortModel->setSourceModel(messageModel = new MessageModel(m_session->myMessages()));
	treeViewMessages->setModel(m_sortModel);
	filterWidget->setTreeView(treeViewMessages);
	treeViewMessages->setRootIsDecorated(false);
	treeViewMessages->setAlternatingRowColors(true); 
	treeViewMessages->setSelectionMode(QAbstractItemView::ExtendedSelection);
	treeViewMessages->header()->setSortIndicatorShown(true);
	treeViewMessages->header()->setClickable(true);
	treeViewMessages->header()->setStretchLastSection(false);
	treeViewMessages->resizeColumnToContents(0);
	treeViewMessages->resizeColumnToContents(2);
	treeViewMessages->resizeColumnToContents(3);
	treeViewMessages->header()->resizeSection(1, 400);
	treeViewMessages->header()->setSortIndicator(0, Qt::DescendingOrder);
	m_sortModel->sort(0, Qt::DescendingOrder);	
	
	connect(treeViewMessages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
			this, SLOT(currentMessageChanged(const QModelIndex &, const QModelIndex &)));	

	QHBoxLayout *bottomLayout = new QHBoxLayout;
	bottomLayout->setMargin(0);
	messagesLayout->addLayout(bottomLayout);
	
	pushButtonRefresh = new QPushButton(tr("&Refresh"));
	connect(pushButtonRefresh, SIGNAL(clicked()), this, SLOT(refreshMessages()));
	bottomLayout->addWidget(pushButtonRefresh);	

	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding,
		QSizePolicy::Fixed);
	bottomLayout->addItem(spacer);

	pushButtonRemove = new QPushButton(tr("Re&move"));
	bottomLayout->addWidget(pushButtonRemove);
	connect(pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeSelectedMessage()));
	
	textEditMessage = new MyTextEdit;
	textEditMessage->setReadOnly(true);
	splitterMain->addWidget(textEditMessage);
	splitterMain->setCollapsible(0, false);
	splitterMain->setCollapsible(1, false);
	
	QList<int> list;
	list.append(0);
	list.append(50);
	splitterMain->setSizes(list);
}

void MessageWidget::currentMessageChanged(const QModelIndex & current, const QModelIndex &)
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	textEditMessage->clear();
	QModelIndex currentSource = m_sortModel->mapToSource(current);
	const MessageItem &message = messageModel->myMessages()[currentSource.row()];
	textEditMessage->addNewLine(message.message(), profile.textSkin().textFont().font(), QColor(0, 0, 0));
}

void MessageWidget::removeSelectedMessage()
{
	if (!m_session->isLogged())
		return;

	// Get all selected ID
	QModelIndexList list = treeViewMessages->selectionModel()->selectedIndexes();
	QList<int> indexes;
	foreach (const QModelIndex &index, list)
	{
		QModelIndex sourceIndex = m_sortModel->mapToSource(index);
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
				m_session->send("clearmsg " + QString::number(low + 1 - offset));
			else // Range
				m_session->send("clearmsg " + QString::number(low + 1 - offset) + " " +
					QString::number(pred + 1 - offset));			
			offset += pred - low + 1;
			low = indexes[i];
			pred = low;
		}
	}
	// Remove the old range
	if (pred == low) // Single
		m_session->send("clearmsg " + QString::number(low + 1 - offset));
	else // Range
		m_session->send("clearmsg " + QString::number(low + 1 - offset) + " " +
			QString::number(pred + 1 - offset));
}

void MessageWidget::newTokenFromSession(const TokenEvent &event)
{
	switch(event.token())
	{
	case Token_MessageEnd:
	case Token_NoMessage:
	case Token_MessageReceived:
	case Token_AllMessagesCleared:
	case Token_MessageCleared:
	case Token_MessagesCleared:
		messageModel->setMyMessages(m_session->myMessages());
		messageModel->refreshDatas();
		if (event.token() == Token_MessageEnd)
			focusLastItem();
		break;	
	default:;
	}
}

void MessageWidget::refreshMessages()
{
	if (!m_session->isLogged())
		return;
	
	m_session->requestTicket(MtpAnalyzer::Command_ShowMsg);
	m_session->send("showmsg");
}

QString MessageWidget::widgetCaption() const
{
	return "Messages";
}

void MessageWidget::focusLastItem()
{
	if (m_sortModel->rowCount())
	{
		QModelIndex lastIndex = m_sortModel->mapFromSource(messageModel->index(messageModel->rowCount() - 1, 0));
		
		treeViewMessages->selectionModel()->select(lastIndex, 
												   QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
		treeViewMessages->setCurrentIndex(lastIndex);
	}
}
