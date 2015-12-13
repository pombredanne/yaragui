#include "main_window.h"
#include <QFileDialog>

MainWindow::MainWindow()
{
  m_ui.setupUi(this);

  show();
}

void MainWindow::getUserInput()
{
  QString target = QFileDialog::getOpenFileName(this, "Select Target");
  QString rules = QFileDialog::getOpenFileName(this, "Select Rules", QString(), "Rules (*.yar)");
  onRequestScan(target.toStdString(), rules.toStdString());
}
