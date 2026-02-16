#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logging/logger.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTrayIcon();
    refreshRules();
    Logger::info("GUI Initialized");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupTrayIcon() {
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/app_icon.ico"));

    m_trayMenu = new QMenu(this);
    auto *restoreAction = m_trayMenu->addAction("Restore");
    auto *quitAction = m_trayMenu->addAction("Quit");

    connect(restoreAction, &QAction::triggered, this, &MainWindow::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    m_trayIcon->setContextMenu(m_trayMenu);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
    m_trayIcon->show();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) hide();
        else showNormal();
    }
}

void MainWindow::refreshRules() {
    ui->listRules->clear();
    auto rules = m_firewall.listRules();
    for (const auto& rule : rules) {
        ui->listRules->addItem(QString::fromWCharArray(rule.name.c_str()));
    }
}

void MainWindow::on_btnAddRule_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Application", "", "Executables (*.exe)");
    if (fileName.isEmpty()) return;

    bool ok;
    QString ruleName = QInputDialog::getText(this, "Rule Name", "Enter name for the rule:", QLineEdit::Normal, QFileInfo(fileName).baseName(), &ok);
    if (!ok || ruleName.isEmpty()) return;

    auto err = m_firewall.addAllowRule(fileName.toStdWString(), ruleName.toStdWString());
    if (err) {
        QMessageBox::critical(this, "Error", "Failed to add rule");
    } else {
        refreshRules();
    }
}

void MainWindow::on_btnRemoveRule_clicked() {
    QListWidgetItem *item = ui->listRules->currentItem();
    if (!item) return;

    auto err = m_firewall.removeRule(item->text().toStdWString());
    if (err) {
        QMessageBox::critical(this, "Error", "Failed to remove rule");
    } else {
        refreshRules();
    }
}

void MainWindow::on_btnRefresh_clicked() {
    refreshRules();
}
