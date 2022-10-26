/*
 * thickLine.h
 *
 * @date 25.03.2013
 * @author  Armin Joachimsmeyer
 * armin.joachimsmeyer@gmail.com
 * @copyright LGPL v3 (http://www.gnu.org/licenses/lgpl.html)
 * @version 1.5.0
 */

#ifndef THICKLINE_H_
#define THICKLINE_H_

#include <cstdint>
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QVector3D>

/*
 * Overlap means drawing additional pixel when changing minor direction
 * Needed for drawThickLine, otherwise some pixels will be missing in the thick line
 */
#define LINE_OVERLAP_NONE 0    // No line overlap, like in standard Bresenham
#define LINE_OVERLAP_MAJOR 0x01 // Overlap - first go major then minor direction. Pixel is drawn as extension after actual line
#define LINE_OVERLAP_MINOR 0x02 // Overlap - first go minor then major direction. Pixel is drawn as extension before next line
#define LINE_OVERLAP_BOTH 0x03  // Overlap - both

#define LINE_THICKNESS_MIDDLE 0                 // Start point is on the line at center of the thick line
#define LINE_THICKNESS_DRAW_CLOCKWISE 1         // Start point is on the counter clockwise border line
#define LINE_THICKNESS_DRAW_COUNTERCLOCKWISE 2  // Start point is on the clockwise border line

#define ASSIGN_COLOR_FUNCTION(F)  std::bind(F, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)

/*#ifdef __cplusplus
extern "C" {
#endif*/

enum DrawState
{
    None = 0,
    NegativeX = 1,
    NegativeY = 2,
    Swap = 4
};

inline DrawState operator|(DrawState a, DrawState b)
{
    return static_cast<DrawState>(static_cast<int>(a) | static_cast<int>(b));
}

class MillingException: public std::runtime_error
{
public:
    explicit MillingException(const std::string what);
};

class QImageOperator
{
private:
    QImage m_bitmap;
    QImage m_borderBitmap;
    std::vector<float> m_stamp;
    int m_stampX;
    int m_stampY;
    bool isToolFlat;
    
    //void BresenhamAlgorithm(QPoint zero, QPoint diff, DrawState state, QColor color);
    void drawLineOverlap(int aXStart, int aYStart, int aXEnd, int aYEnd, uint8_t aOverlap,
                         QColor aColor);
    void ClampPointToDisplay(QPoint &p);
    void ClampPointToDisplay(int &x, int &y);
    void Plot4EllipsePoints(int cx, int cy, int x, int y, QColor color);
    void CreateStampData(int textureRadiusX, int textureRadiusY);
public:
    float MaximumToolSubmersion = 1.5;
    float MaximalGlobalSubmerison = 3.5;
    
    QImageOperator(int bitmapSizeX, int bitmapSizeY);
    QImageOperator();
    void ChangeSize(int bitmapSizeX, int bitmapSizeY);
    
    //bool DrawOnlyLowerValues = true;
    std::function<float(int bitmapX, int bitmapY, QVector3D startCutterPos, QVector3D endCutterPos)> GetRedColorFunction = {};
    
    //void DrawPixel(QPoint zero, int x, int y, DrawState state, QColor color);
    void inline drawPixel(int x, int y, QColor aColor);
    //void drawLine(QPoint A, QPoint B, QColor aColor);
    void drawLine(int aXStart, int aYStart, int aXEnd, int aYEnd, QColor aColor);
    void fillRect(QPoint A, QPoint B, QColor aColor);
    void fillRect(int aXStart, int aYStart, int aXEnd, int aYEnd, QColor aColor);
    void drawThickLine(QPoint aStart, QPoint aEnd, QVector3D simStartPos, QVector3D simFinishPos, unsigned int aThickness, uint8_t aThicknessMode);
    void drawThickLine(int aXStart, int aYStart, int aXEnd, int aYEnd, QVector3D simStartPos, QVector3D simFinishPos,
                       unsigned int aThickness, uint8_t aThicknessMode);
    void drawThickLineSimple(QPoint aStart, QPoint aEnd, unsigned int aThickness, uint8_t aThicknessMode,
                             QColor aColor);
    void drawThickLineSimple(int aXStart, int aYStart, int aXEnd, int aYEnd,
                             unsigned int aThickness, uint8_t aThicknessMode, QColor aColor);
    
    void drawEllipse(QPoint center, QPoint radius, QColor color);
    void FloodFill4(QPoint start, QVector3D simStartPos, QVector3D simFinishPos);
    
    void PrepareSphericalStamp(int textureRadiusX, int textureRadiusY, float R);
    void PrepareCylindricalStamp(int textureRadiusX, int textureRadiusY, float R);
    void ApplyStamp(int cx, int cy, float cutterSimHeight, float blockHeight);
    void CutterMove(QPoint texStart, QPoint texEnd, float startHeight, float endHeight, float blockHeight);
    
    [[nodiscard]] const QImage &GetBitmap() const;
    [[nodiscard]] const QImage &GetBorderBitmap() const;
};

/*#ifdef __cplusplus
}
#endif*/

#endif /* THICKLINE_H_ */
