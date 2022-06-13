/*
    SPDX-FileCopyrightText: 2005-2006 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SETUPGENERAL_H
#define SETUPGENERAL_H

#include <qwidget.h>

class SetupGeneralPriv;

/**
 * @class SetupGeneral
 * These contain the general settings of RSIBreak
 * @author Tom Albers <toma.org>
 */

class SetupGeneral : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit SetupGeneral(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~SetupGeneral();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

    bool useIdleTimer() const;

signals:
    void useIdleTimerChanged(bool useIdleTimer);

private:
    void readSettings();

    SetupGeneralPriv *d;
};

#endif /* SETUPGENERAL_H */
