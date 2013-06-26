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

#ifndef FOILCALCULATOR_H
#define FOILCALCULATOR_H

#include "foillogicfwd/foillogicfwd.h"

#include <QObject>
#include <QThreadPool>
#include <QSharedPointer>
#include <QPainterPath>

namespace foillogic
{
    class FoilCalculator : public QObject
    {
        Q_OBJECT
    public:
        explicit FoilCalculator(Foil* foil);

        Foil* foil();

        void setContourThicknesses(QList<qreal> thicknesses);
        QList<QSharedPointer<QPainterPath> > calculatedContours();

        void calculate();
        bool calculated();

        qreal area();

    signals:
        void foilCalculated(FoilCalculator* sender);

    public slots:

    private:
        bool _calculated;
        QThreadPool _tPool;

        Foil* _foil;

        QList<qreal> _contourThicknesses;
        QList<QSharedPointer<QPainterPath> > _contours;
        qreal _area;

    private slots:
        void foilChanged();
    };

    class AreaCalculator : public QRunnable
    {
    public:
        explicit AreaCalculator(Foil* foil, qreal* area);

        virtual void run();

        virtual ~AreaCalculator();

    private:
        Foil* _foil;
        qreal* _area;
    };
}

#endif // FOILCALCULATOR_H
