/****************************************************************************

 Copyright (c) 2013, Hans Robeers
 All rights reserved.

 BSD 2-Clause License

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonDocument>
#include "hrlib/serialization/serialization.h"
#include "foil.h"

using namespace foileditors;
using namespace foillogic;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    setFoilEditors(new Foil());
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::save()
{
    if (_currentFile.isFile())
        return saveFile(_currentFile.absoluteFilePath());
    else
        return saveAs();
}

bool MainWindow::saveAs()
{
    QString suggestedPath = "untitled.foil";
    if (_currentFile.isFile())
        suggestedPath = _currentFile.absoluteFilePath();

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Fin"),
                                                    suggestedPath,
                                                    tr("Foils (*.foil)"));
    if (filePath.isEmpty())
        return false;

    return saveFile(filePath);
}

void MainWindow::open()
{
    QString currentDir;
    if (_currentFile.isFile())
        currentDir = _currentFile.canonicalPath();

    // if (maybeSave)   TODO
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Fin"),
                                                    currentDir,
                                                    tr("Foils (*.foil);;All files (*)"));
    if (!filePath.isEmpty())
        loadFile(filePath);
}

void MainWindow::setFoilEditors(Foil *foil)
{
    _fin.reset(foil);

    _outlineEditor = new OutlineEditor(_fin.get());
    _profileEditor = new ProfileEditor(_fin.get());
    _thicknessEditor = new ThicknessEditor(_fin.get());
    FoilDataWidget* foilDataWidget = new FoilDataWidget(_outlineEditor->foilCalculator());
    QObject::connect(foilDataWidget, SIGNAL(pxPerUnitChanged(qreal)), _outlineEditor, SLOT(setGridUnitSize(qreal)));

    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* ptLayout = new QVBoxLayout();

    ptLayout->addWidget(_thicknessEditor);
    ptLayout->addWidget(_profileEditor);
    ptLayout->addWidget(foilDataWidget);

    mainLayout->addWidget(_outlineEditor);
    mainLayout->addLayout(ptLayout);

    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new fin"));
    //connect

//    QAction *openAct;
    openAct = new QAction(QIcon(), tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a fin from file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

//    QAction *saveAct;
    saveAct = new QAction(QIcon(), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the fin to current file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

//    QAction *saveAsAct;
    saveAsAct = new QAction(QIcon(), tr("Save &As ..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the fin to a different file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));


    quitAct = new QAction(QIcon(), tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit finFoil"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);
}

bool MainWindow::saveFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Cannot save fin"),
                             tr("Cannot write to file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QJsonDocument json(hrlib::serialization::serialize(_fin.get()));
    out << json.toJson();

    setCurrentFilePath(path);
    statusBar()->showMessage(tr("Fin saved"), 2000);
    return true;
}

void MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Cannot read fin"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QString jsonStr = in.readAll();

    // TODO error handling
    QJsonObject jObj = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    std::unique_ptr<Foil> deserialized(qobject_cast<Foil*>(hrlib::serialization::deserialize(&jObj).release()));

    setFoilEditors(deserialized.release());

    _currentFile = path;
}

void MainWindow::setCurrentFilePath(const QString &path)
{
    _currentFile = path;
}
