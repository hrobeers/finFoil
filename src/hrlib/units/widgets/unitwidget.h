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

#ifndef HRLIB_UNITWIDGETBASE_H
#define HRLIB_UNITWIDGETBASE_H

#include "hrlibfwd/qtfwd.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include "../iunit.h"

namespace hrlib {
namespace units {

    class UnitWidgetBase : public QWidget
    {
        Q_OBJECT
    public:
        explicit UnitWidgetBase(QWidget *parent = 0) : QWidget(parent) {}

    signals:
        void valueChanged(hrlib::units::IUnit *value);

    protected:
        virtual void onEditImplementation(double displayValue) = 0;
        void emitValueChanged(hrlib::units::IUnit *value) { emit valueChanged(value); }

        void connectValueChanged(QDoubleSpinBox *widget)
            { connect(widget, SIGNAL(valueChanged(double)), this, SLOT(onEdit(double))); }

    public slots:
        void onEdit(double displayValue) { onEditImplementation(displayValue); }
    };

    template<class UnitType>
    class UnitWidget : public UnitWidgetBase
    {
    private:
        bool _initialized;
        QLabel* _unitLabel;

    protected:
        UnitType _value;

        virtual QWidget* valueWidget() = 0;
        virtual void onValueChange(UnitType &newValue) = 0;

        virtual void onEditImplementation(double displayValue)
        {
            _value.setValue(displayValue);
            emitValueChanged(&_value);
        }

        virtual void showEvent(QShowEvent *)
        {
            if (!_initialized)
            {
                // Layout
                QHBoxLayout *layout = new QHBoxLayout();

                layout->addWidget(valueWidget());
                layout->addWidget(_unitLabel);

                // Remove margins for blending in with standard edit widgets
                layout->setContentsMargins(0, 0, 0, 0);

                this->setLayout(layout);
                _initialized = true;
            }
        }

    public:
        explicit UnitWidget(QWidget *parent = 0) :
            UnitWidgetBase(parent)
        {
            _initialized = false;
            _unitLabel = new QLabel();
        }

        void setValue(UnitType &newValue)
        {
            _value = newValue;
            _unitLabel->setText(_value.unitSymbol());
            onValueChange(_value);
        }

        virtual void setReadOnly(bool readOnly) = 0;
    };

} // namespace units
} // namespace hrlib

#endif // HRLIB_UNITWIDGETBASE_H
