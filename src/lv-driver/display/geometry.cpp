#include "lv-driver/display/geometry.h"

Region::Region(){ 
    origin = (Point) { (unsigned short) 0, (unsigned short) 0 };
    size = (Size){ (unsigned short) 0, (unsigned short) 0 };
};

Region::Region(unsigned short width, unsigned short height){
    origin = (Point) { (unsigned short) 0, (unsigned short) 0 };
    size = (Size){ width, height };
}