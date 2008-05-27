#ifndef PROPERTY_H
#define PROPERTY_H

#include <QDomElement>
#include <QList>

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
    void init(int value);
    void init(bool value);

    const QString &name() const { return _name; }
    Type type() const { return _type;}
    const QString &strValue() const { return _strValue; }
    int intValue() const { return _intValue; }
    bool boolValue() const { return _boolValue; }

    bool isValid() const { return _name != ""; }

    void save(QDomElement &root) const;

    static QString typeToStr(Type type);
    static Type strToType(const QString &str);

private:
    QString _name;
    Type _type;
    QString _strValue;
    int _intValue;
    bool _boolValue;
};

class Properties : public QList<Property>
{
public:
    bool exists(const QString &propName) const;

    QString getValue(const QString &propName, const QString &defaultValue);
    int getValue(const QString &propName, int defaultValue);
    bool getValue(const QString &propName, bool defaultValue);

    void setValue(const QString &propName, const QString &value);
    void setValue(const QString &propName, int value);
    void setValue(const QString &propName, bool value);
};

#endif
