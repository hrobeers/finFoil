/****************************************************************************
  
 Copyright (c) 2017, Hans Robeers
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

#ifndef PROFILE_HPP
#define PROFILE_HPP

#include "patheditor/fwd/patheditorfwd.hpp"

#include <QObject>
#include <QPointF>
#include "boost/units/quantity.hpp"
#include "boost/units/systems/si/length.hpp"
#include "hrlib/mixin/identifiable.hpp"
#include "jenson.h"

namespace foillogic
{
    class Profile : public QObject, public hrlib::Identifiable
    {
        Q_OBJECT

        // read-only properties
        Q_PROPERTY(qreal thicknessRatio READ thicknessRatio)

        // read-write properties
        Q_PROPERTY(QString symmetry READ symmetryStr WRITE setSymmetryStr)
        Q_PROPERTY(patheditor::Path* topProfile READ pTopProfile WRITE pSetTopProfile)
        Q_PROPERTY(patheditor::Path* botProfile READ pBotProfile WRITE pSetBotProfile RESET pResetBotProfile)
        Q_PROPERTY(int flags READ flags WRITE setFlags RESET resetFlags)

        // optional properties
        Q_PROPERTY_UUID

        // moved properties
        Q_PROPERTY(qreal thickness WRITE pSetThickness_legacy RESET pResetThickness_legacy)

        Q_ENUMS(Symmetry)

    public:
        enum Symmetry { Symmetric, Asymmetric, Flat };

        Q_INVOKABLE explicit Profile(QObject *parent = 0);
        Q_INVOKABLE void onDeserialized();

        patheditor::Path* topProfile();
        patheditor::Path* botProfile();

        Symmetry symmetry() const;
        void setSymmetry(Symmetry symmetry);

        QPointF topProfileTop(qreal* t_top = 0) const;
        QPointF bottomProfileTop(qreal* t_top = 0) const;
        qreal pxThickness() const;
        qreal thicknessRatio() const;
        qreal topRatio() const;

        // Flags
        bool editable() const;
        void setEditable(bool editable);

        // Q_PROPERTY getters
        QString symmetryStr() const;
        patheditor::Path* pTopProfile();
        patheditor::Path* pBotProfile();
        int flags() const;

        // Q_PROPERTY setters
        void setSymmetryStr(QString symmetry);
        void pSetTopProfile(patheditor::Path *topProfile);
        void pSetBotProfile(patheditor::Path *botProfile);
        void setFlags(int flags);

        void pSetThickness_legacy(qreal thickness) { _thickness_legacy = thickness; }
        void pResetThickness_legacy() { _thickness_legacy = NAN; }

        void pResetBotProfile();
        void resetFlags();

        virtual ~Profile();

    signals:
        void profileChanged(Profile* sender);
        void profileReleased(Profile* sender);
        void symmetryChanged(int symmetry);

    public slots:

    private:
        Symmetry _symmetry;

        qshared_ptr<patheditor::Path> _topProfile;
        qunique_ptr<patheditor::Path> _botProfile;

        QPointF _topProfileTop, _botProfileTop;
        qreal t_topProfileTop, t_botProfileTop;

        int _flags;

        void initProfile();
        void attachSignals(patheditor::Path* path);

        void mirror(const patheditor::PathItem *source, patheditor::PathItem* destination);

    // Support for the v1.0 thickness property on Profile
    public:
        qreal _thickness_legacy = NAN;

    private slots:
        void onProfileChanged(patheditor::Path *path);
        void onProfileReleased();
    };
}

#endif // PROFILE_HPP
