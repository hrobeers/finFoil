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

#include "web/exportdialog.hpp"
#include "ui_exportdialog.h"

#include <QNetworkReply>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrlQuery>

using namespace web;

namespace {
  const QString css = "<style>body { font-family: helvetica neue, lucida grande, sans-serif; color: #444; text-align: center; }</style>";
  QString qrcode(QString txt) {
    return "<div id=\"qrcode\"></div><script src=\"js/qrcode.min.js\"></script><script>new QRCode(document.getElementById('qrcode'),'" + txt + "');</script>";
  }
  const QUrlQuery identifier = {{QStringLiteral("origin"), QStringLiteral("qt")}};
}

ExportDialog::ExportDialog(const foillogic::Foil *toExport, const QUrl &baseUrl, const QFileInfo &currentFile,
                           const hrlib::Version &version, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::ExportDialog),
    _baseUrl(baseUrl),
    _stlExport(new StlExport(baseUrl, version)),
    _toExport(toExport),
    _msgReply(nullptr), _postFoilReply(nullptr), _getStlReply(nullptr),
    _fileName(currentFile)
{
    _ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
                   Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint |
                   Qt::WindowCloseButtonHint);

    _ui->progressBar->hide();
    _ui->progressBar->setMinimum(0);
    _ui->progressBar->setMaximum(0);

    connect(_ui->exportButton, &QPushButton::clicked, this, &ExportDialog::exportClicked);
    connect(_ui->closeButton, &QPushButton::clicked, this, &ExportDialog::closeClicked);

    _ui->webView->setContent((css + "<h2>Connecting to server ...</h2><p>If this message does not disappear soon, check your internet connection.<br/>Otherwise contact info@finfoil.io</p>").toUtf8(), "text/html;charset=UTF-8", _baseUrl);
    setLinkDelegation();

    connect(_stlExport.get(), &StlExport::finished, this, &ExportDialog::exportFinished);

    _msgReply.reset(_stlExport->getMessage());
}

ExportDialog::~ExportDialog() {}

void ExportDialog::setLinkDelegation()
{
  // TODO ?
  //_ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}

void ExportDialog::exportClicked()
{
    _fileName = QFileDialog::getSaveFileName(this, tr("Save file"),
                                             _fileName.absolutePath() + "/" + _fileName.baseName() + ".stl",
                                             tr("STL (*.stl);;All files (*)"));
    QFileInfo stlFile(_fileName);
    _getStlReply.reset(_stlExport->getSTL(_stlUrl));
    connect(_getStlReply.get(), &QNetworkReply::downloadProgress, this, &ExportDialog::downloadProgress);

    _ui->progressBar->show();
}

void ExportDialog::closeClicked()
{
    close();
}

void ExportDialog::exportFinished(QNetworkReply *reply)
{
    //
    // Handle message reply (to show in QWebEngineView component)
    //
    if (_msgReply.get() == reply)
    {
        if (reply->size())
        {
          _message = reply->readAll();
          _ui->webView->setContent(_message, "text/html;charset=UTF-8", _baseUrl);
        }
        else
          _message.clear();

        if (reply->error() == QNetworkReply::NoError) {
            _ui->exportButton->setDisabled(false);

            QFileInfo stlFile(_fileName);
            _postFoilReply.reset(_stlExport->generateSTL(_toExport, stlFile.baseName()));
        }

        _msgReply.reset();
    }

    //
    // Handle errors
    //
    else if (reply->error() != QNetworkReply::NoError)
    {
      _ui->webView->setContent(reply->readAll(), "text/html;charset=UTF-8", _baseUrl);
    }

    //
    // Handle postFoil reply (returns the .stl URL)
    //
    else if (_postFoilReply.get() == reply)
    {
        _stlUrl = QString::fromUtf8(reply->readAll());

        auto id = _stlUrl.split('/', QString::SkipEmptyParts)[1];
        QUrl previewUrl = _baseUrl;
        previewUrl.setPath("/r/" + id);
        previewUrl.setQuery(identifier);
        _ui->webView->load(previewUrl);

        _postFoilReply.reset();
    }

    //
    // Handle the getSTL reply (write to file)
    //
    else if (_getStlReply.get() == reply)
    {
        QFile file(_fileName.absoluteFilePath());
        if (!file.open(QFile::WriteOnly)) {
          QMessageBox::warning(this, tr("Cannot save fin"),
                               tr("Cannot write to file %1:\n%2.")
                               .arg(_fileName.absoluteFilePath())
                               .arg(file.errorString()));
        }
        else {
          _ui->webView->setContent(_message, "text/html;charset=UTF-8", _baseUrl);
          file.write(reply->readAll());
        }

        _getStlReply.reset();
        _ui->progressBar->hide();
        _ui->exportButton->setDisabled(true);
    }

    setLinkDelegation();
}

void ExportDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    _ui->progressBar->setMaximum(bytesTotal);
    _ui->progressBar->setValue(bytesReceived);
}

// TODO preview button?
void ExportDialog::linkClicked(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}
