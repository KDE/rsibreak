/*
    SPDX-FileCopyrightText: 2009-2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLASMAEFFECT_H
#define PLASMAEFFECT_H

#include <breakbase.h>

#include <QObject>

class PlasmaEffect : public BreakBase
{
    Q_OBJECT

public:
    explicit PlasmaEffect(QObject *);

public slots:
    void activate() override;
    void deactivate() override;

private slots:
    void slotGray();
};

#endif // PLASMAEFFECT_H
