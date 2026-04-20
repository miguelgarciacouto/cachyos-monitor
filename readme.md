# CachyOS Tracker (temp name) 

This is currently a personal work-in-progress project that I'm doing, the goal being to build a modern, high-performance TUI to track the unique kernel-level features that make **CachyOS** so incredible. 

Since standard system monitors are built for general purpose Linux distros, they often gloss over the specialized patches that CachyOS uses under the hood, this tracker aims to put those features in the spotlight. 

This project wouldn't be able to exist if it wasn't by the amazing community that makes Linux and therefore CachyOS possible. 

*Some relevant articles were*
* **[CPU Schedulers](https://wiki.cachyos.org/kernel/schedulers/)** 
* **[System monitoring](https://wiki.archlinux.org/title/System_monitoring)** 
* **[PSI](https://docs.kernel.org/accounting/psi.html)**

If you want to compile this early version yourself: 

```bash 
git clone ____
cd cachyos-tracker

mkdir build && cd build 
cmake -G Ninja .. 

ninja 
./CachyOSTracker
``` 
