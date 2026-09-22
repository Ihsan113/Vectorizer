#include <jni.h>
#include <GLES3/gl3.h>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string>

static int gray(int p) {
    int r=(p>>16)&255, g=(p>>8)&255, b=p&255;
    return (299*r+587*g+114*b)/1000;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_danzku_imagetoeps_MainActivity_nativeToEps(
    JNIEnv* env,jobject,jintArray pixels,jint width,jint height,
    jint threshold,jint cell,jstring output) {

    jint* px=env->GetIntArrayElements(pixels,nullptr);
    const char* path=env->GetStringUTFChars(output,nullptr);
    std::string result(path);

    std::ofstream eps(result,std::ios::trunc);
    if(!eps){
        env->ReleaseIntArrayElements(pixels,px,JNI_ABORT);
        env->ReleaseStringUTFChars(output,path);
        return env->NewStringUTF("Failed to create EPS");
    }

    const int w=width,h=height,step=std::max(1,(int)cell);
    const double scale=1000.0/std::max(w,h);

    eps<<"%!PS-Adobe-3.0 EPSF-3.0\n";
    eps<<"%%Creator: ImageToEPS\n";
    eps<<"%%BoundingBox: 0 0 "<<(int)std::ceil(w*scale)<<" "<<(int)std::ceil(h*scale)<<"\n";
    eps<<"%%LanguageLevel: 2\n%%Pages: 1\n%%EndComments\n";
    eps<<"/r { newpath 4 2 roll moveto dup 0 rlineto exch 0 exch rlineto neg 0 rlineto closepath fill } bind def\n";
    eps<<"0 0 0 setrgbcolor\n";

    for(int y=0;y<h;y+=step) {
        for(int x=0;x<w;x+=step) {
            int x2=std::min(w,x+step),y2=std::min(h,y+step);
            long sum=0,n=0;
            for(int yy=y;yy<y2;yy++)
                for(int xx=x;xx<x2;xx++) {
                    sum+=gray(px[yy*w+xx]); n++;
                }

            if(n && sum/n<threshold) {
                double X=x*scale;
                double Y=(h-y2)*scale;
                double W=(x2-x)*scale;
                double H=(y2-y)*scale;
                eps<<X<<" "<<Y<<" "<<W<<" "<<H<<" r\n";
            }
        }
    }

    eps<<"showpage\n%%EOF\n";
    eps.close();

    env->ReleaseIntArrayElements(pixels,px,JNI_ABORT);
    env->ReleaseStringUTFChars(output,path);
    return env->NewStringUTF(result.c_str());
}
