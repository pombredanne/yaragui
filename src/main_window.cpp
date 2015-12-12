#include "main_window.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>

MainWindow::MainWindow()
{
  setCentralWidget(new QWidget(this));
  QVBoxLayout* layout = new QVBoxLayout(centralWidget());

  QLineEdit* edit = new QLineEdit(centralWidget());
  layout->addWidget(edit);

  show();
}
