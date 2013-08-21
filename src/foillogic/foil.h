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

#ifndef FOIL_H
#define FOIL_H

#include "patheditorfwd/patheditorfwd.h"

#include <QObject>
#include <QSharedPointer>
#include "path.h"

namespace foillogic
{
    struct Symmetry
    {
        enum e { Symmetric, Asymmetric, Flat };
    };

    class Foil : public QObject
    {
        Q_OBJECT
    public:
        explicit Foil(QObject *parent = 0);

        QSharedPointer<patheditor::Path> outline();
        QSharedPointer<patheditor::Path> topProfile();
        QSharedPointer<patheditor::Path> botProfile();
        QSharedPointer<patheditor::Path> thickness();

        Symmetry::e symmetry();
        void setSymmetry(Symmetry::e symmetry);

        virtual ~Foil();

    signals:
        void foilChanged(Foil* sender);
        void foilReleased(Foil* sender);

    public slots:

    private:
        Symmetry::e _symmetry;

        QSharedPointer<patheditor::Path> _outline;
        QSharedPointer<patheditor::Path> _topProfile;
        QSharedPointer<patheditor::Path> _botProfile;
        QSharedPointer<patheditor::Path> _thickness;

        // parts of the profile for connecting when symmetric
        QSharedPointer<patheditor::CubicBezier> _tPart1;
        QSharedPointer<patheditor::CubicBezier> _tPart2;
        QSharedPointer<patheditor::CubicBezier> _bPart1;
        QSharedPointer<patheditor::CubicBezier> _bPart2;

        void initOutline();
        void initProfile();
        void initThickness();

        void mirror(patheditor::CubicBezier* source, patheditor::CubicBezier* destination);

    private slots:
        void onFoilChanged();
        void onFoilReleased();

        void onProfileChange(patheditor::Path *path);
    };
}

#endif // FOIL_H
