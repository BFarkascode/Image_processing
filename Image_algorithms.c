/*
 *  Created on: Jul 29, 2025
 *  Author: BalazsFarkas
 *  Project: Image_processing
 *  Processor: STM32F429ZI
 *  Program version: 1.0
 *  File: Image_algorithms.c
 */

#include "Image_algorithms.h"


uint8_t* Luminance_RGB565_conversion(uint16_t* rgb565_image, uint16_t pixel_no){

	/*
	 * Function converts existing 16-bit RGB565 image to uint8_t luminance only image
	 * Output is a pointer to the luminance array
	 *
	 */

	uint8_t luminance_only[pixel_no];

	  for(int i = 0; i < pixel_no; i++){

		  uint8_t r = (rgb565_image[i] >> 11) & 0x1F; // 5 bits
		  uint8_t g = (rgb565_image[i] >> 5) & 0x3F;  // 6 bits
		  uint8_t b = rgb565_image[i] & 0x1F;         // 5 bits

		  // Normalise to 8-bit values
		  r = (r << 3) | (r >> 2);
		  g = (g << 2) | (g >> 4);
		  b = (b << 3) | (b >> 2);

		  // Calculate luminance (Y)
		  uint8_t luminance = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);

		  //generate luminacne only image
		  luminance_only[i] = luminance;

	  }

	  return luminance_only;

}


uint8_t* Floyd_steinberg_dithering(uint8_t* luminance_only, uint16_t pixel_no, uint8_t dither_threshold, uint16_t column_no){

	/*
	 * Function converts a luminance array image into a dithered image
	 * Output is a pointer to the dithered image
	 *
	 */

	  uint8_t floyd_steinberg[pixel_no];

	  //we copy the matrix
	  memcpy(luminance_only, floyd_steinberg, pixel_no);

	  //Floyd_Steinberg dithering on the luminance values
	  for(int i = 0; i < pixel_no; i++){

	  	  uint8_t old_pixel = luminance_only[i];
	  	  uint8_t new_pixel = old_pixel > dither_threshold ? 255 : 0; // Thresholding
	  	  luminance_only[i] = new_pixel;
	  	  int error = old_pixel - new_pixel;

	  	  // Distribute the error to neighbouring pixels
	  	  if(i%column_no == (column_no - 1)) {															//if we are at the end of the line

	  		  //do nothing

	  	  } else {

	  		floyd_steinberg[i + 1] += error * 7 / 16;													//next pixel in row

	  		if(i < (pixel_no - column_no)){																//as long as we are not in the last line

	  			floyd_steinberg[i + (column_no - 1)] += error * 3 / 16;									//next line, pixel one before
	  			floyd_steinberg[i + column_no] += error * 5 / 16;
	  			floyd_steinberg[i + (column_no + 1)] += error * 1 / 16;

	  		 }

	  	  }
	  }

	  return floyd_steinberg;

}

uint8_t* Atkinson_dithering(uint8_t* luminance_only, uint16_t pixel_no, uint8_t dither_threshold, uint16_t column_no){

	/*
	 * Function converts a luminance array image into a dithered image
	 * Output is a pointer to the dithered image
	 *
	 */

	  uint8_t atkinson[pixel_no];

	  //we copy the matrix
	  memcpy(luminance_only, atkinson, pixel_no);


	  //Atkinson dithering on the luminance values
	  for(int i = 0; i < pixel_no; i++){
		  uint8_t old_pixel = atkinson[i];
		  uint8_t new_pixel = old_pixel > dither_threshold ? 255 : 0; 								// Thresholding on the luminance values
		  atkinson[i] = new_pixel;
		  int error = old_pixel - new_pixel;

		  // Distribute the error to neighbouring pixels
		  if(i < (pixel_no - (column_no * 2))){																			//as long as we are not in the second to last line

			  if(i%column_no == (column_no - 1)) {													//if we are at the end of the line

				  atkinson[i + (column_no - 1)] += error * 1 / 8;									//next line, pixel one before
				  atkinson[i + column_no] += error * 1 / 8;											//next line, same pixel
				  atkinson[i + (column_no * 2)] += error * 1 / 8;									//second line, same pixel

			  } else if(i%column_no == (column_no - 2)) {											//if we are one before the end of the line

				  atkinson[i + 1] += error * 1 / 8;													//next pixel in row
				  atkinson[i + (column_no - 1)] += error * 1 / 8;									//next line, pixel one before
				  atkinson[i + column_no] += error * 1 / 8;											//next line, same pixel
				  atkinson[i + (column_no + 1)] += error * 1 / 8;									//next line, one pixel after
				  atkinson[i + (column_no * 2)] += error * 1 / 8;									//second line, same pixel

			  } else {

				  atkinson[i + 1] += error * 1 / 8;													//next pixel in row
				  atkinson[i + 2] += error * 1 / 8;													//second next pixel in row
				  atkinson[i + (column_no - 1)] += error * 1 / 8;									//next line, pixel one before
				  atkinson[i + column_no] += error * 1 / 8;											//next line, same pixel
				  atkinson[i + (column_no + 1)] += error * 1 / 8;									//next line, one pixel after
				  atkinson[i + (column_no * 2)] += error * 1 / 8;									//second line, same pixel

			  }

		  } else if ((i > (pixel_no - (column_no * 2))) & (i < (pixel_no - (column_no)))){			//if we are in the second to last line

			  if(i%column_no == (column_no - 1)) {													//if we are at the end of the line

				  atkinson[i + (column_no - 1)] += error * 1 / 8;									//next line, pixel one before
				  atkinson[i + column_no] += error * 1 / 8;											//next line, same pixel

			  } else if(i%column_no == (column_no - 2)) {											//if we are one before the end of the line

				  atkinson[i + 1] += error * 1 / 8;													//next pixel in row
				  atkinson[i + (column_no - 1)] += error * 1 / 8;									//next line, pixel one before
				  atkinson[i + column_no] += error * 1 / 8;											//next line, same pixel
				  atkinson[i + (column_no + 1)] += error * 1 / 8;									//next line, one pixel after

			  } else {

				  atkinson[i + 1] += error * 1 / 8;													//next pixel in row
				  atkinson[i + 2] += error * 1 / 8;													//second next pixel in row
				  atkinson[i + (column_no - 1)] += error * 1 / 8;									//next line, pixel one before
				  atkinson[i + column_no] += error * 1 / 8;											//next line, same pixel
				  atkinson[i + (column_no + 1)] += error * 1 / 8;									//next line, one pixel after

			  }

		  } else {																					//if we are in the last line


			  if(i%column_no == (column_no - 1)) {													//if we are at the end of the line

				  //do nothing

			  } else if(i%column_no == (column_no - 2)) {											//if we are one before the end of the line

				  atkinson[i + 1] += error * 1 / 8;													//next pixel in row

			  } else {

				  atkinson[i + 1] += error * 1 / 8;													//next pixel in row
				  atkinson[i + 2] += error * 1 / 8;													//second next pixel in row

			  }

		  }

	  }

	  return atkinson;

}


uint16_t* Upscaled_x4_1bpp(uint8_t* atkinson, uint16_t pixel_no, uint16_t column_no){

	/*
	 * We generate an upscaled 1bpp version of the original dithered image
	 * Here below we are scaling up by 4 by "padding" each pixel with 3 additional 0x0 values
	 * The padding values are then filled using the original pixel value
	 * This is nearest neighbour interpolation
	 */

	  uint16_t upscaled_x4_1bpp_pixel_no = pixel_no / 4;												//pixel number will be 1/4 of the original pixel_no since we have a buffer of uint16_t and an upscaling of 4

	  uint16_t one_bpp_upscaled_image_buf[upscaled_x4_1bpp_pixel_no];												//sufficiently big buffer for 1bpp upscaled image

	  uint16_t* one_bpp_upscaled_image_ptr = &one_bpp_upscaled_image_buf[0];										//1bpp image buffer pointer

	  //we are generating a 1bpp bmp image here
	  for(int i = 0; i < pixel_no; i+=8){																			//we jump 8 pixels at a time

		  *one_bpp_upscaled_image_ptr = 0x0;																		//non-defined pixel values will always be 0x0 initially
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	    								//Nearest neighbour interpolation
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  								//we add the original value to the padding
		  uint16_t one_upscaled_bpp_buf = 0x0;

		  for(int j = 0; j < 8; j++){

			  if(atkinson[i + j] == 0xFF) {																			//if the dithered pixel value is 0xFF

				  one_upscaled_bpp_buf = *one_bpp_upscaled_image_ptr;												//we read out the existing byte from 1bpp image buffer (which will be 8 pixels!)
				  *one_bpp_upscaled_image_ptr = (one_upscaled_bpp_buf | (3<<((15-(2*j)-1))));						//and update the appropriate bit/pixel values to HIGH
				  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  								//endian must be matched
				  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  								//we use uint16_t for the upscaled buffer and we load every second value only
				  one_upscaled_bpp_buf = 0x0;

			  } else {

				  one_upscaled_bpp_buf = *one_bpp_upscaled_image_ptr;
				  *one_bpp_upscaled_image_ptr = (one_upscaled_bpp_buf & ~(3<<((15-(2*j)-1))));						//that bit/pixel and the padding are LOW
				  one_upscaled_bpp_buf = 0x0;

			  }

		  }

		  one_upscaled_bpp_buf = *one_bpp_upscaled_image_ptr;
		  *one_bpp_upscaled_image_ptr = (0x00FF & (one_upscaled_bpp_buf >> 8)) | (0xFF00 & (one_upscaled_bpp_buf << 8));	//we flip the two 8 pixels' position

		  if((i+8)%column_no == 0){																					//if on the next loop "i" will be a multiple of 320 (meaning we are at the start of a new 320 pixel row)

			  for(uint8_t k = 0; k<(column_no / 8); k++){															//we wipe the next entire row to 0x0

				  one_bpp_upscaled_image_ptr++;
				  *one_bpp_upscaled_image_ptr = 0x0;

			  }

		  } else {

			  //do nothing

		  }

		  one_bpp_upscaled_image_ptr++;																//we step the 1bpp image buffer pointer to the next 8 pixels

	  }


	  //Nearest neighbour interpolation between rows: we duplicate each row
	  for(int i = 0; i < upscaled_x4_1bpp_pixel_no; i+=((column_no / 8)*2)){

		  for(int j = 0; j < (column_no / 8); j++){

			  one_bpp_upscaled_image_buf[i+(column_no / 8)+j] = one_bpp_upscaled_image_buf[i+j];

		  }

	  }

	  return one_bpp_upscaled_image_buf;

}


uint16_t* Rotate_upscaled_x4_1bpp(uint16_t* one_bpp_upscaled_image_buf, uint16_t pixel_no, uint16_t upscaled_column_no, uint16_t upscaled_row_no){

	/*
	 * We rotate our 1bpp image by 90 degrees
	 * the image will be stored as a line array of 40xuint16_t for 640 1bpp pixels - so 640 + 640 + 640 +... pixels
	 * We do this in a separate function so the transition buffer can be removed after it is not necessary
	 *
	 */

	  //generate rotated 1bpp image linear buffer

	  uint16_t upscaled_x4_1bpp_pixel_no = pixel_no / 4;								//same as before

	  uint16_t rotated_one_bpp_upscaled_image_buf[ upscaled_x4_1bpp_pixel_no];

	  for(int i = 0; i <  upscaled_x4_1bpp_pixel_no; i++){

		  rotated_one_bpp_upscaled_image_buf[i] = 0x0;

	  }

	  //------------------------------------------------

	  //rotate image
	  //Note: all necessary image transformations have been found by trial and error
	  for(int i = 0; i < upscaled_row_no; i++){																	//we have 480 rows in the original, 480 columns in the rotated

		  for(int j = 0; j < (upscaled_column_no / 16); j++){													//16*40 = 640 - we read in a row, one uin16_t a time. We publish it as a column to the rotated array

			  uint16_t pixel_buffer = one_bpp_upscaled_image_buf[i*(upscaled_column_no / 16)+j];										//we read in 16 pixels as uint16_t 1bpp from the original 640+640 array

			  for(int k = 15; k >= 0; k--){																		//we check each bit in the pixel buffer, starting from MSB

				  if((pixel_buffer & (1<<k)) == (1<<k)){														//if the "k"-th bit is HIGH in the pixel buffer

					  if((i%16) < 8){																			//endian swap while rotating

						  rotated_one_bpp_upscaled_image_buf[(((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k)] = (rotated_one_bpp_upscaled_image_buf[((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k] | (1<<(i%16 + 8)));		//we take the rotated image buffer and flip the designated bit to HIGH
					  	  	  	  	  	  	  	  	  															//on each uint16_t readout, we skip 16 rows in the rotated array
						  	  	  	  	  	  	  	  															//each row now 30 uint16_t since we have upscaled_row_no = 480 rows and upscaled_column_no = 640 columns
					  	  	  	  	  	  	  	  	  															//on each bit, we update the appropriate bit in the rotated array by checking, which (rotated) column we are filling up
					  	  	  	  	  	  	  	  	  															//we will have 640 pixels in the column - 640 of uint16_t
					  	  	  	  	  	  	  	  	  															//we will have 480 pixels in a row - 30 of uint16_t
					  	  	  	  	  	  	  	  	  	  															//first section defines, which rows we start at in the new rotated array
					  	  	  	  	  	  	  	  	  	  															//second section adds the column
					  	  	  	  	  	  	  	  	  	  															//third section steps "upwards" in the rows

					  } else {

						  rotated_one_bpp_upscaled_image_buf[(((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k)] = (rotated_one_bpp_upscaled_image_buf[((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k] | (1<<(i%16 - 8)));

					  }

				  } else {																						//if the bit is LOW

					  if((i%16) < 8){																			//endian swap while rotating

						  rotated_one_bpp_upscaled_image_buf[(((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k)] = (rotated_one_bpp_upscaled_image_buf[((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k] & ~(1<<(i%16 + 8)));

					  } else {

						  rotated_one_bpp_upscaled_image_buf[(((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k)] = (rotated_one_bpp_upscaled_image_buf[((upscaled_row_no / 16)*16*j) + ((upscaled_row_no / 16) - 1) - (i / 16) + (upscaled_row_no / 16)*k] & ~(1<<(i%16 - 8)));

					  }

				  }

			  }

		  }

	  }

	  //------------------------------------------------

	  //we do an MSB-LSB swap on 8 rows
	  for(int i = 0; i < upscaled_column_no; i+=8){

		  for(int j = 0; j < (upscaled_row_no / 16); j++){																//e.g. 16*30 = 480

			  uint16_t row_swap_bpp_16pixel_buf_1 = 0x0;
			  uint16_t row_swap_bpp_16pixel_buf_2 = 0x0;

			  row_swap_bpp_16pixel_buf_1 = rotated_one_bpp_upscaled_image_buf[i*(upscaled_row_no / 16) + j];
			  row_swap_bpp_16pixel_buf_2 = rotated_one_bpp_upscaled_image_buf[(i + 7)*(upscaled_row_no / 16) + j];
			  rotated_one_bpp_upscaled_image_buf[i*(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_2;
			  rotated_one_bpp_upscaled_image_buf[(i + 7)*(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_1;
			  row_swap_bpp_16pixel_buf_1 = rotated_one_bpp_upscaled_image_buf[(i + 1)*(upscaled_row_no / 16) + j];
			  row_swap_bpp_16pixel_buf_2 = rotated_one_bpp_upscaled_image_buf[(i + 6)*(upscaled_row_no / 16) + j];
			  rotated_one_bpp_upscaled_image_buf[(i + 1) *(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_2;
			  rotated_one_bpp_upscaled_image_buf[(i + 6)*(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_1;
			  row_swap_bpp_16pixel_buf_1 = rotated_one_bpp_upscaled_image_buf[(i + 2)*(upscaled_row_no / 16) + j];
			  row_swap_bpp_16pixel_buf_2 = rotated_one_bpp_upscaled_image_buf[(i + 5)*(upscaled_row_no / 16) + j];
			  rotated_one_bpp_upscaled_image_buf[(i + 2) *(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_2;
			  rotated_one_bpp_upscaled_image_buf[(i + 5)*(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_1;
			  row_swap_bpp_16pixel_buf_1 = rotated_one_bpp_upscaled_image_buf[(i + 3)*(upscaled_row_no / 16) + j];
			  row_swap_bpp_16pixel_buf_2 = rotated_one_bpp_upscaled_image_buf[(i + 4)*(upscaled_row_no / 16) + j];
			  rotated_one_bpp_upscaled_image_buf[(i + 3) *(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_2;
			  rotated_one_bpp_upscaled_image_buf[(i + 4)*(upscaled_row_no / 16) + j] = row_swap_bpp_16pixel_buf_1;

		  }

	  }

	  return rotated_one_bpp_upscaled_image_buf;

}
