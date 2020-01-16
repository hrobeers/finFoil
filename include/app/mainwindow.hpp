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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "foileditors/fwd/foileditorsfwd.hpp"
#include "foillogic/fwd/foillogicfwd.hpp"

#include <QMainWindow>
#include <QFileInfo>
#include <QUrl>
#include "jenson.h"
#include "hrlib/version.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QUrl &baseUrl, const hrlib::Version version, QWidget *parent = 0);
    ~MainWindow();

    bool saveFile(const QString &path);
    bool loadFile(const QString &path);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    bool save();
    bool saveAs();
    void open();

    bool saveOutline();
    void loadOutline();
    void loadOutlineImage();
    bool saveProfile();
    void loadProfile();
    void loadProfileImage();
    bool saveThickness();
    void loadThickness();
    void loadThicknessImage();

    void stlExport();

    void about();

    bool maybeSave();

    void setDirty();
    void setClean();

private:
    Ui::MainWindow *ui;
    const QUrl _baseUrl;
    const hrlib::Version _version;

    sptr<foillogic::Foil> _fin;

    foileditors::OutlineEditor* _outlineEditor;
    foileditors::ProfileEditor* _profileEditor;
    foileditors::ThicknessEditor* _thicknessEditor;
    foileditors::FoilDataWidget* _foilDataWidget;

    bool _dirty;
    QFileInfo _currentFile;

    QMenu *fileMenu;
    QMenu *importMenu;
    QMenu *exportMenu;
    QMenu *aboutMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;

    QAction *loadOutlAct;
    QAction *loadOutlImgAct;
    QAction *saveOutlAct;
    QAction *loadProfAct;
    QAction *loadProfImgAct;
    QAction *saveProfAct;
    QAction *loadThickAct;
    QAction *loadThickImgAct;
    QAction *saveThickAct;

    QAction *stlExportAct;

    QAction *quitAct;

    QAction *aboutAct;
    QAction *aboutQtAct;

    void setFoil(foillogic::Foil* foil);
    void initFoilEditors();
    void createActions();
    void createMenus();

    bool loadFileToJson(const QString &path, QJsonObject &jObj);

    void setCurrentFilePath(const QString &path);
    QString askSaveFileName(const QString &extension, const QString &fileFilter, const QString &title = tr("Save As"));
    QString askOpenFileName(const QString &fileFilter, const QString &title = tr("Open File"));
    bool saveObjectToFile(const QObject *obj, const QString &path);
};

#endif // MAINWINDOW_H
