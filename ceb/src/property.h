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

#ifndef PROPERTY_H
#define PROPERTY_H

class Property
{
public:
    enum Type {
        StringProperty,
        IntegerProperty,
        BooleanProperty
    };

    Property() : _type(StringProperty) {}
    Property(const QString &name, Type type) : _name(name), _type(type) {}
    Property(const QDomElement &root);

    void init(const QString &value);
    void init(long int value);
    void init(bool value);

    const QString &name() const { return _name; }
    Type type() const { return _type;}
    const QString &strValue() const { return _strValue; }
    long int intValue() const { return _intValue; }
    bool boolValue() const { return _boolValue; }

    bool isValid() const { return _name != ""; }

    void save(QDomElement &root) const;

    static QString typeToStr(Type type);
    static Type strToType(const QString &str);

private:
    QString _name;
    Type _type;
    QString _strValue;
    long int _intValue;
    bool _boolValue;
};

class Properties : public QList<Property>
{
public:
    bool exists(const QString &propName) const;

    QString getValue(const QString &propName, const QString &defaultValue);
    long int getValue(const QString &propName, long int defaultValue);
    bool getValue(const QString &propName, bool defaultValue);

    void setValue(const QString &propName, const QString &value);
    void setValue(const QString &propName, long int value);
    void setValue(const QString &propName, bool value);
};

#endif
