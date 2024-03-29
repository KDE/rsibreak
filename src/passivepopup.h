/*
    SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PASSIVEPOPUP_H
#define PASSIVEPOPUP_H

#include <QMouseEvent>
#include <kpassivepopup.h>

class PassivePopup : public KPassivePopup
{
public:
    explicit PassivePopup(QWidget *parent = nullptr);
    void show();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif
