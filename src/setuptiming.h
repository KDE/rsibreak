/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Copright 2005 by Tom Albers <tomalbers@kde.nl>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * ============================================================ */

#ifndef SETUPTIMING_H
#define SETUPTIMING_H

// Qt includes.

#include <qwidget.h>

class QCheckBox;
class QPushButton;
class QLineEdit;
class QFont;
class QHBox;
class QDir;
class KColorButton;
class KFileDialog;
class KIntNumInput;

class SetupTiming : public QWidget
{
    Q_OBJECT
    
public:

    SetupTiming(QWidget* parent = 0);
    ~SetupTiming();

    void applySettings();

private slots:

private:
    void readSettings();

    QLineEdit*          m_tinyInterval;
    QLineEdit*          m_tinyDuration;

    QLineEdit*          m_bigInterval;
    QLineEdit*          m_bigDuration;

};

#endif /* SETUPTIMING_H */
