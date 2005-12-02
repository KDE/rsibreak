/* This file is part of the KDE project
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#ifndef RSIPOPUP_H
#define RSIPOPUP_H

#include <kpassivepopup.h>

class QLabel;
class QPushButton;
class KProgress;

/**
 * @class RSIPopup
 * This is a KPassivePopup which some extra features.
 * It's shown for example when the user should relax for a
 * couple of seconds.
 * @author Bram Schoenmakers <bramschoenmakers@kde.nl>
 */
class RSIPopup : public KPassivePopup
{
  Q_OBJECT
  public:
    /** Constructor */
    RSIPopup( QWidget *parent = 0, const char *name = 0 );
    /** Destructor */
    ~RSIPopup();

  public slots:
    /**
      Shows this popup with the message that the user should rest @p n seconds.
    */
    void relax( int n );

  signals:
    /** Ask the main widget to lock down the desktop. */
    void lock();

  protected:
    /**
      Changes the background color for 0.5 second. The background is restored
      with unflash()
    */
    void flash();

    virtual void mouseReleaseEvent( QMouseEvent * );

  protected slots:
    /** Restores background color after a flash() */
    void unflash();

  private:
    QLabel *m_message;
    KProgress *m_progress;
    QPushButton *m_lockbutton;

    /**
      Counts how many times a request for relax resets due to detected activity
    */
    int m_resetcount;
    QTimer *m_flashtimer;
};

#endif /* RSIPOPUP_H */
