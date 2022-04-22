/*
    SPDX-FileCopyrightText: 2009 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "popupeffect.h"
#include "passivepopup.h"

#include <KLocalizedString>

#include <QLabel>

PopupEffect::PopupEffect(QObject *parent)
    : BreakBase(parent)
{
    m_popup = new PassivePopup(0);

    m_label = new QLabel(i18n("Take a break...."), m_popup);

    m_popup->setView(m_label);
    m_popup->setTimeout(0);
}

PopupEffect::~PopupEffect()
{
    delete m_popup;
}

void PopupEffect::activate()
{
    m_popup->show();
}

void PopupEffect::deactivate()
{
    m_popup->hide();
}

void PopupEffect::setLabel(const QString &text)
{
    // text is a time string like '2 minutes 42 seconds'
    // or '42 seconds', so no plural here
    m_label->setText(i18nc("%1 is a time string like '2 minutes 42 seconds'", "Take a break for %1", text));
}
