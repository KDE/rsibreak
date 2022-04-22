/*
    SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2005-2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "passivepopup.h"

#include <QGuiApplication>
#include <QMouseEvent>
#include <QScreen>

static const int MARGIN = 30;

PassivePopup::PassivePopup(QWidget *parent)
    : KPassivePopup(parent)
{
}

void PassivePopup::show()
{
    // Hardcoded to show at bottom-center for now
    const QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
    const int posX = screenRect.left() + (screenRect.width() - sizeHint().width()) / 2;
    const int posY = screenRect.bottom() - sizeHint().height() - MARGIN;
    KPassivePopup::show(QPoint(posX, posY));
}

void PassivePopup::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    /* eat this! */
}
