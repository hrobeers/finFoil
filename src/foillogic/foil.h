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
#include "path.h"
#include "boost/units/quantity.hpp"
#include "boost/units/systems/si/length.hpp"
#include "boost/units/systems/si/area.hpp"
#include "boost/units/systems/si/plane_angle.hpp"

namespace foillogic
{
    class Foil : public QObject
    {
        Q_OBJECT
    public:
        explicit Foil(QObject *parent = 0);

        std::shared_ptr<patheditor::Path> outline();
        std::shared_ptr<Profile> profile();
        std::shared_ptr<ThicknessProfile> thickness();

        boost::units::quantity<boost::units::si::length, qreal> height();
        boost::units::quantity<boost::units::si::area, qreal> area();
        boost::units::quantity<boost::units::si::plane_angle, qreal> sweep();

        void setHeight(boost::units::quantity<boost::units::si::length, qreal> height);
        void setArea(boost::units::quantity<boost::units::si::area, qreal> area);
        void setSweep(boost::units::quantity<boost::units::si::plane_angle, qreal> sweep);

        virtual ~Foil();

    signals:
        void foilChanged(Foil* sender);
        void foilReleased(Foil* sender);

    public slots:

    private:
        boost::units::quantity<boost::units::si::length, qreal> _height;
        boost::units::quantity<boost::units::si::area, qreal> _area;
        boost::units::quantity<boost::units::si::plane_angle, qreal> _sweep;

        std::shared_ptr<patheditor::Path> _outline;
        std::shared_ptr<foillogic::Profile> _profile;
        std::shared_ptr<foillogic::ThicknessProfile> _thickness;

        void initOutline();
        void initProfile();
        void initThickness();

    private slots:
        void onFoilChanged();
        void onFoilReleased();

        void onProfileChanged();
    };
}

#endif // FOIL_H
