/*
    SPDX-FileCopyrightText: 2009 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "grayeffect.h"

GrayEffect::GrayEffect( QObject *parent )
        : BreakBase( parent )
{
    setGrayEffectOnAllScreens( true );
    setReadOnly( true );
}

GrayEffect::~GrayEffect()
{
}

void GrayEffect::activate()
{
    BreakBase::activate();
}

void GrayEffect::deactivate()
{
    BreakBase::deactivate();
}

void GrayEffect::setLevel( int val )
{
    setGrayEffectLevel( val );
}
