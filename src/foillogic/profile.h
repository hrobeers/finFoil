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

#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>
#include <QSharedPointer>
#include "path.h"

namespace foillogic
{
    struct Symmetry
    {
        enum e { Symmetric, Asymmetric, Flat };
    };

    class Profile : public QObject
    {
        Q_OBJECT
    public:
        explicit Profile(QObject *parent = 0);

        QSharedPointer<patheditor::Path> topProfile();
        QSharedPointer<patheditor::Path> botProfile();

        Symmetry::e symmetry() const;
        void setSymmetry(Symmetry::e symmetry);

        virtual ~Profile();

    signals:
        void profileChanged(Profile* sender);
        void profileReleased(Profile* sender);

    public slots:

    private:
        Symmetry::e _symmetry;

        QSharedPointer<patheditor::Path> _topProfile;
        QSharedPointer<patheditor::Path> _botProfile;

        // parts of the profile for connecting when symmetric
        QSharedPointer<patheditor::CubicBezier> _tPart1;
        QSharedPointer<patheditor::CubicBezier> _tPart2;
        QSharedPointer<patheditor::CubicBezier> _bPart1;
        QSharedPointer<patheditor::CubicBezier> _bPart2;

        void initProfile();

        void mirror(patheditor::CubicBezier* source, patheditor::CubicBezier* destination);

    private slots:
        void onProfileChange(patheditor::Path *path);

        void onProfileChanged();
        void onProfileReleased();
    };
}

#endif // PROFILE_H
