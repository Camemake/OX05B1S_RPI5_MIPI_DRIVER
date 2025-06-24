# OX05B1S Raspberry Pi 5 4‑Lane MIPI CSI‑2 Driver
**Version:** 0.1 **Status:** *Proof‑of‑concept, validated on Raspberry Pi 5 (RP1 silicon)*  

This project brings full‑resolution **2592 × 1944** streaming at **60 frames per second** from the **OX05B1S RGB‑IR** image sensor over **4‑lane MIPI CSI‑2** to the Raspberry Pi 5.  
The driver is implemented as a V4L2 sub‑device (*I²C sensor driver*), packaged for easy out‑of‑tree compilation and distribution.

---

## 1  Features

| Capability | Details |
|------------|---------|
| Resolution | 2592 × 1944 (5 MP) |
| Frame rate | 60 fps @ 4‑lane (~1 Gbps per lane) |
| Bit depth  | RAW12 (Bayer + IR plane) |
| Interface  | MIPI CSI‑2, 4 data‑lanes + 1 clock |
| Controls   | Standby/stream, coarse exposure, analog & digital gain |
| Tested on  | Raspberry Pi 5, 64‑bit Bookworm, kernel 6.6+ |

> **Note:** The OX05B1S outputs an **RGB‑IR quad‑plane pattern** (R/G/B/IR).  
> If you need IR‑only or visible‑only imagery, handle it in userspace (ISP or Python).

---

## 2  Hardware Requirements

* Raspberry Pi 5 (Pi 4 or CM4 do **not** expose four data‑lanes on the main CSI connector).  
* Official 22‑pin → 22‑pin flex cable (do **not** swap pin 1!).  
* OX05B1S camera module with on‑board 24 MHz crystal.  
* 3.3 V and 1.8 V rails supplied by the camera board (or regulator on carrier).  
* GPIO for **RESET** (active low) if your module exposes one.

### 2.1  4‑Lane FFC Pinout (looking at the Pi 5)

| Pin | Signal            | Pin | Signal            |
|:---:|-------------------|:---:|-------------------|
| 1   | GND               | 12  | CAM_D2⁺           |
| 2   | CAM_GPIO0 (RESET) | 13  | CAM_D2⁻           |
| 3   | CAM_SDA           | 14  | GND               |
| 4   | CAM_SCL           | 15  | CAM_D3⁺           |
| 5   | CAM_CLK⁺          | 16  | CAM_D3⁻           |
| 6   | CAM_CLK⁻          | 17  | CAM_IO0 (unused)  |
| 7   | CAM_D0⁺           | 18  | CAM_IO1 (unused)  |
| 8   | CAM_D0⁻           | 19  | 1.8 V             |
| 9   | GND               | 20  | 3.3 V             |
| 10  | CAM_D1⁺           | 21  | GND               |
| 11  | CAM_D1⁻           | 22  | GND               |

---

## 3  Quick Build & Installation

```bash
# 1  Install build dependencies
sudo apt update
sudo apt install raspberrypi-kernel-headers build-essential git

# 2  Grab the driver sources
git clone https://github.com/Camemake/OX05B1S_RPI5_MIPI_DRIVER.git
cd OX05B1S_RPI5_MIPI_DRIVER

# 3  Build the out‑of‑tree kernel module
make -j$(nproc)

# 4  Install the .ko into the running kernel’s module tree
sudo make install      # copies + depmod

# 5  Enable the 4‑lane overlay (loads at boot)
sudo cp overlay/ox05b1s-rpi5-overlay.dtbo /boot/overlays/
echo 'dtoverlay=ox05b1s-rpi5-overlay' | sudo tee -a /boot/config.txt

# 6  Reboot
sudo reboot
```

After the reboot you should see the sub‑device node:

```bash
v4l2-ctl --list-devices
ox05b1s 10-006c (platform: fe801000.csi):
        /dev/video0
```

---

## 4  Capturing Video

The V4L2 node exposes **RAW12 Bayer**.  
Test streaming with DMA‑buffer mmap:

```bash
# 10‑second test stream, no saving
v4l2-ctl -d /dev/video0 --set-fmt-video=width=2592,height=1944,pixelformat=RG12 \
         --stream-mmap --stream-count=600

# Save 100 frames to a single raw file
v4l2-ctl -d /dev/video0 -v width=2592,height=1944,pixelformat=RG12 \
         --stream-to=ox05b1s.raw --stream-count=100
```

### 4.1  Example userspace ISP with libcamera‑apps

```bash
libcamera-raw --camera 0 --width 2592 --height 1944 -o frame_%04d.raw
```

Convert RAW12 to viewable RGB using *rawpy*, *dcraw*, or your own pipeline.

---

## 5  Controls

| V4L2 Control      | Range / Step                      | Sensor Register |
|-------------------|-----------------------------------|-----------------|
| `exposure`        | 1 – 4095 lines (1 line ≈ 16.6 µs) | `0x0202[19:0]`  |
| `analogue_gain`   | 1× – 15.5×                        | `0x0204` + `0x0205` |
| `digital_gain`    | 1× – 8×                           | `0x020E` + `0x020F` |
| `horizontal_flip` | 0 / 1                             | `0x0101` bit 1  |
| `vertical_flip`   | 0 / 1                             | `0x0101` bit 0  |

Set with:

```bash
v4l2-ctl -d /dev/video0 --set-ctrl=exposure=800,analogue_gain=2
```

---

## 6  Device‑Tree Overlay Details

`overlay/ox05b1s-rpi5-overlay.dts` enables:

* **I²C address** `0x6c` on the `i2c_csi_dsi` bus  
* 24 MHz sensor clock from the RP1 camera PLL  
* 4 data‑lanes + clock lane  
* `link-frequencies = /bits/ 64 <1050000000>;` required by `bcm2835-unicam`

Move the sensor to the other port? Change the `target = <&csi>` node and GPIOs, then re‑compile:

```bash
dtc -I dts -O dtb -o ox05b1s-rpi5-overlay.dtbo overlay/ox05b1s-rpi5-overlay.dts
```

Copy the `.dtbo` to `/boot/overlays/` and update `config.txt`.

---

## 7  Advanced Topics

* **HDR (staggered):** OX05B1S supports dual‑exposure HDR. Register sequences welcome!  
* **RGB‑IR demosaicing:** IR channel = fourth pixel of each Bayer quad. `examples/` has a NumPy splitter.  
* **Dynamic Lane Switching:** RP1 supports 1/2/4‑lane per mode; duplicate `ox05b1s_mode` and tweak HTS/VTS/PCLK.  
* **Clock Scaling:** Default PLL 350 MHz → 1.05 Gbps/lane. Drop xvclk to 27 MHz for lower fps.

---

## 8  Troubleshooting

| Symptom | Fix |
|---------|-----|
| **Wrong sensor ID** in *dmesg* | Check I²C pull‑ups or address (`i2cdetect -y 10`). |
| `bcm2835-unicam … fifo overflow` | `link-frequencies` incorrect; ensure 1.05 GHz per lane. |
| All‑black / checkerboard image | RAW12 unpack error; verify byte ordering (little‑endian). |
| Green / purple tint | Wrong Bayer order — sensor default is **RGGB**. |

---

## 9  Contributing

1. Fork → create topic branch → commit (use *Signed‑off‑by*) → pull request.  
2. Follow kernel coding‑style (`scripts/checkpatch.pl`).  
3. Provide logic‑analyzer captures or scope screenshots for timing changes.

