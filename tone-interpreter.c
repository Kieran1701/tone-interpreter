#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tone.h"

#define MAX_LGTH 512
#define MIN_LEN 2
#define WAVE_PTS 3

char** add_element(char** arr, int len, char* element) {
	/* 
	 * Copy the strings to a new array
	 */

	char** new_arr = malloc(len);
	for(int i=0; i<len-1; i++) {
		strcpy(new_arr[i], arr[i]);
	}
	new_arr[len-1] = element;
	free(arr);
	return new_arr;
}

char** get_commands(char* buffer, int* len, char verbose) {
	/* 
	 * Read a line and return an array of individual commands to parse
	 */

	// separate each command
	char* cmd = strtok(buffer, ";");
	char** commands;
	while(cmd!=NULL) {
		// Remove \n from consideration
		// TODO: Kinda hacky.  Needs better solution
		if(strlen(cmd)<=MIN_LEN) {
			break;
		}
		if(verbose) {
			printf("Command: %s\n", cmd);
		}
		// increment len variable for entire program
		(*len)++;
		commands = add_element(commands, *len, cmd);
		// get next command
		cmd = strtok(NULL, ";");
	}

	return commands;
}

struct tone parse_tone(char* command, _Bool verbose) {
	/* 
	 * Parse a tone given a command
	 * Returns a tone struct
	 */

	// Tone to be returned
	struct tone tone_ret;
	char *cmd_half = strtok(command, "/");
	int cnt = 0;
	while(cmd_half!=NULL) {
		if(cnt<1) {
			tone_ret.duration = strtof(cmd_half, NULL);
		}
		else {
			// replace command with just waveforms
			command = cmd_half;
		}
		cnt++;
		cmd_half = strtok(NULL, "/");
	}
	//////// THIS IS TEMPORARY!!! ////////
	// Only allow for a single waveform
	struct waveform wave;
	cnt = 0;
	char *wave_pt = strtok(command, ",");
	while(wave_pt!=NULL && cnt<WAVE_PTS) {
		if(cnt==0) {
			wave.type = atoi(wave_pt);
		}
		else if(cnt==1) {
			wave.freq = atof(wave_pt);
		}
		else if(cnt==2) {
			wave.volume = atof(wave_pt);
		}
		else {
			break;
		}
		cnt++;
		wave_pt = strtok(NULL, ",");
	}
	struct waveform *wave_arr = malloc(1 * sizeof(struct waveform));
	wave_arr[0] = wave;

	// TODO: Multiple waveforms in a tone
	// TODO: count number of waveforms in the tone
	//int num_waves = 0;
	// parse waveforms from the given commannd
	/*char *wave = strtok(command, ":");
	// strings with waveform info to be parsed and turned into waveforms later
	char **wave_str;
	while(wave!=NULL) {
		// TODO: Same hack as above
		if(strlen(wave)<=MIN_LEN) {
			break;
		}
		if(verbose) {
			printf("Waveform: %s\n", wave);
		}
		num_waves++;
		printf("HERE, %s\n", wave);
		// TODO: Invalid pointer in add_element() for free().  This should be wave_str
		wave_str = add_element(wave_str, num_waves, wave);
		for(int i=0; i<num_waves; i++) {
			printf("Wave string: %s\n", wave_str[i]);
		}
		wave = strtok(NULL, ":");
		// TODO make actual waveform
	}*/
	//struct waveform *waves = malloc(num_waves * sizeof(struct waveform));
	/*ret.duration = 1.0;
	waves[0].freq = 440.0;
	waves[0].volume = 10500.0;
	waves[0].type = 0;*/
	tone_ret.waves = wave_arr;
	return tone_ret;
}

int num_waves(struct waveform *waves) {
	/* 
	 * Return number of waveforms in a list of waves to be mixed
	 */

	return sizeof(&waves) / sizeof(waves[0]);
}

int main(int argc, char *argv[]) {
	// TODO: Add command line option to set verbosity flag
	_Bool verbose = 0x01;

    // PulseAudio variables
    pa_simple *pa_s = NULL;
    pa_sample_spec ss;
    int error;

    // Set sample specifications
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = 1;
    ss.rate = 44100;

    // Open a new connection to the default server
    pa_s = pa_simple_new(NULL, "Play Tone", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error);
    if (!pa_s) {
        fprintf(stderr, "pa_simple_new() failed: %s\n", pa_strerror(error));
        return 1;
    }

	char buffer[MAX_LGTH];  // single line of text
	int tone_cnt = 0;
	struct tone *tones_master;
	while(fgets(buffer, MAX_LGTH, stdin) != NULL) {
		int len = 0;  // length of command array
		char** cmd_arr = get_commands(buffer, &len, verbose);
		struct tone *tones = malloc(len * sizeof(struct tone));
		// for i in cmd_arr
		for(int i=0; i<len; i++) {
			// Parse individual command
			struct tone indiv_tone;
			indiv_tone = parse_tone(cmd_arr[i], verbose);
			tones[i] = indiv_tone;
		}
		// TODO: add tones to tones_master
		tones_master = append_tone(tones_master, tones, len, &tone_cnt);
		// TODO
		
		/* size_t len = strlen(buffer);
		char* cmd = strtok(buffer, ";");  // separate each command
		while(cmd!=NULL) {
			double duration;
			int type;
			double freq;
			double vol;
			int cntr = 0;
			if(verbose) {
				printf("Command: %s\n", cmd);
			}
			char* tone = strtok(cmd, "/");  // separate parts of the command
			while(tone!=NULL) {
				if(verbose) {
					printf("%s, %d;   ", tone, cntr);
				}
				if(cntr==0) {
					duration = strtod(tone);
				}
				else if(cntr==1) {
					char* info = strtok(tone, ",");
					int i = 0;
					while(info!=NULL) {
						if(i==0) {
							type = (int) strtol(info);
						}
						else if(i==1) {
							freq = strtod(info);
						}
						else if(i==2) {
							vol = strtod(info);
						}
						else {
							return 3;
						}
						i++;
					}
				}
				else {
					return 2;  // Error condition
				}
				cntr++;
				tone = strtok(NULL, "/");  // terminate loop
			}
			printf("\n");
			printf("%f duration, %d type, %f freq, %f vol", duration, type, freq, vol);
			cmd = strtok(NULL, ";");
		} */
	}
	if(verbose) {
		for(int i=0; i<tone_cnt; i++) {
			printf("TONE %d:\nDuration: %f\nType: %d\nFreq:%f\nVol: %f\n", i, tones_master[i].duration, tones_master[i].waves[0].type, tones_master[i].waves[0].freq, tones_master[i].waves[0].volume);
		}
	}
	int ret_val = play_tones(tones_master, tone_cnt, pa_s, ss);
	if(pa_s) {
		// close connection
		pa_simple_free(pa_s);
	}

	return 0;
}
