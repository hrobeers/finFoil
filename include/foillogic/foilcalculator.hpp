/****************************************************************************
  
 Copyright (c) 2013, Hans Robeers
 All rights reserved.
 
 This code is distributed under the GNU LGPL version 2.1.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 
****************************************************************************/

#ifndef FOILCALCULATOR_HPP
#define FOILCALCULATOR_HPP

#include "foillogic/fwd/foillogicfwd.hpp"

#include <QObject>
#include <QThreadPool>
#include <memory>
#include <QPainterPath>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/area.hpp>

namespace foillogic
{
    class FoilCalculator : public QObject
    {
        Q_OBJECT
    public:
        explicit FoilCalculator(Foil* foil);

        Foil* foil();
        void setFoil(Foil* foil);

        QList<qreal> contourThicknesses() const;
        void setContourThicknesses(QList<qreal> thicknesses);
        void setEquidistantContours(int contourCount);
        QList<std::shared_ptr<QPainterPath> > topContours();
        QList<std::shared_ptr<QPainterPath> > bottomContours();

        void calculate(bool fastCalc);
        bool calculated() const;
        void recalculateArea();

        virtual ~FoilCalculator() {}

    signals:
        void foilCalculated(FoilCalculator* sender);

    public slots:

    private:
        bool _calculated;
        QThreadPool _tPool;

        Foil* _foil;

        QList<qreal> _contourThicknesses;
        QList<std::shared_ptr<QPainterPath> > _topContours;
        QList<std::shared_ptr<QPainterPath> > _botContours;

        bool inProfileSide(qreal thicknessPercent, foillogic::Side::e side);

    private slots:
        void foilChanged();
        void foilReleased();
    };

    class AreaSweepCalculator : public QRunnable
    {
    public:
        explicit AreaSweepCalculator(Foil* foil);

        virtual void run();

    private:
        Foil* _foil;
    };
}

#endif // FOILCALCULATOR_HPP
