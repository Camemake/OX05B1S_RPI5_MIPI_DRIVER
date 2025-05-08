
/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * OX05B1S 5MP RGB-IR image sensor driver
 * Supports 2592x1944 @60fps, 4-lane MIPI CSI-2
 *
 * Copyright (C) 2025
 * Author: Your Name <you@example.com>
 */
#ifndef __OX05B1S_H__
#define __OX05B1S_H__

#include <linux/clk.h>
#include <linux/gpio/consumer.h>
#include <linux/regmap.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>

#define OX05B1S_XVCLK_FREQ   (24000000) /* 24 MHz external crystal */
#define OX05B1S_CHIP_ID      (0x0531)   /* Expected value in ID register */
#define OX05B1S_REG_CHIP_ID_H    0x300A
#define OX05B1S_REG_CHIP_ID_L    0x300B

struct ox05b1s_regval {
    u16 addr;
    u8  val;
};

struct ox05b1s_mode {
    u32 width;
    u32 height;
    u32 hts;           /* total pixels per line */
    u32 vts;           /* total lines per frame */
    u32 pclk;          /* pixel clock (Hz) */
    u64 link_freq;     /* per‑lane CSI-2 bit rate */
    const struct ox05b1s_regval *reg_list;
    unsigned int       num_regs;
};

extern const struct ox05b1s_mode ox05b1s_2592x1944_60fps_mode;

#endif /* __OX05B1S_H__ */
