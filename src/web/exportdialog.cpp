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

using namespace web;

ExportDialog::ExportDialog(const foillogic::Foil *toExport, const hrlib::Version *version, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::ExportDialog),
    _toExport(toExport),
    _stlExport(new StlExport(version)),
    _msgReply(nullptr), _stlReply(nullptr)
{
    _ui->setupUi(this);

    _ui->progressBar->hide();
    _ui->progressBar->setMinimum(0);
    _ui->progressBar->setMaximum(0);

    connect(_ui->exportButton, &QPushButton::clicked, this, &ExportDialog::exportClicked);
    connect(_ui->closeButton, &QPushButton::clicked, this, &ExportDialog::closeClicked);

    _ui->webView->setHtml("<h2>Connection to the internet is required for this functionality</h2>");

    connect(_stlExport.get(), &StlExport::finished, this, &ExportDialog::exportFinished);

    _msgReply.reset(_stlExport->getMessage());
}

ExportDialog::~ExportDialog() {}

void ExportDialog::exportClicked()
{
    // TODO choose file to save to
    _stlReply.reset(_stlExport->generateSTL(_toExport));
    _ui->progressBar->show();
}

void ExportDialog::closeClicked()
{
    close();
}

void ExportDialog::exportFinished(QNetworkReply *reply)
{
    if (_msgReply.get() == reply)
    {
        _ui->webView->setHtml(QString::fromUtf8(reply->readAll()));

        if (reply->error() == QNetworkReply::NoError)
            _ui->exportButton->setDisabled(false);
        _msgReply.reset();
    }
    else if (_stlReply.get() == reply)
    {
        // TODO download (progressbar) & save
        _ui->webView->setHtml(QString::fromUtf8(reply->readAll()));
        _stlReply.reset();
//        close();
    }
}
