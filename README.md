# Image_Processing
A short project to share and explain some image processing algorithms.

## General description
As of 2025, image processing is used everywhere, be that face recognition or just to make that one scatter plot a little bit more according-to-what-is-expected using MS Paint. Image processing tools are available and simple to use, plus they never expose the user to the algorithms that are being utilised. All is conveniently one click away on a PC or a smart phone.

On the other hand, once we need something simple to be done on a platform that is not meant to be used for such processes - i.e., mcu-s – we can find ourselves very quickly stuck between a rock and hard place.

I want to touch upon here on a few tricks I have implemented on an STM32F429VE to share, what can be done and have some form of image processing done on an mcu. Of note, unlike before, I won’t be doing a full project here but simply share the algorithms that could be then applied to an image buffer. The reason for this is that publishing and loading the image buffer by itself would merit an entire project (see my digicam project on the STM32F429ZI disco board) and putting that in here would distract from the core message of this repo.

## Limiting factors
To be fair, most projects aiming to run algorithms like the ones presented here would be running them on the biggest, meanest mcu, which would have significant resources readily available to be exploited by the algorithm. As a matter of fact, one can easily find example projects doing way more than what I discuss here running on ESP32 devices (just check the Memento from Adafruit if you want to be impressed). As such, for anyone not interested in these algorithms, just get an ESP32 and use their existing libraries. They are good and do the trick perfectly well.

For me though who is interested in the “hows” not just the results, I will be sticking with the rather limited STM32F429 mcu. After all, that is my baseline device.

Anyway, we can’t really go far without discussing the limiting factors of such endeavour right of the bat.

### Memory
Memory will be a highly sensitive element in any image processing project. Firstly, because we need to store our image somewhere (in an image buffer) and secondly, we need to make it accessible to our processor without too much time lost. This makes RAM as the most obvious place for our image storage, even though that is likely to be the most limited memory source we have available on the mcu (for the F429, merely 200 kbytes). Internal ROM/FLASH could be used, though I urge against it for stability reasons. Fast enough external FLASH elements (QUAD SPI ICs, for instance, or an appropriately attached SDRAM) may come as a potential middle ground though.

There are two additional things to mention here though:
-	In case we intend to use Touch GFX or LVGL (graphics libraries for mcus), we may need multiple image buffers at the same time to handle transitions.
-	We can limit the image buffer by capturing only a section of the image at one time and then gluing the final result together from these sections by using, say a bmp file opened with “append” command in FatFs. (This should be simple to implement using the projects I have already shared.)
-	
Anyway, I will assume that we will have just one image buffer to store the 320x240 pixel 16-bit RGB565 image and that will store our original image. This will take up 153600 bytes of RAM, which will take away most of the RAM available in the F429. Not ideal, but we can work with it.

### The data package
Publishing and processing speed rarely comes up on small platforms as an issue since we tend to be just happy that we have some form of output to begin with.

If data must flow at an acceptable speed, that is a whole different beast. In case of embedded screens for instance, if we want to avoid a “choppy” live image, we must reach 30 fps or more. This demands that any kind of processing must be done in less than 1/30 = 30 ms.

Self-imposed limitations on what we generate from the image can somewhat aid with the speed since the smaller we can make the generated data package, the faster we can push it through a communication bus or publish it to a screen (assuming clocking is not changed). In other cases, we might be limited in the size of the data package we could load to a data bus at one time as it sometimes happens with Bluetooth.

Anyway, such limitation could be processing a fraction of the image at one time to meet the timing deadline or by decreasing the information within the complete image buffer while still retaining an acceptable level of image detail. (I will only discuss the second option.)

## Previous relevant projects:
Not any, though my digital camera project on the F429 disco board could be used to load the proposed image buffer up with relevant data:
- STM32_SCREEN_LTDC_DCMI_OV7670

## To read
I suggest checking the Wikipedia articles for each algorithm for additional information (see below).

## Particularities
We will be using a linear array of 153600 bytes as image buffer, assuming that it is storing a 16-bit 320x240 RGB565 image. The choice of buffer may not come as obvious, though it must be linear to be compatible with loading communication busses in a serial fashion.

This simplifies and complicates things the same time, as we will see soon enough.
 
### Luminance extraction algorithm for monochrome image
The two most common ways to store raw image data is by using RGB – red, green and blue – colours in various bits-in-bytes mixes (RGB444, RGB555, RGB565) or by something called YUV – luminance and two chroma.

In practical sense, RGB stores the ratio between the three base colours, while YUV stores the luminance/brightness of the pixel in “Y” and the ratio between the base colours in “U” and “V” by using a dedicated U-V colour plane.

Technically, it is perfectly possible to extract one colour value from RGB and zero out all the rest to generate a uni-coloured image, though this wouldn’t be a true monochrome image: it would only be one colour component of an otherwise multi-coloured image. As such, taking an already monochrome blue image and converting it to green monochrome would just lead to an empty/black image.

To generate a true monochrome image, we need to extract the luminance from the image and then “paint” that luminance matrix to a desired colour. Luminance of any coloured image is just a normal black and white image, where each 8-bit value shows the intensity of a pixel.

In the first algorithm, I am implementing a luminance colour calculation after the three RGB565 colours have been normalized to 8 bits. The received luminance value then can be de-normalized to any colour we wish by converting it back to RGB565.

Of note, most embedded cameras have the option to generate a YUV output off the bat, making this conversion irrelevant. Just pay attention, which YUV format the output will be in.
For further information, check Y′UV - Wikipedia.

### Dithering and 1bpp
Dithering is a type of error diffusing algorithm. It means, practically speaking, that we are actively removing data from an image. If done well though, we can trim as much fat from our data package as possible while still retaining basic information.

A good example are the Floyd-Steinberg (Floyd–Steinberg dithering - Wikipedia) and Atkinson (Atkinson dithering - Wikipedia) dithering algorithms, which can turn any image into a 1-bpp (1 bit per pixel) form of itself, drastically reducing the data stored in the data package (in case of RGB565, by 16 times) without compromising the most critical information within the image data (i.e., edges). Thus, in case we only need such critical information extracted from our image, we can speed up our data flow by sacrificing data quality and quantity.

In the code, I am sharing the algorithm for both the Floyd and the Atkinson dithering, though personally I have found the Atkinson to be better for edge detection while Floyd is “smoother” and significantly simpler to implement.

We do the dithering on the luminance array generated above since it is simply easier to define a threshold there compared to a full RGB565 image.

### Image upscaling
It may come to be that the original image we have in our buffer just does not fit the size we need. Ideally, we just take a higher resolution image, though this may not always be possible for hardware reasons (e.g., not enough memory, not enough camera resolution). In such case, we have to upscale our original image by using some form of scaling algorithm.

I am showing the simplest one here which is called “nearest-neighbour interpolation”. What it does is turning 1 pixel into a 2x2 pixel array of itself. The result is a 4-times upscaled image, significantly more pixelated than the original one. The Wikipedia page discusses more complex upscaling algorithms too (Comparison gallery of image scaling algorithms - Wikipedia).

Of note, we don’t have enough RAM to do the scaling on neither 16-bit, not 8-bit versions of our image, thus the choice of 1bpp.  With 1bpp, this simple interpolation works perfectly fine.

In the example I share, we go from a 320x240 1bpp dithered image to a 640x480 dithered 1bpp image.

### Image rotation
Our image buffer for this section is a linear array of 1bpp values packaged up as uint16_t. This means that each half-word stores not one pixel but sixteen (!) pixels.

Obviously, this makes it difficult to rotate since each half-word needs to be split and rearranged - unlike how it would be just a simple array element calling in 2D to rotate.

The algorithm I am sharing here has been partially found as trial-and-error testing, thus I have little explanation to share on it. Nor do I have a Wikipedia page to point to.

At any rate, it works.

## User guide
The source file includes the five functions for the algorithm. Simply transfer the function to your own code, paying attention to apply it on a linear image buffer.

## Conclusion
These are just a few algorithms I have used in one of my recent projects. Sharing them here so others may benefit.

