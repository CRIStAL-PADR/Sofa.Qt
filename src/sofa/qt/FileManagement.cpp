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

#include "FileManagement.h"
#include <iostream>

namespace sofa::qt
{

#include <QDir>

QString getExistingDirectory ( QWidget* parent, const QString & dir, const char * name, const QString & caption)
{
    QFileDialog::Options options = QFileDialog::ShowDirsOnly;
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    options |= QFileDialog::DontUseSheet;
#endif
    return QFileDialog::getExistingDirectory ( parent, name?QString(name):caption, dir, options );
}

QString getOpenFileName ( QWidget* parent, const QString & startWith, const QString & filter, const char * name, const QString & caption, QString * selectedFilter )
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    QFileDialog::Options options = 0;
    options |= QFileDialog::DontUseSheet;
#else
    QFileDialog::Options options;
#endif
    return QFileDialog::getOpenFileName ( parent, name?QString(name):caption, startWith, filter, selectedFilter, options );
}

QString getSaveFileName ( QWidget* parent, const QString & startWith, const QString & filter, const char * name, const QString & caption, QString * selectedFilter )
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    QFileDialog::Options options = 0;
    options |= QFileDialog::DontUseSheet;
#else
    QFileDialog::Options options;
#endif
    return QFileDialog::getSaveFileName ( parent, name?QString(name):caption, startWith, filter, selectedFilter, options );
}

void getFilesInDirectory( const QString &p, std::vector< QString > &files, bool recursive, const std::vector< QString > &filter )
{
    QString path=p;
    if (path.endsWith("/"))
    {
        const int slash=path.indexOf('/',-1);
        path.truncate(slash);
    }
    else if (path.endsWith("\\"))
    {
        const int slash=path.indexOf('\\',-1);
        path.truncate(slash);
    }

    QDir d(path);

    d.setFilter( QDir::Dirs | QDir::Hidden | QDir::NoSymLinks );

    std::vector< QString > subDir;

    const QFileInfoList &listDirectories = d.entryInfoList();
    QStringList filters;
    for (unsigned int i=0; i<filter.size(); ++i)
        filters << filter[i];

    d.setNameFilters(filters);
    for (int j = 0; j < listDirectories.size(); ++j)
    {
        QFileInfo fileInfo=listDirectories.at(j);
        subDir.push_back(fileInfo.fileName());

    }

    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );

    const QFileInfoList &listFiles =
        d.entryInfoList();
    for (int j = 0; j < listFiles.size(); ++j)
    {
        QFileInfo fileInfo=listFiles.at(j);

        files.push_back(path+QString("/")+fileInfo.fileName());

    }

    if (recursive)
    {
        for (unsigned int i=0; i<subDir.size(); ++i)
        {
            if (subDir[i].left(1) == QString(".")) continue;
            if (subDir[i] == QString("OBJ"))       continue;

            QString nextDir=path+QString("/")+subDir[i];
            getFilesInDirectory(nextDir, files, recursive, filter);
        }
    }
}

} //namespace sofa::qt
