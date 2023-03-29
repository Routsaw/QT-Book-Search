#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "about.h"
//#include "register.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), logFile(new QFile("booksearch.log"))
{
    // Create the log file
    // This file will be created in the directory of the build path
    if (logFile->open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
        writeToLogFile("Book Search Log File\n");
    } else {
        qDebug() << "Failed to create log file";
    }

    // Set up the logging category
    QLoggingCategory logCategory("BookSearch");

    // Set up the user interface
    ui = new Ui::MainWindow;
    ui->setupUi(this);

    // Open the database connection
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("bookstore.db");
        if (!db.open()) {
            qDebug() << "Failed to open database";
        }


    // Connect signals and slots
    connect(ui->btnSearch, SIGNAL(clicked()), this, SLOT(on_btnSearch_clicked()));
    //connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(on_btnRegister_clicked()));

    bookCountLabel = new QLabel(this); // Create a new QLabel for displaying the number of books and set its parent to the current MainWindow instance
    int numberOfBooks = getNumberOfBooks(); // Call the getNumberOfBooks() function to retrieve the number of books in the database
    bookCountLabel->setText(QStringLiteral("Number of books in the database: %1").arg(numberOfBooks)); // Set the text of the QLabel to display the number of books
    statusBar()->addWidget(bookCountLabel); // Add the QLabel to the QStatusBar of the MainWindow

}

MainWindow::~MainWindow()
{
    writeToLogFile("User exits the application");
    logFile->close();
    delete logFile;
    delete ui;
    // Close the database connection
    db.close();
}

void MainWindow::writeToLogFile(const QString &message)
{
    if (logFile->isOpen()) {
        QTextStream logStream(logFile);
        logStream << message << "\n";
        logFile->flush();
    }
}




void MainWindow::about()
{
    class about about;
    about.exec();
}

void MainWindow::on_btnSearch_clicked()

    {
        // Get the search criteria from the line edit boxes
        QString isbn = ui->lineISBN->text();
        QString title = ui->lineTitle->text();
        QString author = ui->lineAuthor->text();

        // Construct the SQL query based on the search criteria
        QString query = "SELECT * FROM books WHERE ";
        if (!isbn.isEmpty()) {
            query += QString("ISBN='%1' AND ").arg(isbn);
        }
        if (!title.isEmpty()) {
            query += "lower(`Book-Title`) LIKE :title AND ";
        }
        if (!author.isEmpty()) {
            query += "lower(`Book-Author`) LIKE :author AND ";
        }
        // Remove the trailing "AND" if it exists
        if (query.endsWith("AND ")) {
            query = query.left(query.length() - 4);
        }

        QSqlQuery sqlQuery;
        sqlQuery.prepare(query);
        if (!isbn.isEmpty()) {
            sqlQuery.bindValue(":isbn", isbn);
        }
        if (!title.isEmpty()) {
            sqlQuery.bindValue(":title", QString("%%1%").arg(title.toLower()));
        }
        if (!author.isEmpty()) {
            sqlQuery.bindValue(":author", QString("%%1%").arg(author.toLower()));
        }

        if (!sqlQuery.exec()) {
            qDebug() << "Failed to execute query";
            db.close();
            return;
        }
        writeToLogFile("Successful database operation");


        // Create a widget to hold the search results
        QWidget* resultsWidget = new QWidget();
        QVBoxLayout* resultsLayout = new QVBoxLayout(resultsWidget);
        int count = 0; //counts the number of books for the statusbar.
        while (sqlQuery.next()) {
            // Get the data for each row of the result
            QString isbn = sqlQuery.value("ISBN").toString();
            QString title = sqlQuery.value("Book-Title").toString();
            QString author = sqlQuery.value("Book-Author").toString();
            count++; //counts the number of books for the statusbar.

            // Add the data to a label and add the label to the layout
            QString resultString = QString("%1: %2, by %3")
                    .arg(isbn)
                    .arg(title)
                    .arg(author);
            resultsLayout->addWidget(new QLabel(resultString));

        }

        if(count == 0){
            QMessageBox::warning(this, "Invalid search", "No books found with the search perameters. Please enter again.");
        }

        // Set the search results widget as the widget for the scroll area
        ui->scrollAreaResults->setWidget(resultsWidget);
        // Displays the total amount of books in a status bar
        ui->statusbar->showMessage(QString("Total books: %1").arg(count));

    }

int MainWindow::getNumberOfBooks() {
    QSqlQuery query;
    if (!query.exec("SELECT COUNT(*) FROM books")) {
        qWarning() << "Error: Unable to execute query to get the number of books" << query.lastError();
        return 0;
    }

    int bookCount = 0;
    if (query.next()) {
        bookCount = query.value(0).toInt();
    }

    return bookCount;
}





