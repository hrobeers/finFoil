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

#ifndef THICKNESSPROFILE_HPP
#define THICKNESSPROFILE_HPP

#include "patheditor/fwd/patheditorfwd.hpp"

#include <QObject>
#include <memory>
#include "hrlib/mixin/identifiable.hpp"
#include "jenson.h"

namespace foillogic
{
    class ThicknessProfile : public QObject, public hrlib::Identifiable
    {
        Q_OBJECT

        // read-write properties
        Q_PROPERTY(patheditor::Path* topProfile READ pTopProfile WRITE pSetTopProfile)
        Q_PROPERTY(patheditor::Path* curve READ pCurve WRITE pSetCurve RESET pResetCurve)
        Q_PROPERTY(patheditor::Path* twist READ pTwist WRITE pSetTwist RESET pResetTwist)
        Q_PROPERTY(int flags READ flags WRITE setFlags RESET resetFlags)

        // optional properties
        Q_PROPERTY_UUID

        qreal _thicknessRatio;
        int _flags;

        qunique_ptr<patheditor::Path> _topProfile;
        qunique_ptr<patheditor::Path> _botProfile;
        qunique_ptr<patheditor::Path> _curve;
        qunique_ptr<patheditor::Path> _twist;

    public:
        Q_INVOKABLE explicit ThicknessProfile(QObject *parent = 0);
        void init();

        patheditor::Path* topProfile();
        patheditor::Path* botProfile();

        // Q_PROPERTY getters
        patheditor::Path* pTopProfile();
        patheditor::Path* pCurve();
        patheditor::Path* pTwist();
        int flags() const { return _flags; };

        // Q_PROPERTY setters
        void pSetTopProfile(patheditor::Path *topProfile);
        void pSetCurve(patheditor::Path *curve);
        void pSetTwist(patheditor::Path *twist);
        void setFlags(int flags) { _flags = flags; }

        void pResetCurve();
        void pResetTwist();
        void resetFlags();

        bool editable() const;
        bool aspectRatioEnforced() const;
        void setAspectRatioEnforced(bool enforce);

        virtual ~ThicknessProfile();

    signals:
        void profileChanged(ThicknessProfile* sender);
        void profileReleased(ThicknessProfile* sender);

        void mirrored();

    public slots:
        void setThicknessRatio(qreal profileRatio);

    private:
        void attachSignals(patheditor::Path* path);

        void mirror();

    private slots:
        void onProfileChanged(patheditor::Path *path);
        void onProfileReleased();
    };
}

#endif // THICKNESSPROFILE_HPP
