/*
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSISTATWIDGET_H
#define RSISTATWIDGET_H

#include "rsiglobals.h"

class QGridLayout;

class RSIStatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RSIStatWidget(QWidget *parent = 0);
    ~RSIStatWidget();

protected:
    void addStat(RSIStat stat, QGridLayout *grid, int row);
    void showEvent(QShowEvent *) override;
    void hideEvent(QHideEvent *) override;

private:
    QGridLayout *mGrid;
};

#endif
