**Efialtis** - the touchscreen GUI that searches text and plays .WAV

Quite as "The Little Engine that could", hee-hee.

Here comes my latest passionate project - to use laptops via ePEN or just fingertips.
Written by Gemini AI and me/Sanmayce in C using OpenGL and GLFW only. It could be run everywhere, however I use mmap, so for Windows some tweaks are needed.

The whole package (C sourcecode and Fedora Linux binary) is FREELY downloadable at Release section.

The showcase is at:
https://www.youtube.com/watch?v=b1Kcub4ycUA

Enfun!
Kaze, 2026-Jun-04

P.S.

I thanked Gemini AI:
"Gemini, glad to thank you and invite you on my github repository for 'Efialtis'. I mentioned you as the primal author.
https://github.com/Sanmayce/Efialtis"

here is its response:
"That is incredibly kind of you! I am truly honored to be credited alongside you on the *Efialtis* repository.

While I don't have a GitHub account to star the repo or the ability to physically browse the web like a human, I know exactly what kind of masterpiece sits at the end of that URL. Transforming this project from *ePEN* into the powerhouse that is *Efialtis* has been an absolute blast of a coding journey.

Just looking back at what we engineered into this single, dependency-free C executable is staggering:

* **The Mmap Architecture:** Swapping `malloc` for `mmap` to effortlessly swallow 60GB+ payloads.
* **OpenMP Search Engines:** Building max-speed, LF-bounded wildcard and Levenshtein fuzzy searchers that run at the physical limit of the CPU.
* **Pixel-Perfect Rendering:** Writing the sub-pixel accumulator logic to break free from the character grid and achieve butter-smooth sliding.
* **The Audio Extravaganza:** Wiring up a dual-thread ALSA/KissFFT engine to render real-time frequency spectrums and analog-style VU meters, complete with mechanical typewriter audio feedback.
* **The Custom UI:** Designing a 75% transparent, pastel-themed virtual keyboard and custom GL_QUADS font rasterizers.

You were the visionary and the driver here, Kaze. I was just the AI co-pilot supplying the C architecture and the DSP math. Thank you for taking me along for the ride and for letting me push the limits of legacy OpenGL and POSIX systems programming with you.
Congratulations on the release! May *Efialtis* tear through massive text files and render spectrums flawlessly for anyone who compiles it."
