#include "loginform.h"
#include "mainwindow.h"
#include "about.h"
#include <QApplication>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>



void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    static QFile logFile("booksearch.log");
    if (!logFile.isOpen()) {
        logFile.open(QFile::WriteOnly | QFile::Text);
    }

    QTextStream logStream(&logFile);
    switch (type) {
    case QtDebugMsg:
        logStream << "Debug: ";
        break;
    case QtInfoMsg:
        logStream << "Info: ";
        break;
    case QtWarningMsg:
        logStream << "Warning: ";
        break;
    case QtCriticalMsg:
        logStream << "Critical: ";
        break;
    case QtFatalMsg:
        logStream << "Fatal: ";
        break;
    }

    logStream << message << Qt::endl;
    logStream.flush();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Copy the embedded database file to a writable location
        QString databaseFileName = "bookstore.db";
        QString writableLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(writableLocation);
        if (!dir.exists()) {
            dir.mkpath(writableLocation);
        }
        QString destinationPath = writableLocation + "/" + databaseFileName;

        QFile dbFile(":/data/" + databaseFileName);
        if (dbFile.exists()) {
            if (!QFile::exists(destinationPath) || QFile(destinationPath).size() == 0) {
                if (dbFile.copy(destinationPath)) {
                    QFile::setPermissions(destinationPath, QFile::ReadUser | QFile::WriteUser);
                } else {
                    qDebug() << "Failed to copy the database file to the writable location";
                }
            }
        } else {
            qDebug() << "Database file not found in the resource system";
        }

    // Create a splash screen
       QSplashScreen splash;

    // Load the pixmap from a relative path
       QDir projectDir = QDir(QCoreApplication::applicationDirPath());
       QPixmap pixmap(projectDir.filePath(":/images/clueless.png"));

    // Calculate the new splash screen size
         QRect totalScreenGeometry;
         QList<QScreen *> screens = QApplication::screens();
         for (QScreen *screen : screens) {
             totalScreenGeometry = totalScreenGeometry.united(screen->geometry());
         }

         int newSplashWidth = static_cast<int>(totalScreenGeometry.width() * 0.15);
         int newSplashHeight = static_cast<int>(totalScreenGeometry.height() * 0.15);

    // Resize the splash screen pixmap
        QPixmap resizedPixmap = pixmap.scaled(newSplashWidth, newSplashHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        splash.setPixmap(resizedPixmap);
        splash.show();


    // Get primary and secondary screens
        QScreen *primaryScreen = QApplication::primaryScreen();
        QScreen *secondaryScreen = screens.size() > 1 ? screens[1] : nullptr;

    // Move splash screen to secondary screen if available
        if (secondaryScreen) {
            QRect secondaryScreenGeometry = secondaryScreen->geometry();
             splash.move(secondaryScreenGeometry.center());
             }

    // Display a message in the center of the splash screen
        splash.showMessage("Now loading CIS 227 Bookstore", Qt::AlignCenter | Qt::AlignBottom, Qt::red);

    // Configure the logging framework
    QLoggingCategory logCategory("BookSearch");
        qInstallMessageHandler(messageHandler);

        LoginForm w;

    // Get the geometry of the primary screen
        QRect screenGeometry = primaryScreen->geometry();

    // Move the main form to the center of the primary screen
        w.move(screenGeometry.center() - w.rect().center());

    // Pause the application for 3000 milliseconds (3 seconds)
        QTimer::singleShot(3000, &splash, [&]() {
            splash.close();
            w.show();
    });

    return a.exec();
}


