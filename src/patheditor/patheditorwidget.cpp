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

#include "patheditor/patheditorwidget.hpp"

#include <deque>
#include <QLayout>
#include <QList>
#include <QPushButton>
#include "patheditor/patheditorview.hpp"
#include "patheditor/linerestrictor.hpp"
#include "patheditor/pointrestrictor.hpp"
#include "patheditor/editablepath.hpp"
#include "patheditor/path.hpp"

#define featureEnabled(feat) (_enabledFeatures & feat)

using namespace patheditor;

PathEditorWidget::PathEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    // init member variables
    _scene = new QGraphicsScene(this);
    _view = new PathEditorView(_scene, this);
    _mainLayout = new QVBoxLayout(this);

    _enabledFeatures = Features::None;

    QPointF origin(0,0);
    _originRestrictor.reset(new PointRestrictor(origin));

    QPointF right(1,0);
    _horizontalAxisRestrictor.reset(new LineRestrictor(origin, right));

    QPointF under(0,1);
    _verticalAxisRestrictor.reset(new LineRestrictor(origin, under));

    // view options
    _view->setRenderHint(QPainter::Antialiasing);

    // zoom in
    auto btnuser = new QPushButton(this);
    btnuser->setGeometry(QRect(0,0,30,30));
    btnuser->setText("-");
    _scene->addWidget(btnuser);
    connect(btnuser, SIGNAL(clicked()), this, SLOT(onZoomOut()), Qt::UniqueConnection);
    // zoom out
    btnuser = new QPushButton(this);
    btnuser->setGeometry(QRect(35,0,30,30));
    btnuser->setText("+");
    _scene->addWidget(btnuser);
    connect(btnuser, SIGNAL(clicked()), this, SLOT(onZoomIn()), Qt::UniqueConnection);

    // layout
    _mainLayout->addWidget(_view);
    this->setLayout(_mainLayout);

    connect(this->scene(), SIGNAL(sceneRectChanged(QRectF)), this, SLOT(onSceneRectChanged(QRectF)));
}

void PathEditorWidget::onZoomIn() {
  int pixelD = 20;
  qreal scaleFactor = 1 + ((qreal)pixelD / (qreal)this->height()) / 2;
  _view->scale(scaleFactor, scaleFactor);
}
void PathEditorWidget::onZoomOut() {
  int pixelD = -20;
  qreal scaleFactor = 1 + ((qreal)pixelD / (qreal)this->height()) / 2;
  _view->scale(scaleFactor, scaleFactor);
}

QGraphicsScene *PathEditorWidget::scene()
{
    return _scene;
}

void PathEditorWidget::addPath(EditablePath* path)
{
    connect(path, SIGNAL(pointRemove(PathPoint*, EditablePath*)),
            this, SLOT(onPointRemove(PathPoint*, EditablePath*)), Qt::UniqueConnection);
    connect(path, SIGNAL(pointSplit(PathPoint*, EditablePath*)),
            this, SLOT(onPointSplit(PathPoint*, EditablePath*)), Qt::UniqueConnection);
    connect(path, SIGNAL(pointPathTypeToggle(PathPoint*, EditablePath*)),
            this, SLOT(onPointPathTypeToggle(PathPoint*, EditablePath*)), Qt::UniqueConnection);
    this->scene()->addItem(path);
}

#include<patheditor/pathitem.hpp>
void PathEditorWidget::onPointRemove(PathPoint* toRemove, EditablePath* sender)
{
  _scene->removeItem(sender);
  Path* path = sender->path();
  path->disconnectAll();

  auto newPath = new patheditor::Path();
  for (auto pi : path->pathItems())
    if (pi->endPoint().get() != toRemove)
      newPath->append(pi->clone());

  emit pathChange(newPath);
}

template<typename Tpnt>
struct pnt_access {
  static constexpr auto x(const Tpnt& p) { return p.x(); }
  static constexpr auto y(const Tpnt& p) { return p.y(); }
};
template<typename Tpath, typename Tpnt = typename Tpath::value_type, typename Tpa = pnt_access<Tpnt>>
std::array<std::deque<Tpnt>,2> casteljau(const Tpath& p, double t) {
  if (p.size()==0)
    return std::array<std::deque<Tpnt>,2>();

  std::deque<Tpnt> sub;
  auto it = p.cbegin();
  Tpnt prev = *it;
  while (p.cend() != ++it) {
    sub.push_back(Tpnt {
                        (Tpa::x(*it) - Tpa::x(prev))*t + Tpa::x(prev),
                        (Tpa::y(*it) - Tpa::y(prev))*t + Tpa::y(prev)
      });
    prev = *it;
  }

  auto retval = casteljau(sub, t);
  retval.front().push_front(p.front());
  retval.back().push_back(p.back());

  return retval;
}
#include <patheditor/pathpoint.hpp>
#include <patheditor/controlpoint.hpp>
#include<patheditor/line.hpp>
void PathEditorWidget::onPointSplit(PathPoint* toSplit, EditablePath* sender)
{
  _scene->removeItem(sender);
  Path* path = sender->path();
  path->disconnectAll();

  auto newPath = new patheditor::Path();
  for (auto pi : path->pathItems())
    if (pi->endPoint().get() == toSplit) {
      auto r = casteljau(pi->points(),0.5);
      auto first = pi->clone();
      auto second = pi->clone();
      if (r[0].size() == 4 && r[1].size() == 4) {
        first->controlPoints().front()->setPos(r[0][1].x(), r[0][1].y());
        first->controlPoints().back()->setPos(r[0][2].x(), r[0][2].y());
        first->endPoint()->setPos(r[0][3].x(), r[0][3].y());
        second->controlPoints().front()->setPos(r[1][1].x(), r[1][1].y());
        second->controlPoints().back()->setPos(r[1][2].x(), r[1][2].y());
        second->endPoint()->setPos(r[1][3].x(), r[1][3].y());
        newPath->append(first);
        newPath->append(second);
      }
      if (r[0].size() == 2 && r[1].size() == 2) {
        first->endPoint()->setPos(r[0][1].x(), r[0][1].y());
        second->endPoint()->setPos(r[1][1].x(), r[1][1].y());
        newPath->append(std::shared_ptr<PathItem>(new Line(first->startPoint(), first->endPoint())));
        newPath->append(std::shared_ptr<PathItem>(new Line(second->startPoint(), second->endPoint())));
      }
    }
    else
      newPath->append(pi->clone());

  emit pathChange(newPath);
}

#include<patheditor/cubicbezier.hpp>
void PathEditorWidget::onPointPathTypeToggle(PathPoint* toToggle, EditablePath* sender)
{
  _scene->removeItem(sender);
  Path* path = sender->path();
  path->disconnectAll();

  auto newPath = new patheditor::Path();
  for (auto pi : path->pathItems())
    if (pi->endPoint().get() == toToggle) {
      auto clone = pi->clone();
      if (pi->controlPoints().size())
        newPath->append(std::shared_ptr<PathItem>(new Line(clone->startPoint(), clone->endPoint())));
      else
        newPath->append(std::shared_ptr<PathItem>(new CubicBezier(clone->startPoint(), clone->endPoint())));
    }
    else
      newPath->append(pi->clone());

  emit pathChange(newPath);
}

void PathEditorWidget::addGraphicsItem(QGraphicsItem *item)
{
    this->scene()->addItem(item);
}

void PathEditorWidget::clear()
{
    this->scene()->clear();

    // Re-enableFeatures
    QFlags<Features::e> prevEnabledFeatures = _enabledFeatures;
    _enabledFeatures = Features::None;
    enableFeatures(prevEnabledFeatures);
}

void PathEditorWidget::enableFeatures(QFlags<Features::e> features)
{
    if (features.testFlag(Features::HorizontalAxis) && !_enabledFeatures.testFlag(Features::HorizontalAxis))
    {
        _horizontalAxis = new QGraphicsLineItem(this->scene()->sceneRect().left(), 0,
                                                this->scene()->sceneRect().right(), 0);
        this->scene()->addItem(_horizontalAxis);

        _enabledFeatures |= Features::HorizontalAxis;
    }

    if (features.testFlag(Features::VerticalAxis) && !_enabledFeatures.testFlag(Features::VerticalAxis))
    {
        _verticalAxis = new QGraphicsLineItem(0, this->scene()->sceneRect().bottom(),
                                              0, this->scene()->sceneRect().top());
        this->scene()->addItem(_verticalAxis);

        _enabledFeatures |= Features::VerticalAxis;
    }

    if (features.testFlag(Features::DragImageHereText) && !_enabledFeatures.testFlag(Features::DragImageHereText))
    {
        QFont font;
        font.setWeight(QFont::Light);
        this->scene()->addText(tr("Drag an image here"), font);

        _enabledFeatures |= Features::DragImageHereText;
    }
}

std::shared_ptr<Restrictor> PathEditorWidget::originRestrictor()
{
    return _originRestrictor;
}

std::shared_ptr<Restrictor> PathEditorWidget::horizontalAxisRestrictor()
{
    return _horizontalAxisRestrictor;
}

std::shared_ptr<Restrictor> PathEditorWidget::verticalAxisRestrictor()
{
    return _verticalAxisRestrictor;
}

void PathEditorWidget::setGridUnitSize(qreal pxPerUnit)
{
    _view->setPixelsPerUnit(pxPerUnit);
}

void PathEditorWidget::onSceneRectChanged(const QRectF &rect)
{
    if (featureEnabled(Features::HorizontalAxis))
        _horizontalAxis->setLine(rect.left() + 1, 0, rect.right() - 1, 0);
    if (featureEnabled(Features::VerticalAxis))
        _verticalAxis->setLine(0, rect.bottom() - 1, 0, rect.top() + 1);
}

void PathEditorWidget::setImage(const QString &path)
{
  _view->setImage(path);
}
