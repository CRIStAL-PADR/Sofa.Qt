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
#pragma once
#include <sofa/qt/config.h>
#include <ui_InspectorDock.h>
#include <sofa/core/objectmodel/Base.h>
#include <set>
#include <json.h>

namespace sofa::qt
{

class ItemStyle
{
public:
    std::optional<std::string> group;
    std::optional<int> priority;
    std::optional<bool> readonly;
    std::optional<bool> show;
    sofa::core::objectmodel::BaseData* data;

    ItemStyle();
    virtual ~ItemStyle();
    ItemStyle(const ItemStyle& style);
    ItemStyle(const ItemStyle& style, const ItemStyle& parentStyle);
};

class SOFA_QT_API InspectorDock: public QDockWidget, public Ui::InspectorDock
{
    Q_OBJECT
public:
    InspectorDock(QWidget *parent);

    void setCurrentSelection(const std::set<sofa::core::objectmodel::Base::SPtr>& base);

private:
    void updateContentFromBase(sofa::core::objectmodel::Base* base);

    std::set<sofa::core::objectmodel::Base::SPtr> m_currentBases;
    nlohmann::json itemStyle;

    std::map<std::string, ItemStyle> getDataStylesFor(std::string className);
    ItemStyle getDataStyle(const std::string &dataName,
                           const std::map<std::string, ItemStyle>& allDataStyles, const ItemStyle& parentStyle);
};

} // namespace sofa::qt
