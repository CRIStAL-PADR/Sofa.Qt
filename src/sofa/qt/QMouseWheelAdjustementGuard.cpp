/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program. If not, see <http://www.gnu.org/licenses/>.              *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <QEvent>
#include <QWidget>
#include "QMouseWheelAdjustementGuard.h"

namespace sofa::qt
{

QMouseWheelAdjustmentGuard::QMouseWheelAdjustmentGuard(QObject *parent) : QObject(parent)
{
}

bool QMouseWheelAdjustmentGuard::eventFilter(QObject *o, QEvent *e)
{
    const QWidget* widget = static_cast<QWidget*>(o);
    if (e->type() == QEvent::Wheel && widget && !widget->hasFocus())
    {
        e->ignore();
        return true;
    }

    return QObject::eventFilter(o, e);
}

} //namespace sofa::qt
