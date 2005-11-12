/* This file is part of the KDE project
   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>

   The parts for idle detection is based on
   kdepim's karm idletimedetector.cpp/.h

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

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>

#include "rsitimer.h"

// The order here is important, otherwise Qt headers are preprocessed into garbage.... :-(

#include "config.h"     // HAVE_LIBXSS
#ifdef HAVE_LIBXSS      // Idle detection.
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/extensions/scrnsaver.h>
  #include <fixx11h.h>
#endif // HAVE_LIBXSS

RSITimer::RSITimer( QObject *parent, const char *name )
    : QObject( parent, name ), m_idleLong( false ), m_targetReached( false ),
      m_needBreak( 0 ), m_idleIndex( 0 )
{
    kdDebug() << "Entering RSITimer::RSITimer" << endl;

#ifdef HAVE_LIBXSS      // Idle detection.
    int event_base, error_base;
    if(XScreenSaverQueryExtension(qt_xdisplay(), &event_base, &error_base))
        m_idleDetection = true;
#endif

    if (m_idleDetection)
        kdDebug() << "IDLE Detection is possible" << endl;
    else
        kdDebug() << "IDLE Detection is not possible" << endl;

    m_timer_max = new QTimer(this);
    connect(m_timer_max, SIGNAL(timeout()), SLOT(slotMaximize()));

    m_timer_min = new QTimer(this);
    connect(m_timer_min, SIGNAL(timeout()),  SLOT(slotMinimize()));

    m_normalTimer = startTimer( 1000 );

    readConfig();
    startMinimizeTimer();
}

RSITimer::~RSITimer()
{
    kdDebug() << "Entering RSITimer::~RSITimer" << endl;
}

void RSITimer::startMinimizeTimer()
{
    kdDebug() << "Entering RSITimerstart::startMinimizeTimer" << endl;

    m_timer_min->stop();
    m_targetTime = QTime::currentTime().addSecs(m_timeMinimized);
    m_timer_max->start(m_timeMinimized*1000, true);
}

int RSITimer::idleTime()
{
    int totalIdle = 0;

#ifdef HAVE_LIBXSS      // Idle detection.
    XScreenSaverInfo*  _mit_info;
    _mit_info= XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(qt_xdisplay(), qt_xrootwin(), _mit_info);
    totalIdle = (_mit_info->idle/1000);
#endif // HAVE_LIBXSS

    return totalIdle;
}

void RSITimer::breakNow( int t )
{
    kdDebug() << "Entering RSITimer::breakNow for " << t << "seconds " << endl;
    m_targetTime = QTime::currentTime().addSecs(t);
    m_timer_min->start(t*1000, true);

    emit setCounters();
    emit breakNow();
}

// -------------------------- SLOTS ------------------------//

void RSITimer::slotMaximize()
{
    kdDebug() << "Entering RSITimer::slotMaximize" << endl;

    if (m_currentInterval > 0)
        m_currentInterval--;

    m_needBreak = 0;
    m_timer_max->stop();

    int totalIdle = idleTime();
    int minNeeded;
    if ( m_currentInterval == 0 )
        minNeeded = m_bigTimeMaximized;
    else
        minNeeded = m_timeMaximized;

    kdDebug() << "BigBreak in " << m_currentInterval << "; "
            << "Idle " << totalIdle << "s; "
            << "Needed " << minNeeded << "s; "
            << "IdleLong: " << m_idleLong << "; "
            << endl;

    // If user has been idle since the last break, it will
    // get a bonus in the way that it gains a tinyBreak
    if (totalIdle >= m_timeMinimized)
    {
        m_idleLong=true;
        kdDebug() << "Next break will be delayed, "
                     "you have been idle a while now" << endl;

        m_currentInterval++; 		// give back the this one
        if (m_currentInterval < m_bigInterval)
            m_currentInterval++;	// give a bonus

        startMinimizeTimer();
        return;
    }

    // if user has been idle for at least two breaks, there is no
    // need to break immediatly, we can postpone the break
    if ( m_idleLong )
    {
        kdDebug() << "Break delayed, you have been idle for a while recently" << endl;
        m_currentInterval++;
        startMinimizeTimer();
        m_idleLong=false;
        return;
    }

    kdDebug() << "You need a break, monitoring keyboard for the right moment..." << endl;
    m_needBreak=minNeeded;
    if ( m_currentInterval == 0 )
        m_currentInterval=m_bigInterval;
}

void RSITimer::slotMinimize()
{
    kdDebug() << "Entering RSITimer::slotMinimize" << endl;
    emit minimize();
    startMinimizeTimer();
}

void RSITimer::slotStop( )
{
    kdDebug() << "Entering RSITimer::slotStop" << endl;
    emit minimize();
    m_timer_max->stop();
    m_needBreak=false;
}

void RSITimer::slotRestart( )
{
    kdDebug() << "Entering RSITimer::slotInterupted" << endl;
    // the interuption can not be considered a real break
    // only needed fot a big break of course
    if (m_currentInterval == m_bigInterval)
        m_currentInterval=0;

    emit minimize();
    startMinimizeTimer();
}


void RSITimer::slotReadConfig()
{
    kdDebug() << "Entering RSITimer::slotReadConfig" << endl;
    readConfig();
    startMinimizeTimer();
}

// ----------------------------- EVENTS -----------------------//

void RSITimer::timerEvent( QTimerEvent* )
{
    emit setCounters();

    int t = idleTime();
    if (t == 0)
        m_idleIndex++;
    else
        m_idleIndex--;
    m_idleIndexAmount++;

    int idleAvg = m_idleIndexAmount == 0 ? 0 : (int)(m_idleIndex*100 / m_idleIndexAmount);

    emit updateIdleAvg( idleAvg );

    // If we are waiting for the right time to have a break, check the idle timeout
    // and activate the break if needed...
    if ( m_needBreak > 0 )
    {
        if (!m_idleDetection)
        {
            breakNow( m_needBreak );
            m_needBreak=0;
            return;
        }

        emit relax( m_needBreak - t );

        // if user is idle for more then 5 seconds, remember that.
        if (t >= 5 || QTime::currentTime().secsTo(m_targetTime) <= -30)
            m_targetReached=true;

        kdDebug() << "Idle for: " << t << "s; "
                << "BreakFor "<< m_needBreak << "s; "
                << "TargetReached " << m_targetReached << "; "
                << "Waiting for " << QTime::currentTime().secsTo(m_targetTime) << "s"
                << endl;

        // User has been idle for 5 seconds, and wants to start working again -> break Now!
        if (m_targetReached && t < 5)
        {
            kdDebug() << "Activity detected, break!" << endl;
            breakNow( m_needBreak );
            m_targetReached=false;
            emit relax( -1 );
            m_needBreak=0;
        }

        // User has been idle for the time of the break now, this break is no longer needed!
        if (m_targetReached && t >= m_needBreak)
        {
            kdDebug() << "You have been idle for the duration of the break, thanks!" << endl;
            m_targetReached=false;
            m_needBreak=0;
            emit relax( -1 );
            startMinimizeTimer();
        }
    }
}

//--------------------------- CONFIG ----------------------------//

void RSITimer::readConfig()
{
    kdDebug() << "Entering RSITimer::readConfig" << endl;
    KConfig* config = kapp->config();

    config->setGroup("General Settings");
    m_timeMinimized = config->readNumEntry("TinyInterval", 10)*60;
    m_timeMaximized = config->readNumEntry("TinyDuration", 20);
    m_bigInterval = config->readNumEntry("BigInterval", 3);
    m_bigTimeMaximized = config->readNumEntry("BigDuration", 1)*60;
    m_currentInterval = m_bigInterval;

    if (config->readBoolEntry("DEBUG"))
    {
        kdDebug() << "Debug mode activated" << endl;
        m_timeMinimized = m_timeMinimized/60;
        m_bigTimeMaximized = m_bigTimeMaximized/60;
    }
}

#include "rsitimer.moc"
