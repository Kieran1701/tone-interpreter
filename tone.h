#include <math.h>
#include <stdint.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define PI 3.14159265358979323846

#ifndef TONE_H
#define TONE_H

struct waveform {
	/* 
	 * An individual waveform part of a tone
	 */

	int type;
	double freq;
	double volume;
};

struct tone {
	/* 
	 * Tone based on an inputted command
	 */

	double duration;
	struct waveform *waves;
};

struct tone* append_tone(struct tone *tones, struct tone *tone_arr, int arr_len, int *tones_len) {
	/* 
	 * Append an array of tones to a master tone array (*tones)
	 */
	int tot_len = arr_len + *tones_len;
	struct tone *ret_tones = malloc(tot_len * sizeof(struct tone));
	if(*tones_len>0) {
		for(int i=0; i<*tones_len; i++) {
			ret_tones[i] = tones[i];
		}
	}
	for(int i=0; i<arr_len; i++) {
		ret_tones[i+*tones_len] = tone_arr[i];
	}
	*tones_len = tot_len;  // change total length globally
	return ret_tones;
}

int16_t sine_wave_sample(double f, double t, double vol, double phase) {
	/*
	 * Create a sine wave sample based on given values
	 * s(t) = sin(2pi * f * t) sine wave
	 */

	return (int16_t)(vol * sin((2.0 * PI * f * t) + phase));
}

int16_t sawtooth_wave_sample(double f, double t, double vol) {
	/* 
	 * Create a sawtooth wave sample based on given values
	 * s(t) = 2 * (t % (1 / f) * f - 1 sawtooth
	 */

	return (int16_t)(vol * 2 * (t * f - floor(0.50 + (t * f))));
}

int16_t triangle_wave_sample(double f, double t, double vol) {
	/* 
	 * Create a triangular wave sample based on given values
	 * s(t) = 2 * |tf - floor(tf + 0.5)|
	 */

	return (int16_t)(vol * 2.0 * fabs((t * f) - floor((t * f) + 0.5)));
}

int16_t square_wave_sample(double f, double t, double vol) {
	/* 
	 * Create a square wave sample based on given values
	 * s(t) = sgn(sin(2pi * f * t) square wave
	 */

	int16_t val = sine_wave_sample(f, t, vol, 0.0);
	return (int16_t) vol * copysign(1.0, val);
}

int play_tones(struct tone *tones, int tone_cnt, pa_simple *pa_s, pa_sample_spec ss) {
	/* 
	 * Use PulseAudio to play tones
	 */
	int error;
	for(int i=0; i<tone_cnt; i++) {
		// calculate the number of samples
		size_t num_samples = tones[i].duration * ss.rate;
		// generate and play tone
		for(size_t j=0; j<num_samples; j++) {
			int16_t sample;
			// calculate t for the wave being sampled
			double t = (float) j / (float) ss.rate;
			// TODO: allow for more than one waveform in a tone
			if(tones[i].waves[0].type==0) {
				sample = sine_wave_sample(tones[i].waves[0].freq, t, tones[i].waves[0].volume, 0.0);
			}
			else if (tones[i].waves[0].type==1) {
				sample = sawtooth_wave_sample(tones[i].waves[0].freq, t, tones[i].waves[0].volume);
			}
			else if (tones[i].waves[0].type==2) {
				sample = triangle_wave_sample(tones[i].waves[0].freq, t, tones[i].waves[0].volume);
			}
			else if (tones[i].waves[0].type==3) {
				sample = square_wave_sample(tones[i].waves[0].freq, t, tones[i].waves[0].volume);
			}
			else {
				// default to 0
				sample = (int16_t) 0;
			}

			if(pa_simple_write(pa_s, &sample, sizeof(sample), &error) < 0) {
				fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
				return 1;
			}
		}
	}
	// wait for playback to complete
	if(pa_simple_drain(pa_s, &error) < 0) {
		fprintf(stderr, "pa_simple_drain() failed: %s\n", pa_strerror(error));
		return 2;
	}

	return 0;
}

#endif
