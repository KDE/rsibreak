/*
    SPDX-FileCopyrightText: 2005-2006, 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "breakbase.h"
#include <QWidget>

class SlideWidget;
class QLabel;

class SlideEffect : public BreakBase
{
    Q_OBJECT

public:
    explicit SlideEffect(QObject *parent);
    ~SlideEffect();
    void reset(const QString &path, bool recursive, bool showSmallImages, bool expandImageToFullScreen, int interval);
    void activate() override;
    void deactivate() override;
    bool hasImages();
    void loadImage();

private slots:
    void slotGray();
    void slotNewSlide();

private:
    void findImagesInFolder(const QString &folder);

    SlideWidget *m_slidewidget;
    QString m_basePath;
    QTimer *m_timer_slide;

    bool m_searchRecursive;
    bool m_showSmallImages;
    bool m_expandImageToFullScreen;
    int m_slideInterval;

    QStringList m_files;
    QStringList m_files_done;
};

class SlideWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent Parent Widget
     * @param name Name
     */
    explicit SlideWidget(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~SlideWidget();

    void setImage(const QImage &image);

private slots:
    void slotDimension();

private:
    QLabel *m_imageLabel;
};

#endif
