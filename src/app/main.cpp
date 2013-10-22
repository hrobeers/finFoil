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

#include <QApplication>
#include <QLayout>
#include <QGraphicsView>
#include <QDebug>
#include "finfoil_version.h"
#include "mainwindow.h"
#include "foileditors.h"
#include "../hrlib/exceptions.h"
#include "foil.h"

using namespace foileditors;
using namespace foillogic;

int main(int argc, char *argv[])
{
    hrlib::Version version = finfoil::version();
    qDebug() << "Starting finFoil" << version.toString().toStdString().c_str()
             << "git-hash" << version.commit().toStdString().c_str();

    try
    {
        QApplication a(argc, argv);

        QSharedPointer<Foil> fin(new Foil());

        OutlineEditor* outlineEditor = new OutlineEditor(fin.data());
        ProfileEditor* profileEditor = new ProfileEditor(fin.data());
        ThicknessEditor* thicknessEditor = new ThicknessEditor(fin.data());
        FoilDataWidget* foilDataWidget = new FoilDataWidget(outlineEditor->foilCalculator());
        QObject::connect(foilDataWidget, SIGNAL(pxPerUnitChanged(qreal)), outlineEditor, SLOT(setGridUnitSize(qreal)));

        QHBoxLayout* mainLayout = new QHBoxLayout();
        QVBoxLayout* ptLayout = new QVBoxLayout();

        ptLayout->addWidget(thicknessEditor);
        ptLayout->addWidget(profileEditor);
        ptLayout->addWidget(foilDataWidget);

        mainLayout->addWidget(outlineEditor);
        mainLayout->addLayout(ptLayout);

        QWidget* centralWidget = new QWidget();
        centralWidget->setLayout(mainLayout);

        MainWindow w;
        w.setCentralWidget(centralWidget);
        w.setWindowTitle("finFoil v" + version.toString());
        w.show();

        return a.exec();
    }
    catch (hrlib::Exception &ex)
    {
        qDebug() << ex.message();
        return 1;
    }
}
