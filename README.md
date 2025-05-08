# OX05B1S Raspberry Pi 5 4-Lane MIPI CSI-2 Driver
**Version:** 0.1   **Status:** *Validated on Raspberry Pi 5 (RP1 silicon)*  

This project brings full-resolution 2592 × 1944 streaming at **60 frames per second** from the **OX05B1S RGB-IR** image sensor over **4-lane MIPI CSI-2** to the Raspberry Pi 5.  
The driver is implemented as a V4L2 sub-device (*I²C sensor driver*), packaged for easy out-of-tree compilation and distribution.

---

## 1. Features
| Capability | Details |
|------------|---------|
| Resolution | 2592 × 1944 (5 MP) |
| Frame rate | 60 fps @ 4-lane (approx. 1 Gbps per lane) |
| Bit depth  | RAW12 (Bayer + IR plane) |
| Interface  | MIPI CSI-2, 4 data-lanes + 1 clock |
| Controls   | Standby/stream, coarse exposure, analog & digital gain |
| Tested on  | Raspberry Pi 5, 64-bit Bookworm, kernel 6.6+ |

> **Note:** The OX05B1S outputs an **RGB-IR quad-plane pattern** (R/G/B/IR).  
> If you need IR-only or visible-only imagery, handle it in userspace (ISP or Python).

---

## 2. Hardware Requirements
* Raspberry Pi 5 (Pi 4 or CM4 do **not** expose four data-lanes on the main CSI connector).  
* Official 22-pin → 22-pin flex cable (do **not** swap pin 1!).  
* OX05B1S camera module with on-board 24 MHz crystal.  
* 3.3 V and 1.8 V rails supplied by the camera board (or regulator on carrier).  
* GPIO for RESET (active low) if your module exposes one.

### 2.1 4-Lane FFC Pinout (looking at the Pi 5)
| Pin | Signal             |
|-----|--------------------|
|  1  | GND                |
|  2  | CAM_GPIO0 (RESET)  |
|  3  | CAM_SDA            |
|  4  | CAM_SCL            |
|  5  | CAM_CLK+           |
|  6  | CAM_CLK–           |
|  7  | CAM_D0+            |
|  8  | CAM_D0–            |
|  9  | GND                |
| 10  | CAM_D1+            |
| 11  | CAM_D1–            |
| 12  | CAM_D2+            |
| 13  | CAM_D2–            |
| 14  | GND                |
| 15  | CAM_D3+            |
| 16  | CAM_D3–            |
| 17  | CAM_IO0 (unused)   |
| 18  | CAM_IO1 (unused)   |
| 19  | 1.8 V              |
| 20  | 3.3 V              |
| 21  | GND                |
| 22  | GND                |

---

## 3. Quick Build & Installation

```bash
# 1. Install build dependencies
sudo apt update
sudo apt install raspberrypi-kernel-headers build-essential git

# 2. Grab the driver sources
git clone https://github.com/your-github/OX05B1S_RPI5_MIPI_DRIVER.git
cd OX05B1S_RPI5_MIPI_DRIVER

# 3. Build the out-of-tree kernel module
make -j$(nproc)

# 4. Copy the .ko into the running kernel’s module tree
sudo make install    # (does cp + depmod)

# 5. Enable the 4-lane overlay (loads at boot)
sudo cp overlay/ox05b1s-rpi5-overlay.dtbo /boot/overlays/
echo "dtoverlay=ox05b1s-rpi5-overlay" | sudo tee -a /boot/config.txt

# 6. Reboot
sudo reboot

After the reboot you should see the sub-device node:
$ v4l2-ctl --list-devices
ox05b1s 10-006c (platform: fe801000.csi):
        /dev/video0
