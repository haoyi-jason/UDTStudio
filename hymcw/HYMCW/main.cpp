#include "mainwindow.h"
#include <QApplication>
#include <QTextStream>
#include <QDir>

const QString logPath = "./Log";


void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "Debug"}, {QtInfoMsg, "Info"}, {QtWarningMsg, "Warning"}, {QtCriticalMsg, "Critical"}, {QtFatalMsg, "Fatal"}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
//    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QString dateString = QDateTime::currentDateTime().toString("yyyy-mm-dd");
    QString formattedTime = QDateTime::currentDateTime().toString("yyyy/mm/dd hh:mm:ss.zzz");
    //QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    //QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    QString path = QString("%1\\%2\\%3").arg(logPath,dateString,msgLevelHash[type]);
    if(!QDir(path).exists()){
        QDir().mkpath(path);
    }

    QString filePath = QString("%1\\%2\\%3\\log.txt").arg(logPath,dateString,msgLevelHash[type]);
    QString txt = QString("%1 %2: (%3)").arg(formattedTime, msg,  context.file);
    QFile outFile(filePath);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();



//    if (logToFile) {
//        QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
//        QFile outFile(logFilePath);
//        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//        QTextStream ts(&outFile);
//        ts << txt << endl;
//        outFile.close();
//    } else {
//        fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
//        fflush(stderr);
//    }

//    if (type == QtFatalMsg)
//        abort();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //qInstallMessageHandler(customMessageOutput);
    MainWindow w;
    w.show();

    return a.exec();
}
