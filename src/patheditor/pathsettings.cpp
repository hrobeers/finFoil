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

#include "pathsettings.h"

using namespace patheditor;

PathSettings::PathSettings()
{
    _lineWidth = 2;
    _controlLineWidth = 1;
    _handleSize = 10;

    // linePen
    _linePen.setWidth(_lineWidth);

    // controlLinePen
    _controlLinePen.setWidth(_controlLineWidth);

    // pointBrush
    _pointBrush.setStyle(Qt::SolidPattern);
    QColor blue(Qt::blue);
    blue.setAlpha(100);
    _pointBrush.setColor(blue);

    // controlPointBrush
    _controlPointBrush.setStyle(Qt::SolidPattern);
    QColor red(Qt::red);
    red.setAlpha(100);
    _controlPointBrush.setColor(red);
}

PathSettings PathSettings::Default()
{
    return PathSettings();
}

int &PathSettings::handleSize()
{
    return _handleSize;
}

QPen &PathSettings::linePen()
{
    return _linePen;
}

QPen &PathSettings::controlLinePen()
{
    return _controlLinePen;
}

QBrush &PathSettings::pointBrush()
{
    return _pointBrush;
}

QBrush &PathSettings::controlPointBrush()
{
    return _controlPointBrush;
}
