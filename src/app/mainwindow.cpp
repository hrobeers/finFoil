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

#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonDocument>
#include <QCloseEvent>
#include "main.h"
#include "foileditors.h"
#include "jenson.h"
#include "foil.h"
#include "foilcalculator.h"
#include "profile.h"
#include "thicknessprofile.h"

using namespace foileditors;
using namespace foillogic;
using namespace jenson;

MainWindow::MainWindow(const hrlib::Version version, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _version(version),
    _dirty(false)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    initFoilEditors();
    newFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        // TODO store settings
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave())
    {
        QFileInfo empty;
        _currentFile = empty;
        setFoil(new Foil());
        setClean();
    }
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
    QString filePath = askSaveFileName(".foil", tr("Foils (*.foil)"));

    if (filePath.isEmpty())
        return false;

    return saveFile(filePath);
}

void MainWindow::open()
{
    if (maybeSave())
    {
        QString filePath = askOpenFileName(tr("Foils (*.foil);;All files (*)"));

        if (!filePath.isEmpty())
        {
            if (loadFile(filePath))
                setClean();
        }
    }
}

bool MainWindow::saveProfile()
{
  QString filePath = askSaveFileName(".fprof", tr("Profiles (*.fprof)"), tr("Export Profile"));

  if (filePath.isEmpty())
      return false;

  return saveObjectToFile(_fin->profile(), filePath);
}

void MainWindow::loadProfile()
{
  QString filePath = askOpenFileName(tr("Profiles (*.fprof);;All files (*)")); // TODO support: Foils (*.foil)

  if (filePath.isEmpty())
      return;

  QString errorMsg;
  QJsonObject jObj;
  if (!loadFileToJson(filePath, jObj)) return;
  auto deserialized = JenSON::deserialize<Profile>(&jObj, &errorMsg);

  if (!deserialized)
    {
      errorMsg.prepend(tr("Failed to open profile: "));
      statusBar()->showMessage(errorMsg, 5000);
      qCritical(errorMsg.toStdString().c_str());
      return;
    }

  _fin->pSetProfile(deserialized.release());
  _profileEditor->setFoil(_fin.get());
  _fin->onDeserialized();
}

bool MainWindow::saveThickness()
{
  QString filePath = askSaveFileName(".fthick", tr("Thickness Profiles (*.fthick)"), tr("Export Thickness Profile"));

  if (filePath.isEmpty())
    return false;

  return saveObjectToFile(_fin->thicknessProfile(), filePath);
}

void MainWindow::loadThickness()
{
  QString filePath = askOpenFileName(tr("Thickness Profiles (*.fthick);;All files (*)")); // TODO support: Foils (*.foil)

  if (filePath.isEmpty())
      return;

  QString errorMsg;
  QJsonObject jObj;
  if (!loadFileToJson(filePath, jObj)) return;
  auto deserialized = JenSON::deserialize<ThicknessProfile>(&jObj, &errorMsg);

  if (!deserialized)
    {
      errorMsg.prepend(tr("Failed to open thickness profile: "));
      statusBar()->showMessage(errorMsg, 5000);
      qCritical(errorMsg.toStdString().c_str());
      return;
    }

  _fin->pSetThicknessProfile(deserialized.release());
  _thicknessEditor->setFoil(_fin.get());
  _fin->onDeserialized();
}

void MainWindow::stlExport()
{
    // TODO
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About finFoil"), aboutHtml());
}

bool MainWindow::maybeSave()
{
    if (_dirty)
    {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Unsaved changes"),
            tr("The fin has unsaved changes.\nDo you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    return true;
}

void MainWindow::setDirty()
{
    if (!_dirty)
    {
        _dirty = true;
        this->setWindowTitle(_currentFile.fileName() + "*" + " -- finFoil v" + _version.toString());
    }
}

void MainWindow::setClean()
{
    _dirty = false;
    this->setWindowTitle(_currentFile.fileName() + " -- finFoil v" + _version.toString());
}

void MainWindow::setFoil(Foil *foil)
{
    _outlineEditor->setFoil(foil);
    _profileEditor->setFoil(foil);
    _thicknessEditor->setFoil(foil);
    _foilDataWidget->setFoilCalculator(_outlineEditor->foilCalculator());
    _fin.reset(foil);

    connect(_foilDataWidget, SIGNAL(pxPerUnitOutlineChanged(qreal)), _outlineEditor, SLOT(setGridUnitSize(qreal)));
    connect(_foilDataWidget, SIGNAL(pxPerUnitProfileChanged(qreal)), _profileEditor, SLOT(setGridUnitSize(qreal)));

    // connect dirty flagging
    connect(_outlineEditor->foilCalculator()->foil(), SIGNAL(foilChanged(Foil*)), this, SLOT(setDirty()));
    connect(_foilDataWidget, SIGNAL(depthChanged(qt::units::Length*)), this, SLOT(setDirty()));
    connect(_foilDataWidget, SIGNAL(thicknessChanged(qt::units::Length*)), this, SLOT(setDirty()));
}

void MainWindow::initFoilEditors()
{
    _outlineEditor = new OutlineEditor(this);
    _profileEditor = new ProfileEditor(this);
    _thicknessEditor = new ThicknessEditor(this);
    _foilDataWidget = new FoilDataWidget(this);

    QSplitter* ptSplitter = new QSplitter(Qt::Vertical);
    ptSplitter->addWidget(_thicknessEditor);
    ptSplitter->addWidget(_profileEditor);
    ptSplitter->addWidget(_foilDataWidget);

    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(_outlineEditor);
    mainSplitter->addWidget(ptSplitter);
    QList<int> sizes = { this->width()/2, this->width()/2 };
    mainSplitter->setSizes(sizes);

    setCentralWidget(mainSplitter);
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new fin"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(), tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a fin from file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the fin to current file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(), tr("Save &As ..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the fin to a different file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    loadProfAct = new QAction(QIcon(), tr("Import Profile"), this);
    loadProfAct->setStatusTip(tr("Import profile from file"));
    connect(loadProfAct, SIGNAL(triggered()), this, SLOT(loadProfile()));

    saveProfAct = new QAction(QIcon(), tr("Export Profile"), this);
    saveProfAct->setStatusTip(tr("Export profile to file"));
    connect(saveProfAct, SIGNAL(triggered()), this, SLOT(saveProfile()));

    loadThickAct = new QAction(QIcon(), tr("Import Thickness"), this);
    loadThickAct->setStatusTip(tr("Import thickness profile from file"));
    connect(loadThickAct, SIGNAL(triggered()), this, SLOT(loadThickness()));

    saveThickAct = new QAction(QIcon(), tr("Export Thickness"), this);
    saveThickAct->setStatusTip(tr("Export thickness profile to file"));
    connect(saveThickAct, SIGNAL(triggered()), this, SLOT(saveThickness()));

    stlExportAct = new QAction(QIcon(), tr("Export STL file"), this);
    stlExportAct->setStatusTip(tr("Export to STL file (internet connection required)"));
    connect(stlExportAct, SIGNAL(triggered()), this, SLOT(stlExport()));

    quitAct = new QAction(QIcon(), tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit finFoil"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(QIcon(), tr("About f&inFoil"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(), tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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

    importExportMenu = menuBar()->addMenu(tr("&Import/Export"));
    importExportMenu->addAction(loadProfAct);
    importExportMenu->addAction(saveProfAct);
    importExportMenu->addAction(loadThickAct);
    importExportMenu->addAction(saveThickAct);
    importExportMenu->addSeparator();
    importExportMenu->addAction(stlExportAct);

    aboutMenu = menuBar()->addMenu(tr("&About"));
    aboutMenu->addAction(aboutAct);
    aboutMenu->addAction(aboutQtAct);
}

bool MainWindow::saveFile(const QString &path)
{
    if (!saveObjectToFile(_fin.get(), path))
      return false;

    setCurrentFilePath(path);
    statusBar()->showMessage(tr("Fin saved"), 2000);

    setClean();
    return true;
}

bool MainWindow::loadFileToJson(const QString &path, QJsonObject &jObj)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
      QMessageBox::warning(this, tr("Cannot read file"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return false;
    }

    jObj = QJsonDocument::fromJson(file.readAll()).object();
    return true;
}

bool MainWindow::loadFile(const QString &path)
{
    QString errorMsg;
    QJsonObject jObj;
    if (!loadFileToJson(path, jObj)) return false;
    auto deserialized = JenSON::deserialize<Foil>(&jObj, &errorMsg);

    if (deserialized)
    {
        setFoil(deserialized.release());
        _currentFile = path;

        return true;
    }
    else
    {
        errorMsg.prepend(tr("Failed to open fin: "));
        statusBar()->showMessage(errorMsg, 5000);
        qCritical(errorMsg.toStdString().c_str());

        return false;
    }
}

void MainWindow::setCurrentFilePath(const QString &path)
{
    _currentFile = path;
}

QString MainWindow::askSaveFileName(const QString &extension, const QString &fileFilter, const QString &title)
{
  QString suggestedPath = tr("untitled") + extension;
  if (_currentFile.isFile())
    suggestedPath = _currentFile.absoluteDir().absoluteFilePath(_currentFile.baseName() + extension);

  return QFileDialog::getSaveFileName(this, title,
                                      suggestedPath,
                                      fileFilter);
}

QString MainWindow::askOpenFileName(const QString &fileFilter, const QString &title)
{
  QString currentDir;
  if (_currentFile.isFile())
    currentDir = _currentFile.canonicalPath();

  return QFileDialog::getOpenFileName(this, title,
                                      currentDir,
                                      fileFilter);
}

bool MainWindow::saveObjectToFile(const QObject *obj, const QString &path)
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

    QJsonDocument json(JenSON::serialize(obj));
    file.write(json.toJson(QJsonDocument::Compact));

    return true;
}
