# Image → EPS Android

MVP Android app for converting raster images into real vector EPS paths.

## Current architecture

- Kotlin Android UI
- C++ NDK native vector engine
- OpenGL ES 3 library linked for the GPU pipeline
- GitHub Actions builds the APK
- No cloud image upload required

## Current vectorizer

The first MVP uses grayscale thresholding and emits vector rectangles as PostScript paths.
It is deliberately simple so the project has a reliable end-to-end pipeline.

### Next engine upgrade

Replace the cell vectorizer with:

1. GPU grayscale / denoise
2. GPU threshold / segmentation
3. connected-component extraction
4. contour tracing
5. Ramer-Douglas-Peucker simplification
6. cubic Bézier fitting
7. multi-color vector layers
8. EPS path output

That will produce much cleaner SVG/EPS-style artwork from logos, illustrations, icons and line art.

## Build locally

Open in Android Studio with Android SDK 35 and NDK installed.

Or use Gradle:

```bash
./gradlew assembleDebug
```

## GitHub Actions

Push the repository to GitHub. The workflow at
`.github/workflows/android.yml` builds a debug APK automatically and
publishes it as an Actions artifact.

The build does not use a GitHub-hosted GPU. The phone GPU is used by
the application at runtime when the GPU preprocessing pipeline is enabled.
