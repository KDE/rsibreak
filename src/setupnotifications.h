/*
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SETUPNOTIFICATIONS_H
#define SETUPNOTIFICATIONS_H

#include <qwidget.h>

class KNotifyConfigWidget;

/**
 * @class SetupTiming
 * These contain the timings settings of RSIBreak, currently
 * that means settings for tiny and big breaks
 * @author Tom Albers <toma.org>
 */

class SetupNotifications : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit SetupNotifications(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~SetupNotifications();

    void save();

private:
    KNotifyConfigWidget *m_notify;
};

#endif /* SETUPNOTIFICATIONS_H */
