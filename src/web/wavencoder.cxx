/*
 * WebRadio web-based Software Defined Radio
 *
 * Copyright (C) 2013 Mike Stirling
 * Copyright (C) 2013 TT
 *
 * This file is part of WebRadio (http://www.mike-stirling.com/webradio)
 *
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vector>
#include <string.h>

typedef struct {
    char riff[4];
    unsigned long total_size;    // 全体サイズ
    char wave[4];               // "WAVE"
    char fmt[4];               // "fmt "
    unsigned long fmt_size;
    unsigned short format;
    unsigned short channel;
    unsigned long rate;
    unsigned long avgbyte;
    unsigned short block;
    unsigned short bit;
    char data[4];               // = "data"
    unsigned long data_size;      // データサイズ
} WAVEFORMAT;

#include "debug.h"
#include "audioencoder.h"

WavEncoder::WavEncoder(unsigned int samplerate, unsigned int channels) :
    AudioEncoder(samplerate, channels)
{
    LOG_DEBUG("Configuring for %u Hz %u channels\n", samplerate, channels);
    is_first = false;
}

WavEncoder::~WavEncoder()
{
}

const vector<char> WavEncoder::encode(const vector<float> &interleaved)
{
	size_t size = interleaved.size();
	vector<char> buffer(size * 2 + 50);

	if (_channels > 2) {
		LOG_ERROR("Unsupported number of channels %u\n", _channels);
		return vector<char>();
	}

#define DUMMY_SIZE 0x7ffff000
	WAVEFORMAT wav = {
	  {'R','I','F','F'},
	  DUMMY_SIZE + 36,
	  {'W','A','V','E'},
	  {'f','m','t',' '},
	  16,
	  1, // PCM
	  _channels,
	  _samplerate,
	  _samplerate * _channels * 2,
	  _channels * 2,
	  16,
	  {'d','a','t','a'},
	  DUMMY_SIZE
	};

	const float *ptr = interleaved.data();
	short *head = (short*)buffer.data();
	short *dst = (short*)buffer.data();
	if (is_first) {
	  memcpy(dst, &wav, sizeof wav);
	  dst += sizeof wav / sizeof *dst;
	  is_first = false;
	}

	for (unsigned int n = 0; n < size; n++) {
	  *dst++ = (*ptr++) * 32768.0;
	}

	buffer.resize((char*)dst - (char*)head);
	return buffer;
}
