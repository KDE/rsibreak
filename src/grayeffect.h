/*
    SPDX-FileCopyrightText: 2009 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef GRAYEFFECT_H
#define GRAYEFFECT_H

#include <QObject>
#include <breakbase.h>

class GrayWidget;

class GrayEffect : public BreakBase
{
    Q_OBJECT

public:
    explicit GrayEffect( QObject *parent );
    ~GrayEffect();
    void activate() override;
    void deactivate() override;
    void setLevel( int val );

private:
    QList<GrayWidget*> m_widgets;
};

#   endif
