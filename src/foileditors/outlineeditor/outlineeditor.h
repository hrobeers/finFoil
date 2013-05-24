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

#ifndef OUTLINEEDITOR_H
#define OUTLINEEDITOR_H

#include "hrlibfwd/qtfwd.h"
#include "patheditorfwd/patheditorfwd.h"

#include <QWidget>
#include "outlinedatawidget.h"

using namespace patheditor;

namespace foileditors
{
    /**
     * Editor to edit the foil's outline.
     * This editor can display the thickness contours while editing.
     */
    class OutlineEditor : public QWidget
    {
        Q_OBJECT
    public:
        explicit OutlineEditor(QWidget *parent = 0);

        virtual ~OutlineEditor() {}

    signals:
        void profileChanged(EditablePath *sender);
        void thicknessChanged(EditablePath *sender);

    public slots:
        void onProfileChange(EditablePath *sender);
        void onThicknessChange(EditablePath *sender);

    private:
        QVBoxLayout* _mainLayout;
        patheditor::PathEditorWidget* _pathEditor;
        OutlineDataWidget* _outlineDataWidget;

    };
}

#endif // OUTLINEEDITOR_H
