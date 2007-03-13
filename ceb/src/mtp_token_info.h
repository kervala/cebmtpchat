#ifndef MTP_TOKEN_INFO_H
#define MTP_TOKEN_INFO_H

#include <QString>

#include "mtp_token.h"

class MtpTokenInfo
{
public:
	static QString tokenToIDString(MtpToken token);
	static MtpToken IDStringToToken(const QString &ID);
	static QString tokenToDisplayString(MtpToken token);	
};

#endif
