#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include "firewall/firewall.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAddRule_clicked();
    void on_btnRemoveRule_clicked();
    void on_btnRefresh_clicked();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupTrayIcon();
    void refreshRules();

    Ui::MainWindow *ui;
    Firewall m_firewall;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
};
