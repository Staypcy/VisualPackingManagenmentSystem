#include "entities/parkspace.h"

ParkSpace::ParkSpace(QWidget *parent)
    : QWidget(parent)
    , haveCar(false)
    , pos(0, 0)
{
    setStyleSheet(
        "background-color: #5cb85c;"
        "border: 2px solid #3d8b40;"
        "border-radius: 3px;"
    );
}
