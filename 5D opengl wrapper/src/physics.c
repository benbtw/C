#include "physics.h"

// Function to check for collision between two rectangles
CollisionSide checkCollision(const bRect *rect1, const bRect *rect2)
{
    // Check if rect1's right edge is to the right of rect2's left edge
    bool xOverlap = rect1->x < rect2->x + rect2->w && rect1->x + rect1->w > rect2->x;

    // Check if rect1's bottom edge is below rect2's top edge
    bool yOverlap = rect1->y < rect2->y + rect2->h && rect1->y + rect1->h > rect2->y;

    // If there is overlap along both axes, the rectangles intersect
    if (xOverlap && yOverlap)
    {
        // Calculate overlap on each side
        float overlapTop = rect2->y + rect2->h - rect1->y;
        float overlapBottom = rect1->y + rect1->h - rect2->y;
        float overlapLeft = rect2->x + rect2->w - rect1->x;
        float overlapRight = rect1->x + rect1->w - rect2->x;

        // Determine which side has the least overlap
        float minOverlap = fminf(fminf(overlapTop, overlapBottom), fminf(overlapLeft, overlapRight));

        if (minOverlap == overlapTop)
            return COLLISION_TOP;
        else if (minOverlap == overlapBottom)
            return COLLISION_BOTTOM;
        else if (minOverlap == overlapLeft)
            return COLLISION_LEFT;
        else
            return COLLISION_RIGHT;
    }

    // No collision
    return NO_COLLISION;
}

// Increase x, y with speed and using a velocity vector
void Move(int *x, int *y, int speedX, int speedY, float dt)
{
    // need to add plus one when the speed is positive due to some weird bug with deltaTime,
    // so it can match the same speed when it is negative
    if (speedX > 0)
        *x += speedX * dt + 1;
    if (speedY > 0)
        *y += speedY * dt + 1;
    if (speedX < 0 || speedY < 0)
    {
        *x += speedX * dt;
        *y += speedY * dt;
    }
}