/*
 * Copyright (c) 2006 Ryan Martell. (rdm4@martellventures.com)
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

int nms_base64_decode(uint8_t * out, const char *in, int out_length);
int nms_hex_decode(uint8_t * out, const char *in, int out_length);

inline uint64_t nms_consume(void ** buff, uint8_t n_bytes);
inline uint32_t nms_consume_4(void ** buff);
inline uint32_t nms_consume_3(void ** buff);
inline uint16_t nms_consume_2(void ** buff);
#define nms_consume_1(buff) *((char*)(*(buff))++)

