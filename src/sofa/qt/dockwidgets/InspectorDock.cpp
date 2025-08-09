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

#include <fstream>

namespace sofa::qt
{


InspectorDock::InspectorDock(QWidget* parent) : QDockWidget(parent)
{
    setupUi(this);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setMinimumWidth(400);

    std::ifstream f{helper::system::DataRepository.getFile("styles/inspector/default.json")};
    itemStyle = nlohmann::json::parse(f);
}

ItemStyle::ItemStyle(){ data = {nullptr}; }
ItemStyle::~ItemStyle(){}

ItemStyle::ItemStyle(const ItemStyle& style)
{
    data = style.data;
    if( style.readonly.has_value() )
        readonly = style.readonly;
    if(style.group.has_value())
        group = style.group;
    if(style.priority.has_value())
        priority = style.priority;
    if(style.show.has_value())
        show = style.show;
}

ItemStyle::ItemStyle(const ItemStyle& style, const ItemStyle& parentStyle)
{
    data = style.data;
    if( style.readonly.has_value() )
        readonly = style.readonly;
    else
        readonly = parentStyle.readonly;

    if(style.group.has_value())
        group = style.group;
    else
        group = parentStyle.group;

    if(style.priority.has_value())
        priority = style.priority;
    else
        priority = parentStyle.priority;

    if(style.show.has_value())
        show = style.show;
    else
        show = parentStyle.show;
}

std::map<std::string, ItemStyle> InspectorDock::getDataStylesFor(std::string className)
{
    std::map<std::string, ItemStyle> datastyles;

    ItemStyle defaultStyle;
    defaultStyle.data = nullptr;
    defaultStyle.group = "Properties";
    defaultStyle.readonly = true;
    defaultStyle.show = true;
    defaultStyle.priority = 1;

    datastyles["name"] = defaultStyle;

    return datastyles;
}

ItemStyle InspectorDock::getDataStyle(const std::string& dataName,
                                      const std::map<std::string, ItemStyle>& allDataStyles, const ItemStyle& parentStyle)
{
    if(allDataStyles.contains(dataName))
        return ItemStyle(allDataStyles.at(dataName), parentStyle);
    return ItemStyle{parentStyle};
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
    std::vector<std::string> ordering={"Info", "Properties", "States", "Forces", "Visualization", "Transformation"};

    // search for style matching the class Name
    std::map<std::string, ItemStyle> styles = getDataStylesFor(base->getClassName());

    ItemStyle defaultStyle;
    defaultStyle.data = nullptr;
    defaultStyle.group = "Properties";
    defaultStyle.readonly = true;
    defaultStyle.show = true;
    defaultStyle.priority = 1;

    // Compute the style for each data
    std::map<sofa::core::objectmodel::BaseData*, ItemStyle> dataStyles;
    for(auto& data : base->getDataFields())
    {
        // Get per class style
        ItemStyle classStyle = getDataStyle(data->getName(), styles, defaultStyle);

        // Get per instance style
        ItemStyle instanceStyle {classStyle};
        instanceStyle.data = data ;
        auto groupName = data->getGroup();
        if(!groupName.empty())
            instanceStyle.group = data->getGroup();

        dataStyles[data] = instanceStyle;
    }

    // Group data by their group name, if no group is given the data is in the group named "Properties"
    std::map<std::string, std::vector<sofa::core::objectmodel::BaseData*>> groups;
    for(auto& data : base->getDataFields())
    {
        auto style = dataStyles[data];
        auto groupName = style.group.value();

        if(!groups.contains(groupName) )
        {
            groups[groupName] = {};
        }

        if(style.show.value())
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
                if(checked){
                    for (int i = 0; i < box->layout()->count(); ++i) {
                        if( (i/2) % 2 )
                        {
                            QLayoutItem* label = box->layout()->itemAt(i);
                            QLayoutItem* content = box->layout()->itemAt(i+1);
                            label->widget()->show(); // cache le widget
                            content->widget()->show(); // cache le widget
                        }
                    }
                }else{
                    //box->setMaximumHeight(20);
                    for (int i = 0; i < box->layout()->count(); ++i) {
                        if( (i/2) % 2 )
                        {
                            QLayoutItem* label = box->layout()->itemAt(i);
                            QLayoutItem* content = box->layout()->itemAt(i+1);
                            label->widget()->hide(); // cache le widget
                            //content->widget()->hide(); // cache le widget
                        }
                    }
                }
            });

            for(auto data : datasInGroup)
            {
                auto& dataStyle = dataStyles[data];

                ModifyObjectFlags flags;
                flags.setFlagsForSofa();

                DataWidget::CreatorArgument dwarg;
                dwarg.parent = this;
                dwarg.data = dataStyle.data;
                dwarg.name =  dataStyle.data->getName();
                dwarg.readOnly = dataStyle.readonly.value();
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
