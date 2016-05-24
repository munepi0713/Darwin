
#include <string>
#include <iostream>
#include <inttypes.h>

#include <opencv2/opencv.hpp>

#include "graph.h"

double sq(double a)
{
    return a * a;
}

double dist(double a, double b)
{
    return sqrt(a * a + b * b);
}

void NR2(const cv::Mat& src, const cv::Mat& obj, const cv::Mat& bkg, cv::Mat* dst)
{
    int dx[4] = { 1, -1, 0, 1 };
    int dy[4] = { 0,  1, 1, 1 };
    int n = src.cols;
    int m = src.rows;
    printf("%d x %d\n", n, m);

    // Calculate histogram of src.
    int fgbin[256];
    int bgbin[256];
    int fgtotal = 0;
    int bgtotal = 0;
    memset(fgbin, 0, sizeof(int) * 256);
    memset(bgbin, 0, sizeof(int) * 256);
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            int ip = src.at<unsigned char>(j, i);
            int io = obj.at<unsigned char>(j, i);
            int ib = bkg.at<unsigned char>(j, i);
            if ( io > 0 ) {
                // Object (Foreground)
                fgbin[ip]++;
                fgtotal++;
            }
            else if ( ib > 0 ) {
                // Background
                bgbin[ip]++;
                bgtotal++;
            }
        }
    }

    // Calculate frequency of intensities of src.
    double fgfreq[256], fgsum = 0;
    double bgfreq[256], bgsum = 0;
    for (int i = 0; i < 256; i++) {
        fgfreq[i] = fgbin[i] == 0 ? 0 : -log((double)fgbin[i] / fgtotal);
        bgfreq[i] = bgbin[i] == 0 ? 0 : -log((double)bgbin[i] / bgtotal);
        fgsum += (double)fgbin[i] / fgtotal;
        bgsum += (double)bgbin[i] / bgtotal;
        printf("[%3d] %.3f(%.3f) %.3f(%.3f)\n", i, (double)fgbin[i] / fgtotal, fgfreq[i], (double)bgbin[i] / bgtotal, bgfreq[i]);
    }
    printf("total %.3f        %.3f\n", fgsum, bgsum);

    // Calcualte sigma;
    double s1, s2, sigma2;
    int nn = 0;
    s1 = s2 = sigma2 = 0;
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            int ip = src.at<unsigned char>(j, i);
            for (int d = 0; d < 4; d++) {
                int x = i + dx[d];
                int y = j + dy[d];
                if ( 0 <= x && 0 <= y && x < n && y < m ) {
                    int iq = src.at<unsigned char>(x, y);
                    double delta = ip - iq;
                    nn++;
                    delta -= s1;
                    s1 += delta / nn;
                    s2 += (nn - 1) * delta * delta / nn;
                    //printf(".");
                }
            }
        }
    }
    printf("%f, %f\n", s1, s2 / (nn - 1));
    sigma2 = nn == 0 ? 0 : s2 / (nn - 1);
    //abort();

    //
    typedef Graph<int, int, int>  GraphType;
    
    int lambda = 10;//10;
    double alpha = 10;
    double kappa = 0.5 / sigma2; // 25; // 25;
    GraphType* g = new GraphType(n * m, n * m * 4); // Appox numbers of vertices and edges.
    printf("lambda=%.3f, alpha=%.3f, kappa=%.3f\n", (double)lambda, alpha, kappa);

    double maxSumB = 0;
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            int ip = src.at<unsigned char>(j, i);
            for (int d = 0; d < 4; d++) {
                int x = i + dx[d];
                int y = j + dy[d];
                double sumB = 0;
                if ( 0 <= x && 0 <= y && x < n && y < m ) {
                    int iq = src.at<unsigned char>(x, y);

                    sumB += alpha * exp(- kappa * sq((double)ip - iq)) / dist(dx[d], dy[d]);
                }
                if ( sumB > maxSumB ) maxSumB = sumB;
            }
        }
    }

    g->add_node(n * m);
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            int ip = src.at<unsigned char>(j, i);
            int io = obj.at<unsigned char>(j, i);
            int ib = bkg.at<unsigned char>(j, i);

            // Add edge between neighbors
            int maxB = 0;
            for (int d = 0; d < 4; d++) {
                int x = i + dx[d];
                int y = j + dy[d];
                if ( 0 <= x && 0 <= y && x < n && y < m ) {
                    int hpq, hqp;
                    int iq = src.at<unsigned char>(x, y);
                    if ( ip == iq ) {
                        hpq = hqp = 0;
                    }
                    else {
                        int B;
                        B = alpha * exp(- kappa * sq((double)ip - iq)) / dist(dx[d], dy[d]);
#if 0
                        hpq = hqp = B;
#else
                        if ( ip > iq ) {
                            hpq = B;
                            hqp = 0;
                        }
                        else {
                            hpq = 0;
                            hqp = B;
                        }
#endif
                    }
                    //if ( io == 255 )
                    //printf("H %d, %d | %f %f %f\n", hpq, hqp, kappa, sq((double)ip - iq), dist(dx[d], dy[d]));
                    g->add_edge(j * n + i, y * n + x, hpq, hqp);
                }
            }

            // Add edges between a pixel to source and sink.
            int g0, g1;
#if 0
            if ( ip > 0 ) {
                g1 = 0;
                g0 = lambda;
            }
            else {
                g1 = lambda;
                g0 = 0;
            }
#elif 1
            if ( io == 255 ) {
                g1 = 0;
                g0 = 1 + maxSumB;
            }
            else if ( ib == 255 ) {
                g1 = 1 + maxSumB;
                g0 = 0;
            }
            else {
                g1 = bgfreq[ip];
                g0 = fgfreq[ip];
            }
            g0 *= lambda;
            g1 *= lambda;
#endif
            //if ( io == 255 )
            //printf("G %d, %d\n", g1, g0);
            g->add_tweights(j * n + i, g1, g0);
        }
    }
    g->maxflow();
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            if ( g->what_segment(j * n + i) == GraphType::SOURCE ) {
                dst->at<unsigned char>(j, i) = 0;
            }
            else {
                dst->at<unsigned char>(j, i) = 255;
            }
        }
    }
    delete g;
}


int main(int argc, const char* argv[])
{
    const char* source = argv[1]; // input grayscale image.
    const char* object = argv[2]; // binary object image.
    const char* backgr = argv[3]; // binary background image.
    const char* result = argc > 4 ? argv[4] : "result.png"; // output binary image.

    // Load input images.
   	cv::Mat src = cv::imread(source, 0);
   	cv::Mat obj = cv::imread(object, 0);
   	cv::Mat bkg = cv::imread(backgr, 0);
    cv::Mat dst = cv::Mat(src.rows, src.cols, CV_8U);

    // Binarize seed images.
    cv::threshold(obj, obj, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    cv::threshold(bkg, bkg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    // Save input image for confirmation.
    cv::imwrite("_source.png", src);
    cv::imwrite("_object.png", obj);
    cv::imwrite("_background.png", bkg);

    // Noise reduction by Graph-Cut
    NR2(src, obj, bkg, &dst);

    // Write result.
    cv::imwrite(result, dst);

    return 0;
}
