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

#ifndef TOKEN_DISPLAY_H
#define TOKEN_DISPLAY_H

#include "token_factory.h"

class TokenDisplayElement
{
public:
    TokenDisplayElement() : _font(0), _color(0) {}
    TokenDisplayElement(const TokenDisplayElement &source) : _font(0), _color(0) { operator=(source); }
    virtual ~TokenDisplayElement();

    QFont *font() const { return _font; }
    QColor *color() const { return _color; }

    bool isEmpty() const { return !_font && !_color; }

    void setFont(const QFont &font);
    void resetFont();
    void setColor(const QColor &color);
    void resetColor();

    const TokenDisplayElement &operator=(const TokenDisplayElement &source);

private:
    QFont *_font;
    QColor *_color;
};

class TokenDisplay
{
public:
    TokenDisplay() : _tokenType(Token::Unknown) {}
    TokenDisplay(Token::Type tokenType);

    void setTokenType(Token::Type tokenType);

    TokenDisplayElement allDisplayElement() const { return _elements[0]; }
    void setAllDisplayElement(const TokenDisplayElement &value) { _elements[0] = value; }

    QList<TokenDisplayElement> &elements() { return _elements; }
    const QList<TokenDisplayElement> &elements() const { return _elements; }

    bool isEmpty() const;

private:
    static const TokenFactory _tokenFactory;

    Token::Type _tokenType;
    QList<TokenDisplayElement> _elements;
};

class TextSkin
{
public:
    TextSkin() : _isDefaultSkin(false),
                 _backgroundColor(0),
                 _awayBackgroundColor(0) { init(); }
    TextSkin(const TextSkin &source) { operator=(source); }
    virtual ~TextSkin();

    static const TextSkin &defaultSkin() { return *_defaultSkin; }

    static void createDefaultSkin();
    static void freeDefaultSkin();

    void clear(); // Reset to defaultSkin
    void load(const QDomElement &root);
    void save(QDomElement &root) const;

    // General parameters
    class MtpFont
    {
    public:
        MtpFont() : _defaultFont(0), _font(0), _color(0) {}
        virtual ~MtpFont() { if (_color) delete _color; if (_font) delete _font; }
        MtpFont(const MtpFont &source) { operator=(source); }

        void setDefaultFont(const MtpFont *value) { _defaultFont = value; }

        bool isForcedFont() const { return _font; }
        QFont font() const { if (_font) return *_font; else return *(_defaultFont->_font); }
        void setFont(const QFont &value) { if (!_font) _font = new QFont;	*_font = value; }
        void resetFont() { if (!_font) return;	delete _font; _font = 0; }

        bool isForcedColor() const { return _color; }
        QColor color() const { if (_color) return *_color; else return *(_defaultFont->_color); }
        void setColor(const QColor &value) { if (!_color) _color = new QColor; *_color = value; }
        void resetColor() { if (!_color) return; delete _color; _color = 0; }

        const MtpFont &operator=(const MtpFont &source)
            {
                if (source._defaultFont)
                    _defaultFont = source._defaultFont;
                resetFont();
                if (source._font)
                    _font = new QFont(*source._font);
                resetColor();
                if (source._color)
                    _color = new QColor(*source._color);
                return *this;
            }

    private:
        const MtpFont *_defaultFont;
        QFont *_font;
        QColor *_color;
    };

    MtpFont &textFont() { return _textFont; }
    const MtpFont &textFont() const { return _textFont; }
    MtpFont &inputTextFont() { return _inputTextFont; }
    const MtpFont &inputTextFont() const { return _inputTextFont; }
    MtpFont &whoTextFont() { return _whoTextFont; }
    const MtpFont &whoTextFont() const { return _whoTextFont; }
    MtpFont &topicTextFont() { return _topicTextFont; }
    const MtpFont &topicTextFont() const { return _topicTextFont; }
    MtpFont &timeStampFont() { return _timeStampFont; }
    const MtpFont &timeStampFont() const { return _timeStampFont; }

    bool isForcedBackgroundColor() const { return _backgroundColor; }
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &value);
    void resetBackgroundColor();

    bool isForcedAwayBackgroundColor() const { return _awayBackgroundColor; }
    QColor awayBackgroundColor() const;
    void setAwayBackgroundColor(const QColor &value);
    void resetAwayBackgroundColor();

    // If <arg> is 0, the entire line is asked
    bool isForcedToken(Token::Type tokenType) const;
    bool isForcedToken(Token::Type tokenType, int arg) const;
    bool isForcedFont(Token::Type tokenType, int arg) const;
    bool isForcedColor(Token::Type tokenType, int arg) const;
    QFont tokenFont(Token::Type tokenType, int arg) const;
    QColor tokenColor(Token::Type tokenType, int arg) const;
    void setTokenFont(Token::Type tokenType, int arg, const QFont &font);
    void setTokenColor(Token::Type tokenType, int arg, const QColor &color);
    void resetTokenFont(Token::Type tokenType, int arg);
    void resetTokenColor(Token::Type tokenType, int arg);

    const TextSkin &operator=(const TextSkin &source);

private:
    TextSkin(bool isDefaultSkin) : _isDefaultSkin(isDefaultSkin),
                                   _backgroundColor(0),
                                   _awayBackgroundColor(0) { init(); }

    bool _isDefaultSkin;
    MtpFont _textFont;
    MtpFont _inputTextFont;
    MtpFont _whoTextFont;
    MtpFont _topicTextFont;
    MtpFont _timeStampFont;

    QColor *_backgroundColor;
    QColor *_awayBackgroundColor;

    QMap<Token::Type, TokenDisplay> _tokenDisplays;

    static TextSkin *_defaultSkin;

    void init();
    void writeMtpFont(QDomElement &root, const QString &arg, const MtpFont &font) const;
};

#endif
