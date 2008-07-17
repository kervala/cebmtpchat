#include "who_user.h"

QString WhoUser::propertyByName(const QString &name) const
{
    if (!name.compare("group", Qt::CaseInsensitive))
        return _group;
    else if (!name.compare("client", Qt::CaseInsensitive))
        return _client;
    else if (!name.compare("fromHost", Qt::CaseInsensitive))
        return _fromHost;
    else
        return "";
}

//////////////////////////////////////////////////

WhoUser WhoPopulation::userForLogin(const QString &login) const
{
    foreach (const WhoUser &user, _users)
        if (!user.login().compare(login, Qt::CaseInsensitive))
            return user;
    return WhoUser();
}

int WhoPopulation::userIndexForLogin(const QString &login) const
{
    for (int i = 0; i < _users.count(); ++i)
        if (!_users[i].login().compare(login, Qt::CaseInsensitive))
            return i;
    return -1;
}

void WhoPopulation::addUser(const WhoUser &user)
{
    _users << user;
    emit userAdded(_users.count() - 1);
    emit userAdded(user);
}

void WhoPopulation::changeUser(const QString &login, const WhoUser &user)
{
    int index = userIndexForLogin(login);
    if (index >= 0)
    {
        WhoUser &oldUser = _users[index];
        WhoUser old = oldUser;
        oldUser = user;
        emit userChanged(index);
        emit userChanged(old, user);
    }
}

void WhoPopulation::removeUser(const QString &login)
{
    int index = userIndexForLogin(login);
    if (index < 0)
        return;

    WhoUser user = _users[index];
    _users.removeAt(index);
    emit userRemoved(index);
    emit userRemoved(user);
}

void WhoPopulation::clear()
{
    _users.clear();
    emit populationReset();
}
