/*
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

// Local includes.
#include "setupnotifications.h"

// QT includes.
#include <QVBoxLayout>

// KDE includes.
#include <KNotifyConfigWidget>

SetupNotifications::SetupNotifications( QWidget* parent )
        : QWidget( parent )
{
    QVBoxLayout *l = new QVBoxLayout( this );
    m_notify = new KNotifyConfigWidget( this );
    m_notify->setApplication( "rsibreak" );
    l->addWidget( m_notify );
}

SetupNotifications::~SetupNotifications()
{
}

void SetupNotifications::save()
{
    m_notify->save();
}
