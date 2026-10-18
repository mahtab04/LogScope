#include "LogFileReader.h"

bool LogFileReader::open(const QString& filename) {
    // A failed switch leaves the reader closed rather than reading the old file.
    close();
    if (filename.isEmpty()) {
        error_ = QStringLiteral("The file path is empty.");
        return false;
    }
    file_.setFileName(filename);
    if (!file_.open(QIODevice::ReadOnly)) {
        error_ = file_.errorString();
        return false;
    }
    return true;
}

void LogFileReader::close() {
    file_.close();
    pending_.clear();
    error_.clear();
}

bool LogFileReader::isOpen() const {
    return file_.isOpen();
}

QString LogFileReader::errorString() const {
    return error_;
}

bool LogFileReader::reset() {
    error_.clear();
    if (!isOpen()) {
        error_ = QStringLiteral("No log file is open.");
        return false;
    }
    if (!file_.seek(0)) {
        error_ = file_.errorString();
        return false;
    }
    pending_.clear();
    return true;
}

QStringList LogFileReader::readNewLines() {
    error_.clear();
    if (!isOpen()) {
        error_ = QStringLiteral("No log file is open.");
        return {};
    }
    // Handle a file truncated below our current byte position.
    if (file_.size() < file_.pos() && !reset()) {
        return {};
    }

    const auto bytes = file_.readAll();
    if (file_.error() != QFileDevice::NoError) {
        error_ = file_.errorString();
    }
    pending_.append(bytes);

    QStringList lines;
    qsizetype start = 0;
    qsizetype end;
    while ((end = pending_.indexOf('\n', start)) != -1) {
        auto length = end - start;
        if (length > 0 && pending_.at(end - 1) == '\r') {
            --length;
        }
        lines.append(QString::fromUtf8(pending_.constData() + start, length));
        start = end + 1;
    }
    pending_.remove(0, start);
    return lines;
}
