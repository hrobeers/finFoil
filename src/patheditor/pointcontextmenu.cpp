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

#include "patheditor/pointcontextmenu.hpp"

#include <QMenu>
#include "patheditor/pathpoint.hpp"
#include "patheditor/pathsettings.hpp"

using namespace patheditor;

namespace { const std::string curvePoint = "patheditor::CurvePoint"; }

void PointContextMenu::Show(const QPoint &pos)
{
    // Only show the menu for a CurvePoint
    if (curvePoint.compare(_point->metaObject()->className()))
        return;

    // Continuity, Duplicate/split, remove
    QMenu menu;

    QAction *contAct = new QAction("Smooth", &menu);
    contAct->setCheckable(true);
    contAct->setChecked(_point->continuous());
    connect(contAct, SIGNAL(toggled(bool)), this, SLOT(setContinuous(bool)));
    menu.addAction(contAct);

    // TODO implement split & remove
//    QAction *splitAct = new QAction("Split", &menu);
//    menu.addAction(splitAct);

    if (_settings->pointRemove) {
      QAction *removeAct = new QAction("Remove", &menu);
      connect(removeAct, SIGNAL(triggered()), this, SLOT(remove()));
      menu.addAction(removeAct);

      QAction *splitAct = new QAction("Split", &menu);
      connect(splitAct, SIGNAL(triggered()), this, SLOT(split()));
      menu.addAction(splitAct);
    }

    menu.exec(pos);
}

void PointContextMenu::setContinuous(bool continuous)
{
    _point->setContinuous(continuous);
}

void PointContextMenu::remove()
{
    _point->remove();
}

void PointContextMenu::split()
{
    _point->split();
}
