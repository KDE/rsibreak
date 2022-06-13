/*
    SPDX-FileCopyrightText: 2006-2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SETUPTIMING_H
#define SETUPTIMING_H

#include <qwidget.h>

class SetupTimingPriv;

/**
 * @class SetupTiming
 * These contain the timings settings of RSIBreak, currently
 * that means settings for tiny and big breaks
 * @author Tom Albers <toma.org>
 */

class SetupTiming : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit SetupTiming(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~SetupTiming();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

public slots:
    void slotSetUseIdleTimer(const bool useIdleTimer);

private slots:
    void slotTinyValueEnabled(bool enabled);
    void slotTinyValueChanged(const int tinyIntervalValue);
    void slotTinyDurationValueChanged(const int tinyDurationValue);
    void slotBigDurationValueChanged(const int bigDurationValue);

private:
    void readSettings();
    SetupTimingPriv *d;
};

#endif /* SETUPTIMING_H */
