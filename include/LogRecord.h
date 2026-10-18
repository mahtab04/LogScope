#if !defined(LOGRECORD_H)
#define LOGRECORD_H

#include <QString>
#include <QDateTime>

struct LogRecord {
    QDateTime timestamp;
    QString level;
    QString source;
    QString message;
};

#endif // !defined(LOGRECORD_H)
