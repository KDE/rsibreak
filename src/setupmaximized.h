/*
    SPDX-FileCopyrightText: 2005, 2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SETUPMAXIMIZED_H
#define SETUPMAXIMIZED_H

#include <qwidget.h>

class SetupMaximizedPriv;

/**
 * @class SetupMaximized
 * These contain the general settings of RSIBreak when maximized
 * that means settings for the Counter, Image Path and
 * minimize button
 * @author Tom Albers <toma.org>
 */

class SetupMaximized : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit SetupMaximized(QWidget *parent = 0);

    /**
     * Destructor
     */
    ~SetupMaximized();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

public slots:
    void slotSetUseIdleTimer(bool useIdleTimer);

private slots:
    void slotEffectChanged(int current);
    void slotFolderPicker();
    void slotFolderEdited(const QString &newPath);
    void slotHideFlash();

private:
    void readSettings();
    SetupMaximizedPriv *d;
};

#endif /* SETUPMAXIMIZED_H */
