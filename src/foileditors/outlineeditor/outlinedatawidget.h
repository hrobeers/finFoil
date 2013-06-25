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

#ifndef OUTLINEDATAWIDGET_H
#define OUTLINEDATAWIDGET_H

#include "hrlibfwd/qtfwd.h"
#include "patheditorfwd/patheditorfwd.h"
#include "foillogicfwd/foillogicfwd.h"

#include <QWidget>

using namespace patheditor;

namespace foileditors
{
    class OutlineDataWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit OutlineDataWidget(foillogic::FoilCalculator* foilCalculator, QWidget *parent = 0);

    signals:
        void depthChanged(qreal depth);
        void pxPerUnitChanged(qreal pxPerUnit);

    public slots:
        void onDepthChange(double depth);
        void onOutlineChange(EditablePath *sender);
//        void onAreaChange(qreal area, EditablePath *sender);

    private:
        qreal _depth;
        qreal _pxPerUnit;
        EditablePath* _outlinePtr;
        foillogic::FoilCalculator* _foilCalculator;

        QFormLayout* _formLayout;
        QDoubleSpinBox* _depthEdit;
        QLineEdit* _areaEdit;
    };
}

#endif // OUTLINEDATAWIDGET_H
