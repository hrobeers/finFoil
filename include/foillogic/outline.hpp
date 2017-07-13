/****************************************************************************

 Copyright (c) 2014, Hans Robeers
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

#ifndef OUTLINE_HPP
#define OUTLINE_HPP

#include "patheditor/fwd/patheditorfwd.hpp"

#include <QObject>
#include <memory>
#include "boost/units/quantity.hpp"
#include "boost/units/systems/si/length.hpp"
#include "boost/units/systems/si/area.hpp"
#include "boost/units/systems/si/plane_angle.hpp"
#include "hrlib/mixin/identifiable.hpp"
#include "jenson.h"

namespace foillogic
{
    class Outline : public QObject, public hrlib::Identifiable
    {
        Q_OBJECT

        // read-only properties
        Q_PROPERTY(qreal area READ pArea)
        Q_PROPERTY(qreal sweep READ pSweep)

        // read-write properties
        Q_PROPERTY(patheditor::Path* path READ pPath WRITE pSetPath)
        Q_PROPERTY(qreal height READ pHeight WRITE pSetHeight)

        // optional properties
        Q_PROPERTY_UUID

    public:
        Q_INVOKABLE explicit Outline(QObject *parent = 0);

        patheditor::Path* path();

        boost::units::quantity<boost::units::si::length, qreal> height() const;
        boost::units::quantity<boost::units::si::area, qreal> area() const;
        boost::units::quantity<boost::units::si::plane_angle, qreal> sweep() const;

        void setHeight(boost::units::quantity<boost::units::si::length, qreal> height);
        void setArea(boost::units::quantity<boost::units::si::area, qreal> area);
        void setSweep(boost::units::quantity<boost::units::si::plane_angle, qreal> sweep);

        // Q_PROPERTY getters
        patheditor::Path* pPath();
        qreal pHeight() const { return height().value(); }
        qreal pArea() const { return area().value(); }
        qreal pSweep() const { return sweep().value(); }

        // Q_PROPERTY setters
        void pSetPath(patheditor::Path *path);
        void pSetHeight(qreal height) { setHeight(height * boost::units::si::meter); }

        virtual ~Outline();

    signals:
        void outlineChanged(Outline* sender);
        void outlineReleased(Outline* sender);

    public slots:

    private:
        std::unique_ptr<patheditor::Path> _path;

        boost::units::quantity<boost::units::si::length, qreal> _height;
        boost::units::quantity<boost::units::si::area, qreal> _area;
        boost::units::quantity<boost::units::si::plane_angle, qreal> _sweep;

        void initPath();
        void attachSignals(patheditor::Path* path);

    private slots:
        void onOutlineChanged();
        void onOutlineReleased();
    };
}

#endif // OUTLINE_HPP
