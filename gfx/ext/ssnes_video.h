/////
// API header for external SSNES video and input plugins.
//
//

#ifndef __SSNES_VIDEO_DRIVER_H
#define __SSNES_VIDEO_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define SSNES_API_DECL __decltype(dllexport) __cdecl
#else
#define SSNES_API_DECL
#endif

// Since we don't want to rely on C++ or C99 for a proper boolean type, and make sure return semantics are perfectly clear ... ;)
#define SSNES_OK 1
#define SSNES_ERROR 0
#define SSNES_TRUE 1
#define SSNES_FALSE 0

#define SSNES_COLOR_FORMAT_XRGB1555 0
#define SSNES_COLOR_FORMAT_ARGB8888 1

typedef struct ssnes_video_info
{ 
   // Width of window. 
   // If fullscreen mode is requested, a width of 0 means the resolution of the desktop should be used.
   unsigned width;

   // Height of window. 
   // If fullscreen mode is requested, a height of 0 means the resolutiof the desktop should be used.
   unsigned height;

   // If true, start the window in fullscreen mode.
   int fullscreen;

   // If true, VSync should be enabled.
   int vsync;

   // If true, the output image should have the aspect ratio as set in aspect_ratio.
   int force_aspect;

   // Aspect ratio. Only takes effect if force_aspect is enabled.
   float aspect_ratio;

   // Requests that the image is smoothed, using bilinear filtering or otherwise.
   // If this cannot be implemented efficiently, this can be disregarded.
   // If smooth is false, nearest-neighbor scaling is requested.
   int smooth;

   // input_scale defines the maximum size of the picture that will ever be used with the frame callback.
   // This scale is relative to 256x256 size, so an input scale of 2 means you should allocate a texture or of 512x512. It is normally set to 2 due to the possibility of pseudo-hires on the SNES. CPU filters or otherwise might increase this value.
   unsigned input_scale;

   // Defines the coloring format used of the input frame.
   // XRGB1555 format is 16-bit and has byte ordering: 0RRRRRGGGGGBBBBB, in native endian.
   // ARGB8888 is AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB, native endian. Alpha channel should be disregarded.
   int color_format;

   // If non-NULL, requests the use of an XML shader. Can be disregarded.
   const char *xml_shader;

   // Requestes that a certain TTF font is used for rendering messages to the screen.
   // Can be disregarded.
   const char *ttf_font;
} ssnes_video_info_t;

#define SSNES_AXIS_NEG(x) (((unsigned)(x) << 16) | 0xFFFFU)
#define SSNES_AXIS_POS(x) ((unsigned)(x) | 0xFFFF0000U)
#define SSNES_AXIS_NONE (0xFFFFFFFFU)

#define SSNES_AXIS_NEG_GET(x) (((unsigned)(x) >> 16) & 0xFFFFU)
#define SSNES_AXIS_POS_GET(x) ((unsigned)(x) & 0xFFFFU)

#define SSNES_NO_BTN ((unsigned short)0xFFFFU) // I hope no joypad will ever have this many buttons ... ;)

#define SSNES_HAT_UP_MASK (1 << 15)
#define SSNES_HAT_DOWN_MASK (1 << 14)
#define SSNES_HAT_LEFT_MASK (1 << 13)
#define SSNES_HAT_RIGHT_MASK (1 << 12)
#define SSNES_HAT_MAP(x, hat) ((x & ((1 << 12) - 1)) | hat)

#define SSNES_HAT_MASK (HAT_UP_MASK | HAT_DOWN_MASK | HAT_LEFT_MASK | HAT_RIGHT_MASK)
#define SSNES_GET_HAT_DIR(x) (x & HAT_MASK)
#define SSNES_GET_HAT(x) (x & (~HAT_MASK))

struct ssnes_keybind
{
   // If analog_x is true, we request an analog device to be polled. The returned value should be the delta of last frame and current frame in the X-axis.
   int analog_x;
   // If analog_y is true, we request an analog device to be polled. The returned value should be the delta of last frame and current frame in the Y-axis.
   int analog_y;

   // Keyboard key. The key values use the SDL keysyms, which probably need to be transformed to the native format.
   unsigned short key;

   // Joypad key.
   unsigned short joykey;

   // Joypad axis. Negative and positive axes are embedded into this variable.
   unsigned joyaxis;
};

typedef struct ssnes_input_driver
{
   // Inits input driver.
   void* (*init)(const unsigned joypad_index[5]);

   // Polls input. Called once every frame.
   void (*poll)(void* data);

   // Queries input state for a certain key on a certain player. Players are 1 - 5.
   int (*input_state)(void* data, const struct snes_keybind *bind, unsigned player);

   // Frees the input struct.
   void (*free)(void* data);

   // Human readable indentification string.
   const char *ident;
} ssnes_input_driver_t;

typedef struct ssnes_video_driver
{
   // Inits the video driver. Returns an opaque handle pointer to the driver.
   // Returns NULL on error.
   //
   // Should the video driver request that a certain input driver is used,
   // it is possible to set the driver to *input, and driver handle to *input_data.
   // If no certain driver is desired, set both of these to NULL.
   void* (*init)(const video_info_t *video, const input_driver_t **input, void **input_data); 

   // Updates frame on the screen. frame can be either XRGB1555 or ARGB32 format depending on rgb32 setting in ssnes_video_info_t. Pitch is the distance in bytes between two scanlines in memory. When msg is non-NULL, it's a message that should be displayed to the user.
   int (*frame)(void* data, const void* frame, unsigned width, unsigned height, unsigned pitch, const char *msg);

   // Requests nonblocking operation. True = VSync is turned off. False = VSync is turned on.
   void (*set_nonblock_state)(void* data, int toggle);

   // This must return false when the user exits the emulator.
   int (*alive)(void *data);

   // Does the window have focus?
   int (*focus)(void *data);

   // Sets an XML shader. Implementing this function is optional, and can be set to NULL.
   int (*xml_shader)(void *data, const char *path);

   // Frees the video driver.
   void (*free)(void* data);

   // A human-readable identification of the video driver.
   const char *ident;
} video_driver_t;

SSNES_API_DECL const ssnes_video_info_t* ssnes_video_init(void);

#ifdef __cplusplus
}
#endif

#endif