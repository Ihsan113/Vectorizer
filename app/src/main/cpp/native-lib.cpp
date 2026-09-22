#include <jni.h>
#include <GLES3/gl3.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

static inline int gray(int argb) {
    int r = (argb >> 16) & 255;
    int g = (argb >> 8) & 255;
    int b = argb & 255;
    return (299*r + 587*g + 114*b) / 1000;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_danzku_imagetoeps_MainActivity_nativeToEps(
        JNIEnv* env, jobject,
        jintArray pixels, jint width, jint height,
        jint threshold, jint cell, jstring outPath) {

    const int w = width, h = height;
    const int step = std::max(1, (int)cell);

    jint* px = env->GetIntArrayElements(pixels, nullptr);
    const char* path = env->GetStringUTFChars(outPath, nullptr);

    std::ofstream eps(path, std::ios::out | std::ios::trunc);
    if (!eps) {
        env->ReleaseIntArrayElements(pixels, px, JNI_ABORT);
        env->ReleaseStringUTFChars(outPath, path);
        return env->NewStringUTF("Failed to open output");
    }

    // EPS uses points. Preserve image aspect ratio at a 1000pt long side.
    const double scale = 1000.0 / std::max(w, h);
    const double W = w * scale;
    const double H = h * scale;

    eps << "%!PS-Adobe-3.0 EPSF-3.0\n";
    eps << "%%Creator: ImageToEPS native vector engine\n";
    eps << "%%BoundingBox: 0 0 " << (int)std::ceil(W) << " "
        << (int)std::ceil(H) << "\n";
    eps << "%%LanguageLevel: 2\n";
    eps << "%%Pages: 1\n";
    eps << "%%EndComments\n";
    eps << "0 0 0 setrgbcolor\n";

    // MVP vectorizer: adaptive binary cells emitted as vector rectangles.
    // This is intentionally real vector EPS (paths), not an embedded bitmap.
    for (int y = 0; y < h; y += step) {
        for (int x = 0; x < w; x += step) {
            long sum = 0;
            int count = 0;
            const int y2 = std::min(h, y + step);
            const int x2 = std::min(w, x + step);
            for (int yy = y; yy < y2; ++yy) {
                for (int xx = x; xx < x2; ++xx) {
                    sum += gray(px[yy*w + xx]);
                    ++count;
                }
            }
            int g = count ? (int)(sum / count) : 255;
            if (g < threshold) {
                double X = x * scale;
                double Y = (h - y2) * scale;
                double CW = (x2 - x) * scale;
                double CH = (y2 - y) * scale;
                eps << X << " " << Y << " " << CW << " " << CH
                    << " rectfill\n";
            }
        }
    }

    eps << "showpage\n%%EOF\n";
    eps.close();

    env->ReleaseIntArrayElements(pixels, px, JNI_ABORT);
    env->ReleaseStringUTFChars(outPath, path);

    return env->NewStringUTF(path);
}
