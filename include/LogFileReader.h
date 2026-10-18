#ifndef LOGFILEREADER_H
#define LOGFILEREADER_H

#include <QFile>
#include <QStringList>

class LogFileReader {
public:
    bool open(const QString& filename);
    void close();
    bool isOpen() const;
    QString errorString() const;

    // Return complete UTF-8 lines added since the previous call, without CR/LF.
    // An unfinished final line is retained until a newline is appended.
    QStringList readNewLines();
    bool reset();

private:
    QFile file_;
    QByteArray pending_;
    QString error_;
};

#endif // LOGFILEREADER_H
