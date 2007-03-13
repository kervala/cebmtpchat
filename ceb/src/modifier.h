#ifndef MODIFIER_H
#define MODIFIER_H

#include <QList>

#include "session.h"
#include "mtp_token.h"
#include "render_segment.h"

void openModifiers();
void executeModifier(Session *session, MtpToken token, QList<RenderSegment> &segments);
void closeModifiers();

#endif
