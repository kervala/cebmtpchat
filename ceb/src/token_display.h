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

#include <QFont>
#include <QColor>
#include <QList>
#include <QMap>
#include <QDomElement>

#include "mtp_analyzer.h"

class TokenDisplayElement
{
public:
    TokenDisplayElement() : m_font(0), m_color(0) {}
    TokenDisplayElement(const TokenDisplayElement &source) : m_font(0), m_color(0) { operator=(source); }
    virtual ~TokenDisplayElement();

    QFont *font() const { return m_font; }
    QColor *color() const { return m_color; }

    bool isEmpty() const { return !m_font && !m_color; }

    void setFont(const QFont &font);
    void resetFont();
    void setColor(const QColor &color);
    void resetColor();

    const TokenDisplayElement &operator=(const TokenDisplayElement &source);

private:
    QFont *m_font;
    QColor *m_color;
};

class TokenDisplay
{
public:
    TokenDisplay() : _tokenType(Token::Unknown) {}
    TokenDisplay(Token::Type tokenType);

    void setTokenType(Token::Type tokenType);

    TokenDisplayElement allDisplayElement() const { return m_elements[0]; }
    void setAllDisplayElement(const TokenDisplayElement &value) { m_elements[0] = value; }

    QList<TokenDisplayElement> &elements() { return m_elements; }
    const QList<TokenDisplayElement> &elements() const { return m_elements; }

    bool isEmpty() const;

private:
    static const MtpAnalyzer m_analyzer;

    Token::Type _tokenType;
    QList<TokenDisplayElement> m_elements;
};

class TextSkin
{
public:
    TextSkin() : m_isDefaultSkin(false),
                 m_backgroundColor(0),
                 m_awayBackgroundColor(0) { init(); }
    TextSkin(const TextSkin &source) { operator=(source); }
    virtual ~TextSkin();

    static const TextSkin &defaultSkin() { return *m_defaultSkin; }

    static void createDefaultSkin();

    void clear(); // Reset to defaultSkin
    void load(const QDomElement &root);
    void save(QDomElement &root) const;

    // General parameters
    class MtpFont
    {
    public:
        MtpFont() : m_defaultFont(0), m_font(0), m_color(0) {}
        MtpFont(const MtpFont &source) { operator=(source); }

        void setDefaultFont(const MtpFont *value) { m_defaultFont = value; }

        bool isForcedFont() const { return m_font; }
        QFont font() const { if (m_font) return *m_font; else return *(m_defaultFont->m_font); }
        void setFont(const QFont &value) { if (!m_font) m_font = new QFont;	*m_font = value; }
        void resetFont() { if (!m_font) return;	delete m_font; m_font = 0; }

        bool isForcedColor() const { return m_color; }
        QColor color() const { if (m_color) return *m_color; else return *(m_defaultFont->m_color); }
        void setColor(const QColor &value) { if (!m_color) m_color = new QColor; *m_color = value; }
        void resetColor() { if (!m_color) return; delete m_color; m_color = 0; }

        const MtpFont &operator=(const MtpFont &source)
            {
                if (source.m_defaultFont)
                    m_defaultFont = source.m_defaultFont;
                resetFont();
                if (source.m_font)
                    m_font = new QFont(*source.m_font);
                resetColor();
                if (source.m_color)
                    m_color = new QColor(*source.m_color);
                return *this;
            }

    private:
        const MtpFont *m_defaultFont;
        QFont *m_font;
        QColor *m_color;
    };

    MtpFont &textFont() { return m_textFont; }
    const MtpFont &textFont() const { return m_textFont; }
    MtpFont &inputTextFont() { return m_inputTextFont; }
    const MtpFont &inputTextFont() const { return m_inputTextFont; }
    MtpFont &whoTextFont() { return m_whoTextFont; }
    const MtpFont &whoTextFont() const { return m_whoTextFont; }
    MtpFont &topicTextFont() { return m_topicTextFont; }
    const MtpFont &topicTextFont() const { return m_topicTextFont; }
    MtpFont &timeStampFont() { return m_timeStampFont; }
    const MtpFont &timeStampFont() const { return m_timeStampFont; }

    bool isForcedBackgroundColor() const { return m_backgroundColor; }
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &value);
    void resetBackgroundColor();

    bool isForcedAwayBackgroundColor() const { return m_awayBackgroundColor; }
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
    TextSkin(bool isDefaultSkin) : m_isDefaultSkin(isDefaultSkin),
                                   m_backgroundColor(0),
                                   m_awayBackgroundColor(0) { init(); }

    bool m_isDefaultSkin;
    MtpFont m_textFont;
    MtpFont m_inputTextFont;
    MtpFont m_whoTextFont;
    MtpFont m_topicTextFont;
    MtpFont m_timeStampFont;

    QColor *m_backgroundColor;
    QColor *m_awayBackgroundColor;

    QMap<Token::Type, TokenDisplay> m_tokenDisplays;

    static TextSkin *m_defaultSkin;

    void init();
    void writeMtpFont(QDomElement &root, const QString &arg, const MtpFont &font) const;
};

#endif
