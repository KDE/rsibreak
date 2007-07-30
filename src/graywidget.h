/* This file is part of the KDE project
   Copyright (C) 2005-2007 Tom Albers <tomalbers@kde.nl>
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#ifndef GRAYWIDGET_H
#define GRAYWIDGET_H

#include <QWidget>
class QLabel;

class KDialog;
class KAction;

/**
 * @class GrayWidget
 * This widget is the main widget for RSIBreak.
 * It minimizes and maximized the widget
 * @author Tom Albers <tomalbers@kde.nl>
 */
class GrayWidget : public QWidget
{
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param parent Parent Widget
         */
        explicit GrayWidget( QWidget *parent = 0);

        /**
         * Destructor
         */
        ~GrayWidget();

        /**
         * reset
         */
        void reset();

        void setLabel( const QString& );
        void showMinimize( bool ok );
        void disableShortcut( bool ok );

    public slots:
        void slotGrayEffect();

    protected:
        virtual void paintEvent( QPaintEvent* );

    signals:
        void skip();
        void lock();

    private:
        int             m_currentY;
        KDialog*        m_dialog;
        QLabel*         m_label;
        bool            m_showMinimize;
        bool            m_disableShort;
        void showDialog();
        void loadDialog();
};

#   endif
