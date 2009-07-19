/*
   Copyright (C) 2009 Tom Albers <toma@kde.org>

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

#ifndef BREAKCONTROL_H
#define BREAKCONTROL_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;

class BreakControl : public QWidget
{
    Q_OBJECT

public:
    BreakControl( QWidget*, Qt::WindowType );
    void setText( const QString& );
    void showMinimize( bool show );

protected:
    void paintEvent( QPaintEvent *event );

private slots:
    void slotLock();

signals:
    void skip();
    void lock();

private:
    QLabel* m_textLabel;
    QPushButton* m_skipButton;
    QPushButton* m_lockButton;
    QVBoxLayout* m_vbox;
};

#endif // BREAKCONTROL_H
