/*
    SPDX-FileCopyrightText: 2009 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BREAKBASE_H
#define BREAKBASE_H

#include <QHash>
#include <QObject>
#include <QWidget>

class BreakControl;
class GrayWidget;
class GrayEffectOnAllScreens;

class BreakBase : public QObject
{
    Q_OBJECT

public:
    explicit BreakBase(QObject *parent);
    ~BreakBase();
    virtual void activate();
    virtual void deactivate();
    virtual void setLabel(const QString &);
    void setReadOnly(bool);
    bool readOnly() const;
    void showMinimize(bool);
    void showLock(bool);
    void showPostpone(bool);
    void disableShortcut(bool disable);
    void setGrayEffectOnAllScreens(bool on);
    void setGrayEffectLevel(int level);
    void excludeGrayEffectOnScreen(QScreen *screen);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void skip();
    void lock();
    void postpone();

private:
    BreakControl *m_breakControl;
    GrayEffectOnAllScreens *m_grayEffectOnAllScreens;
    bool m_readOnly;
    bool m_disableShortcut;
    bool m_grayEffectOnAllScreensActivated;
};

class GrayEffectOnAllScreens
{
public:
    GrayEffectOnAllScreens();
    ~GrayEffectOnAllScreens();
    void activate();
    void deactivate();
    void setLevel(int val);
    void disable(QScreen *screen);

private:
    QHash<QScreen *, GrayWidget *> m_widgets;
};

class GrayWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit GrayWidget(QWidget *parent = nullptr);
    void setLevel(int);

protected:
    bool event(QEvent *event) override;
};

#endif // BREAKBASE_H
