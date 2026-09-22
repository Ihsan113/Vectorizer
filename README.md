# ImageToEPS

Android Image → EPS MVP.

## Build

GitHub Actions installs only valid Android SDK packages:
- platform-tools
- Android 35 platform
- build-tools 35.0.0
- NDK 27.2.12479018
- CMake 3.22.1

There is intentionally **no `sdkmanager tools` command** because `tools` is not a valid modern SDK package.

## Runtime

The current engine is a CPU native vector-cell tracer. OpenGL ES 3 is linked and the project is prepared for the next GPU preprocessing stage.

Important: linking GLES does not magically move the vectorizer to the GPU. The next engine revision can implement actual OpenGL ES shader preprocessing (grayscale, thresholding, blur/edge passes) before CPU contour/path extraction.

## Output

The MVP produces real EPS/PostScript vector rectangles. It is not a raster image renamed to `.eps`.

## GitHub Actions

Push to GitHub and Actions will build the debug APK and expose it as an artifact named `ImageToEPS-debug`.
