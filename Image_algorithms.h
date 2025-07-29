/*
 *  Created on: Jul 29, 2025
 *  Author: BalazsFarkas
 *  Project: Image_processing
 *  Processor: STM32F429ZI
 *  Program version: 1.0
 *  File: Image_algorithms.h
 */

#ifndef INC_IMAGE_ALGORITHMS_H_
#define INC_IMAGE_ALGORITHMS_H_

#include "stdint.h"

//LOCAL CONSTANT

//LOCAL VARIABLE

//EXTERNAL VARIABLE

//FUNCTION PROTOTYPES
uint8_t* Luminance_RGB565_conversion(uint16_t* rgb565_image, uint16_t pixel_no);
uint8_t* Atkinson_dithering(uint8_t* luminance_only, uint16_t pixel_no, uint8_t dither_threshold, uint16_t column_no);
uint16_t* Upscaled_x4_1bpp(uint8_t* atkinson, uint16_t pixel_no, uint16_t column_no);
uint16_t* Rotate_upscaled_x4_1bpp(uint16_t* one_bpp_upscaled_image_buf, uint16_t pixel_no, uint16_t upscaled_column_no, uint16_t upscaled_row_no);


#endif /* INC_IMAGE_ALGORITHMS_H_ */
