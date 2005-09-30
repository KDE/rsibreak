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

#ifndef SETUPEDITOR_H
#define SETUPEDITOR_H

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

class SetupEditor : public QWidget
{
    Q_OBJECT
    
public:

    SetupEditor(QWidget* parent = 0);
    ~SetupEditor();

    void applySettings();

private slots:
    void slotHideCounter();
    void slotFontPicker();
    void slotFolderPicker();
    void slotFolderEdited(const QString& newPath);

private:
    void readSettings();

    QHBox*          m_colorBox;
    QHBox*          m_fontBox;
    QPushButton*    m_counterFontBut;
    QPushButton*    m_folderBut;
    QLineEdit*      m_imageFolderEdit;

    QFont           m_counterFont;
    KColorButton*   m_counterColor;
    QCheckBox*      m_searchRecursiveCheck;
    QCheckBox*      m_hideMinimizeButton;
    QCheckBox*      m_hideCounter;
};

#endif /* SETUPEDITOR_H */
