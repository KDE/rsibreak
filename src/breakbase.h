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

#ifndef BREAKBASE_H
#define BREAKBASE_H

#include <QObject>

class BreakControl;

class BreakBase : public QObject
{
    Q_OBJECT

public:
    BreakBase( QObject* parent );
    ~BreakBase();
    virtual void setReadOnly( bool );
    virtual bool readOnly() const;
    virtual void activate();
    virtual void deactivate();
    virtual void setLabel( const QString& );
    void showMinimize( bool );
    void disableShortcut( bool disable );

protected:
    virtual bool eventFilter( QObject *obj, QEvent *event );

signals:
    void skip();
    void lock();

private:
    BreakControl* m_breakControl;
    QObject* m_parent;
    bool m_readOnly;
    bool m_disableShortcut;
};

#endif // BREAKBASE_H
