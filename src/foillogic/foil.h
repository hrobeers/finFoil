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
#include "boost/units/quantity.hpp"
#include "boost/units/systems/si/length.hpp"
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
        Q_PROPERTY(foillogic::ThicknessProfile* thickness READ thicknessProfile WRITE pSetThicknessProfile)
        Q_PROPERTY(qreal thick READ pThickness WRITE pSetThickness RESET pResetThickness)

        // optional properties
        Q_PROPERTY(int layerCount READ layerCount WRITE setLayerCount RESET resetLayerCount)
        Q_PROPERTY(qreal minThick READ pMinThickness WRITE pSetMinThickness RESET pResetMinThickness)
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


        boost::units::quantity<boost::units::si::length, qreal> thickness() const;
        void setThickness(boost::units::quantity<boost::units::si::length, qreal> thickness);
        boost::units::quantity<boost::units::si::length, qreal> minThickness() const;
        void setMinThickness(boost::units::quantity<boost::units::si::length, qreal> minThickness);

        // Q_PROPERTY getters
        Outline* outline() { return _outline.get(); }
        Profile* profile() { return _profile.get(); }
        ThicknessProfile* thicknessProfile() { return _thicknessProfile.get(); }
        int layerCount() { return _layerCount; }
        qreal pThickness() const { return thickness().value(); }
        qreal pMinThickness() const { return minThickness().value(); }

        // Q_PROPERTY setters
        void pSetOutline(Outline *outline);
        void pSetProfile(Profile *profile);
        void pSetThicknessProfile(ThicknessProfile *thicknessProfile); // TODO read thickness from object if it exists
        void setLayerCount(int layerCount);
        void pSetThickness(qreal thickness) { setThickness(thickness * boost::units::si::meter); }
        void pSetMinThickness(qreal minThickness) { setMinThickness(minThickness * boost::units::si::meter); }

        void resetLayerCount();
        void pResetThickness();
        void pResetMinThickness();

        virtual ~Foil();

    signals:
        void foilChanged(Foil* sender);
        void foilReleased(Foil* sender);

    public slots:

    private:
        std::unique_ptr<foillogic::Outline> _outline;
        qunique_ptr<foillogic::Profile> _profile;
        qunique_ptr<foillogic::ThicknessProfile> _thicknessProfile;
        int _layerCount;
        boost::units::quantity<boost::units::si::length, qreal> _thickness;
        boost::units::quantity<boost::units::si::length, qreal> _minThickness;

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
