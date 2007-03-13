#ifndef TOKEN_RENDERER_H
#define TOKEN_RENDERER_H

#include "session.h"
#include "my_textedit.h"
#include "token_event.h"

class TokenRenderer
{
public:
	TokenRenderer() : m_session(0), m_textEdit(0) {}
	TokenRenderer(MyTextEdit *textEdit) : m_session(0) { m_textEdit = textEdit; }

	void displayToken(const TokenEvent &event, bool timeStamp = false);

	MyTextEdit *textEdit() const { return m_textEdit; }
	void setTextEdit(MyTextEdit *value) { m_textEdit = value; }

	Session *session() const { return m_session; }
	void setSession(Session *session) { m_session = session; }

private:
	Session *m_session;
	MyTextEdit *m_textEdit;
};

#endif
