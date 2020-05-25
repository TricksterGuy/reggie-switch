#include <switch.h>

struct gimp_img {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 pixel_data[720 * 1080 * 3 + 1];
};

extern gimp_img reggie;

#define FB_WIDTH  1280
#define FB_HEIGHT 720

// Most of this code adopted from the devkitpro switch/simplegfx.
int main(int argc, char* argv[])
{
    // Retrieve the default window
    NWindow* win = nwindowGetDefault();

    Framebuffer fb;
    framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
    framebufferMakeLinear(&fb);

    const u8* imageptr = reinterpret_cast<const u8*>(reggie.pixel_data);

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu

        // Retrieve the framebuffer
        u32 stride;
        u32* framebuf = (u32*) framebufferBegin(&fb, &stride);

        for (u32 y = 0; y < reggie.height; y++)
        {
            for (u32 x = 0; x < reggie.width; x++)
            {
                // Draw image rotated centered horizontally.
                u32 pos = y * reggie.width + x;
                u32 color = RGBA8_MAXALPHA(imageptr[pos*3+0], imageptr[pos*3+1], imageptr[pos*3+2]);
                framebuf[x * stride / sizeof(u32) + y + 100] = color;
            }
        }

        framebufferEnd(&fb);
    }

    framebufferClose(&fb);
    return 0;
}
