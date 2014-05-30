/*
 * Proc_Wave.c
 *
 *  Created on: 2014/05/29
 *      Author: iwabuchik
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

/*********************************
 * includes
 *
 * main.h	lib.h		ppmlib.h
 * wave.h	methods.h
 *
**********************************/

#ifndef MAIN_H_
#include "../include/main.h"
#endif

//#ifndef LIB_H
//#include "../include/lib.h"
//#endif

#ifndef PPMLIB_H
#include "../include/ppmlib.h"
#endif

#ifndef WAVE_H
//#define WAVE_H
#include "../include/wave.h"
#endif

//#ifndef IMG_LIB_H
//#include "../include/img_lib.h"
//#endif

#ifndef METHODS_H
#include "../include/methods.h"
#endif

/////////////////////////////////////

// global vars

/////////////////////////////////////
MONO_PCM pcm0, pcm1;

char *file_src_wave;
char *file_dst_wave;

char *operation;

int save_file = true;

char *op_names[] = {

		"wave-to-ppm",
		NULL
};

char *ppm_file_dst;

int ppm_size[2] = {255, 255};
int max_bright = 255;

char *bg_color;

PPM *ppm;


/////////////////////////////////////

// prototypes

/////////////////////////////////////
void _Setup_Options_ProcWave(int argc, char **argv);
void _Setup_Options_ProcWave__Src(char **argv);
void _Setup_Options_ProcWave__Dst(char **argv);
void _Setup_Options_ProcWave__SaveFile(char **argv, int argc);
void _Setup_Options_ProcWave__Op(char **argv, int argc);


void Proc_Wave__SaveWave(void);
void Proc_Wave__ReadWave(void);

void Proc_Wave__Analyze(void);
void Proc_Wave__Op_Dispatch(void);

// Op: wave to ppm
void _op_Wave2PPM(void);
void build_PPM_Header_Wave(void);
void build_PPM_Pixels_Wave(void);

/////////////////////////////////////

// Proc_Wave()

/////////////////////////////////////
void Proc_Wave(int argc, char **argv)
{
//	MONO_PCM pcm0, pcm1;
//	int n;

	/*********************************
	 * Options
	**********************************/
	_Setup_Options_ProcWave(argc, argv);

	/*********************************
	 * Read: wave
	**********************************/
	Proc_Wave__ReadWave();

	/*********************************
	 * Analyze
	**********************************/
	Proc_Wave__Analyze();


	/*********************************
	 * Operations
	**********************************/
	Proc_Wave__Op_Dispatch();

	/*********************************
	 * Save: wav file
	**********************************/
	if (save_file == true) {

		Proc_Wave__SaveWave();

	}

	/*********************************
	 * Free: pcms
	**********************************/
	free(pcm0.s); /* メモリの解放 */
	free(pcm1.s); /* メモリの解放 */
//

	//log
	printf("[%s : %d] Proc_Wave => done\n",
				base_name(__FILE__), __LINE__);


}//void Proc_Wave(int argc, char **argv)

void _Setup_Options_ProcWave(int argc, char **argv)
{
	/*********************************
	 * Opt: size
	**********************************/
	_Setup_Options_ProcWave__Src(argv);

	/*********************************
	 * Opt: dst
	**********************************/
	_Setup_Options_ProcWave__Dst(argv);

	/*********************************
	 * Opt: save file
	**********************************/
	_Setup_Options_ProcWave__SaveFile(argv, argc);

	/*********************************
	 * Opt: operation
	**********************************/
	_Setup_Options_ProcWave__Op(argv, argc);

//	/*********************************
//	 * Opt: bright
//	**********************************/
//	_Setup_Options_ProcWave__Bright(argv);
//
//	/*********************************
//	 * Opt: background
//	**********************************/
//	_Setup_Options_ProcWave__Bg(argv);
//

}

void _Setup_Options_ProcWave__Src(char **argv) {

	char *opt_key_Src = "-src";

	file_src_wave = (char *) get_Opt_Value(argv, opt_key_Src);

	/*********************************
	 * validate: exists?
	**********************************/
	int res_i = fileExists(file_src_wave);

	if (res_i == false) {

		consolColor_Change(RED);
		//log
		printf("[%s : %d] Src file => deosn't exist(%s)\n",
				(char *) base_name(__FILE__), __LINE__, file_src_wave);

		consolColor_Reset();

		exit(-1);

	}

	consolColor_Change(GREEN);
	//log
	printf("[%s : %d] file_src_wave => %s\n",
			(char *) base_name(__FILE__), __LINE__, file_src_wave);

	consolColor_Reset();

}//void _Setup_Options_ProcWave__Src(char **argv)

void _Setup_Options_ProcWave__Dst(char **argv) {

	char *opt_key_Dst = "-dst";

	file_dst_wave = (char *) get_Opt_Value(argv, opt_key_Dst);

//	char delim = '.';
//
//	int position = 1; int num_of_tokens;
//
//	char **tokens = (char **) str_split_r_2
//			(file_dst_wave, delim, position, &num_of_tokens);

	//log
	printf("[%s : %d] dir name => %s\n",
			base_name(__FILE__), __LINE__, dir_name(file_dst_wave));

	/*********************************
	 * validate: folder exists?
	**********************************/
	char *dname = dir_name(file_dst_wave);

	if (dname != NULL) {

		int res_i = dirExists(dname);

		if (res_i != 1) {

			consolColor_Change(RED);

			//log
			printf("[%s : %d] Dst dir => doesn't exist: %s\n",
						base_name(__FILE__), __LINE__, dname);

			consolColor_Reset();

			exit(-1);

		}

	}

	/*********************************
	 * Add: time string
	**********************************/
	char delim = '.';

	int position = 1; int num_of_tokens;

	char **tokens = (char **) str_split_r_2
			(file_dst_wave, delim, position, &num_of_tokens);

	int tmp_i = 4;

	char **tmp = (char **) malloc(sizeof(char *) * tmp_i);

	tmp[0] = tokens[0];
	tmp[1] = "_";
	tmp[2] = get_TimeLabel_Now(SERIAL);
	tmp[3] = ".wav";
//	tmp[0] = "./audio/";
//	tmp[1] = "b_";
//	tmp[2] = get_TimeLabel_Now(SERIAL);
//	tmp[3] = ".wav";

	char *file_dst = join_simple(tmp, tmp_i);

	free(file_dst_wave);

	file_dst_wave = file_dst;


	consolColor_Change(GREEN);
	//log
	printf("[%s : %d] file_dst_wave => %s\n",
			(char *) base_name(__FILE__), __LINE__, file_dst_wave);

	consolColor_Reset();

}//void _Setup_Options_ProcWave__Dst(char **argv)

void Proc_Wave__SaveWave()
{
//	char delim = '.';
//
//	int position = 1; int num_of_tokens;
//
//	char **tokens = (char **) str_split_r_2
//			(file_dst_wave, delim, position, &num_of_tokens);
//
//	int tmp_i = 4;
//
//	char **tmp = (char **) malloc(sizeof(char *) * tmp_i);
//
//	tmp[0] = tokens[0];
//	tmp[1] = "_";
//	tmp[2] = get_TimeLabel_Now(SERIAL);
//	tmp[3] = ".wav";
////	tmp[0] = "./audio/";
////	tmp[1] = "b_";
////	tmp[2] = get_TimeLabel_Now(SERIAL);
////	tmp[3] = ".wav";
//
//	char *file_dst = join_simple(tmp, tmp_i);
//
//	free(file_dst_wave);
//
//	file_dst_wave = file_dst;

	mono_wave_write(&pcm1, file_dst_wave); /* WAVEファイルにモノラルの音データを出力する */
//	mono_wave_write(&pcm1, file_dst_wave); /* WAVEファイルにモノラルの音データを出力する */
//	mono_wave_write(&pcm1, file_dst); /* WAVEファイルにモノラルの音データを出力する */
//	mono_wave_write(&pcm1, "b.wav"); /* WAVEファイルにモノラルの音データを出力する */

	//log
	printf("[%s : %d] wave => written: %s\n",
			base_name(__FILE__), __LINE__, file_dst_wave);
//	printf("[%s : %d] wave => written: %s\n", base_name(__FILE__), __LINE__, file_dst);

}

void Proc_Wave__ReadWave()
{
	int n;

	mono_wave_read(&pcm0, file_src_wave); /* WAVEファイルからモノラルの音データを入力する */
//	mono_wave_read(&pcm0, "./audio/a.wav"); /* WAVEファイルからモノラルの音データを入力する */
	//  mono_wave_read(&pcm0, ".\\audio\\a.wav"); /* WAVEファイルからモノラルの音データを入力する */
	//  mono_wave_read(&pcm0, "..\\audio\\a.wav"); /* WAVEファイルからモノラルの音データを入力する */
	//  mono_wave_read(&pcm0, "a.wav"); /* WAVEファイルからモノラルの音データを入力する */

	pcm1.fs = pcm0.fs; /* 標本化周波数 */
	pcm1.bits = pcm0.bits; /* 量子化精度 */
	pcm1.length = pcm0.length; /* 音データの長さ */
	pcm1.s = calloc(pcm1.length, sizeof(double)); /* メモリの確保 */

	for (n = 0; n < pcm1.length; n++)
	{
		pcm1.s[n] = pcm0.s[n]; /* 音データのコピー */
	}

}

void Proc_Wave__Analyze(void)
{
	double tmp_d;

	int len = pcm0.length;

	//log
	printf("[%s : %d] pcm0.length = %d\n", base_name(__FILE__), __LINE__, pcm0.length);

	//log
	printf("[%s : %d] pcm0.fs = %d\n", base_name(__FILE__), __LINE__, pcm0.fs);

	//log
	printf("[%s : %d] pcm0.bits = %d\n", base_name(__FILE__), __LINE__, pcm0.bits);

	int i;
//	int start = 1000000;
	int start = 0;

	for (i = start; i < start + 10; ++i) {
//	for (i = 0; i < 10; ++i) {

		//log
		printf("[%s : %d] pcm0.s[%d] = %f\n", base_name(__FILE__), __LINE__, i, pcm0.s[i]);

	}

	/*********************************
	 * Get: max and minimum wave value
	**********************************/
	double max_val, min_val, tmp_dbl;

	max_val = pcm0.s[0];

	for (i = 1; i < pcm0.length; ++i) {

		tmp_dbl = pcm0.s[i];

		if(max_val < tmp_dbl) {

			max_val = tmp_dbl;

		}

	}

	/*********************************
	 * min_val
	**********************************/
	min_val = pcm0.s[0];

	for (i = 1; i < pcm0.length; ++i) {

		tmp_dbl = pcm0.s[i];

		if(min_val > tmp_dbl) {

			min_val = tmp_dbl;

		}

	}


	//log
	printf("[%s : %d] max_val = %f\n", base_name(__FILE__), __LINE__, max_val);
	printf("[%s : %d] min_val = %f\n", base_name(__FILE__), __LINE__, min_val);



//	while(pcm0.s)


}

void _Setup_Options_ProcWave__SaveFile(char **argv, int argc)
{
	char *opt_key_Save = "-save";

	save_file = option_Exists(argv, argc, opt_key_Save);
//	file_dst_wave = (char *) get_Opt_Value(argv, opt_key_Dst);

}

void _Setup_Options_ProcWave__Op(char **argv, int argc)
{
	char *opt_key_Op = "-op";

	operation = (char *) get_Opt_Value(argv, opt_key_Op);

	/*********************************
	 * Validate
	**********************************/
	int tmp_i = 0;
	while(*(op_names + tmp_i)) tmp_i ++;

	int res_i = is_InArray(operation, op_names, tmp_i);

	if (res_i == false) {

		consolColor_Change(RED);

		//log
		printf("[%s : %d] op name is '%s' => not in the list\n",
					base_name(__FILE__), __LINE__);

		consolColor_Reset();

		char *tmp_str = join(',', op_names, tmp_i);

		//log
		printf("[%s : %d] Available op names => %s\n",
				base_name(__FILE__), __LINE__, tmp_str);

		exit(-1);

	}

	/*********************************
	 * Report
	**********************************/
	consolColor_Change(GREEN);

	//log
	printf("[%s : %d] operation => %s\n",
				base_name(__FILE__), __LINE__, operation);

	consolColor_Reset();


}

void Proc_Wave__Op_Dispatch(void)
{
	if (!strcmp(operation, op_names[0])) {	// wave-to-ppm

		_op_Wave2PPM();

	} else {

	}

}//void Proc_Wave__Op_Dispatch(void)

void _op_Wave2PPM(void)
{

	/*********************************
	 * Build: PPM header
	**********************************/
	build_PPM_Header_Wave();

	//log
	printf("[%s : %d] PPM header => created\n", base_name(__FILE__), __LINE__);

	/*********************************
	 * Build: PPM pixels
	**********************************/
	build_PPM_Pixels_Wave();

	//log
	printf("[%s : %d] PPM pixels => created\n", base_name(__FILE__), __LINE__);

	/*********************************
	 * dst file path
	**********************************/
	char delim = '.';

	int position = 1; int num_of_tokens;

	char **tokens = (char **) str_split_r_2
			(file_dst_wave, delim, position, &num_of_tokens);

	int tmp_i = 2;

	char **tmp = (char **) malloc(sizeof(char *) * tmp_i);

	tmp[0] = tokens[0];
	tmp[1] = ".ppm";

	char *file_dst_ppm = join_simple(tmp, tmp_i);

	/*********************************
	 * Save: ppm file
	**********************************/
	int res_i = save_PPM(file_dst_ppm, ppm);

	if (res_i == 1) {

		consolColor_Change(GREEN);

		//log
		printf("[%s : %d] PPM => saved: %s\n",
					base_name(__FILE__), __LINE__, file_dst_ppm);

		consolColor_Reset();


	} else {

		consolColor_Change(RED);

		//log
		printf("[%s : %d] PPM => not saved: %s\n",
					base_name(__FILE__), __LINE__, file_dst_ppm);

		consolColor_Reset();


	}

	/*********************************
	 * Free: ppm
	**********************************/
	free(ppm);

	/*********************************
	 * Record: execution infos
	**********************************/
	char *tmp_str = "<ppm file saved>";

	int len = strlen(file_dst_ppm) + strlen(tmp_str);

	char *message = (char *) malloc(sizeof(char) * (len + 2));

	sprintf(message, "%s %s", tmp_str, file_dst_ppm);

	message[len + 1] = '\0';

	write_Log(__FILE__, __LINE__, message);


}//void _op_Wave2PPM(void)

void build_PPM_Header_Wave()
{
	ppm = (PPM *) malloc(sizeof(PPM) * 1);

	strcpy(ppm->format, "P6");

	ppm->format[2] = '\0';

	ppm->x = ppm_size[0];
	ppm->y = ppm_size[1];

	ppm->max_brightness = max_bright;

}

void build_PPM_Pixels_Wave()
{
    int position = 0;

    int i, j;

    int num_of_pixels = ppm->x * ppm->y;

    ppm->pixels = (pixel *) malloc (sizeof(pixel) * num_of_pixels);

    int rgb[3] = {PIXEL_BLUE};

    for (i = 0; i < ppm->y; ++i) {

		for (j = 0; j < ppm->x; ++j) {

			set_PixelVals(ppm, position, rgb);	// set_PixelVals() : ppmlib.c

			position ++;

		}
    }


}