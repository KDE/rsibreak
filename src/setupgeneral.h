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
 *
 * ============================================================ */

#ifndef SETUPGENERAL_H
#define SETUPGENERAL_H

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
class QString;

/** 
 * @class SetupGeneral
 * These contain the general settings of RSIBreak, currently
 * that means settings for the Counter, Image Path and 
 * minimize button
 * This file is originally copied from showfoto
 * @author Gilles Caulier <caulier dot gilles at free.fr>
 * @author Tom Albers <tomalbers@kde.nl>
 */

class SetupGeneral : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent Widget
     */
    SetupGeneral(QWidget* parent = 0);

    /**
     * Destructor
     */
    ~SetupGeneral();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

private slots:
    void slotHideCounter();
    void slotHideShortcut();
    void slotFontPicker();
    void slotFolderPicker();
    void slotFolderEdited(const QString& newPath);
    void slotShortcutPicker();

private:
    void readSettings();

    QHBox*            m_colorBox;
    QHBox*            m_fontBox;
    QHBox*            m_shortcutBox;
    QPushButton*      m_counterFontBut;
    QPushButton*      m_folderBut;
    QPushButton*      m_shortcutBut;
    QLineEdit*        m_imageFolderEdit;
    QString           m_shortcut;
    QFont             m_counterFont;
    KColorButton*     m_counterColor;
    QCheckBox*        m_autoStart;
    QCheckBox*        m_searchRecursiveCheck;
    QCheckBox*        m_hideMinimizeButton;
    QCheckBox*        m_hideCounter;
    QCheckBox*        m_disableAccel;
};

#endif /* SETUPGENERAL_H */
