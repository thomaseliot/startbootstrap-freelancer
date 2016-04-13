/**************************************************************************//**
 * @file plotEx.h
 * @author Jeff Stewart
 * @brief This file provides definitions and a structure for the external
 *        plot object interface functions.
 * @details
 * @version RTC-<2345>: (Dec 5, 2013) Initial development.
 *****************************************************************************/
/*
 * Copyright (c) 2013 Altia Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia Deep Screen license. Permission to sell or
 * distribute this source code is denied. Altia makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *****************************************************************************/
#ifndef PLOTEX_H_
#define PLOTEX_H_

/******************************************************************
 * Determine if function prototyping is enabled.
 ******************************************************************/
#ifndef Altiafp
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus) || defined(_MSC_VER)
#define Altiafp
#else
#undef Altiafp
#endif /* Check for function prototyping requirement */
#endif /* Altiafp */

/******************************************************************************
 * Plot External Interface types and structures
 *****************************************************************************/

/*! Contains a plot's id (object id) */
typedef signed long PlotId;

/*! Contains the id of a specific line on a plot. */
typedef signed long PlotLineId;
#define PLOT_LINE_ID_INVALID    (-1)

/*! Contains the id of a plot buffer created by the user. */
typedef signed long PlotBuffId;

/*! Defines a color in HEX 0xAARRGGBB format */
typedef unsigned long PlotColorVal;

/*
 * This is the data type for almost all data within the plot object. Set
 * it as appropriate for the build environment you are in.
 */
typedef ALTIA_DOUBLE plotData_t;
#define PLOT_DATA_T_MAX_VALUE   (plotData_t)DBL_MAX
#define PLOT_DATA_T_MIN_VALUE   (plotData_t)DBL_MIN

/*!
 * @brief Struct to hold data for one plotted point.
 */
typedef struct
{
    plotData_t x; //!< X Coordinate Value.
    plotData_t y; //!< Y Coordinate Value.
}PlotPoint;


/* Macros to load point data */
#define PLOT_INTEGER(value)   (plotData_t)((value) << 14)
#define PLOT_DOUBLE(value)    (plotData_t)((double)(value) * 0x4000)



/*!
 * @brief Enumerates the different plotting modes.
 */
typedef enum
{
    PLOT_XY = 0,    //!< Standard XY plot.
    PLOT_STRIPCHART //!< Plots data vs. time in a continuous fashion.
}PlotMode;

/*!
 * @brief Enumerates the different plot axes.
 */
typedef enum
{
    X_AXIS,
    Y_AXIS,
    AXIS_COUNT /* Must be last */
}PlotAxisId;

/*!
 * @brief Enumerated some common line styles.
 */
typedef enum
{
    NO_LINE=0x0000,
    DOTTED_LINE=0xAAAA,
    LARGE_DOTTED_LINE = 0xCCCC,
    SMALL_DASHED_LINE=0xF0F0,
    DASHED_LINE=0xFF00,
    DASHDOT_LINE=0xFF18,
    SOLID_LINE=0xFFFF
}PlotLineStyle;

/*!
 * @brief Structure to hold value axis label data.
 */
typedef struct
{
    ALTIA_COORD pixelPosition; //!< Pixel Location of the label along the axis.
    plotData_t value; //!< Label value.
}PlotAxisLabel;

typedef void * PlotAnnotationId;

typedef struct
{
    plotData_t x;
    plotData_t y;
    PlotColorVal color;
}PlotPixel;

typedef struct
{
    plotData_t x0; //!< X Location of drawing start (may be bottom left for rectangles or the center for circles, etc.)
    plotData_t y0; //!< Y Location of drawing start (may be bottom left for rectangles or the center for circles, etc.)
    plotData_t x1;
    plotData_t y1;
    PlotColorVal lineColor;
    PlotLineStyle lineStyle;
    ALTIA_UINT32 lineWidth;
}PlotLineSegment;

typedef struct
{
    plotData_t x0; //!< X Location of drawing start (may be bottom left for rectangles or the center for circles, etc.)
    plotData_t y0; //!< Y Location of drawing start (may be bottom left for rectangles or the center for circles, etc.)
    plotData_t x1;
    plotData_t y1;
    PlotColorVal fillColor;
    PlotColorVal strokeColor;
    PlotLineStyle strokeStyle;
    ALTIA_UINT32 strokeWidth;
}PlotRectangle;

typedef struct
{
    plotData_t x0;
    plotData_t y0;
    plotData_t xRadius;
    plotData_t yRadius;
    PlotColorVal fillColor;
    PlotColorVal strokeColor;
    PlotLineStyle strokeStyle;
    ALTIA_UINT32 strokeWidth;
}PlotEllipse;

typedef struct
{
    plotData_t * xPts;
    plotData_t * yPts;
    ALTIA_UINT32 numPts;
    PlotColorVal fillColor;
    PlotColorVal strokeColor;
    PlotLineStyle strokeStyle;
    ALTIA_UINT32 strokeWidth;
}PlotPolygon;

typedef struct
{
    plotData_t x0;
    plotData_t y0;
    ALTIA_CHAR * text;  /* NULL Terminated */
    PlotColorVal color;
}PlotText;

/******************************************************************************
 * Plot External Interface functions
 *****************************************************************************/
/*! Creates a circular buffer for use with a plot object. */
extern PlotBuffId altiaExCreatePlotBuffer(
#ifdef Altiafp
        unsigned int bufferSize,
        PlotPoint * bufferStorageLoc,
        unsigned int bufferStorageSize);
#endif

typedef PlotBuffId (*altiaExCreatePlotBufferFunc)(
#ifdef Altiafp
        unsigned int bufferSize,
        PlotPoint * bufferStorageLoc,
        unsigned int bufferStorageSize);
#endif

/*! Destroys a plot buffer and deallocates any associated memory. */
extern ALTIA_BOOLEAN altiaExDestroyPlotBuffer(
#ifdef Altiafp
        PlotBuffId buffId);
#endif

typedef ALTIA_BOOLEAN (*altiaExDestroyPlotBufferFunc)(
#ifdef Altiafp
        PlotBuffId buffid);
#endif


/*! Puts data into a plot buffer at the head position. */
extern ALTIA_BOOLEAN altiaExInsertPlotDataAtHead(
#ifdef Altiafp
        PlotBuffId buffId,
        PlotPoint * dataIn,
        unsigned int dataCount);
#endif

typedef ALTIA_BOOLEAN (*altiaExInsertPlotDataAtHeadFunc)(
#ifdef Altiafp
        PlotBuffId buffId,
        PlotPoint * dataIn,
        unsigned int dataCount);
#endif

/*! Puts data into a plot buffer at the tail position. */
extern ALTIA_BOOLEAN altiaExInsertPlotDataAtTail(
#ifdef Altiafp
        PlotBuffId buffId,
        PlotPoint * dataIn,
        unsigned int dataCount);
#endif

typedef ALTIA_BOOLEAN (*altiaExInsertPlotDataAtTailFunc)(
#ifdef Altiafp
        PlotBuffId buffId,
        PlotPoint * dataIn,
        unsigned int dataCount);
#endif

/*! Reads data at an offset from the head position. */
extern ALTIA_BOOLEAN altiaExGetDataAtOffsetFromHead(
#ifdef Altiafp
        PlotBuffId buffId,
        unsigned int offset,
        PlotPoint * dataOut,
        unsigned int dataCount);
#endif

typedef ALTIA_BOOLEAN (*altiaExGetDataAtOffsetFromHeadFunc)(
#ifdef Altiafp
        PlotBuffId buffId,
        unsigned int offset,
        PlotPoint * dataOut,
        unsigned int dataCount);
#endif

/*! Reads data an an offset from the tail position. */
extern ALTIA_BOOLEAN altiaExGetDataAtOffsetFromTail(
#ifdef Altiafp
        PlotBuffId buffId,
        unsigned int offset,
        PlotPoint * dataOut,
        unsigned int dataCount);
#endif

typedef ALTIA_BOOLEAN (*altiaExGetDataAtOffsetFromTailFunc)(
#ifdef Altiafp
        PlotBuffId buffId,
        unsigned int offset,
        PlotPoint * dataOut,
        unsigned int dataCount);
#endif

/*! Registers a circular buffer with a plot object for use. */
extern ALTIA_BOOLEAN altiaExPlotRegisterDataBuffer(
#ifdef Altiafp
        PlotBuffId buffId,
        PlotId plotId,
        PlotLineId line
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExPlotRegisterDataBufferFunc)(
#ifdef Altiafp
        PlotBuffId buffId,
        PlotId plotId,
        PlotLineId line
#endif
        );

/*!
 * @brief Unregisters a circular buffer with a plot.
 */
extern ALTIA_BOOLEAN altiaExPlotUnregisterDataBuffer(
#ifdef Altiafp
    PlotBuffId buffId,
    PlotId plotId,
    PlotLineId line
#endif
    );

typedef ALTIA_BOOLEAN (*altiaExPlotUnregisterDataBufferFunc)(
#ifdef Altiafp
    PlotBuffId buffId,
    PlotId plotId,
    PlotLineId line
#endif
    );

/*!
 * @brief Informs plot object that it has been updated and it should redraw.
 */
extern ALTIA_BOOLEAN altiaExPlotUpdate(
#ifdef Altiafp
        PlotId plotId
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExPlotUpdateFunc)(
#ifdef Altiafp
        PlotId plotId
#endif
        );

/*!
 * @brief Preloads a plot's internal buffer with data from the
 *        indicated plot buffer.
 */
extern ALTIA_BOOLEAN altiaExPreloadPlotWithBuffData(
#ifdef Altiafp
        PlotId plotId,
        PlotBuffId buffId
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExPreloadPlotWithBuffDataFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotBuffId buffId
#endif
        );

/*!
 * @brief Gets plot axis labels for major grid locations
 */
extern ALTIA_BOOLEAN altiaExGetAxisMajorGridLabels(
#ifdef Altiafp
        PlotId plotId,
        PlotAxisId ax,
        ALTIA_UINT32 * count,
        PlotAxisLabel * labels
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExGetAxisMajorGridLabelsFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotAxisId ax,
        ALTIA_UINT32 * count,
        PlotAxisLabel * labels
#endif
        );

/*!
 * @brief Draws one-pixel annotation on a plot
 */
extern ALTIA_BOOLEAN altiaExDrawPlotPoint(
#ifdef Altiafp
        PlotId plotId,
        PlotPixel * point,
        PlotAnnotationId * id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExDrawPlotPointFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotPixel * point,
        PlotAnnotationId * id
#endif
        );

/*!
 * @brief Draws line annotation on a plot
 */
extern ALTIA_BOOLEAN altiaExDrawPlotLine(
#ifdef Altiafp
        PlotId plotId,
        PlotLineSegment * line,
        PlotAnnotationId * id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExDrawPlotLineFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotLineSegment * line,
        PlotAnnotationId * id
#endif
        );

/*!
 * @brief Draws rectangle annotation on a plot
 */
extern ALTIA_BOOLEAN altiaExDrawPlotRect(
#ifdef Altiafp
        PlotId plotId,
        PlotRectangle * rect,
        PlotAnnotationId * id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExDrawPlotRectFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotRectangle * rect,
        PlotAnnotationId * id
#endif
        );

/*!
 * @brief Draws ellipse annotation on a plot
 */
extern ALTIA_BOOLEAN altiaExDrawPlotEllipse(
#ifdef Altiafp
        PlotId plotId,
        PlotEllipse * ellip,
        PlotAnnotationId * id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExDrawPlotEllipseFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotEllipse * ellip,
        PlotAnnotationId * id
#endif
        );

/*!
 * @brief Draws polygon annotation on a plot
 */
extern ALTIA_BOOLEAN altiaExDrawPlotPolygon(
#ifdef Altiafp
        PlotId plotId,
        PlotPolygon * poly,
        PlotAnnotationId * id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExDrawPlotPolygonFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotPolygon * poly,
        PlotAnnotationId * id
#endif
        );

/*!
 * @brief Draws text annotation on a plot
 */
extern ALTIA_BOOLEAN altiaExDrawPlotText(
#ifdef Altiafp
        PlotId plotId,
        PlotText * txt,
        PlotAnnotationId * id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExDrawPlotTextFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotText * txt,
        PlotAnnotationId * id
#endif
        );

/*!
 * @brief Removes an annotation from a plot
 */
extern ALTIA_BOOLEAN altiaExRemovePlotAnnotation(
#ifdef Altiafp
        PlotId plotId,
        PlotAnnotationId id
#endif
        );

typedef ALTIA_BOOLEAN (*altiaExRemovePlotAnnotationFunc)(
#ifdef Altiafp
        PlotId plotId,
        PlotAnnotationId id
#endif
        );

typedef struct
{
    altiaExCreatePlotBufferFunc             altiaExCreatePlotBuffer;
    altiaExDestroyPlotBufferFunc            altiaExDestroyPlotBuffer;
    altiaExInsertPlotDataAtHeadFunc         altiaExInsertPlotDataAtHead;
    altiaExInsertPlotDataAtTailFunc         altiaExInsertPlotDataAtTail;
    altiaExGetDataAtOffsetFromHeadFunc      altiaExGetDataAtOffsetFromHead;
    altiaExGetDataAtOffsetFromTailFunc      altiaExGetDataAtOffsetFromTail;
    altiaExPlotRegisterDataBufferFunc       altiaExPlotRegisterDataBuffer;
    altiaExPlotUnregisterDataBufferFunc     altiaExPlotUnregisterDataBuffer;
    altiaExPlotUpdateFunc                   altiaExPlotUpdate;
    altiaExPreloadPlotWithBuffDataFunc      altiaExPreloadPlotWithBuffData;
    altiaExGetAxisMajorGridLabelsFunc       altiaExGetAxisMajorGridLabels;
    altiaExDrawPlotPointFunc                altiaExDrawPlotPoint;
    altiaExDrawPlotLineFunc                 altiaExDrawPlotLine;
    altiaExDrawPlotRectFunc                 altiaExDrawPlotRect;
    altiaExDrawPlotEllipseFunc              altiaExDrawPlotEllipse;
    altiaExDrawPlotPolygonFunc              altiaExDrawPlotPolygon;
    altiaExDrawPlotTextFunc                 altiaExDrawPlotText;
    altiaExRemovePlotAnnotationFunc         altiaExRemovePlotAnnotation;
}AltiaPlotFuncs_t;

#include "circbuffer.h"

#endif // PLOTEX_H_ 
