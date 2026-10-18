#if !defined(LOGPARSER_H)
#define LOGPARSER_H

#include <QString>
#include <optional>
#include "LogRecord.h"

class LogParser {
public:
    std::optional<LogRecord> parseJsonLine(const QString& line) const;
};

#endif // !defined(LOGPARSER_H)