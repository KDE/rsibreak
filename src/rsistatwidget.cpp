/*
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsistatwidget.h"
#include "rsistats.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLocale>
#include <QTime>

#include <KLocalizedString>
#include <QFontDatabase>

RSIStatWidget::RSIStatWidget(QWidget *parent)
    : QWidget(parent)
{
    mGrid = new QGridLayout(this);

    QGroupBox *gb = new QGroupBox(i18n("Time"), this);
    QGridLayout *subgrid = new QGridLayout(gb);
    addStat(TOTAL_TIME, subgrid, 0);
    addStat(ACTIVITY, subgrid, 1);
    addStat(IDLENESS, subgrid, 2);
    addStat(CURRENT_IDLE_TIME, subgrid, 3);
    addStat(MAX_IDLENESS, subgrid, 4);
    mGrid->addWidget(gb, 0, 0);

    gb = new QGroupBox(i18n("Short Breaks"), this);
    subgrid = new QGridLayout(gb);
    addStat(TINY_BREAKS, subgrid, 0);
    addStat(LAST_TINY_BREAK, subgrid, 1);
    addStat(TINY_BREAKS_SKIPPED, subgrid, 2);
    addStat(TINY_BREAKS_POSTPONED, subgrid, 3);
    addStat(IDLENESS_CAUSED_SKIP_TINY, subgrid, 4);
    mGrid->addWidget(gb, 0, 1);

    gb = new QGroupBox(i18n("Pause"), this);
    subgrid = new QGridLayout(gb);
    addStat(ACTIVITY_PERC, subgrid, 0);
    addStat(ACTIVITY_PERC_MINUTE, subgrid, 1);
    addStat(ACTIVITY_PERC_HOUR, subgrid, 2);
    addStat(ACTIVITY_PERC_6HOUR, subgrid, 3);
    addStat(PAUSE_SCORE, subgrid, 4);
    mGrid->addWidget(gb, 1, 0);

    gb = new QGroupBox(i18n("Long Breaks"), this);
    subgrid = new QGridLayout(gb);
    addStat(BIG_BREAKS, subgrid, 0);
    addStat(LAST_BIG_BREAK, subgrid, 1);
    addStat(BIG_BREAKS_SKIPPED, subgrid, 2);
    addStat(BIG_BREAKS_POSTPONED, subgrid, 3);
    addStat(IDLENESS_CAUSED_SKIP_BIG, subgrid, 4);
    mGrid->addWidget(gb, 1, 1);
}

RSIStatWidget::~RSIStatWidget()
{
}

void RSIStatWidget::addStat(RSIStat stat, QGridLayout *grid, int row)
{
    QLabel *l = RSIGlobals::instance()->stats()->getDescription(stat);
    l->setParent(this);

    QLabel *m = RSIGlobals::instance()->stats()->getLabel(stat);
    m->setParent(grid->parentWidget());
    m->setAlignment(Qt::AlignRight);

    grid->addWidget(l, row, 0);
    grid->addWidget(m, row, 1);

    // measure minimal width with current font settings
    QFontMetrics fm(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    int width = 0;
    switch (stat) {
    case TOTAL_TIME:
    case ACTIVITY:
    case IDLENESS:
    case MAX_IDLENESS:
        width = fm.boundingRect("One one and "
                                + i18nc("Translate this as the longest plural form. This is used to "
                                        "calculate the width of window",
                                        "minutes")
                                + i18nc("Translate this as the longest plural form. This is used to "
                                        "calculate the width of window",
                                        "seconds"))
                    .width();
        break;
    case LAST_TINY_BREAK:
    case LAST_BIG_BREAK: {
        QTime dt(QTime::currentTime());
        width = (int)(fm.boundingRect(QLocale().toString(dt)).width() * 1.25);
        break;
    }
    default:;
    }

    if (width > 0)
        m->setMinimumWidth(width);
}

void RSIStatWidget::showEvent(QShowEvent *)
{
    RSIGlobals::instance()->stats()->doUpdates(true);
}

void RSIStatWidget::hideEvent(QHideEvent *)
{
    RSIGlobals::instance()->stats()->doUpdates(false);
}
