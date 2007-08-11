/*
   Copyright (C) 2007 Tom Albers <tomalbers@kde.nl>

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

#ifndef BOXDIALOG_H
#define BOXDIALOG_H

#include <QWidget>
class QLabel;
#include <KDialog>

/**
 * @class BoxDialog
 * This widget is shown on top of the widgets used during the breaks.
 * @author Tom Albers <tomalbers@kde.nl>
 */
class BoxDialog : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit BoxDialog( QWidget *parent = 0, Qt::WFlags flags = 0 );

    /**
     * Destructor
     */
    ~BoxDialog();

    void showMinimize( bool ok );
    void disableShortcut( bool ok );
    void setLabel( const QString& );
    void reject() {
        m_dialog->reject();
    };

public slots:
    void showDialog();

signals:
    void skip();
    void lock();

private:
    void loadDialog();

    KDialog*        m_dialog;
    QLabel*         m_label;
    bool            m_showMinimize;
    bool            m_disableShort;


};

#   endif
