#pragma once

struct POS
{
    float x;
    float y;
};

#define CANVAS_BORDER_COLOR     101
#define CANVAS_BORDER_MIN_AREA  30000

// size that canvas finally scale to
#define CANVAS_SCALETO_WIDTH    (383/2)
#define CANVAS_SCALETO_HEIGHT   (511/2)

// ratio rect of bird positionX in canvas
#define BIRDX_RATIO             0.314f
#define BIRDX_RATIO_OFFSET      0.02f

#define BIRD_CONTOUR_AREA           250
#define BIRD_CONTROU_AREA_OFFSET    50
