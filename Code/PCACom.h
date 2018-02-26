#ifndef PCACOM_H
#define PCACOM_H
#include "widget.h"

#include <QDir>
#include <QPixmap>
#include <QImage>
#include <QRgb>
#include <QDebug>
#include <QList>

class PCACom
{
public:
    PCACom(QList<QImage> a,int h,int w,int tn);
    QList<QImage> GreyImage;
    unsigned char *srcBits;

    int IMG_HEIGHT;
    int IMG_WIDTH;
    int TRAIN_NUM;

    void load_data(double *T,unsigned char *src,int k);
    void calc_mean(double *T,double *m);
    void calc_covariance_matrix(double *T,double *L,double *m);
    void pick_eignevalue(double *b,double *q,double *p_q,int num_q);

    void cstrq(double a[],int n,double q[],double b[],double c[]);
    int csstq(int n,double b[],double c[],double q[],double eps,int l);
    void matrix_mutil(double *c,double *a,double *b,int x,int y,int z);
    void matrix_reverse(double *src,double *dest,int row,int col);

    bool getComImg();

    QList<QImage> result;

};

#endif // PCACOM_H
