/****************************************************************************

 Copyright (c) 2015, Hans Robeers
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

#include "exportdialog.hpp"
#include "ui_exportdialog.h"

#include <QNetworkReply>
#include <QFileDialog>
#include <QMessageBox>

using namespace web;

ExportDialog::ExportDialog(const foillogic::Foil *toExport, const QUrl &baseUrl, const QFileInfo &currentFile,
                           const hrlib::Version &version, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::ExportDialog),
    _toExport(toExport), _fileName(currentFile),
    _stlExport(new StlExport(baseUrl, version)),
    _msgReply(nullptr), _postFoilReply(nullptr)
{
    _ui->setupUi(this);

    _ui->progressBar->hide();
    _ui->progressBar->setMinimum(0);
    _ui->progressBar->setMaximum(0);

    connect(_ui->exportButton, &QPushButton::clicked, this, &ExportDialog::exportClicked);
    connect(_ui->closeButton, &QPushButton::clicked, this, &ExportDialog::closeClicked);

    _ui->webView->setHtml("<style>body { font-family: helvetica neue, lucida grande, sans-serif; color: #444; text-align: center; }</style><h2>Connecting to server ...</h2><p>If this message does not disappear soon, check your internet connection.<br/>Otherwise contact finfoil.io</p>");

    connect(_stlExport.get(), &StlExport::finished, this, &ExportDialog::exportFinished);

    _msgReply.reset(_stlExport->getMessage());
}

ExportDialog::~ExportDialog() {}

void ExportDialog::exportClicked()
{
    _fileName = QFileDialog::getSaveFileName(this, tr("Save file"),
                                             _fileName.absolutePath() + "/" + _fileName.baseName() + ".stl",
                                             tr("STL (*.stl);;All files (*)"));
    QFileInfo stlFile(_fileName);
    _postFoilReply.reset(_stlExport->generateSTL(_toExport, stlFile.baseName()));
    _ui->progressBar->show();
}

void ExportDialog::closeClicked()
{
    close();
}

void ExportDialog::exportFinished(QNetworkReply *reply)
{
    //
    // Handle message reply (to show in QWebView component)
    //
    if (_msgReply.get() == reply)
    {
        if (reply->size())
            _ui->webView->setHtml(QString::fromUtf8(reply->readAll()));

        if (reply->error() == QNetworkReply::NoError)
            _ui->exportButton->setDisabled(false);

        _msgReply.reset();
    }

    //
    // Handle postFoil reply (returns the .stl URL)
    //
    else if (_postFoilReply.get() == reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            _getStlReply.reset(_stlExport->getSTL(reply->readAll()));
            connect(_getStlReply.get(), &QNetworkReply::downloadProgress, this, &ExportDialog::downloadProgress);
        }
        else
        {
            _ui->webView->setHtml(QString::fromUtf8(reply->readAll()));
        }
        _postFoilReply.reset();
    }

    //
    // Handle the getSTL reply (write to file)
    //
    else if (_getStlReply.get() == reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QFile file(_fileName.absoluteFilePath());
            if (!file.open(QFile::WriteOnly | QFile::Text))
            {
                QMessageBox::warning(this, tr("Cannot save fin"),
                                     tr("Cannot write to file %1:\n%2.")
                                     .arg(_fileName.absoluteFilePath())
                                     .arg(file.errorString()));
            }

            file.write(reply->readAll());
            close();
        }
        else
        {
            _ui->webView->setHtml(QString::fromUtf8(reply->readAll()));
        }
        _getStlReply.reset();
        _ui->progressBar->hide();
    }
}

void ExportDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    _ui->progressBar->setMaximum(bytesTotal);
    _ui->progressBar->setValue(bytesReceived);
}
