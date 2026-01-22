#pragma once

/**
 * @file mainwindow.hpp
 * @brief Main window for FileForge GUI
 */

#include <fileforge/fileforge.hpp>

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QLabel>
#include <QThread>

#include <memory>

class OrganizeWorker : public QObject {
    Q_OBJECT

public:
    OrganizeWorker(fileforge::Config config, QString sourcePath, QString destPath)
        : m_config(std::move(config))
        , m_sourcePath(std::move(sourcePath))
        , m_destPath(std::move(destPath)) {}

public slots:
    void process();

signals:
    void progress(int current, int total, QString filename);
    void finished(fileforge::OrganizationStats stats);
    void error(QString message);

private:
    fileforge::Config m_config;
    QString m_sourcePath;
    QString m_destPath;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void browseSource();
    void browseDestination();
    void startOrganization();
    void cancelOrganization();
    void updateProgress(int current, int total, QString filename);
    void organizationFinished(fileforge::OrganizationStats stats);
    void organizationError(QString message);

private:
    void setupUi();
    void setupConnections();
    void updateUiState(bool organizing);
    void appendLog(const QString& message, const QString& color = "black");

    // UI Components
    QLineEdit* m_sourceEdit;
    QLineEdit* m_destEdit;
    QPushButton* m_browseSourceBtn;
    QPushButton* m_browseDestBtn;
    QCheckBox* m_recursiveCheck;
    QCheckBox* m_renameCheck;
    QCheckBox* m_dryRunCheck;
    QCheckBox* m_includeHiddenCheck;
    QSpinBox* m_threadsSpin;
    QPushButton* m_startBtn;
    QPushButton* m_cancelBtn;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QTextEdit* m_logView;

    // Worker thread
    QThread* m_workerThread = nullptr;
    OrganizeWorker* m_worker = nullptr;
    bool m_isOrganizing = false;
};
