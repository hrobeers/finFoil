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

#include "foillogicfwd/foillogicfwd.h"
#include "patheditorfwd/patheditorfwd.h"

#include <QObject>
#include <memory>
#include "mixin/identifiable.hpp"
#include "mixin/historical.hpp"
#include "jenson.h"

namespace foillogic
{
    class Foil : public QObject, public hrlib::Identifiable, public hrlib::THistorical<5>
    {
        Q_OBJECT

        // read-write properties
        Q_PROPERTY(foillogic::Outline* outline READ outline WRITE pSetOutline)
        Q_PROPERTY(foillogic::Profile* profile READ profile WRITE pSetProfile)
        Q_PROPERTY(foillogic::ThicknessProfile* thickness READ thickness WRITE pSetThickness)

        // optional properties
        Q_PROPERTY(int layerCount READ layerCount WRITE setLayerCount RESET resetLayerCount)
        Q_PROPERTY_UUID
        Q_PROPERTY_HISTORY_STRLIST

    public:
        Q_INVOKABLE explicit Foil(QObject *parent = 0);
        Q_INVOKABLE void onDeserialized();

        // SI paths
        std::unique_ptr<patheditor::IPath> outlineSI();
        std::unique_ptr<patheditor::IPath> topProfileSI();
        std::unique_ptr<patheditor::IPath> botProfileSI();
        std::unique_ptr<patheditor::IPath> topThicknessSI();
        std::unique_ptr<patheditor::IPath> botThicknessSI();

        // Q_PROPERTY getters
        Outline* outline() { return _outline.get(); }
        Profile* profile() { return _profile.get(); }
        ThicknessProfile* thickness() { return _thickness.get(); }
        int layerCount() { return _layerCount; }

        // Q_PROPERTY setters
        void pSetOutline(Outline *outline);
        void pSetProfile(Profile *profile);
        void pSetThickness(ThicknessProfile *thickness);
        void setLayerCount(int layerCount);

        void resetLayerCount();

        virtual ~Foil();

    signals:
        void foilChanged(Foil* sender);
        void foilReleased(Foil* sender);

    public slots:

    private:
        std::unique_ptr<foillogic::Outline> _outline;
        std::unique_ptr<foillogic::Profile> _profile;
        std::unique_ptr<foillogic::ThicknessProfile> _thickness;
        int _layerCount;

        void initOutline();
        void initProfile();
        void initThickness();

        void connectOutline();
        void connectProfile();
        void connectThickness();

    private slots:
        void onFoilChanged();
        void onFoilReleased();

        void onProfileChanged();
    };
}

#endif // FOIL_H
