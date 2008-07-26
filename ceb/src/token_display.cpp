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

#include <xml_handler.h>

#include "token_factory.h"
#include "token_info.h"

#include "token_display.h"

TokenDisplayElement::~TokenDisplayElement()
{
    if (_font)
        delete _font;
    if (_color)
        delete _color;
}

const TokenDisplayElement &TokenDisplayElement::operator=(const TokenDisplayElement &source)
{
    // Font
    if (source._font)
    {
        if (_font)
            *_font = *source._font;
        else
            _font = new QFont(*source._font);
    }
    else
        resetFont();

    // Color
    if (source._color)
    {
        if (_color)
            *_color = *source._color;
        else
            _color = new QColor(*source._color);
    }
    else
        resetColor();

    return *this;
}

void TokenDisplayElement::setFont(const QFont &value)
{
    if (_font)
        *_font = value;
    else
        _font = new QFont(value);
}

void TokenDisplayElement::resetFont()
{
    if (_font)
    {
        delete _font;
        _font = 0;
    }
}

void TokenDisplayElement::setColor(const QColor &value)
{
    if (_color)
        *_color = value;
    else
        _color = new QColor(value);
}

void TokenDisplayElement::resetColor()
{
    if (_color)
    {
        delete _color;
        _color = 0;
    }
}

///////////////////////////////////////////////////////////

const TokenFactory TokenDisplay::_tokenFactory;

TokenDisplay::TokenDisplay(Token::Type tokenType)
{
    setTokenType(tokenType);
}

void TokenDisplay::setTokenType(Token::Type tokenType)
{
    if (_tokenType == tokenType)
        return;

    _tokenType = tokenType;

    _elements.clear();

    // All line
    _elements << TokenDisplayElement();

    // Other arguments
    const MtpRegExp &regExp = _tokenFactory.tokenRegexp()[_tokenType];
    for (int i = 0; i < regExp.arguments().count(); ++i)
        _elements << TokenDisplayElement();
}

bool TokenDisplay::isEmpty() const
{
    for (int i = 0; i < _elements.count(); ++i)
        if (!_elements[i].isEmpty())
            return false;
    return true;
}

///////////////////////////////////////////////////////////

TextSkin *TextSkin::_defaultSkin = 0;

void TextSkin::createDefaultSkin()
{
    if (!_defaultSkin)
        _defaultSkin = new TextSkin(true);
}

void TextSkin::init()
{
    if (_isDefaultSkin)
    {
        // General parameters
#ifdef Q_OS_WIN32
        QFont font("Courier New", 8);
        QFont timeStampFont("Courier New", 7);
#elif Q_OS_DARWIN
		QFont font("Monaco", 16);
		QFont timeStampFont("Monaco", 14);
#else
        QFont font("Bitstream Vera Sans Mono", 8);
		QFont timeStampFont("Bitstream Vera Sans Mono", 7);
#endif
        _textFont.setFont(font);
        _textFont.setColor(Qt::black);

        QFont defaultFont;
        QColor defaultColor = Qt::black;
        _inputTextFont.setFont(defaultFont);
        _inputTextFont.setColor(defaultColor);
        _whoTextFont.setFont(defaultFont);
        _whoTextFont.setColor(defaultColor);
        _topicTextFont.setFont(defaultFont);
        _topicTextFont.setColor(defaultColor);
        _timeStampFont.setFont(timeStampFont);
        _timeStampFont.setColor(QColor(50, 50, 50));

        _backgroundColor = new QColor(255, 255, 240);
        _awayBackgroundColor = new QColor(215, 215, 200);

        QColor systemColor(0, 0, 200);

        // Token displays
        setTokenColor(Token::IndicatedActiveServer, 0, systemColor);
        setTokenColor(Token::LoginAsked, 0, systemColor);
        setTokenColor(Token::InvalidLogin, 0, QColor(200, 0, 0));
        setTokenColor(Token::OnlyRegisteredUsers, 0, QColor(200, 0, 0));
        setTokenColor(Token::PasswordAsked, 0, systemColor);
        setTokenColor(Token::IncorrectPassword, 0, QColor(200, 0, 0));
        setTokenColor(Token::MtpSays, 0, systemColor);
        setTokenColor(Token::Date, 0, systemColor);
        setTokenColor(Token::Welcome, 0, systemColor);
        setTokenColor(Token::Topic, 0, systemColor);
        setTokenColor(Token::Topic, 1, QColor(0, 0, 230));
        defaultFont.setItalic(true);
        setTokenFont(Token::Topic, 2, defaultFont);
        setTokenColor(Token::YouSetTopic, 0, systemColor);
        setTokenColor(Token::SomeoneSetTopic, 0, systemColor);
        setTokenColor(Token::SomeoneTellsYou, 0, systemColor);
        setTokenColor(Token::YouTellToSomeone, 0, systemColor);
        setTokenColor(Token::SomeoneAsksYou, 0, systemColor);
        setTokenColor(Token::YouAskToSomeone, 0, systemColor);
        setTokenColor(Token::SomeoneReplies, 0, systemColor);
        setTokenColor(Token::YouReply, 0, systemColor);
        setTokenColor(Token::SomeoneBeepsYou, 0, systemColor);

        setTokenColor(Token::WallBegin, 0, systemColor);
        setTokenColor(Token::WallEnd, 0, systemColor);
        setTokenColor(Token::WallLine, 1, QColor(0, 100, 0));
        setTokenColor(Token::WallLine, 2, QColor(0, 100, 0));
        setTokenColor(Token::WallLine, 3, QColor(180, 0, 0));

        setTokenColor(Token::WhoBegin, 0, systemColor);
        setTokenColor(Token::WhoSeparator, 0, systemColor);
        setTokenColor(Token::WhoEnd, 0, systemColor);
        setTokenColor(Token::WhoEnd, 1, QColor(180, 0, 180));
        setTokenColor(Token::WhoEndNoUser, 0, systemColor);

        setTokenColor(Token::GroupsBegin, 0, systemColor);
        setTokenColor(Token::GroupsSeparator, 0, systemColor);
        setTokenColor(Token::GroupsEnd, 0, systemColor);
        setTokenColor(Token::GroupsEnd, 1, QColor(180, 0, 180));

        setTokenColor(Token::HistoryBegin, 0, systemColor);
        setTokenColor(Token::HistoryEnd, 0, systemColor);

        setTokenColor(Token::FingerEnd, 0, systemColor);
        setTokenColor(Token::SomeoneShouts, 0, systemColor);
        setTokenColor(Token::SomeoneShouts, 2, QColor(200, 0, 0));
        setTokenColor(Token::YouShout, 0, QColor(180, 0, 180));
        setTokenColor(Token::YouShout, 1, QColor(200, 0, 0));
        setTokenColor(Token::SomeoneAway, 0, systemColor);
        setTokenColor(Token::SomeoneBack, 0, systemColor);
        setTokenColor(Token::YouAway, 0, systemColor);
        setTokenColor(Token::YouBack, 0, systemColor);
        setTokenColor(Token::SystemAliases, 0, systemColor);
        setTokenColor(Token::UserAliases, 0, systemColor);
        setTokenColor(Token::AliasesEnd, 0, systemColor);
        setTokenColor(Token::NoUserAlias, 0, systemColor);
        setTokenColor(Token::MessageBegin, 0, systemColor);
        setTokenColor(Token::MessageLine, 1, QColor(200, 0, 0));
        setTokenColor(Token::MessageLine, 2, QColor(0, 100, 0));
        setTokenColor(Token::MessageLine, 3, QColor(200, 0, 0));
        setTokenColor(Token::MessageEnd, 0, systemColor);
        setTokenColor(Token::MessageEnd, 1, QColor(200, 0, 0));
        setTokenColor(Token::NoMessage, 0, systemColor);
        setTokenColor(Token::MessageReceived, 0, systemColor);
        setTokenColor(Token::AllMessagesCleared, 0, systemColor);
        setTokenColor(Token::MessageCleared, 0, systemColor);
        setTokenColor(Token::MessageCleared, 1, QColor(180, 0, 180));
        setTokenColor(Token::MessagesCleared, 0, systemColor);
        setTokenColor(Token::MessagesCleared, 1, QColor(180, 0, 180));
        setTokenColor(Token::MessagesCleared, 2, QColor(180, 0, 180));
        setTokenColor(Token::HelpBegin, 0, systemColor);
        setTokenColor(Token::HelpEndNormal, 0, systemColor);
        setTokenColor(Token::HelpEndNoHelp, 0, systemColor);
        setTokenColor(Token::UnknownUser, 0, QColor(200, 0, 0));
        setTokenColor(Token::UserLoginRenamed, 0, systemColor);
        setTokenColor(Token::YourLoginRenamed, 0, systemColor);
        setTokenColor(Token::SomeoneComesIn, 0, systemColor);
        setTokenColor(Token::SomeoneLeaves, 0, systemColor);
        setTokenColor(Token::SomeoneLeaves, 2, QColor(180, 0, 180));
        setTokenColor(Token::SomeoneDisconnects, 0, systemColor);
        setTokenColor(Token::SomeoneDisconnects, 2, QColor(180, 0, 180));
        setTokenColor(Token::SomeoneIsKicked, 0, systemColor);
        setTokenColor(Token::SomeoneIsKicked, 3, QColor(180, 0, 180));
        setTokenColor(Token::YouKickSomeone, 0, systemColor);
        setTokenColor(Token::YouKickSomeone, 2, QColor(180, 0, 180));
        setTokenColor(Token::YouLeave, 0, systemColor);
        setTokenColor(Token::YouAreKicked, 0, QColor(200, 0, 0));
        setTokenColor(Token::YouAreKicked, 2, QColor(180, 0, 180));
        setTokenColor(Token::YouJoinChannel, 0, systemColor);
        setTokenColor(Token::YouLeaveChannel, 0, systemColor);
        setTokenColor(Token::SomeoneJoinChannel, 0, systemColor);
        setTokenColor(Token::SomeoneFadesIntoTheShadows, 0, systemColor);
        setTokenColor(Token::SomeoneLeaveChannel, 0, systemColor);
        setTokenColor(Token::SomeoneAppearsFromTheShadows, 0, systemColor);
        setTokenColor(Token::SomeoneSays, 1, QColor(150, 0, 0));
        setTokenColor(Token::YourClientIs, 0, systemColor);
        setTokenColor(Token::YourClientIs, 1, QColor(180, 0, 180));
        setTokenColor(Token::SomeoneGroup, 0, systemColor);
        setTokenColor(Token::UnregisteredUser, 0, systemColor);

        setTokenColor(Token::Data, 0, QColor(100, 0, 0));
    }
    else
    {
        _textFont.setDefaultFont(&defaultSkin()._textFont);
        _inputTextFont.setDefaultFont(&defaultSkin()._inputTextFont);
        _whoTextFont.setDefaultFont(&defaultSkin()._whoTextFont);
        _topicTextFont.setDefaultFont(&defaultSkin()._topicTextFont);
        _timeStampFont.setDefaultFont(&defaultSkin()._timeStampFont);
    }
}

TextSkin::~TextSkin()
{
    if (_backgroundColor)
        delete _backgroundColor;
    if (_awayBackgroundColor)
        delete _awayBackgroundColor;
}

void TextSkin::clear()
{
    _textFont.resetFont();
    _textFont.resetColor();
    _inputTextFont.resetFont();
    _inputTextFont.resetColor();
    _whoTextFont.resetFont();
    _whoTextFont.resetColor();
    resetBackgroundColor();
    resetAwayBackgroundColor();

    _tokenDisplays.clear();
}

void TextSkin::load(const QDomElement &root)
{
    clear();

    QDomElement textSkinElem = root.firstChildElement("text_skin");
    if (textSkinElem.isNull())
        return;

    QDomElement mtpFontElem = textSkinElem.firstChildElement("mtp_font");
    while (!mtpFontElem.isNull())
    {
        QString type = mtpFontElem.attribute("type");

        QString fontStr = XmlHandler::read(mtpFontElem, "font", "");
        QString colorStr = XmlHandler::read(mtpFontElem, "color", "");

        MtpFont mtpFont;
        if (fontStr != "")
        {
            QFont font;
            font.fromString(fontStr);
            mtpFont.setFont(font);
        }

        if (colorStr != "")
        {
            QColor color;
            color.setNamedColor(colorStr);
            mtpFont.setColor(color);
        }
        if (type == "output")
            _textFont = mtpFont;
        else if (type == "input")
            _inputTextFont = mtpFont;
        else if (type == "who")
            _whoTextFont = mtpFont;
        else if (type == "topic")
            _topicTextFont = mtpFont;
        else if (type == "time_stamp")
            _timeStampFont = mtpFont;

        // To the next!
        mtpFontElem = mtpFontElem.nextSiblingElement("mtp_font");
    }

    // Background color
    resetBackgroundColor();
    QString colorStr = XmlHandler::read(textSkinElem, "background_color", "");
    if (colorStr != "")
    {
        _backgroundColor = new QColor;
        _backgroundColor->setNamedColor(colorStr);
    }

    // Background away color
    resetAwayBackgroundColor();
    colorStr = XmlHandler::read(textSkinElem, "away_background_color", "");
    if (colorStr != "")
    {
        _awayBackgroundColor = new QColor;
        _awayBackgroundColor->setNamedColor(colorStr);
    }

    // Token displays
    QDomElement tokenDisplaysElem = textSkinElem.firstChildElement("token_displays");
    if (tokenDisplaysElem.isNull())
        return;

    QDomElement tokenDisplayElem = tokenDisplaysElem.firstChildElement("token_display");
    while (!tokenDisplayElem.isNull())
    {
        // "token" attribute
        Token::Type tokenType = TokenInfo::IDStringToToken(tokenDisplayElem.attribute("token"));

        // Create a token display
        TokenDisplay tokenDisplay(tokenType);

        // Get all display elements
        QDomElement displayElementElem = tokenDisplayElem.firstChildElement("display_element");
        while (!displayElementElem.isNull())
        {
            TokenDisplayElement element;
            QString fontString = XmlHandler::read(displayElementElem, "font", "");
            if (fontString != "")
            {
                QFont font;
                font.fromString(fontString);
                element.setFont(font);
            }
            QString colorString = XmlHandler::read(displayElementElem, "color", "");
            if (colorString != "")
            {
                QColor color;
                color.setNamedColor(colorString);
                element.setColor(color);
            }

            // "arg" attribute
            QString arg = displayElementElem.attribute("arg");

            if (arg == "") // Entire line
                tokenDisplay.setAllDisplayElement(element);
            else // Argument
            {
                int argNum = arg.toInt();
                if (argNum < tokenDisplay.elements().count())
                {
                    QList<TokenDisplayElement> &l = tokenDisplay.elements();
                    l[argNum] = element;
                }
            }

            // To the next!
            displayElementElem = displayElementElem.nextSiblingElement("display_element");
        }
        _tokenDisplays[tokenType] = tokenDisplay;

        // To the next!
        tokenDisplayElem = tokenDisplayElem.nextSiblingElement("token_display");
    }
}

void TextSkin::save(QDomElement &root) const
{
    QDomDocument doc = root.ownerDocument();

    QDomElement textSkinElem = doc.createElement("text_skin");
    root.appendChild(textSkinElem);

    // General parameters
    writeMtpFont(textSkinElem, "output", _textFont);
    writeMtpFont(textSkinElem, "input", _inputTextFont);
    writeMtpFont(textSkinElem, "who", _whoTextFont);
    writeMtpFont(textSkinElem, "topic", _topicTextFont);
    writeMtpFont(textSkinElem, "time_stamp", _timeStampFont);

    if (_backgroundColor)
        XmlHandler::write(textSkinElem, "background_color", _backgroundColor->name());
    if (_awayBackgroundColor)
        XmlHandler::write(textSkinElem, "away_background_color", _awayBackgroundColor->name());

    // Token displays
    QDomElement tokenDisplaysElem = doc.createElement("token_displays");
    textSkinElem.appendChild(tokenDisplaysElem);

    foreach (Token::Type tokenType, _tokenDisplays.keys())
    {
        TokenDisplay tokenDisplay = _tokenDisplays[tokenType];
        QDomElement tokenDisplayElem = doc.createElement("token_display");
        tokenDisplaysElem.appendChild(tokenDisplayElem);
        tokenDisplayElem.setAttribute("token", TokenInfo::tokenToIDString(tokenType));

        // Main line
        if (!tokenDisplay.allDisplayElement().isEmpty())
        {
            QDomElement displayElementElem = doc.createElement("display_element");
            tokenDisplayElem.appendChild(displayElementElem);
            if (tokenDisplay.allDisplayElement().font())
                XmlHandler::write(displayElementElem, "font", tokenDisplay.allDisplayElement().font()->toString());
            if (tokenDisplay.allDisplayElement().color())
                XmlHandler::write(displayElementElem, "color", tokenDisplay.allDisplayElement().color()->name());
        }
        // Arguments
        for (int i = 1; i < tokenDisplay.elements().count(); ++i)
        {
            TokenDisplayElement elem = tokenDisplay.elements()[i];
            if (elem.isEmpty())
                continue;

            QDomElement displayElementElem = doc.createElement("display_element");
            tokenDisplayElem.appendChild(displayElementElem);
            displayElementElem.setAttribute("arg", QString::number(i));
            if (elem.font())
                XmlHandler::write(displayElementElem, "font", elem.font()->toString());
            if (elem.color())
                XmlHandler::write(displayElementElem, "color", elem.color()->name());
        }
    }
}

bool TextSkin::isForcedToken(Token::Type tokenType) const
{
    return _tokenDisplays.find(tokenType) != _tokenDisplays.end();
}

bool TextSkin::isForcedToken(Token::Type tokenType, int arg) const
{
    return isForcedFont(tokenType, arg) || isForcedColor(tokenType, arg);
}

bool TextSkin::isForcedFont(Token::Type tokenType, int arg) const
{
    if (_tokenDisplays.find(tokenType) == _tokenDisplays.end())
        return false;
    else
        return _tokenDisplays.find(tokenType).value().elements()[arg].font();
}

bool TextSkin::isForcedColor(Token::Type tokenType, int arg) const
{
    if (_tokenDisplays.find(tokenType) == _tokenDisplays.end())
        return false;
    else
        return _tokenDisplays[tokenType].elements()[arg].color();
}

QFont TextSkin::tokenFont(Token::Type tokenType, int arg) const
{
    // In itself
    if (_tokenDisplays.find(tokenType) != _tokenDisplays.end())
    {
        const TokenDisplay &tokenDisplay = _tokenDisplays.find(tokenType).value();

        const TokenDisplayElement &displayElement = tokenDisplay.elements()[arg];
        const TokenDisplayElement &allDisplayElement = tokenDisplay.elements()[0];

        if (displayElement.font())
            return *displayElement.font();
        else if (allDisplayElement.font())
            return *allDisplayElement.font();
    }

    // In default
    if (!_isDefaultSkin && defaultSkin()._tokenDisplays.find(tokenType) != defaultSkin()._tokenDisplays.end())
    {
        const TokenDisplay &tokenDisplay = defaultSkin()._tokenDisplays.find(tokenType).value();

        const TokenDisplayElement &displayElement = tokenDisplay.elements()[arg];
        const TokenDisplayElement &allDisplayElement = tokenDisplay.elements()[0];

        if (displayElement.font())
            return *displayElement.font();
        else if (allDisplayElement.font())
            return *allDisplayElement.font();
    }

    // No token font => general text font
    return _textFont.font();
}

QColor TextSkin::tokenColor(Token::Type tokenType, int arg) const
{
    // In itself
    if (_tokenDisplays.find(tokenType) != _tokenDisplays.end())
    {
        const TokenDisplay &tokenDisplay = _tokenDisplays.find(tokenType).value();

        const TokenDisplayElement &displayElement = tokenDisplay.elements()[arg];
        const TokenDisplayElement &allDisplayElement = tokenDisplay.elements()[0];

        if (displayElement.color())
            return *displayElement.color();
        else if (allDisplayElement.color())
            return *allDisplayElement.color();
    }

    // In default
    if (!_isDefaultSkin && defaultSkin()._tokenDisplays.find(tokenType) != defaultSkin()._tokenDisplays.end())
    {
        const TokenDisplay &tokenDisplay = defaultSkin()._tokenDisplays.find(tokenType).value();

        const TokenDisplayElement &displayElement = tokenDisplay.elements()[arg];
        const TokenDisplayElement &allDisplayElement = tokenDisplay.elements()[0];

        if (displayElement.color())
            return *displayElement.color();
        else if (allDisplayElement.color())
            return *allDisplayElement.color();
    }

    // No token color => general text color
    return _textFont.color();
}

void TextSkin::setTokenFont(Token::Type tokenType, int arg, const QFont &font)
{
    TokenDisplay &tokenDisplay = _tokenDisplays[tokenType];
    tokenDisplay.setTokenType(tokenType);
    tokenDisplay.elements()[arg].setFont(font);
}

void TextSkin::setTokenColor(Token::Type tokenType, int arg, const QColor &color)
{
    TokenDisplay &tokenDisplay = _tokenDisplays[tokenType];
    tokenDisplay.setTokenType(tokenType);
    tokenDisplay.elements()[arg].setColor(color);
}

void TextSkin::resetTokenFont(Token::Type tokenType, int arg)
{
    if (_tokenDisplays.find(tokenType) == _tokenDisplays.end())
        return;
    else
    {
        TokenDisplay tokenDisplay = _tokenDisplays[tokenType];
        TokenDisplayElement element = tokenDisplay.elements()[arg];
        element.resetFont();
        tokenDisplay.elements()[arg] = element;
        _tokenDisplays[tokenType] = tokenDisplay;

        if (tokenDisplay.isEmpty())
            _tokenDisplays.remove(tokenType);
    }
}

void TextSkin::resetTokenColor(Token::Type tokenType, int arg)
{
    if (_tokenDisplays.find(tokenType) == _tokenDisplays.end())
        return;
    else
    {
        TokenDisplay tokenDisplay = _tokenDisplays[tokenType];
        TokenDisplayElement element = tokenDisplay.elements()[arg];
        element.resetColor();
        tokenDisplay.elements()[arg] = element;
        _tokenDisplays[tokenType] = tokenDisplay;

        if (tokenDisplay.isEmpty())
            _tokenDisplays.remove(tokenType);
    }
}

QColor TextSkin::backgroundColor() const
{
    if (_backgroundColor)
        return *_backgroundColor;
    else
        return *defaultSkin()._backgroundColor;
}

void TextSkin::setBackgroundColor(const QColor &value)
{
    if (!_backgroundColor)
        _backgroundColor = new QColor;
    *_backgroundColor = value;
}

void TextSkin::resetBackgroundColor()
{
    if (!_backgroundColor)
        return;
    delete _backgroundColor;
    _backgroundColor = 0;
}

QColor TextSkin::awayBackgroundColor() const
{
    if (_awayBackgroundColor)
        return *_awayBackgroundColor;
    else
        return *defaultSkin()._awayBackgroundColor;
}

void TextSkin::setAwayBackgroundColor(const QColor &value)
{
    if (!_awayBackgroundColor)
        _awayBackgroundColor = new QColor;
    *_awayBackgroundColor = value;
}

void TextSkin::resetAwayBackgroundColor()
{
    if (!_awayBackgroundColor)
        return;
    delete _awayBackgroundColor;
    _awayBackgroundColor = 0;
}

const TextSkin &TextSkin::operator=(const TextSkin &source)
{
    _tokenDisplays = source._tokenDisplays;
    _textFont = source._textFont;
    _inputTextFont = source._inputTextFont;
    _whoTextFont = source._whoTextFont;
    _topicTextFont = source._topicTextFont;
    _timeStampFont = source._timeStampFont;

    resetBackgroundColor();
    if (source._backgroundColor)
        _backgroundColor = new QColor(*source._backgroundColor);
    resetAwayBackgroundColor();
    if (source._awayBackgroundColor)
        _awayBackgroundColor = new QColor(*source._awayBackgroundColor);

    return *this;
}

void TextSkin::writeMtpFont(QDomElement &root, const QString &arg, const MtpFont &font) const
{
    if (font.isForcedFont() || font.isForcedColor())
    {
        QDomElement mtpFontElem = root.ownerDocument().createElement("mtp_font");
        root.appendChild(mtpFontElem);
        mtpFontElem.setAttribute("type", arg);
        if (font.isForcedFont())
            XmlHandler::write(mtpFontElem, "font", font.font().toString());
        if (font.isForcedColor())
            XmlHandler::write(mtpFontElem, "color", font.color().name());
    }
}
