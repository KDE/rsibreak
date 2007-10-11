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

#include <QThread>
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QX11Info>
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <string>
#include <cstring>

#if 1
#include <mmintrin.h>
#include <emmintrin.h>
#endif

#include "grayouteffect.h"
#include "grayouteffect.moc"

static int inline multiply(int a, int b)
{
    int res = a * b + 0x80;
    return (res + (res >> 8)) >> 8;
}


static void inline load(const quint32 src, int *r, int *g, int *b)
{
    *r = (src >> 16) & 0xff;
    *g = (src >> 8) & 0xff;
    *b = src & 0xff;
}


static void inline load16 (const __uint16_t src, int *r, int *g, int *b)
{
    *r = ((src >> 8) & 0x00f8) | (src >> 13) & 0x0007;
    *g = ((src >> 3) & 0x00fc) | (src >>  9) & 0x0003;
    *b = ((src << 3) & 0x00f8) | (src >>  2) & 0x0007;
}


static quint32 inline store(const int r, const int g, const int b)
{
    return (r << 16) | (g << 8) | b | 0xff000000;
}


static quint16 inline store16 (const int r, const int g, const int b)
{
    return (((r << 8) | (b >> 3)) & 0xf81f) | ((g << 3) & 0x07e0);
}


static void scanline_blend(const quint32 *over, const quint8 alpha, const quint32 *under,
                           quint32 *result, uint length)
{
    for (uint i = 0; i < length; ++i)
    {
        int sr, sg, sb, dr, dg, db;	

        load (over[i],  &sr, &sg, &sb);
        load (under[i], &dr, &dg, &db);

        dr = multiply ((sr - dr), alpha) + dr;
        dg = multiply ((sg - dg), alpha) + dg;
        db = multiply ((sb - db), alpha) + db;

        result[i] = store (dr, dg, db);
    }
}


static void scanline_blend_16(const quint16 *over, const quint8 alpha, const quint16 *under,
                              quint16 *result, uint length)
{
    for (uint i = 0; i < length; ++i)
    {
        int sr, sg, sb, dr, dg, db;	

        load16 (over[i],  &sr, &sg, &sb);
        load16 (under[i], &dr, &dg, &db);

        dr = multiply ((sr - dr), alpha) + dr;
        dg = multiply ((sg - dg), alpha) + dg;
        db = multiply ((sb - db), alpha) + db;

        result[i] = store16 (dr, dg, db);
    }
}




// ----------------------------------------------------------------------------


#if 1

static __m64 inline multiply(const __m64 m1, const __m64 m2)
{
    __m64 res = _mm_mullo_pi16 (m1, m2);
    res = _mm_adds_pi16 (res, _mm_set1_pi16 (0x0080));
    res = _mm_adds_pi16 (res, _mm_srli_pi16 (res, 8));
    return _mm_srli_pi16 (res, 8);
}


static __m64 inline add(const __m64 m1, const __m64 m2)
{
    return _mm_adds_pi16 (m1, m2);
}


static __m64 inline load(const __uint32_t pixel, const __m64 zero)
{
    __m64 m = _mm_cvtsi32_si64 (pixel);
    return _mm_unpacklo_pi8 (m, zero);
}

static __uint32_t inline store(const __m64 pixel, const __m64 zero)
{
    __m64 packed = _mm_packs_pu16 (pixel, zero);
    return _mm_cvtsi64_si32 (packed);
}


static void scanline_blend_mmx(const quint32 *over, const quint8 a, const quint32 *under,
                               quint32 *result, uint length)
{
    register const __m64 alpha    = _mm_set1_pi16 (__uint16_t (a));	
    register const __m64 negalpha = _mm_xor_si64 (alpha, _mm_set1_pi16 (0x00ff));
    register const __m64 zero     = _mm_setzero_si64 ();

    for (uint i = 0; i < length; ++i)
    {
        __m64 src = load (over[i],  zero);
        __m64 dst = load (under[i], zero);

        src = multiply (src, alpha);
        dst = multiply (dst, negalpha);
        dst = add (src, dst);

        result[i] = store (dst, zero);
    }

    _mm_empty();
}
#endif


// ----------------------------------------------------------------------------



class BlendingThread : public QThread
{
public:
    BlendingThread(QObject *parent);
    ~BlendingThread();

    void setImage(XImage *image);
    void setAlpha(int alpha) { m_alpha = alpha; }

private:
    void toGray16(quint8 *data);
    void toGray32(quint8 *data);
    void blend16();
    void blend32();

protected:
    void run();

private:
    int m_alpha;
    XImage *m_image;
    quint8 *m_original;
    quint8 *m_final;
};


BlendingThread::BlendingThread(QObject *parent)
    : QThread(parent)
{
}


BlendingThread::~BlendingThread()
{
    delete [] m_final;
    delete [] m_original;
}


void BlendingThread::setImage(XImage *image)
{
    m_image = image;
    int size = m_image->bytes_per_line * m_image->height;

    m_original = new quint8[size];
    m_final = new quint8[size];

    std::memcpy((void*)m_original, (const void*)m_image->data, size);
    std::memcpy((void*)m_final,    (const void*)m_image->data, size);

    if (m_image->depth == 24 || m_image->depth == 32)
        toGray32(m_final);
    else
        toGray16(m_final);
}


void BlendingThread::toGray16(quint8 *data)
{
    for (int y = 0; y < m_image->height; y++)
    {
        quint16 *pixels = (quint16*)(data + (m_image->bytes_per_line * y));
        for (int x = 0; x < m_image->width; x++)
        {
            int red, green, blue;
            load16(pixels[x], &red, &green, &blue);

            int val = int(red * .30 + green * .59 + blue * .11) & 0xff;
            pixels[x] = store16(val, val, val);
        }
    }
}


void BlendingThread::toGray32(quint8 *data)
{
    for (int y = 0; y < m_image->height; y++)
    {
        quint32 *pixels = (quint32*)(data + (m_image->bytes_per_line * y));
        for (int x = 0; x < m_image->width; x++)
        {
            int red, green, blue;
            load(pixels[x], &red, &green, &blue);

            int val = int(red * .10 + green * .19 + blue * .01) & 0xff;
            pixels[x] = store(val, val, val);
        }
    }
}


void BlendingThread::blend16()
{
    for (int y = 0; y < m_image->height; y++)
    {
        uint start = m_image->bytes_per_line * y;
        quint16 *over   = (quint16*)(m_original + start);
        quint16 *under  = (quint16*)(m_final + start);
        quint16 *result = (quint16*)(m_image->data + start);

        scanline_blend_16(over, m_alpha, under, result, m_image->width);
    }
}


void BlendingThread::blend32()
{
    for (int y = 0; y < m_image->height; y++)
    {
        int start = m_image->bytes_per_line * y;
        quint32 *over   = (quint32*)(m_original + start);
        quint32 *under  = (quint32*)(m_final + start);
        quint32 *result = (quint32*)(m_image->data + start);

        scanline_blend_mmx(over, m_alpha, under, result, m_image->width);
        //scanline_blend(over, m_alpha, under, result, m_image->width);
    }
}


void BlendingThread::run()
{
    if (m_image->depth == 24 || m_image->depth == 32)
        blend32();
    else
        blend16();
}



// ----------------------------------------------------------------------------



GrayOutEffect::GrayOutEffect(QWidget *parent, QPixmap *pixmap)
    : QObject(parent), widget(parent), pixmap(pixmap), blender(NULL) 
{
    Display *dpy = parent->x11Info().display();

    if (!supportedFormat(pixmap))
        return;

    image = XGetImage(dpy, pixmap->handle(), 0, 0, pixmap->width(),
                      pixmap->height(), AllPlanes, ZPixmap);

    blender = new BlendingThread(this);
    blender->setImage(image);

    gc = XCreateGC(dpy, pixmap->handle(), 0, NULL);
    firstFrame = true;
}


GrayOutEffect::~GrayOutEffect()
{
    if (blender)
    {
        blender->wait();
        XDestroyImage(image);
        XFreeGC(QX11Info::display(), gc);
    }
}


bool GrayOutEffect::supportedFormat(const QPixmap *pixmap) const
{
    int depth = pixmap->depth();
    Visual *visual = (Visual*)pixmap->x11Info().visual();

    if (ImageByteOrder(pixmap->x11Info().display()) != LSBFirst)
        return false;

    // Assume this means the pixmap is ARGB32
    if (pixmap->hasAlphaChannel())
        return true;

    // 24/34 bit x8a8r8g8b8
    if ((depth == 24 || depth == 32) &&
        visual->red_mask   == 0x00ff0000 &&
	visual->green_mask == 0x0000ff00 &&
	visual->blue_mask  == 0x000000ff)
    {
        return true;
    }

    // 16 bit r5g6b5
    if (depth == 16 &&
        visual->red_mask   == 0xf800 &&
	visual->green_mask == 0x07e0 &&
	visual->blue_mask  == 0x001f)
    {
        return true;
    }

    return false;
}


void GrayOutEffect::start()
{
    if (blender)
    {
        done = false;
        alpha = 255;

	time.start();
	nextFrame();
    }
}


void GrayOutEffect::nextFrame()
{
    const qreal runTime = 2000; // milliseconds
    const int framesPerSecond = 30;

    if (!firstFrame)
    {
        blender->wait();
        XPutImage(QX11Info::display(), pixmap->handle(), gc, image, 0, 0, 0, 0, image->width, image->height);
        widget->update();

        alpha = int(qMax(255. - (255. * (qreal(time.elapsed() / runTime))), 0.0));
    }

    if (!done)
    {
        blender->setAlpha(alpha);
        blender->start();

        if (!firstFrame) {
	    XSync(QX11Info::display(), False);
            //usleep(250000);
        }

        QTimer::singleShot(1, this, SLOT(nextFrame()));
    }

    if (alpha == 0)
    {
        done = true;
        emit ready();
    }

    firstFrame = false;
}

