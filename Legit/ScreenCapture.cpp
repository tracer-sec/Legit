#include "ScreenCapture.hpp"

#include <png.h>

#ifdef _WIN32

#else
    #include <X11/Xlib.h>
    #include <X11/X.h>
#endif

using namespace Legit;
using namespace std;

void PngWriteCallback(png_struct *png, png_byte *data, png_size_t length)
{
    auto out = reinterpret_cast<vector<unsigned char> *>(png_get_io_ptr(png));
    out->insert(out->end(),data, data + length);
}

void SavePng(vector<unsigned char> data, unsigned int width, unsigned int height, vector<unsigned char> &out)
{
    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    auto info = png_create_info_struct(png);

    png_set_IHDR(png, info, width, height,
        8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_write_fn(png, &out, PngWriteCallback, nullptr);

    png_write_info(png, info);
    //setjmp(png_jmpbuf(png));

    for (unsigned int i = 0; i < height; i ++)
        png_write_row(png, &(data[i * width * 3]));

    //setjmp(png_jmpbuf(png));

    png_write_end(png, nullptr);
}

bool Legit::GrabScreen(vector<unsigned char> &out)
{
    int width, height;
    unsigned char *imageData;

    #ifdef _WIN32

    // TODO

    #else

    Display *display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);

    XWindowAttributes windowAttributes;

    XGetWindowAttributes(display, root, &windowAttributes);
    width = windowAttributes.width;
    height = windowAttributes.height;

    auto image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);
    imageData = reinterpret_cast<unsigned char *>(image->data);

    #endif

    vector<unsigned char> data(width * height * 3);

    for (int x = 0; x < width; x ++)
    {
        for (int y = 0; y < height; y ++)
        {
            unsigned int index = y * width + x;
            unsigned char b = imageData[index * 4 + 0];
            unsigned char g = imageData[index * 4 + 1];
            unsigned char r = imageData[index * 4 + 2];
            data[index * 3 + 0] = r;
            data[index * 3 + 1] = g;
            data[index * 3 + 2] = b;
        }
    }

    SavePng(data, width, height, out);
    
    return true;
}

