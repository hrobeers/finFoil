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

#ifndef OUTLINE_H
#define OUTLINE_H

#include "patheditorfwd/patheditorfwd.h"

#include <QObject>
#include <memory>
#include "boost/units/quantity.hpp"
#include "boost/units/systems/si/length.hpp"
#include "boost/units/systems/si/area.hpp"
#include "boost/units/systems/si/plane_angle.hpp"
#include "serialization/serialization.h"

namespace foillogic
{
    class Outline : public QObject
    {
        Q_OBJECT

        // read-only properties
        Q_PROPERTY(qreal area READ pArea)
        Q_PROPERTY(qreal sweep READ pSweep)

        // read-write properties
        Q_PROPERTY(patheditor::Path* path READ pPath)
        Q_PROPERTY(qreal height READ pHeight WRITE pSetHeight)

    public:
        Q_INVOKABLE explicit Outline(QObject *parent = 0);

        std::shared_ptr<patheditor::Path> path();

        boost::units::quantity<boost::units::si::length, qreal> height();
        boost::units::quantity<boost::units::si::area, qreal> area();
        boost::units::quantity<boost::units::si::plane_angle, qreal> sweep();

        void setHeight(boost::units::quantity<boost::units::si::length, qreal> height);
        void setArea(boost::units::quantity<boost::units::si::area, qreal> area);
        void setSweep(boost::units::quantity<boost::units::si::plane_angle, qreal> sweep);

        // Q_PROPERTY getters
        patheditor::Path* pPath();
        qreal pHeight() { return height().value(); }
        qreal pArea() { return area().value(); }
        qreal pSweep() { return sweep().value(); }

        // Q_PROPERTY setters
        void pSetHeight(qreal height) { setHeight(height * boost::units::si::meter); }

    signals:
        void outlineChanged(Outline* sender);
        void outlineReleased(Outline* sender);

    public slots:

    private:
        std::shared_ptr<patheditor::Path> _outline;

        boost::units::quantity<boost::units::si::length, qreal> _height;
        boost::units::quantity<boost::units::si::area, qreal> _area;
        boost::units::quantity<boost::units::si::plane_angle, qreal> _sweep;

        void initPath();

    private slots:
        void onOutlineChanged();
        void onOutlineReleased();
    };
}
SERIALIZABLE(foillogic::Outline, outline)

#endif // OUTLINE_H
