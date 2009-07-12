/*
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2007 Tom Albers <toma.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef RSISTATWIDGET_H
#define RSISTATWIDGET_H

#include "rsiglobals.h"

class QGridLayout;

class RSIStatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RSIStatWidget( QWidget *parent = 0 );
    ~RSIStatWidget();

protected:
    void addStat( RSIStat stat, QGridLayout *grid, int row );
    virtual void showEvent( QShowEvent * );
    virtual void hideEvent( QHideEvent * );
private:
    QGridLayout *mGrid;
};

#endif
