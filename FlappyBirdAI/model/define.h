#pragma once

struct POS
{
    float x;
    float y;
};


struct PARABOLA_POINT
{
    int x;
    int y;

    // type: 0-origin data, 1-fill in data
    BYTE type;
};


#define CANVAS_BORDER_COLOR     101
#define CANVAS_BORDER_MIN_AREA  30000

// size that canvas finally scale to
#define CANVAS_SCALETO_WIDTH    (383/2)
#define CANVAS_SCALETO_HEIGHT   (511/2)

#define CANVAS_NOGROUND_HEIGHT  223

// ratio rect of bird positionX in canvas
#define BIRDX_RATIO             0.314f
#define BIRDX_RATIO_OFFSET      0.02f

#define BIRD_CONTOUR_AREA           250
#define BIRD_CONTROU_AREA_OFFSET    50


#define PARABOLA_GRAPH_W        200
#define PARABOLA_GRAPH_H        150

#define JUMPRANGE_GRAPH_W       160
#define JUMPRANGE_GRAPH_H       100


#define WINDOW_NAME_CANVAS      "Canvas"
#define WINDOW_NAME_PARABOLA    "Parabola"
#define WINDOW_NAME_JUMPRANGE   "Jump Range"

#define DEFAULT_PIPE_HEIGHT     0.5f

#define PIPE_VERTICAL_DISTANCE  (64.0f/CANVAS_NOGROUND_HEIGHT)

#define MIN_HEIGHT_DATA_TO_JUMP 20