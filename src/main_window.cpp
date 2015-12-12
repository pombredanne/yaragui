#include "main_window.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>

MainWindow::MainWindow()
{
  setCentralWidget(new QWidget(this));
  QVBoxLayout* layout = new QVBoxLayout(centralWidget());

  QLineEdit* edit = new QLineEdit(centralWidget());
  layout->addWidget(edit);

  show();
}

void MainWindow::getUserInput()
{
  QString target = QFileDialog::getOpenFileName(this, "Select Target");
  QString rules = QFileDialog::getOpenFileName(this, "Select Rules", QString(), "Rules (*.yar)");
  onRequestScan(target.toStdString(), rules.toStdString());
}
