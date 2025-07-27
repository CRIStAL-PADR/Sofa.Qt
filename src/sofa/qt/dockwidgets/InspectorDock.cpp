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
#include "InspectorDock.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBox>
#include <QPushButton>
#include <sofa/qt/DataWidget.h>

#include <sofa/simulation/Node.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/qt/QDisplayDataWidget.h>
#include <sofa/core/objectmodel/DataFileName.h>

namespace sofa::qt
{

InspectorDock::InspectorDock(QWidget* parent) : QDockWidget(parent)
{
    setupUi(this);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setMinimumWidth(400);
}

void InspectorDock::setCurrentSelection(const std::set<sofa::core::objectmodel::Base::SPtr>& bases)
{
    m_currentBases = bases;

    if(m_currentBases.empty())
        return;

    updateContentFromBase((*m_currentBases.begin()).get());
}

void clearLayout(QLayout *layout) {
    if (!layout)
        return;

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->setParent(nullptr); // Optionnel, utile si le widget est affiché ailleurs
            widget->deleteLater();      // Nettoyage asynchrone (meilleur avec Qt)
        }
        else if (QLayout *childLayout = item->layout()) {
            clearLayout(childLayout);   // Appel récursif
            delete childLayout;
        }
        delete item;
    }
}

void InspectorDock::updateContentFromBase(sofa::core::objectmodel::Base* base)
{
    std::vector<std::string> ordering={"Info", "Transform", "Properties"};

    // Group data by their group name, if no group is given the data is in the group named "Properties"
    std::map<std::string, std::vector<sofa::core::objectmodel::BaseData*>> groups;
    for(auto& data : base->getDataFields())
    {
        auto groupName = data->getGroup();
        if(groupName.empty())
            groupName = "Properties";

        if(!groups.contains(groupName) )
        {
            groups[groupName] = {};
        }
        groups[groupName].emplace_back(data);
    }

    for(auto [groupName, data] : groups)
    {
        if(std::find(ordering.begin(), ordering.end(), groupName)==ordering.end())
            ordering.emplace_back(groupName);
    }

    clearLayout(verticalLayout);
    for(auto groupName : ordering)
    {
        if(groups.contains(groupName))
        {
            auto& datasInGroup = groups[groupName];

            QGroupBox* box = new QGroupBox(QString::fromStdString(groupName), this);
            QFormLayout* formLayout = new QFormLayout(box);
            box->setLayout(formLayout);
            box->setCheckable(true);
            box->setMaximumWidth(350);
            verticalLayout->addWidget(box);
            verticalLayout->setAlignment(Qt::AlignTop);

            formLayout->setLabelAlignment(Qt::AlignLeft |Qt::AlignVCenter);
            formLayout->setAlignment(Qt::AlignLeft |Qt::AlignVCenter);
            formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

            QObject::connect(box, &QGroupBox::toggled, [box](bool checked) {
                if(checked)
                    box->setMaximumHeight(box->layout()->totalSizeHint().height());
                else
                    box->setMaximumHeight(20);
            });

            for(auto data : datasInGroup)
            {
                ModifyObjectFlags flags;
                flags.setFlagsForSofa();

                DataWidget::CreatorArgument dwarg;
                dwarg.name =  data->getName();
                dwarg.data = data;
                dwarg.parent = this;
                dwarg.readOnly = true;
                if( dynamic_cast<core::objectmodel::DataFileName*>(data) != nullptr )
                {
                    // a bit of a hack for DataFileName widgets.
                    // A custom widget is used by default if we run this code from the Modeler

                    std::string widgetName=data->getWidget();
                    if( widgetName.empty() )
                    {
                        data->setWidget("widget_filename");
                    }
                }

                auto dataWidget = DataWidget::CreateDataWidget(dwarg);
                formLayout->addRow(QString::fromStdString(data->getName()), dataWidget);
            }
        }
    }
}

} // namespace sofa::qt
