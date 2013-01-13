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

#include <QtGui/QApplication>
#include <QLayout>
#include <QGraphicsView>
#include "mainwindow.h"
#include "patheditor/patheditorwidget.h"
#include "patheditor/cubicbezier.h"
#include "patheditor/line.h"
#include "patheditor/editablepath.h"

using namespace patheditor;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedPointer<QPointF> point1(new QPointF(0,0));
    QSharedPointer<QPointF> point2(new QPointF(100,100));
    QSharedPointer<QPointF> point3(new QPointF(100,0));
    QSharedPointer<QPointF> point4(new QPointF(200,0));

    EditablePath* path = new EditablePath();
    path->append(QSharedPointer<PathItem>(new CubicBezier(point1, point2)));
    path->append(QSharedPointer<PathItem>(new CubicBezier(point1, point3)));
    path->append(QSharedPointer<PathItem>(new Line(point1, point4)));

    PathEditorWidget* widget = new PathEditorWidget();
    widget->addPath(path);

    MainWindow w;
    w.setCentralWidget(widget);
    w.show();

    return a.exec();
}
