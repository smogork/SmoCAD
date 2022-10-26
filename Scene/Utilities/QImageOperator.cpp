/**
 *  @file thickLine.cpp
 *
 *  @brief Draw a solid line with thickness using a modified Bresenhams algorithm.
 *
 *  Copyright (C) 2013-2022  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of STMF3-Discovery-Demos https://github.com/ArminJo/STMF3-Discovery-Demos.
 *
 *  STMF3-Discovery-Demos is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 */

#include <QPoint>
#include <stack>
#include "QImageOperator.h"

/*void QImageOperator::DrawPixel(QPoint zero, int x, int y, DrawState state, QColor color)
{
    
    if ((state & NegativeX) != 0)
        x = -x;
    if ((state & NegativeY) != 0)
        y = -y;
    if ((state & Swap) != 0)
    {
        int temp = x;
        x = y;
        y = temp;
    }
    
    m_bitmap.setPixelColor(zero.x() + x, zero.y() + y, color);
    //bitmap.SetPixel(zero.X + x, zero.Y + y, color);
}

void QImageOperator::drawLine(QPoint A, QPoint B, QColor aColor)
{
    QPoint diff = B - A;
    
    DrawState state = None;
    if (std::abs(diff.y()) > std::abs(diff.x()))
    {
        state = state | Swap;
        diff = QPoint(diff.y(), diff.x());
    }
    if (diff.x() < 0)
    {
        state = state | NegativeX;
        diff.setX(-diff.x());
    }
    if (diff.y() < 0)
    {
        state = state | NegativeY;
        diff.setY(-diff.y());
    }
    
    //bresenham
    BresenhamAlgorithm(A, diff, state, aColor);
}

void QImageOperator::BresenhamAlgorithm(QPoint zero, QPoint diff, DrawState state, QColor color)
{
    int dx = diff.x();
    int dy = diff.y();
    int d = 2 * dy - dx;
    int incrE = 2 * dy;
    int incrNE = 2 * (dy - dx);
    int y = 0;
    
    DrawPixel(zero, 0, y, state, color);
    for (int x = 0; x < diff.x(); ++x)
    {
        if (d < 0)//choose E
            d += incrE;
        else//chose NE
        {
            d += incrNE;
            ++y;
        }
        DrawPixel(zero, x, y, state, color);
    }
}*/

/** @addtogroup Graphic_Library
 * @{
 */

/**
 * Modified Bresenham draw(line) with optional overlap. Required for drawThickLine().
 * Overlap draws additional pixel when changing minor direction. For standard bresenham overlap, choose LINE_OVERLAP_NONE (0).
 *
 *  Sample line:
 *
 *    00+
 *     -0000+
 *         -0000+
 *             -00
 *
 *  0 pixels are drawn for normal line without any overlap
 *  + pixels are drawn if LINE_OVERLAP_MAJOR
 *  - pixels are drawn if LINE_OVERLAP_MINOR
 */



QImageOperator::QImageOperator(int bitmapSizeX, int bitmapSizeY)
{
    ChangeSize(bitmapSizeX, bitmapSizeY);
}

QImageOperator::QImageOperator()
{
    ChangeSize(1, 1);
}

const QImage &QImageOperator::GetBitmap() const
{
    return m_bitmap;
}

void QImageOperator::ClampPointToDisplay(QPoint &p)
{
    if (p.x() > m_bitmap.size().width() - 1 || p.x() < 0)
        p.setX(std::clamp(p.x(), 0, m_bitmap.size().width()));
    if (p.y() > m_bitmap.size().height() - 1 || p.y() < 0)
        p.setY(std::clamp(p.y(), 0, m_bitmap.size().height()));
}

void QImageOperator::ClampPointToDisplay(int &x, int &y)
{
    if (x > m_bitmap.size().width() - 1 || x < 0)
        x = std::clamp(x, (int) 0, (int) m_bitmap.size().width());
    if (y > m_bitmap.size().height() - 1 || y < 0)
        y = std::clamp(y, (int) 0, (int) m_bitmap.size().height());
}

void inline QImageOperator::drawPixel(int x, int y, QColor aColor)
{
    if (x < 0 || x >= m_bitmap.width() || y < 0 || y >= m_bitmap.height() )
        return;
    
    //zbuffor
    if (aColor.red() < qRed(m_bitmap.pixel(x, y)))
        m_bitmap.setPixelColor(x, y, aColor);
}

void QImageOperator::drawLine(int aXStart, int aYStart, int aXEnd, int aYEnd, QColor aColor)
{
    int dx, dy, p, x, y;
    dx = aXEnd - aXStart;
    dy = aYEnd - aYStart;
    x = aXStart;
    y = aYStart;
    p = 2 * dy - dx;
    while (x < aXEnd)
    {
        if (p >= 0)
        {
            drawPixel(x, y, aColor);
            y = y + 1;
            p = p + 2 * dy - 2 * dx;
        } else
        {
            drawPixel(x, y, aColor);
            p = p + 2 * dy;
        }
        x = x + 1;
    }
}

void QImageOperator::fillRect(QPoint A, QPoint B, QColor aColor)
{
    fillRect(A.x(), A.y(), B.x(), B.y(), aColor);
}

void QImageOperator::fillRect(int aXStart, int aYStart, int aXEnd, int aYEnd, QColor aColor)
{
    for (int y = aYStart; y <= aYEnd; ++y)
        for (int x = aXStart; x < aXEnd; ++x)
            drawPixel(x, y, aColor);
}

/**
 * Draws a line from aXStart/aYStart to aXEnd/aYEnd including both ends
 * @param aOverlap One of LINE_OVERLAP_NONE, LINE_OVERLAP_MAJOR, LINE_OVERLAP_MINOR, LINE_OVERLAP_BOTH
 */
void
QImageOperator::drawLineOverlap(int aXStart, int aYStart, int aXEnd, int aYEnd, uint8_t aOverlap,
                                QColor aColor)
{
    int16_t tDeltaX, tDeltaY, tDeltaXTimes2, tDeltaYTimes2, tError, tStepX, tStepY;
    
    /*
     * Clip to display size
     */
    ClampPointToDisplay(aXStart, aYStart);
    ClampPointToDisplay(aXEnd, aYEnd);
    
    if ((aXStart == aXEnd) || (aYStart == aYEnd))
    {
        //horizontal or vertical line -> fillRect() is faster than drawLine()
        fillRect(aXStart, aYStart, aXEnd, aYEnd, aColor);
    } else
    {
        //calculate direction
        tDeltaX = aXEnd - aXStart;
        tDeltaY = aYEnd - aYStart;
        if (tDeltaX < 0)
        {
            tDeltaX = -tDeltaX;
            tStepX = -1;
        } else
        {
            tStepX = +1;
        }
        if (tDeltaY < 0)
        {
            tDeltaY = -tDeltaY;
            tStepY = -1;
        } else
        {
            tStepY = +1;
        }
        tDeltaXTimes2 = tDeltaX << 1;
        tDeltaYTimes2 = tDeltaY << 1;
        //draw start pixel
        drawPixel(aXStart, aYStart, aColor);
        if (tDeltaX > tDeltaY)
        {
            // start value represents a half step in Y direction
            tError = tDeltaYTimes2 - tDeltaX;
            while (aXStart != aXEnd)
            {
                // step in main direction
                aXStart += tStepX;
                if (tError >= 0)
                {
                    if (aOverlap & LINE_OVERLAP_MAJOR)
                    {
                        // draw pixel in main direction before changing
                        drawPixel(aXStart, aYStart, aColor);
                    }
                    // change Y
                    aYStart += tStepY;
                    if (aOverlap & LINE_OVERLAP_MINOR)
                    {
                        // draw pixel in minor direction before changing
                        drawPixel(aXStart - tStepX, aYStart, aColor);
                    }
                    tError -= tDeltaXTimes2;
                }
                tError += tDeltaYTimes2;
                drawPixel(aXStart, aYStart, aColor);
            }
        } else
        {
            tError = tDeltaXTimes2 - tDeltaY;
            while (aYStart != aYEnd)
            {
                aYStart += tStepY;
                if (tError >= 0)
                {
                    if (aOverlap & LINE_OVERLAP_MAJOR)
                    {
                        // draw pixel in main direction before changing
                        drawPixel(aXStart, aYStart, aColor);
                    }
                    aXStart += tStepX;
                    if (aOverlap & LINE_OVERLAP_MINOR)
                    {
                        // draw pixel in minor direction before changing
                        drawPixel(aXStart, aYStart - tStepY, aColor);
                    }
                    tError -= tDeltaYTimes2;
                }
                tError += tDeltaXTimes2;
                drawPixel(aXStart, aYStart, aColor);
            }
        }
    }
}

/**
 * Bresenham with thickness
 * No pixel missed and every pixel only drawn once!
 * The code is bigger and more complicated than drawThickLineSimple() but it tends to be faster, since drawing a pixel is often a slow operation.
 * aThicknessMode can be one of LINE_THICKNESS_MIDDLE, LINE_THICKNESS_DRAW_CLOCKWISE, LINE_THICKNESS_DRAW_COUNTERCLOCKWISE
 */
void QImageOperator::drawThickLine(int aXStart, int aYStart, int aXEnd, int aYEnd, QVector3D simStartPos, QVector3D simFinishPos,
                                   unsigned int aThickness, uint8_t aThicknessMode)
{
    int16_t i, tDeltaX, tDeltaY, tDeltaXTimes2, tDeltaYTimes2, tError, tStepX, tStepY;
    
    /*if (aThickness <= 1)
    {
        drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, LINE_OVERLAP_NONE, aColor);
    }*/
    
    /*
     * Clip to display size
     */
    ClampPointToDisplay(aXStart, aYStart);
    ClampPointToDisplay(aXEnd, aYEnd);
    
    /**
     * For coordinate system with 0.0 top left
     * Swap X and Y delta and calculate clockwise (new delta X inverted)
     * or counterclockwise (new delta Y inverted) rectangular direction.
     * The right rectangular direction for LINE_OVERLAP_MAJOR toggles with each octant
     */
    tDeltaY = aXEnd - aXStart;
    tDeltaX = aYEnd - aYStart;
    // mirror 4 quadrants to one and adjust deltas and stepping direction
    bool tSwap = true; // count effective mirroring
    if (tDeltaX < 0)
    {
        tDeltaX = -tDeltaX;
        tStepX = -1;
        tSwap = !tSwap;
    } else
    {
        tStepX = +1;
    }
    if (tDeltaY < 0)
    {
        tDeltaY = -tDeltaY;
        tStepY = -1;
        tSwap = !tSwap;
    } else
    {
        tStepY = +1;
    }
    tDeltaXTimes2 = tDeltaX << 1;
    tDeltaYTimes2 = tDeltaY << 1;
    bool tOverlap;
    // adjust for right direction of thickness from line origin
    int tDrawStartAdjustCount = aThickness / 2;
    if (aThicknessMode == LINE_THICKNESS_DRAW_COUNTERCLOCKWISE)
    {
        tDrawStartAdjustCount = aThickness - 1;
    } else if (aThicknessMode == LINE_THICKNESS_DRAW_CLOCKWISE)
    {
        tDrawStartAdjustCount = 0;
    }
    
    /*
     * Now tDelta* are positive and tStep* define the direction
     * tSwap is false if we mirrored only once
     */
    // which octant are we now
    if (tDeltaX >= tDeltaY)
    {
        // Octant 1, 3, 5, 7 (between 0 and 45, 90 and 135, ... degree)
        if (tSwap)
        {
            tDrawStartAdjustCount = (aThickness - 1) - tDrawStartAdjustCount;
            tStepY = -tStepY;
        } else
        {
            tStepX = -tStepX;
        }
        /*
         * Vector for draw direction of the starting points of lines is rectangular and counterclockwise to main line direction
         * Therefore no pixel will be missed if LINE_OVERLAP_MAJOR is used on change in minor rectangular direction
         */
        // adjust draw start point
        tError = tDeltaYTimes2 - tDeltaX;
        for (i = tDrawStartAdjustCount; i > 0; i--)
        {
            // change X (main direction here)
            aXStart -= tStepX;
            aXEnd -= tStepX;
            if (tError >= 0)
            {
                // change Y
                aYStart -= tStepY;
                aYEnd -= tStepY;
                tError -= tDeltaXTimes2;
            }
            tError += tDeltaYTimes2;
        }
        
        //draw start line
        QColor color = Qt::black;
        color.setRedF(GetRedColorFunction(aXStart, aYStart, simStartPos, simFinishPos));
        drawLine(aXStart, aYStart, aXEnd, aYEnd, color);
        
        // draw aThickness number of lines
        tError = tDeltaYTimes2 - tDeltaX;
        for (i = aThickness; i > 1; i--)
        {
            // change X (main direction here)
            aXStart += tStepX;
            aXEnd += tStepX;
            tOverlap = LINE_OVERLAP_NONE;
            if (tError >= 0)
            {
                // change Y
                aYStart += tStepY;
                aYEnd += tStepY;
                tError -= tDeltaXTimes2;
                /*
                 * Change minor direction reverse to line (main) direction
                 * because of choosing the right (counter)clockwise draw vector
                 * Use LINE_OVERLAP_MAJOR to fill all pixel
                 *
                 * EXAMPLE:
                 * 1,2 = Pixel of first 2 lines
                 * 3 = Pixel of third line in normal line mode
                 * - = Pixel which will additionally be drawn in LINE_OVERLAP_MAJOR mode
                 *           33
                 *       3333-22
                 *   3333-222211
                 * 33-22221111
                 *  221111                     /\
				 *  11                          Main direction of start of lines draw vector
                 *  -> Line main direction
                 *  <- Minor direction of counterclockwise of start of lines draw vector
                 */
                tOverlap = LINE_OVERLAP_MAJOR;
            }
            tError += tDeltaYTimes2;
    
            float red = GetRedColorFunction(aXStart, aYStart, simStartPos, simFinishPos);
            if (red != red)//nan check
                break;
            color.setRedF(red);
            drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, tOverlap, color);
        }
    } else
    {
        // the other octant 2, 4, 6, 8 (between 45 and 90, 135 and 180, ... degree)
        if (tSwap)
        {
            tStepX = -tStepX;
        } else
        {
            tDrawStartAdjustCount = (aThickness - 1) - tDrawStartAdjustCount;
            tStepY = -tStepY;
        }
        // adjust draw start point
        tError = tDeltaXTimes2 - tDeltaY;
        for (i = tDrawStartAdjustCount; i > 0; i--)
        {
            aYStart -= tStepY;
            aYEnd -= tStepY;
            if (tError >= 0)
            {
                aXStart -= tStepX;
                aXEnd -= tStepX;
                tError -= tDeltaYTimes2;
            }
            tError += tDeltaXTimes2;
        }
        
        //draw start line
        QColor color = Qt::black;
        color.setRedF(GetRedColorFunction(aXStart, aYStart, simStartPos, simFinishPos));
        drawLine(aXStart, aYStart, aXEnd, aYEnd, color);
        
        // draw aThickness number of lines
        tError = tDeltaXTimes2 - tDeltaY;
        for (i = aThickness; i > 1; i--)
        {
            aYStart += tStepY;
            aYEnd += tStepY;
            tOverlap = LINE_OVERLAP_NONE;
            if (tError >= 0)
            {
                aXStart += tStepX;
                aXEnd += tStepX;
                tError -= tDeltaYTimes2;
                tOverlap = LINE_OVERLAP_MAJOR;
            }
            tError += tDeltaXTimes2;
            
            float red = GetRedColorFunction(aXStart, aYStart, simStartPos, simFinishPos);
            if (red != red)//nan check
                break;
            color.setRedF(red);
            drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, tOverlap, color);
        }
    }
}

void
QImageOperator::drawThickLine(QPoint aStart, QPoint aEnd, QVector3D simStartPos, QVector3D simFinishPos, unsigned int aThickness,
                              uint8_t aThicknessMode)
{
    drawThickLine(aStart.x(), aStart.y(), aEnd.x(), aEnd.y(), simStartPos, simFinishPos, aThickness, aThicknessMode);
}

/**
 * The same as before, but no clipping to display range, some pixel are drawn twice (because of using LINE_OVERLAP_BOTH)
 * and direction of thickness changes for each octant (except for LINE_THICKNESS_MIDDLE and aThickness value is odd)
 * aThicknessMode can be LINE_THICKNESS_MIDDLE or any other value
 *
 */
void QImageOperator::drawThickLineSimple(int aXStart, int aYStart, int aXEnd, int aYEnd,
                                         unsigned int aThickness, uint8_t aThicknessMode, QColor aColor)
{
    int16_t i, tDeltaX, tDeltaY, tDeltaXTimes2, tDeltaYTimes2, tError, tStepX, tStepY;
    
    tDeltaY = aXStart - aXEnd;
    tDeltaX = aYEnd - aYStart;
    // mirror 4 quadrants to one and adjust deltas and stepping direction
    if (tDeltaX < 0)
    {
        tDeltaX = -tDeltaX;
        tStepX = -1;
    } else
    {
        tStepX = +1;
    }
    if (tDeltaY < 0)
    {
        tDeltaY = -tDeltaY;
        tStepY = -1;
    } else
    {
        tStepY = +1;
    }
    tDeltaXTimes2 = tDeltaX << 1;
    tDeltaYTimes2 = tDeltaY << 1;
    bool tOverlap;
    // which octant are we now
    if (tDeltaX > tDeltaY)
    {
        if (aThicknessMode == LINE_THICKNESS_MIDDLE)
        {
            // adjust draw start point
            tError = tDeltaYTimes2 - tDeltaX;
            for (i = aThickness / 2; i > 0; i--)
            {
                // change X (main direction here)
                aXStart -= tStepX;
                aXEnd -= tStepX;
                if (tError >= 0)
                {
                    // change Y
                    aYStart -= tStepY;
                    aYEnd -= tStepY;
                    tError -= tDeltaXTimes2;
                }
                tError += tDeltaYTimes2;
            }
        }
        //draw start line
        drawLine(aXStart, aYStart, aXEnd, aYEnd, aColor);
        // draw aThickness lines
        tError = tDeltaYTimes2 - tDeltaX;
        for (i = aThickness; i > 1; i--)
        {
            // change X (main direction here)
            aXStart += tStepX;
            aXEnd += tStepX;
            tOverlap = LINE_OVERLAP_NONE;
            if (tError >= 0)
            {
                // change Y
                aYStart += tStepY;
                aYEnd += tStepY;
                tError -= tDeltaXTimes2;
                tOverlap = LINE_OVERLAP_BOTH;
            }
            tError += tDeltaYTimes2;
            drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, tOverlap, aColor);
        }
    } else
    {
        // adjust draw start point
        if (aThicknessMode == LINE_THICKNESS_MIDDLE)
        {
            tError = tDeltaXTimes2 - tDeltaY;
            for (i = aThickness / 2; i > 0; i--)
            {
                aYStart -= tStepY;
                aYEnd -= tStepY;
                if (tError >= 0)
                {
                    aXStart -= tStepX;
                    aXEnd -= tStepX;
                    tError -= tDeltaYTimes2;
                }
                tError += tDeltaXTimes2;
            }
        }
        //draw start line
        drawLine(aXStart, aYStart, aXEnd, aYEnd, aColor);
        tError = tDeltaXTimes2 - tDeltaY;
        for (i = aThickness; i > 1; i--)
        {
            aYStart += tStepY;
            aYEnd += tStepY;
            tOverlap = LINE_OVERLAP_NONE;
            if (tError >= 0)
            {
                aXStart += tStepX;
                aXEnd += tStepX;
                tError -= tDeltaYTimes2;
                tOverlap = LINE_OVERLAP_BOTH;
            }
            tError += tDeltaXTimes2;
            drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, tOverlap, aColor);
        }
    }
}

void QImageOperator::drawThickLineSimple(QPoint aStart, QPoint aEnd, unsigned int aThickness, uint8_t aThicknessMode,
                                         QColor aColor)
{
    drawThickLineSimple(aStart.x(), aStart.y(), aEnd.x(), aEnd.y(), aThickness, aThicknessMode, aColor);
}

void QImageOperator::ChangeSize(int bitmapSizeX, int bitmapSizeY)
{
    m_bitmap = QImage({bitmapSizeX, bitmapSizeY}, QImage::Format_RGB32);
    m_borderBitmap = QImage({bitmapSizeX, bitmapSizeY}, QImage::Format_RGB32);
    
    QBrush b(Qt::red), w(Qt::black);
    QPainter p(&m_bitmap), p2(&m_borderBitmap);
    p.fillRect(0, 0, bitmapSizeX, bitmapSizeY, b);
    p2.fillRect(0, 0, bitmapSizeX, bitmapSizeY, w);
    
    //m_borderBitmap.save("chuj.kurwa");
}

void QImageOperator::drawEllipse(QPoint center, QPoint radius, QColor color)
{
    int XRadius = radius.x(), YRadius = radius.y();
    int CX = center.x(), CY = center.y();
    int  X, Y;
    int XChange, YChange;
    int EllipseError;
    int TwoASquare, TwoBSquare;
    int StoppingX, StoppingY;
    TwoASquare = 2*XRadius*XRadius;
    TwoBSquare = 2*YRadius*YRadius;
    X = XRadius;
    Y = 0;
    XChange : YRadius*YRadius*(1 - 2*XRadius);
    YChange : XRadius*XRadius;
    EllipseError = 0;
    StoppingX = TwoBSquare*XRadius;
    StoppingY = 0;
    while ( StoppingX >= StoppingY )
    {
        Plot4EllipsePoints(CX, CY, X, Y, color);
        Y++;
        StoppingY += TwoASquare;
        EllipseError += YChange;
        YChange += TwoASquare;
        if ((2 * EllipseError + XChange) > 0)
            X--;
        StoppingX -= TwoBSquare;
        EllipseError += XChange;
        XChange += TwoBSquare;
    }
    
    //{ 1st point set is done; start the 2nd set of points }
    X = 0;
    Y = YRadius;
    XChange = YRadius*YRadius;
    YChange = XRadius*XRadius*(1 - 2*YRadius);
    EllipseError = 0;
    StoppingX = 0;
    StoppingY = TwoASquare*YRadius;
    while ( StoppingX <= StoppingY )
    {
        Plot4EllipsePoints(CX, CY, X, Y, color);
        X++;
        StoppingX += TwoBSquare;
        EllipseError += XChange;
        XChange += TwoBSquare;
        if ((2 * EllipseError + YChange) > 0)
            Y--;
        StoppingY -= TwoASquare;
        EllipseError += YChange;
        YChange += TwoASquare;
    }
}

void QImageOperator::Plot4EllipsePoints(int cx, int cy, int x, int y, QColor color)
{
    drawPixel(cx+x, cy+y, color);
    drawPixel(cx-x, cy+y, color);
    drawPixel(cx-x, cy-y, color);
    drawPixel(cx+x, cy-y, color);
}

void QImageOperator::FloodFill4(QPoint start, QVector3D simStartPos, QVector3D simFinishPos)
{
    static int counter = 0;
    std::stack<QPoint> s;
    s.push(start);
    
    while (!s.empty())
    {
        QPoint cur = s.top();
        s.pop();
        
        if (cur.x() < 0 || cur.x() >= m_bitmap.width() ||
            cur.y() < 0 || cur.y() >= m_bitmap.height())
            continue;
    
        //Jezeli pixel jest troche zielony to dotarlismy do granicy
        auto test = qRed(m_borderBitmap.pixel(cur));
        if (test > 0)
        {
            //m_borderBitmap.save("chuj.png");
            continue;
        }
    
        //Ewaluujemy kolor
        QColor color;
        float red = GetRedColorFunction(cur.x(), cur.y(), simStartPos, simFinishPos);
        if (red != red)//nan check
            continue;
        color.setRedF(red);
        drawPixel(cur.x(), cur.y(), color);
        m_borderBitmap.setPixelColor(cur.x(), cur.y(), Qt::red);
        //counter = (counter + 1) % 1000;
        /*if (counter == 0)
            m_borderBitmap.save("chuj.png");*/

        s.push({cur.x() + 1, cur.y()});
        s.push({cur.x(), cur.y() + 1});
        s.push({cur.x() - 1, cur.y()});
        s.push({cur.x(), cur.y() - 1});
    }
}

const QImage &QImageOperator::GetBorderBitmap() const
{
    return m_borderBitmap;
}

void QImageOperator::PrepareSphericalStamp(int textureRadiusX, int textureRadiusY, float R)
{
    isToolFlat = false;
    CreateStampData(textureRadiusX, textureRadiusY);
    
    for (int y = 0 ; y < m_stampY; ++y)
        for (int x = 0 ; x < m_stampX; ++x)
        {
            float dx = (abs(x - textureRadiusX) / (float)textureRadiusX) * R;
            float dy = (abs(y - textureRadiusY) / (float)textureRadiusY) * R;
            float dist = sqrt(dx * dx + dy * dy);
            
            if (dist > R)
                m_stamp[y * m_stampX + x] = NAN;
            else
                m_stamp[y * m_stampX + x] = R - sqrt(R * R - dist * dist);
        }
}

void QImageOperator::CreateStampData(int textureRadiusX, int textureRadiusY)
{
    m_stampX = 2 * textureRadiusX;
    m_stampY = 2 * textureRadiusY;
    m_stamp.resize(m_stampX * m_stampY);
}

void QImageOperator::ApplyStamp(int cx, int cy, float cutterSimHeight, float blockHeight, bool toolLowered)
{
    QColor c(Qt::black);
    int offsetX = -m_stampX/2;
    int offsetY = -m_stampY/2;
    
    for (int y = 0 ; y < m_stampY; ++y)
    {
        if (cy + offsetY + y < 0 || cy + offsetY + y >= m_bitmap.height())
            continue;
        
        for (int x = 0; x < m_stampX; ++x)
        {
            if (cx + offsetX + x < 0 || cx + offsetX + x >= m_bitmap.width())
                continue;
            
            if (m_stamp[y * m_stampX + x] != m_stamp[y * m_stampX + x])//nan check
                continue;
        
            float height = m_stamp[y * m_stampX + x] + cutterSimHeight;
            float curHeight = (qRed(m_bitmap.pixel(cx + offsetX + x, cy + offsetY + y))) / 255.0f *
                              blockHeight;//TODO: wywalic cala bitmape
            float toolSub = curHeight - height;
            if (toolSub < 0.0f)
                continue;
        
            if (toolSub > MaximumToolSubmersion)
                throw MillingException("Tool undercut detected");
        
            if (isToolFlat and toolSub > 0.0f and toolLowered)
                throw MillingException("Flat cutter went down in material");
        
            float texVal = height / blockHeight;
            c.setRedF(texVal);
            drawPixel(cx + offsetX + x, cy + offsetY + y, c);
        }
    }
}

void QImageOperator::CutterMove(QPoint texStart, QPoint texEnd, float startHeight, float endHeight, float blockHeight)
{
    float lineLength = sqrt((texEnd - texStart).x() * (texEnd - texStart).x() + (texEnd - texStart).y() * (texEnd - texStart).y());
    int x1 = texStart.x();
    int y1 = texStart.y();
    int x2 = texEnd.x();
    int y2 = texEnd.y();
    
    if (blockHeight - endHeight > MaximalGlobalSubmerison)
        throw MillingException("Milling below global limit detected");
    
    int d, dx, dy, ai, bi, xi, yi;
    int x = x1, y = y1;
    if (x1 < x2) {
        xi = 1;
        dx = x2 - x1;
    } else {
        xi = -1;
        dx = x1 - x2;
    }
    if (y1 < y2) {
        yi = 1;
        dy = y2 - y1;
    } else {
        yi = -1;
        dy = y1 - y2;
    }
    
    ApplyStamp(x, y, startHeight, blockHeight, startHeight - endHeight > 1e-5);
    if (dx > dy) {
        ai = (dy - dx) * 2;
        bi = dy * 2;
        d = bi - dx;
        while (x != x2) {
            float fromStart = sqrt((x - texStart.x()) * (x - texStart.x()) + (y - texStart.y()) * (y - texStart.y())) / lineLength ;
            float height = startHeight + fromStart * (endHeight - startHeight);
            if (d >= 0) {
                x += xi;
                y += yi;
                d += ai;
            } else {
                d += bi;
                x += xi;
            }
            ApplyStamp(x, y, height, blockHeight, startHeight - endHeight > 1e-5);
        }
    } else {
        ai = ( dx - dy ) * 2;
        bi = dx * 2;
        d = bi - dy;
        while (y != y2) {
            float fromStart = sqrt((x - texStart.x()) * (x - texStart.x()) + (y - texStart.y()) * (y - texStart.y())) / lineLength ;
            float height = startHeight + fromStart * (endHeight - startHeight);
            if (d >= 0) {
                x += xi;
                y += yi;
                d += ai;
            } else {
                d += bi;
                y += yi;
            }
            ApplyStamp(x, y, height, blockHeight, startHeight - endHeight > 1e-5);
        }
    }
}

void QImageOperator::PrepareCylindricalStamp(int textureRadiusX, int textureRadiusY, float R)
{
    isToolFlat = true;
    CreateStampData(textureRadiusX, textureRadiusY);
    for (int i = 0; i < m_stampX * m_stampY; ++i)
        m_stamp[i] = 0;
    
    for (int y = 0 ; y < m_stampY; ++y)
        for (int x = 0 ; x < m_stampX; ++x)
        {
            float dx = (abs(x - textureRadiusX) / (float)textureRadiusX) * R;
            float dy = (abs(y - textureRadiusY) / (float)textureRadiusY) * R;
            float dist = sqrt(dx * dx + dy * dy);
            
            if (dist > R)
                m_stamp[y * m_stampX + x] = NAN;
        }
}

/**
 * @}
 */
MillingException::MillingException(const std::string what): std::runtime_error(what)
{

}
