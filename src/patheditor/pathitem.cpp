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

#include "pathitem.h"

using namespace patheditor;

PathItem::PathItem(QSharedPointer<RestrictablePoint> startPoint, QSharedPointer<RestrictablePoint> endPoint,
                   QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent, scene)
{
    setStartPoint(startPoint);
    setEndPoint(endPoint);
}

QSharedPointer<RestrictablePoint> PathItem::startPoint()
{
    return _startPoint;
}

QSharedPointer<RestrictablePoint> PathItem::endPoint()
{
    return _endPoint;
}

void PathItem::setStartPoint(QSharedPointer<RestrictablePoint> startPoint)
{
    _startPoint = startPoint;
}

void PathItem::setEndPoint(QSharedPointer<RestrictablePoint> endPoint)
{
    _endPoint = endPoint;
}

QSharedPointer<PathItem> PathItem::nextPathItem()
{
    return _nextPathItem;
}

QSharedPointer<PathItem> PathItem::prevPathItem()
{
    return _prevPathItem;
}

void PathItem::setNextPathItem(QSharedPointer<PathItem> nextPathItem)
{
    _nextPathItem = nextPathItem;
}

void PathItem::setPrevPathItem(QSharedPointer<PathItem> prevPathItem)
{
    _prevPathItem = prevPathItem;
}