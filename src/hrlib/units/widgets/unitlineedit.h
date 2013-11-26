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

#ifndef HRLIB_UNITLINEEDIT_H
#define HRLIB_UNITLINEEDIT_H

#include <QLineEdit>
#include "unitwidget.h"

namespace hrlib {
namespace units {

    template<class UnitType>
    class UnitLineEdit : public UnitWidget<UnitType>
    {
    private:
        QLineEdit *_lineEdit;

    public:
        explicit UnitLineEdit(QWidget *parent = 0) :
            UnitWidget<UnitType>(parent)
        {
            _lineEdit = new QLineEdit();
        }

        virtual void setReadOnly(bool readOnly)
        {
            _lineEdit->setReadOnly(readOnly);
        }

    protected:
        virtual QWidget *valueWidget()
        {
            return _lineEdit;
        }

        virtual void onValueChange(UnitType &newValue)
        {
            _lineEdit->setText(QString::number(newValue.value()));
        }
    };

} // namespace units
} // namespace hrlib

#endif // HRLIB_UNITLINEEDIT_H
