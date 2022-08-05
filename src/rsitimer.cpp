/*
    SPDX-FileCopyrightText: 2005-2006, 2008-2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    The parts for idle detection is based on
    kdepim's karm idletimedetector.cpp/.h

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsitimer.h"

#include <QDebug>
#include <QTimer>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kwindowsystem.h>

#include "rsiglobals.h"
#include "rsistats.h"

RSITimer::RSITimer(QObject *parent)
    : QObject(parent)
    , m_idleTimeInstance(new RSIIdleTimeImpl())
    , m_intervals(RSIGlobals::instance()->intervals())
    , m_state(TimerState::Monitoring)
{
    updateConfig(true);
    run();
}

RSITimer::RSITimer(std::unique_ptr<RSIIdleTime> &&_idleTime, const QVector<int> _intervals, const bool _usePopup, const bool _useIdleTimers)
    : QObject(nullptr)
    , m_idleTimeInstance(std::move(_idleTime))
    , m_usePopup(_usePopup)
    , m_useIdleTimers(_useIdleTimers)
    , m_intervals(_intervals)
    , m_state(TimerState::Monitoring)
{
    createTimers();
    run();
}

void RSITimer::createTimers()
{
    int bigThreshold = m_useIdleTimers ? m_intervals[BIG_BREAK_THRESHOLD] : INT_MAX;
    int tinyThreshold = m_useIdleTimers ? m_intervals[TINY_BREAK_THRESHOLD] : INT_MAX;

    m_bigBreakCounter = std::unique_ptr<RSITimerCounter>{new RSITimerCounter(m_intervals[BIG_BREAK_INTERVAL], m_intervals[BIG_BREAK_DURATION], bigThreshold)};
    m_tinyBreakCounter = !m_intervals[TINY_BREAK_INTERVAL]
        ? nullptr
        : std::unique_ptr<RSITimerCounter>{new RSITimerCounter(m_intervals[TINY_BREAK_INTERVAL], m_intervals[TINY_BREAK_DURATION], tinyThreshold)};
}

void RSITimer::run()
{
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RSITimer::timeout);
    timer->setTimerType(Qt::TimerType::CoarseTimer);
    timer->start(1000);
}

bool RSITimer::suppressionDetector()
{
    for (WId win : KWindowSystem::windows()) {
        KWindowInfo info(win, NET::WMDesktop | NET::WMState | NET::XAWMState);
        if ((info.state() & NET::FullScreen) && !info.isMinimized() && info.isOnCurrentDesktop()) {
            return true;
        }
    }
    return false;
}

void RSITimer::hibernationDetector(const int totalIdle)
{
    // poor mans hibernation detector....
    static QDateTime last = QDateTime::currentDateTime();
    QDateTime current = QDateTime::currentDateTime();
    if (last.secsTo(current) > 60) {
        qDebug() << "Not been checking idleTime for more than 60 seconds, "
                 << "assuming the computer hibernated, resetting timers"
                 << "Last: " << last << "Current: " << current << "Idle, s: " << totalIdle;
        m_bigBreakCounter->reset();
        if (m_tinyBreakCounter) {
            m_tinyBreakCounter->reset();
        }
        resetAfterBreak();
    }
    last = current;
}

int RSITimer::idleTime()
{
    int totalIdle = m_idleTimeInstance->getIdleTime() / 1000;
    hibernationDetector(totalIdle);
    return totalIdle;
}

void RSITimer::doBreakNow(const int breakTime, const bool nextBreakIsBig)
{
    m_state = TimerState::Resting;
    m_pauseCounter = std::unique_ptr<RSITimerCounter>{new RSITimerCounter(breakTime, breakTime, INT_MAX)};
    m_popupCounter = nullptr;
    m_shortInputCounter = std::unique_ptr<RSITimerCounter>{new RSITimerCounter(m_intervals[SHORT_INPUT_INTERVAL], 1, 1)};
    if (nextBreakIsBig) {
        emit startLongBreak();
    } else {
        emit startShortBreak();
    }
    emit updateWidget(breakTime);
    emit breakNow();
}

void RSITimer::resetAfterBreak()
{
    m_state = TimerState::Monitoring;
    m_pauseCounter = nullptr;
    m_popupCounter = nullptr;
    m_shortInputCounter = nullptr;
    defaultUpdateToolTip();
    emit updateIdleAvg(0.0);
    emit relax(-1, false);
    emit minimize();
    if (m_bigBreakCounter->isReset()) {
        emit endLongBreak();
    } else {
        emit endShortBreak();
    }
}

// -------------------------- SLOTS ------------------------//

void RSITimer::slotStart()
{
    m_state = TimerState::Monitoring;
}

void RSITimer::slotStop()
{
    m_state = TimerState::Suspended;
    emit updateIdleAvg(0.0);
    emit updateToolTip(0, 0);
}

void RSITimer::slotSuspended(bool suspend)
{
    suspend ? slotStop() : slotStart();
}

void RSITimer::slotLock()
{
    resetAfterBreak();
}

void RSITimer::skipBreak()
{
    if (m_bigBreakCounter->isReset()) {
        RSIGlobals::instance()->stats()->increaseStat(BIG_BREAKS_SKIPPED);
        emit bigBreakSkipped();
    } else {
        RSIGlobals::instance()->stats()->increaseStat(TINY_BREAKS_SKIPPED);
        emit tinyBreakSkipped();
    }
    resetAfterBreak();
}

void RSITimer::postponeBreak()
{
    if (m_bigBreakCounter->isReset()) {
        m_bigBreakCounter->postpone(m_intervals[POSTPONE_BREAK_INTERVAL]);
        RSIGlobals::instance()->stats()->increaseStat(BIG_BREAKS_POSTPONED);
    } else {
        m_tinyBreakCounter->postpone(m_intervals[POSTPONE_BREAK_INTERVAL]);
        RSIGlobals::instance()->stats()->increaseStat(TINY_BREAKS_POSTPONED);
    }
    resetAfterBreak();
}

void RSITimer::updateConfig(bool doRestart)
{
    KConfigGroup popupConfig = KSharedConfig::openConfig()->group("Popup Settings");
    m_usePopup = popupConfig.readEntry("UsePopup", true);

    bool oldUseIdleTimers = m_useIdleTimers;
    KConfigGroup generalConfig = KSharedConfig::openConfig()->group("General Settings");
    m_suppressable = generalConfig.readEntry("SuppressIfPresenting", true);
    m_useIdleTimers = !(generalConfig.readEntry("UseNoIdleTimer", false));
    doRestart = doRestart || (oldUseIdleTimers != m_useIdleTimers);

    const QVector<int> oldIntervals = m_intervals;
    m_intervals = RSIGlobals::instance()->intervals();
    doRestart = doRestart || (m_intervals != oldIntervals);

    if (doRestart) {
        qDebug() << "Timeout parameters have changed, counters were reset.";
        createTimers();
    }
}

// ----------------------------- EVENTS -----------------------//

void RSITimer::timeout()
{
    // Don't change the tray icon when suspended, or evaluate a possible break.
    if (m_state == TimerState::Suspended) {
        return;
    }

    const int idleSeconds = idleTime(); // idleSeconds == 0 means activity

    RSIGlobals::instance()->stats()->increaseStat(TOTAL_TIME);
    RSIGlobals::instance()->stats()->setStat(CURRENT_IDLE_TIME, idleSeconds);
    if (idleSeconds == 0) {
        RSIGlobals::instance()->stats()->increaseStat(ACTIVITY);
    } else {
        RSIGlobals::instance()->stats()->setStat(MAX_IDLENESS, idleSeconds, true);
    }

    switch (m_state) {
    case TimerState::Monitoring: {
        // This is a weird thing to track as now when user was away, they will get back to zero counters,
        // not to an arbitrary time elapsed since last "idleness-skip-break".
        bool bigWasReset = m_bigBreakCounter->isReset();
        bool tinyWasReset = !m_tinyBreakCounter || m_tinyBreakCounter->isReset();

        int breakTime = m_bigBreakCounter->tick(idleSeconds);
        if (m_tinyBreakCounter) {
            breakTime = std::max(breakTime, m_tinyBreakCounter->tick(idleSeconds));
        }
        if (breakTime > 0) {
            suggestBreak(breakTime);
        } else {
            // Not a time for break yet, but if one of the counters got reset, that means we were idle enough to skip.
            if (!bigWasReset && m_bigBreakCounter->isReset()) {
                RSIGlobals::instance()->stats()->increaseStat(BIG_BREAKS);
                RSIGlobals::instance()->stats()->increaseStat(IDLENESS_CAUSED_SKIP_BIG);
            }
            if (!tinyWasReset && m_tinyBreakCounter && m_tinyBreakCounter->isReset()) {
                RSIGlobals::instance()->stats()->increaseStat(TINY_BREAKS);
                RSIGlobals::instance()->stats()->increaseStat(IDLENESS_CAUSED_SKIP_TINY);
            }
        }
        const double rawvalue = m_tinyBreakCounter ? m_tinyBreakCounter->counterLeft() / (double)m_intervals[TINY_BREAK_INTERVAL]
                                                   : m_bigBreakCounter->counterLeft() / (double)m_intervals[BIG_BREAK_INTERVAL];
        const double value = 100.0 - (rawvalue * 100.0);
        emit updateIdleAvg(value);
        break;
    }
    case TimerState::Suggesting: {
        // Using popupCounter to count down our patience here.
        int breakTime = m_popupCounter->tick(idleSeconds);
        if (breakTime > 0) {
            // User kept working throw the suggestion timeout. Well, their loss.
            emit relax(-1, false);
            breakTime = m_pauseCounter->counterLeft();
            doBreakNow(breakTime, false);
            break;
        }

        bool isInputLong = (m_shortInputCounter->tick(idleSeconds) > 0);
        int inverseTick = (idleSeconds == 0 && isInputLong) ? 1 : 0; // inverting as we account idle seconds here.
        breakTime = m_pauseCounter->tick(inverseTick);
        if (breakTime > 0) {
            // User has waited out the pause, back to monitoring.
            resetAfterBreak();
            break;
        }
        emit relax(m_pauseCounter->counterLeft(), false);
        emit updateWidget(m_pauseCounter->counterLeft());
        break;
    }
    case TimerState::Resting: {
        bool isInputLong = (m_shortInputCounter->tick(idleSeconds) > 0);
        int inverseTick = (idleSeconds == 0 && isInputLong) ? 1 : 0; // inverting as we account idle seconds here.
        int breakTime = m_pauseCounter->tick(inverseTick);
        if (breakTime > 0) {
            resetAfterBreak();
        } else {
            emit updateWidget(m_pauseCounter->counterLeft());
        }
        break;
    }
    default:
        qDebug() << "Reached unexpected state";
    }
    defaultUpdateToolTip();
}

void RSITimer::suggestBreak(const int breakTime)
{
    if (m_suppressable && suppressionDetector()) {
        return;
    }

    if (m_bigBreakCounter->isReset()) {
        RSIGlobals::instance()->stats()->increaseStat(BIG_BREAKS);
        RSIGlobals::instance()->stats()->setStat(LAST_BIG_BREAK, QVariant(QDateTime::currentDateTime()));
    } else {
        RSIGlobals::instance()->stats()->increaseStat(TINY_BREAKS);
        RSIGlobals::instance()->stats()->setStat(LAST_TINY_BREAK, QVariant(QDateTime::currentDateTime()));
    }

    bool nextOneIsBig = !m_tinyBreakCounter || m_bigBreakCounter->counterLeft() <= m_tinyBreakCounter->getDelayTicks();
    if (!m_usePopup) {
        doBreakNow(breakTime, nextOneIsBig);
        return;
    }

    m_state = TimerState::Suggesting;

    // When pause is longer than patience, we need to reset patience timer so that we don't flip to break now in
    // mid-pause. Patience / 2 is a good alternative to it by extending patience if user was idle long enough.
    m_popupCounter = std::unique_ptr<RSITimerCounter>{new RSITimerCounter(m_intervals[PATIENCE_INTERVAL], breakTime, m_intervals[PATIENCE_INTERVAL] / 2)};
    // Threshold of one means the timer is reset on every non-zero tick.
    m_pauseCounter = std::unique_ptr<RSITimerCounter>{new RSITimerCounter(breakTime, breakTime, 1)};

    // For measuring input duration in order to limit influence of short inputs on resetting pause counter.
    // Example of short input is: mouse sent input due to accidental touch or desk vibration.
    m_shortInputCounter = std::unique_ptr<RSITimerCounter>{new RSITimerCounter(m_intervals[SHORT_INPUT_INTERVAL], 1, 1)};

    emit relax(breakTime, nextOneIsBig);
}

void RSITimer::defaultUpdateToolTip()
{
    emit updateToolTip(m_tinyBreakCounter ? m_tinyBreakCounter->counterLeft() : 0, m_bigBreakCounter->counterLeft());
}
