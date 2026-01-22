/**
 * @file mainwindow.cpp
 * @brief Main window implementation for FileForge GUI
 */

#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDateTime>

// OrganizeWorker implementation
void OrganizeWorker::process() {
    try {
        fileforge::Organizer organizer(m_config);

        organizer.setProgressCallback([this](std::size_t current, std::size_t total,
                                             const std::string& filename) {
            emit progress(static_cast<int>(current), static_cast<int>(total),
                         QString::fromStdString(filename));
        });

        fileforge::OrganizationStats stats;
        if (m_destPath.isEmpty()) {
            stats = organizer.organize(m_sourcePath.toStdString());
        } else {
            stats = organizer.organize(m_sourcePath.toStdString(),
                                       m_destPath.toStdString());
        }

        emit finished(stats);
    } catch (const std::exception& e) {
        emit error(QString::fromStdString(e.what()));
    }
}

// MainWindow implementation
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("FileForge - Forging Order from Chaos");
    setMinimumSize(700, 500);

    setupUi();
    setupConnections();
}

MainWindow::~MainWindow() {
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void MainWindow::setupUi() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);

    // Source/Destination group
    auto* pathGroup = new QGroupBox("Paths", this);
    auto* pathLayout = new QFormLayout(pathGroup);

    auto* sourceLayout = new QHBoxLayout();
    m_sourceEdit = new QLineEdit(this);
    m_sourceEdit->setPlaceholderText("Select folder to organize...");
    m_browseSourceBtn = new QPushButton("Browse...", this);
    sourceLayout->addWidget(m_sourceEdit);
    sourceLayout->addWidget(m_browseSourceBtn);
    pathLayout->addRow("Source:", sourceLayout);

    auto* destLayout = new QHBoxLayout();
    m_destEdit = new QLineEdit(this);
    m_destEdit->setPlaceholderText("Leave empty for default (Source/Organized)");
    m_browseDestBtn = new QPushButton("Browse...", this);
    destLayout->addWidget(m_destEdit);
    destLayout->addWidget(m_browseDestBtn);
    pathLayout->addRow("Destination:", destLayout);

    mainLayout->addWidget(pathGroup);

    // Options group
    auto* optionsGroup = new QGroupBox("Options", this);
    auto* optionsLayout = new QHBoxLayout(optionsGroup);

    auto* leftOptions = new QVBoxLayout();
    m_recursiveCheck = new QCheckBox("Recursive (include subdirectories)", this);
    m_renameCheck = new QCheckBox("Intelligent renaming", this);
    m_renameCheck->setChecked(true);
    leftOptions->addWidget(m_recursiveCheck);
    leftOptions->addWidget(m_renameCheck);

    auto* rightOptions = new QVBoxLayout();
    m_dryRunCheck = new QCheckBox("Dry run (preview only)", this);
    m_includeHiddenCheck = new QCheckBox("Include hidden files", this);
    rightOptions->addWidget(m_dryRunCheck);
    rightOptions->addWidget(m_includeHiddenCheck);

    auto* threadsLayout = new QHBoxLayout();
    auto* threadsLabel = new QLabel("Threads:", this);
    m_threadsSpin = new QSpinBox(this);
    m_threadsSpin->setRange(1, 32);
    m_threadsSpin->setValue(4);
    threadsLayout->addWidget(threadsLabel);
    threadsLayout->addWidget(m_threadsSpin);
    threadsLayout->addStretch();

    optionsLayout->addLayout(leftOptions);
    optionsLayout->addLayout(rightOptions);
    optionsLayout->addLayout(threadsLayout);

    mainLayout->addWidget(optionsGroup);

    // Progress group
    auto* progressGroup = new QGroupBox("Progress", this);
    auto* progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);

    m_statusLabel = new QLabel("Ready", this);

    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(progressGroup);

    // Log view
    auto* logGroup = new QGroupBox("Log", this);
    auto* logLayout = new QVBoxLayout(logGroup);

    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setFont(QFont("Courier", 9));

    logLayout->addWidget(m_logView);
    mainLayout->addWidget(logGroup);

    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel", this);
    m_cancelBtn->setEnabled(false);

    m_startBtn = new QPushButton("Start Organization", this);
    m_startBtn->setDefault(true);

    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_startBtn);

    mainLayout->addLayout(buttonLayout);

    setCentralWidget(centralWidget);

    // Set default path to Downloads
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (!downloadsPath.isEmpty()) {
        m_sourceEdit->setText(downloadsPath);
    }
}

void MainWindow::setupConnections() {
    connect(m_browseSourceBtn, &QPushButton::clicked, this, &MainWindow::browseSource);
    connect(m_browseDestBtn, &QPushButton::clicked, this, &MainWindow::browseDestination);
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::startOrganization);
    connect(m_cancelBtn, &QPushButton::clicked, this, &MainWindow::cancelOrganization);
}

void MainWindow::browseSource() {
    QString dir = QFileDialog::getExistingDirectory(
        this, "Select Source Directory",
        m_sourceEdit->text().isEmpty()
            ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            : m_sourceEdit->text()
    );

    if (!dir.isEmpty()) {
        m_sourceEdit->setText(dir);
    }
}

void MainWindow::browseDestination() {
    QString dir = QFileDialog::getExistingDirectory(
        this, "Select Destination Directory",
        m_destEdit->text().isEmpty()
            ? m_sourceEdit->text()
            : m_destEdit->text()
    );

    if (!dir.isEmpty()) {
        m_destEdit->setText(dir);
    }
}

void MainWindow::startOrganization() {
    QString sourcePath = m_sourceEdit->text();

    if (sourcePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a source directory.");
        return;
    }

    if (!QDir(sourcePath).exists()) {
        QMessageBox::warning(this, "Error", "Source directory does not exist.");
        return;
    }

    // Create config
    fileforge::Config config;
    config.setRecursive(m_recursiveCheck->isChecked());
    config.setEnableRenaming(m_renameCheck->isChecked());
    config.setDryRun(m_dryRunCheck->isChecked());
    config.setIncludeHidden(m_includeHiddenCheck->isChecked());
    config.setParallelThreads(static_cast<std::size_t>(m_threadsSpin->value()));
    config.setGenerateLog(true);

    // Setup worker thread
    m_workerThread = new QThread(this);
    m_worker = new OrganizeWorker(config, sourcePath, m_destEdit->text());
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, m_worker, &OrganizeWorker::process);
    connect(m_worker, &OrganizeWorker::progress, this, &MainWindow::updateProgress);
    connect(m_worker, &OrganizeWorker::finished, this, &MainWindow::organizationFinished);
    connect(m_worker, &OrganizeWorker::error, this, &MainWindow::organizationError);
    connect(m_worker, &OrganizeWorker::finished, m_workerThread, &QThread::quit);
    connect(m_worker, &OrganizeWorker::error, m_workerThread, &QThread::quit);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    // Clear log and start
    m_logView->clear();
    appendLog("Starting organization...", "blue");
    appendLog(QString("Source: %1").arg(sourcePath));
    appendLog(QString("Destination: %1").arg(
        m_destEdit->text().isEmpty() ? sourcePath + "/Organized" : m_destEdit->text()));
    appendLog(QString("Dry run: %1").arg(m_dryRunCheck->isChecked() ? "Yes" : "No"));

    updateUiState(true);
    m_workerThread->start();
}

void MainWindow::cancelOrganization() {
    if (m_workerThread && m_workerThread->isRunning()) {
        appendLog("Cancelling...", "orange");
        m_workerThread->requestInterruption();
    }
}

void MainWindow::updateProgress(int current, int total, QString filename) {
    int percentage = total > 0 ? (current * 100 / total) : 0;
    m_progressBar->setValue(percentage);
    m_statusLabel->setText(QString("Processing: %1 (%2/%3)")
                          .arg(filename).arg(current).arg(total));
}

void MainWindow::organizationFinished(fileforge::OrganizationStats stats) {
    updateUiState(false);

    appendLog("", "black");
    appendLog("=== Organization Complete ===", "green");
    appendLog(QString("Total files: %1").arg(stats.totalFiles));
    appendLog(QString("Processed: %1").arg(stats.filesProcessed), "green");
    appendLog(QString("Moved: %1").arg(stats.filesMoved));
    appendLog(QString("Renamed: %1").arg(stats.filesRenamed));

    if (stats.filesSkipped > 0) {
        appendLog(QString("Skipped: %1").arg(stats.filesSkipped), "orange");
    }
    if (stats.filesFailed > 0) {
        appendLog(QString("Failed: %1").arg(stats.filesFailed), "red");
    }

    appendLog(QString("Duration: %1 ms").arg(stats.duration.count()));
    appendLog(QString("Success rate: %1%").arg(stats.successRate(), 0, 'f', 1));

    m_progressBar->setValue(100);
    m_statusLabel->setText(QString("Complete - %1 files processed").arg(stats.filesProcessed));

    if (stats.filesFailed == 0) {
        QMessageBox::information(this, "Success",
            QString("Organization complete!\n\n%1 files processed successfully.")
            .arg(stats.filesProcessed));
    } else {
        QMessageBox::warning(this, "Completed with Errors",
            QString("Organization complete with some errors.\n\n"
                   "Processed: %1\nFailed: %2")
            .arg(stats.filesProcessed).arg(stats.filesFailed));
    }
}

void MainWindow::organizationError(QString message) {
    updateUiState(false);
    appendLog(QString("Error: %1").arg(message), "red");
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::updateUiState(bool organizing) {
    m_isOrganizing = organizing;
    m_startBtn->setEnabled(!organizing);
    m_cancelBtn->setEnabled(organizing);
    m_sourceEdit->setEnabled(!organizing);
    m_destEdit->setEnabled(!organizing);
    m_browseSourceBtn->setEnabled(!organizing);
    m_browseDestBtn->setEnabled(!organizing);
    m_recursiveCheck->setEnabled(!organizing);
    m_renameCheck->setEnabled(!organizing);
    m_dryRunCheck->setEnabled(!organizing);
    m_includeHiddenCheck->setEnabled(!organizing);
    m_threadsSpin->setEnabled(!organizing);
}

void MainWindow::appendLog(const QString& message, const QString& color) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logView->append(QString("<span style='color: gray;'>[%1]</span> "
                             "<span style='color: %2;'>%3</span>")
                     .arg(timestamp, color, message));
}
