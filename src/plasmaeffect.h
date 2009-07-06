/*
   Copyright (C) 2009 Tom Albers <tomalbers@kde.nl>

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

#ifndef PLASMAEFFECT_H
#define PLASMAEFFECT_H

#include <QWidget>

class PlasmaEffect : public QObject
{
    Q_OBJECT
public:
    PlasmaEffect( QWidget* parent );
    void setReadOnly( bool );
    bool readOnly();

protected:
    virtual bool eventFilter( QObject *obj, QEvent *event );

public slots:
    void Activate();
    void Deactivate();

private:
    bool m_readOnly;
    QWidget* m_parent;
};

#endif // PLASMAEFFECT_H
