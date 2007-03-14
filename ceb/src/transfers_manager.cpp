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

#include <stdlib.h>

#include <QFileInfo>
#include <QRegExp>

#include "session_manager.h"
#include "transfers_manager.h"

Transfer::Transfer(Session *session, int id, int peerId,
				   Direction dir, const QString &nickName, const QString &fileName)
{
	srand(time(NULL));
	_session = session;
	_id = id;
	_peerId = peerId;
	_state = Initiated;
	_dir = dir;
	_nickName = nickName;
	_fileName = fileName;
}

void Transfer::send(const QString &data)
{
	_session->send(data);
}

QString Transfer::dirString() const
{
	if (_dir == Direction_In)
		return "in";
	else
		return "out";
}

bool Transfer::finished() const
{
	return _state == Canceled ||
		_state == Refused;
}

////////////////////////////////////////////////////////////

TransfersManager *TransfersManager::_instance = 0;

TransfersManager::TransfersManager() : QObject(0)
{
	connect(&SessionManager::instance(), SIGNAL(newSessionToken(Session *, const TokenEvent &)),
			this, SLOT(newSessionToken(Session *, const TokenEvent &)));

	_tcpServer.listen(QHostAddress::Any, 4001);
	connect(&_tcpServer, SIGNAL(newConnection()),
			this, SLOT(newConnection()));
}

TransfersManager::~TransfersManager()
{
	qDeleteAll(_transfers);
}

Transfer *TransfersManager::newTransfer(Session *session, const QString &nickName,
										const QString &fileName, Transfer::Direction dir,
										int peerId)
{
	Transfer *transfer = getTransferByPeerId(session, peerId);
	if (transfer)
	{
		// TODO warn the user that a transfer with the same peer id already exists
		return 0;
	}

	transfer = new Transfer(session, getUniqId(), peerId, dir, nickName, fileName);
	_transfers << transfer;
	emit newTransferAdded(transfer);
	return transfer;
}

int TransfersManager::getUniqId() const
{
	for (int i = 0; i < _transfers.count(); ++i)
	{
		bool good = true;
		foreach (Transfer *transfer, _transfers)
			if (transfer->id() == i)
			{
				good = false;
				break;
			}
		if (good)
			return i;
	}
	return _transfers.count();
}

bool TransfersManager::isCommand(const QString &str)
{
	QRegExp regExp("\\|file\\|(.*)");
	return regExp.exactMatch(str);
}

bool TransfersManager::isProposeCommand(const QString &str, QString &file)
{
	QRegExp regExp("\\|file\\|\\|propose\\|(\\d)+:(.*)");
	if (regExp.exactMatch(str))
	{
		file = regExp.cap(2);
		qDebug(qPrintable(file));
		return true;
	}
	return false;
}

void TransfersManager::propose(Session *session, const QString &nickName, const QString &fileName)
{
	// Create an entry in transfers
	Transfer *transfer = newTransfer(session, nickName, fileName, Transfer::Direction_Out);

	// Send a senddata to the peer
	transfer->send(QString("senddata %1 |file||propose|%2:%3").
				   arg(nickName).
				   arg(transfer->id()).
				   arg(transfer->baseFileName()));
}


void TransfersManager::cancel(Transfer *transfer)
{
	transfer->_state = Transfer::Canceled;
	emit transferStateChanged(transfer);

	// Send a senddata to the peer
	transfer->send(QString("senddata %1 |file||cancel_%2|%3").
				   arg(transfer->nickName()).
				   arg(transfer->dirString()).
				   arg(transfer->id()));
}

void TransfersManager::refuse(Transfer *transfer)
{
	transfer->_state = Transfer::Refused;
	emit transferStateChanged(transfer);

	// Send a senddata to the peer
	transfer->send(QString("senddata %1 |file||refuse|%2").
				   arg(transfer->nickName()).
				   arg(transfer->peerId()));
}

void TransfersManager::accept(Transfer *transfer)
{
	transfer->_state = Transfer::Accepted;
	emit transferStateChanged(transfer);

	// Send a senddata to the peer
	transfer->send(QString("senddata %1 |file||accept|%2").
				   arg(transfer->nickName()).
				   arg(transfer->peerId()));
}

void TransfersManager::sendTransferKeys(Transfer *transfer)
{
	transfer->setPeerKey(QString("%1").arg(rand()));
	transfer->send(QString("senddata %1 |file||server|%2:%3:%4:%5").
				   arg(transfer->nickName()).
				   arg(transfer->id()).
				   arg(transfer->session()->localAddress()).
				   arg(_tcpServer.serverPort()).
				   arg(transfer->peerKey()));
}

TransfersManager &TransfersManager::instance()
{
	if (!_instance)
		_instance = new TransfersManager;
	return *_instance;
}

void TransfersManager::free()
{
	if (_instance)
	{
		delete _instance;
		_instance = 0;
	}
}

bool TransfersManager::destroyed()
{
	return !_instance;
}

QList<Transfer*> TransfersManager::getTransfers(Session *session) const
{
	QList<Transfer*> transfers;

	foreach (Transfer *transfer, _transfers)
		if (transfer->session() == session)
			transfers << transfer;

	return transfers;
}

Transfer *TransfersManager::getTransferById(int id) const
{
	foreach (Transfer *transfer, _transfers)
		if (transfer->id() == id)
			return transfer;
	return 0;
}

Transfer *TransfersManager::getTransferByPeerId(Session *session, int peerId) const
{
	if (peerId < 0)
		return 0;
	foreach (Transfer *transfer, _transfers)
		if (transfer->session() == session && transfer->peerId() == peerId)
			return transfer;
	return 0;
}

void TransfersManager::newSessionToken(Session *session, const TokenEvent &event)
{
	if (event.token() != Token_Data)
		return;

	QString nickName = event.arguments()[1];
	QString data = event.arguments()[2];

	if (!isCommand(data))
		return;

	QStringList args = getCommandArguments(data);
	Q_ASSERT_X(args.count() > 0, "TransfersManager::newSessionToken", "getCommandArguments() return 0 arguments");

	if (args[0] == "propose") // Someone propose you a file
	{
		QRegExp regExp("^(\\d+):(.*)$"); // For instance, 123:toto.xml
		if (regExp.exactMatch(args[1]))
		{
			int peerId = regExp.cap(1).toInt();
			QString file = regExp.cap(2);

			newTransfer(session, nickName, file, Transfer::Direction_In, peerId);
		}
	}
	else if (args[0] == "cancel_in") // The peer canceled an outcomming transfer
	{
		Transfer *transfer = getTransferById(args[1].toInt());
		if (transfer)
		{
			transfer->_state = Transfer::Canceled;
			emit transferStateChanged(transfer);
		}
		else
		{
			// TODO warn the user that no file with those attributs exists
		}
	}
	else if (args[0] == "cancel_out") // The peer canceled an incomming transfer
	{
		Transfer *transfer = getTransferByPeerId(session, args[1].toInt());
		if (transfer)
		{
			transfer->_state = Transfer::Canceled;
			emit transferStateChanged(transfer);
		}
		else
		{
			// TODO warn the user that no file with those attributs exists
		}
	}
	else if (args[0] == "refuse") // The peer declines my proposition :-(
	{
		Transfer *transfer = getTransferById(args[1].toInt());
		if (transfer)
		{
			transfer->_state = Transfer::Refused;
			emit transferStateChanged(transfer);
		}
		else
		{
			// TODO warn the user that no file with those attributs exists
		}
	}
	else if (args[0] == "accept") // The peer accepts my proposition :-)
	{
		Transfer *transfer = getTransferById(args[1].toInt());
		if (transfer)
		{
			transfer->_state = Transfer::Accepted;
			emit transferStateChanged(transfer);

			// Send coordinates
			sendTransferKeys(transfer);
		}
		else
		{
			// TODO warn the user that no file with those attributs exists
		}
	}
}

QStringList TransfersManager::getCommandArguments(const QString &data)
{
	QRegExp regExp("^\\|file\\|\\|([a-z_]+)\\|(.*)$");

	if (regExp.exactMatch(data))
	{
		QStringList strLst = regExp.capturedTexts();
		strLst.removeAt(0);
		return strLst;
	}
	else
		return QStringList();
}

void TransfersManager::cleanUpFinishedTransfers(Session *session)
{
	foreach (Transfer *transfer, _transfers)
		if (transfer->session() == session && transfer->finished())
		{
			emit transferRemoved(transfer);
			_transfers.removeAt(_transfers.indexOf(transfer));
			delete transfer;
		}
}

void TransfersManager::newConnection()
{
	qDebug("New connection !");
	QTcpSocket *socket = _tcpServer.nextPendingConnection();
	socket->write("salut gars !\n");
}
