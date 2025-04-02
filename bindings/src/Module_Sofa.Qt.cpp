/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2021 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/

#include <pybind11/pybind11.h>

#include <sofa/qt/init.h>
#include <sofa/qt/qt.conf.h>

#include <sofa/helper/logging/Messaging.h>
#include <sofa/helper/Utils.h>

namespace py { using namespace pybind11; }

namespace sofa::qt::python3
{

PYBIND11_MODULE(SofaQt, m)
{
    std::string sofaPrefixAbsolute = sofa::helper::Utils::getSofaPathPrefix();
    std::string inputFilepath = FileSystem::cleanPath(sofaPrefixAbsolute + "/bin/qt.conf");
    bool success = sofa::qt::loadQtConfWithCustomPrefix(inputFilepath, sofaPrefixAbsolute);
    if(success)
    {
        msg_info("Sofa.Gui") << "Loaded qt.conf from " << inputFilepath << " customized with Prefix = " << sofaPrefixAbsolute;
    }
    else
    {
        msg_warning("Sofa.Gui") << "Failed loading and/or customizing qt.conf from " << inputFilepath;

        std::cout << "qt_resource_data:" << std::endl;
        for (auto d : qt_resource_data)
        {
           std::cout << d;
        }
        std::cout << std::endl;
    }
    
    sofa::qt::init();
}

} // namespace sofa::qt::python3
