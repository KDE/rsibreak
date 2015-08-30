/*
   Copyright (C) 2005-2006 Tom Albers <toma@kde.org>
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

#ifndef RSIWIDGET_H
#define RSIWIDGET_H

#include "rsitimer.h"

class RSIDock;
class RSIRelaxPopup;
class BreakBase;

class QLabel;

/**
 * @class RSIObject
 * This controls all RSIBreak components
 * @author Tom Albers <toma.org>
 */
class RSIObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO( "D-Bus Interface", "org.rsibreak.rsiwidget" )

public:

    enum Effects {  SimpleGray = 0, Plasma, SlideShow, Popup };

    /**
     * Constructor
     * @param parent Parent Widget
     * @param name Name
     */
    explicit RSIObject( QWidget *parent = 0 );

    /**
     * Destructor
     */
    ~RSIObject();

    /**
     * Access to the timer
     */
    RSITimer* timer() {
        return m_timer;
    };

private slots:
    void slotWelcome();
    void slotLock();
    void minimize();
    void maximize();
    void setCounters( int );
    void updateIdleAvg( double );
    void readConfig();
    void tinyBreakSkipped();
    void bigBreakSkipped();

protected:
    /** Sets appropriate icon in tooltip and docker. */
    void setIcon( int );

private:
    void findImagesInFolder( const QString& folder );
    void loadImage();
    void startTimer( bool idle );

    RSIDock*        m_tray;
    RSITimer*       m_timer;
    BreakBase*      m_effect;

    QLabel*         m_tool;

    bool            m_useImages;

    bool            m_usePlasma;
    bool            m_usePlasmaRO;

    RSIRelaxPopup*  m_relaxpopup;

    QString         m_currentIcon;


    /* Available through D-Bus */
public Q_SLOTS:
    void resume() {
        timer()->slotSuspended( false );
    };
    void suspend() {
        timer()->slotSuspended( true );
    };
    int idleTime() {
        return timer()->idleTime();
    };
    int tinyLeft() {
        return timer()->tinyLeft();
    };
    int bigLeft() {
        return timer()->bigLeft();
    };
    QString currentIcon() {
        return m_currentIcon;
    };
};

#   endif
