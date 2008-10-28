/*
 * Copyright © 2007 Fredrik Höglund <fredrik@kde.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GRAYOUTEFFECT_H
#define GRAYOUTEFFECT_H

#include <QObject>
#include <QTime>

#include <X11/Xlib.h>
#include <fixx11h.h>

class QWidget;
class QPixmap;
class BlendingThread;


/**
 * This class implements an effect that will slowly fade a pixmap from color to grayscale.
 *
 * This class is X11 specific, and currently only works on little-endian systems with
 * 24 or 16 plane pixmaps. On other systems it does nothing.
 */
class GrayOutEffect : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates the GrayOutEffect.
     *
     * The pixmap you pass to this constructor should contain the image
     * (usually a screenshot) that you want to fade to gray.
     * This pixmap will be updated with each frame after you call start()
     * to start the animation. Each time the pixmap is updated, update()
     * will be called in the @p parent widget, which must then paint
     * the pixmap.
     *
     * The actual blending is done in a separate thread to keep the event
     * loop alive.
     *
     * The GrayOutEffect can safely be deleted at any time, even while the
     * effect is running, but it's important that the pixmap isn't deleted
     * before the effect object.
     *
     * @param parent The parent widget, which will draw the pixmap.
     * @param pixmap The pixmap with the image that will be faded to gray.
     */
    GrayOutEffect( QWidget *parent, QPixmap *pixmap );

    /**
     * Destroys the grayout effect.
     */
    ~GrayOutEffect();

    /**
     * Starts the animation.
     */
    void start();

signals:
    void ready();

private slots:
    /** @internal */
    void nextFrame();

private:
    /** @internal */
    bool supportedFormat( const QPixmap *pixmap ) const;

private:
    QWidget *widget;
    QPixmap *pixmap;
    BlendingThread *blender;
    int alpha;
    bool firstFrame;
    bool done;
    XImage *image;
    GC gc;
    QTime time;
};

#endif
