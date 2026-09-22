#include <jni.h>
#include <GLES3/gl3.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

static inline int gray(int argb) {
    const int r = (argb >> 16) & 255;
    const int g = (argb >> 8) & 255;
    const int b = argb & 255;
    return (299*r + 587*g + 114*b) / 1000;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_danzku_imagetoeps_MainActivity_nativeToEps(
        JNIEnv* env, jobject,
        jintArray pixels, jint width, jint height,
        jint threshold, jint cell, jstring outPath) {

    const int w = width;
    const int h = height;
    const int step = std::max(1, (int)cell);

    jint* px = env->GetIntArrayElements(pixels, nullptr);
    const char* path = env->GetStringUTFChars(outPath, nullptr);

    const std::string outputPath(path);
    std::ofstream eps(outputPath, std::ios::out | std::ios::trunc);

    if (!eps) {
        env->ReleaseIntArrayElements(pixels, px, JNI_ABORT);
        env->ReleaseStringUTFChars(outPath, path);
        return env->NewStringUTF("Failed to open EPS output");
    }

    const double scale = 1000.0 / std::max(w, h);
    const double W = w * scale;
    const double H = h * scale;

    eps << "%!PS-Adobe-3.0 EPSF-3.0\n";
    eps << "%%Creator: ImageToEPS\n";
    eps << "%%BoundingBox: 0 0 "
        << (int)std::ceil(W) << " "
        << (int)std::ceil(H) << "\n";
    eps << "%%LanguageLevel: 2\n";
    eps << "%%Pages: 1\n";
    eps << "%%EndComments\n";
    eps << "/rectfill where { pop } { /rectfill { 4 2 roll moveto 1 index 0 rlineto 0 exch rlineto neg 0 rlineto closepath fill } bind def } ifelse\n";
    eps << "0 0 0 setrgbcolor\n";

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

            const int g = count ? (int)(sum / count) : 255;

            if (g < threshold) {
                const double X = x * scale;
                const double Y = (h - y2) * scale;
                const double CW = (x2 - x) * scale;
                const double CH = (y2 - y) * scale;
                eps << X << " " << Y << " " << CW << " " << CH << " rectfill\n";
            }
        }
    }

    eps << "showpage\n%%EOF\n";
    eps.close();

    env->ReleaseIntArrayElements(pixels, px, JNI_ABORT);
    env->ReleaseStringUTFChars(outPath, path);

    return env->NewStringUTF(outputPath.c_str());
}
