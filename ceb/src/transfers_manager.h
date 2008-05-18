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

#ifndef TRANSFERS_MANAGER_H
#define TRANSFERS_MANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QTcpServer>

#include "session.h"

class Transfer
{
    friend class TransfersManager;

public:
    enum Direction
    {
        Direction_In,
        Direction_Out
    };

    enum State
    {
        Initiated,
        Canceled,
        Refused,
        Accepted
    };

    Transfer(Session *session, int id, int peerId,
             Direction dir, const QString &nickName, const QString &fileName);

    void send(const QString &data);

    Session *session() const { return _session; }
    int id() const { return _id; } // Is uniq in all client transfers
    int peerId() const { return _peerId; } // Is uniq for the session itself but not for all client transfers
    const QString &peerKey() const { return _peerKey; } // A random key used to authentificate and associate peer connections
    void setPeerKey(const QString &value) { _peerKey = value; }
    State state() const { return _state; }
    const QString &nickName() const { return _nickName; }
    const QString &fileName() const { return _fileName; }
    QString baseFileName() const { return QFileInfo(_fileName).fileName(); }
    Direction dir() const { return _dir; }
    QString dirString() const;
    const QString &destinationDir() const { return _dstDirectory; }
    void setDstDirectory(const QString &path) { _dstDirectory = path; }

    bool finished() const;

private:
    Session *_session;
    int _id;
    int _peerId;
    QString _peerKey;
    State _state;
    QString _nickName;
    QString _fileName;
    Direction _dir;
    QString _dstDirectory;
};

class TransfersManager : public QObject
{
    Q_OBJECT

public:
    static TransfersManager &instance();
    static void free();
    static bool destroyed();

    Transfer *newTransfer(Session *session, const QString &nickName,
                          const QString &fileName, Transfer::Direction dir,
                          int peerId = -1);

    QList<Transfer*> &transfers() { return _transfers; }
    const QList<Transfer*> &tranfers() const { return _transfers; }

    QList<Transfer*> getTransfers(Session *session) const;
    Transfer *getTransferById(int id) const;
    Transfer *getTransferByPeerId(Session *session, int id) const;

    static bool isCommand(const QString &str);
    static bool isProposeCommand(const QString &str, QString &fileName);

    void propose(Session *session, const QString &nickName, const QString &fileName);
    void cancel(Transfer *transfer);
    void refuse(Transfer *transfer);
    void accept(Transfer *transfer);
    void sendTransferKeys(Transfer *transfer);

    void cleanUpFinishedTransfers(Session *session);

signals:
    void newTransferAdded(Transfer *transfer);
    void transferStateChanged(Transfer *transfer);
    void transferRemoved(Transfer *transfer);

private:
    TransfersManager();
    ~TransfersManager();

    static TransfersManager *_instance;
    QList<Transfer*> _transfers;
    QTcpServer _tcpServer;

    QStringList getCommandArguments(const QString &data);
    int getUniqId() const;

private slots:
    void newSessionToken(Session *session, const TokenEvent &event);
    void newConnection();
};

#endif
