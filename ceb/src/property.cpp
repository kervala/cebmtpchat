#include <xml_handler.h>

#include "property.h"

Property::Property(const QDomElement &root)
{
    _name = root.attribute("name");
    _type = strToType(root.attribute("type"));
    switch (_type)
    {
    case IntegerProperty:
        _intValue = XmlHandler::read(root, "value", 0);
        break;
    case BooleanProperty:
        _boolValue = XmlHandler::read(root, "value", false);
        break;
    default: // string
        _strValue = XmlHandler::read(root, "value", "");
    }
}

QString Property::typeToStr(Type type)
{
    switch (type)
    {
    case StringProperty: return "string";
    case IntegerProperty: return "integer";
    case BooleanProperty: return "boolean";
    default: return "string";
    }
}

Property::Type Property::strToType(const QString &str)
{
    if (!str.compare("string"))
        return StringProperty;
    else if (!str.compare("integer"))
        return IntegerProperty;
    else if (!str.compare("boolean"))
        return BooleanProperty;
    else
        return StringProperty;
}

void Property::save(QDomElement &root) const
{
    root.setAttribute("name", _name);
    root.setAttribute("type", typeToStr(_type));
    switch (_type)
    {
    case IntegerProperty:
        XmlHandler::write(root, "value", _intValue);
        break;
    case BooleanProperty:
        XmlHandler::write(root, "value", _boolValue);
        break;
    default: // string
        XmlHandler::write(root, "value", _strValue);
    }
}

void Property::init(const QString &value)
{
    _type = StringProperty;
    _strValue = value;
}

void Property::init(int value)
{
    _type = IntegerProperty;
    _intValue = value;
}

void Property::init(bool value)
{
    _type = BooleanProperty;
    _boolValue = value;
}

/////////////////////////////////////////////

bool Properties::exists(const QString &propName) const
{
    foreach (const Property &property, *this)
        if (!property.name().compare(propName))
            return true;
    return false;
}

QString Properties::getValue(const QString &propName, const QString &defaultValue)
{
    foreach (const Property &property, *this)
        if (!property.name().compare(propName))
            return property.strValue();
    return defaultValue;
}

int Properties::getValue(const QString &propName, int defaultValue)
{
    foreach (const Property &property, *this)
        if (!property.name().compare(propName))
            return property.intValue();
    return defaultValue;
}

bool Properties::getValue(const QString &propName, bool defaultValue)
{
    foreach (const Property &property, *this)
        if (!property.name().compare(propName))
            return property.boolValue();
    return defaultValue;
}

void Properties::setValue(const QString &propName, const QString &value)
{
    for (QList<Property>::iterator it = this->begin(); it != this->end(); ++it)
    {
        Property &prop = *it;
        if (!prop.name().compare(propName))
        {
            prop.init(value);
            return;
        }
    }
    Property prop(propName, Property::StringProperty);
    prop.init(value);
    (*this) << prop;
}

void Properties::setValue(const QString &propName, int value)
{
    for (QList<Property>::iterator it = this->begin(); it != this->end(); ++it)
    {
        Property &prop = *it;
        if (!prop.name().compare(propName))
        {
            prop.init(value);
            return;
        }
    }
    Property prop(propName, Property::IntegerProperty);
    prop.init(value);
    (*this) << prop;
}

void Properties::setValue(const QString &propName, bool value)
{
    for (QList<Property>::iterator it = this->begin(); it != this->end(); ++it)
    {
        Property &prop = *it;
        if (!prop.name().compare(propName))
        {
            prop.init(value);
            return;
        }
    }
    Property prop(propName, Property::BooleanProperty);
    prop.init(value);
    (*this) << prop;
}

