/*
    SPDX-FileCopyrightText: 2009 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef POPUPEFFECT_H
#define POPUPEFFECT_H

#include <breakbase.h>

#include <QObject>

class PassivePopup;
class QLabel;

class PopupEffect : public BreakBase
{
    Q_OBJECT

public:
    explicit PopupEffect(QObject *);
    ~PopupEffect();
    void setLabel(const QString &) override;

public slots:
    void activate() override;
    void deactivate() override;

private:
    PassivePopup *m_popup;
    QLabel *m_label;
};

#endif // POPUPEFFECT_H
