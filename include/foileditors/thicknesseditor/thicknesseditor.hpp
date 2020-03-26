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

#ifndef THICKNESSEDITOR_HPP
#define THICKNESSEDITOR_HPP

#include "hrlib/fwd/qtfwd.hpp"
#include "foillogic/fwd/foillogicfwd.hpp"
#include "patheditor/fwd/patheditorfwd.hpp"

#include <QWidget>
#include <QComboBox>

using namespace patheditor;

namespace foileditors
{
    class ThicknessEditor : public QWidget
    {
        Q_OBJECT
    public:
        explicit ThicknessEditor(QWidget *parent = 0);

        void setFoil(foillogic::Foil* foil);

        void setImage(const QString &path);

        virtual ~ThicknessEditor() {}

    signals:

    private:
        QVBoxLayout* _mainLayout;
        patheditor::PathEditorWidget* _pathEditor;

        foillogic::Foil* _foil;
        EditablePath* _topPath;
        EditablePath* _botPath;

        QComboBox* _modeCombo;

    private slots:
        void update();
        void modeChanged(int mode);
    };
}

#endif // THICKNESSEDITOR_HPP
