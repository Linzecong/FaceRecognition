#include "widget.h"
#include "ui_widget.h"
#include "NeuralNet.h"
#include <QDir>
#include <QPixmap>
#include <QImage>
#include <QRgb>
#include <QDebug>
#include <vector>
#include <QFileDialog>
#include <map>
#include <algorithm>
#define HEIGHT 50
#define WIDTH 50
#define TRAIN_NUM 48
using namespace std;

Widget::Widget(QWidget *parent) :QWidget(parent),ui(new Ui::Widget){

    ui->setupUi(this);

    QDir dir("train/");
    QStringList infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);

    for(int i=0; i<infolist.size(); i++)
        ui->ImgDirList->addItem(infolist[i]);

    connect(ui->ImgDirList,&QListWidget::clicked,this,&Widget::dirClick);
    connect(ui->ImgList,&QListWidget::clicked,this,&Widget::imgClick);
    connect(ui->ClearButton,&QPushButton::clicked,this,&Widget::clearClick);
    connect(ui->FilteButton,&QPushButton::clicked,this,&Widget::filteClick);
    connect(ui->TrainButton,&QPushButton::clicked,this,&Widget::train);
    connect(ui->RecoButton,&QPushButton::clicked,this,&Widget::test);
    connect(ui->ViewAll,&QPushButton::clicked,this,&Widget::viewAll);
    connect(ui->testAllButton,&QPushButton::clicked,this,&Widget::testAll);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::dirClick()
{
    ui->ImgList->clear();
    QDir dir("train/"+ui->ImgDirList->currentItem()->text()+"/");
    QStringList infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);

    for(int i=0; i<infolist.size(); i++)
        ui->ImgList->addItem(infolist[i]);
}

void Widget::imgClick()
{
    QImage image;
    image = QImage("train/"+ui->ImgDirList->currentItem()->text()+"/"+ui->ImgList->currentItem()->text()).scaled(HEIGHT,WIDTH);
    ui->PreviewImg->setPixmap(QPixmap::fromImage(image));
    getGrey(image);
}


void Widget::filteClick()
{
    for(int i=0;i<ui->ImgDirList->count();i++){
        QDir dir("train/"+ui->ImgDirList->item(i)->text()+"/");
        QStringList infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
        if(infolist.size()<=10){
            ui->ImgDirList->takeItem(i);
            i--;
        }
    }
    qDebug()<<ui->ImgDirList->count();

}

QImage Widget::getGrey(QImage image,int flag)
{
    int w,h;
    w = image.width();
    h = image.height();
    QImage iGray(w,h,QImage::Format_RGB32);
    for(int i=0; i<HEIGHT; i++){
        for(int j=0;j<WIDTH; j++){
            QRgb pixel = image.pixel(i,j);
            int gray = qGray(pixel);
            QRgb grayPixel = qRgb(gray,gray,gray);
            iGray.setPixel(i,j,grayPixel);
        }
    }
    if(flag!=2)ui->PreImgGrey->setPixmap(QPixmap::fromImage(iGray));
    getLBP(iGray);
    if(!flag)getCom();
    return iGray;
}

uchar getgreynum(QImage *img,int x,int y){
    QRgb pixel = img->pixel(x,y);
    int gray = qGray(pixel);
    return gray;
}

void Widget::getCom()
{
    QList<QImage> list;
    for(int i=0;i<ui->ImgList->count();i++){
        QImage image;
        image = QImage("train/"+ui->ImgDirList->currentItem()->text()+"/"+ui->ImgList->item(i)->text()).scaled(HEIGHT,WIDTH);
        list.append(image);
    }
    PCACom pca(list,HEIGHT,WIDTH,ui->ImgList->count());
    if(pca.getComImg()==true){
        QImage img(HEIGHT,WIDTH,QImage::Format_RGB32);
        for(int i=0;i<HEIGHT;i++)
            for(int j=0;j<WIDTH;j++)
            {
                int num=0;
                for(int k=0;k<pca.result.count();k++){

                    num+=getgreynum(&pca.result[k],i,j);

                }
                img.setPixelColor(i,j,qRgb(num/pca.result.count(),num/pca.result.count(),num/pca.result.count()));
            }

        ui->PreImgComPCA->setPixmap(QPixmap::fromImage(img));
    }
    else
        ui->PreImgComPCA->setText("获取特征失败！");
}

void Widget::viewAll()
{
    QList<QImage> list;
    for(int i=0;i<ui->ImgList->count();i++){
        QImage image;
        image = QImage("train/"+ui->ImgDirList->currentItem()->text()+"/"+ui->ImgList->item(i)->text()).scaled(HEIGHT,WIDTH);
        list.append(image);
    }
    PCACom pca(list,HEIGHT,WIDTH,ui->ImgList->count());
    if(pca.getComImg()==true){

                for(int k=0;k<pca.result.count();k++){

                    QLabel* temp=new QLabel;
                    temp->setGeometry(300+k*20,300+k*20,150,150);
                    temp->setScaledContents(true);
                    temp->setPixmap(QPixmap::fromImage(pca.result[k]));
                    temp->show();

                }
    }
}



QImage Widget::getLBP(QImage img)
{
    QImage result(HEIGHT,WIDTH,QImage::Format_RGB32);

    for(int i = 1; i<HEIGHT - 1; i++)
    {
        for(int j = 1;j<WIDTH -1; j++)
        {
            uchar center = getgreynum(&img,i,j);

            uchar code = 0;
            code |= (getgreynum(&img,i-1,j-1) >= center)<<7;
            code |= (getgreynum(&img,i-1, j) >= center)<<6;
            code |= (getgreynum(&img,i-1, j+1) >= center)<<5;
            code |= (getgreynum(&img,i, j+1) >= center)<<4;
            code |= (getgreynum(&img,i+1, j+1) >= center)<<3;
            code |= (getgreynum(&img,i+1, j) >= center)<<2;
            code |= (getgreynum(&img,i+1, j-1) >= center)<<1;
            code |= (getgreynum(&img,i, j-1) >= center)<<0;
            result.setPixelColor(i,j,QColor(code,code,code));
        }
    }

    ui->PreImgComLBP->setPixmap(QPixmap::fromImage(result));
    return result;
}

void Widget::train()
{
    //建立一个数据类
    Data* MyData=new Data(HEIGHT*WIDTH,TRAIN_NUM);
    int count =0;
    for(int qq=0;qq<TRAIN_NUM;qq++){
        vector<float> input1;
        vector<float> output1;
        for(int i=0;i<TRAIN_NUM;i++)
            if(qq==i)
                output1.push_back(1.0);
            else
                output1.push_back(0.0);

        //读取每一张图像
        for(int i=0;i<ui->ImgDirList->count();i++){
            if(i!=qq)
                continue;
            QDir dir("train/"+ui->ImgDirList->item(i)->text()+"/");
            QStringList infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
            for(int j=0;j<infolist.size();j++){
                input1.clear();
                QImage img=getGrey(QImage("train/"+ui->ImgDirList->item(i)->text()+"/"+infolist[j]).scaled(HEIGHT,WIDTH),2);


                qDebug()<<"train/"+ui->ImgDirList->item(i)->text()+"/"+infolist[j];

                //转为灰度值
                for(int ii=0;ii<HEIGHT;ii++)
                    for(int jj=0;jj<WIDTH;jj++)
                    {
                        QRgb pixel = img.pixel(ii,jj);
                        int gray = qGray(pixel);
                        input1.push_back(gray/255.0);
                    }
                MyData->AddData(input1,output1);//添加数据
                count++;
            }
        }
    }
    qDebug()<<"Loaded  "<<count<<" Datas ";
    NeuralNet* Brain=new NeuralNet(HEIGHT*WIDTH,TRAIN_NUM,75,0.15,NeuralNet::ERRORSUM,true);//新建一个神经网络，输入神经元个数，输出神经元个数，隐藏层神经元个数，学习率，停止训练方法（次数或误差最小），是否输出误差值（用于观察是否收敛）
    Brain->SetErrorThrehold(100);//设置误差
    qDebug()<<"Train!!!!!";
    Brain->Train(MyData);//通过数据，开始训练
    qDebug()<<"Done!!!!!";
    Brain->saveNet("model1111.txt");//保存网络

//    NeuralNet* Brain2=new NeuralNet("D:\\1.txt");//通过文件读取网络
//    cout<<Brain2->Update(input1)[0]<<endl;
//    cout<<Brain2->Update(input2)[0]<<endl;
//    cout<<Brain2->Update(input3)[0]<<endl;
//    cout<<Brain2->Update(input4)[0]<<endl;

}

void Widget::test()
{
    NeuralNet* Brain=new NeuralNet("model1111.txt");//通过文件读取网络
    vector<float> input1;
   // QImage img=getGrey(QImage(QFileDialog::getOpenFileName()).scaled(HEIGHT,WIDTH));


    QImage img;
    img = QImage(QFileDialog::getOpenFileName()).scaled(HEIGHT,WIDTH);
    ui->PreviewImg->setPixmap(QPixmap::fromImage(img));
    img=getGrey(img,1);


    for(int ii=0;ii<HEIGHT;ii++)
        for(int jj=0;jj<WIDTH;jj++)
        {
            QRgb pixel = img.pixel(ii,jj);
            int gray = qGray(pixel);
            input1.push_back(gray/255.0);
        }

    vector<float> out=Brain->Update(input1);

    vector<pair<float,int> > li;
    for(int i=0;i<TRAIN_NUM;i++)
        li.push_back(make_pair(out[i],i));
    sort(li.begin(),li.end());



    QDir dir= QDir("train/"+ui->ImgDirList->item(li[TRAIN_NUM-1].second)->text());
    QStringList infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
    ui->ReImg1->setPixmap(QPixmap::fromImage(QImage("train/"+ui->ImgDirList->item(li[TRAIN_NUM-1].second)->text()+"/"+infolist[0])));
    dir= QDir("train/"+ui->ImgDirList->item(li[TRAIN_NUM-2].second)->text());
    infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
    ui->ReImg2->setPixmap(QPixmap::fromImage(QImage("train/"+ui->ImgDirList->item(li[TRAIN_NUM-2].second)->text()+"/"+infolist[0])));
    dir= QDir("train/"+ui->ImgDirList->item(li[TRAIN_NUM-3].second)->text());
    infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
    ui->ReImg3->setPixmap(QPixmap::fromImage(QImage("train/"+ui->ImgDirList->item(li[TRAIN_NUM-3].second)->text()+"/"+infolist[0])));
    dir= QDir("train/"+ui->ImgDirList->item(li[TRAIN_NUM-4].second)->text());
    infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
    ui->ReImg4->setPixmap(QPixmap::fromImage(QImage("train/"+ui->ImgDirList->item(li[TRAIN_NUM-4].second)->text()+"/"+infolist[0])));


    ui->ReName1->setText(ui->ImgDirList->item(li[TRAIN_NUM-1].second)->text());
    ui->ReName2->setText(ui->ImgDirList->item(li[TRAIN_NUM-2].second)->text());
    ui->ReName3->setText(ui->ImgDirList->item(li[TRAIN_NUM-3].second)->text());
    ui->ReName4->setText(ui->ImgDirList->item(li[TRAIN_NUM-4].second)->text());

    for(int i=0;i<TRAIN_NUM;i++)
        qDebug()<<out[i];

    qDebug()<<" ";
    qDebug()<<" ";
    qDebug()<<" ";
}

void Widget::testAll()
{
    static NeuralNet* Brain=new NeuralNet("model1111.txt");//通过文件读取网络
    vector<float> input1;
    int total=0;
    int correct=0;
    for(int i=0;i<ui->ImgDirList->count();i++){

        QDir dir("test/"+ui->ImgDirList->item(i)->text()+"/");
        QStringList infolist = dir.entryList(QDir::NoDotAndDotDot|QDir::AllEntries);
        for(int j=0;j<infolist.size();j++){
            input1.clear();
            QImage img=getGrey(QImage("test/"+ui->ImgDirList->item(i)->text()+"/"+infolist[j]).scaled(HEIGHT,WIDTH),2);
            qDebug()<<"train/"+ui->ImgDirList->item(i)->text()+"/"+infolist[j];

            total++;
            for(int ii=0;ii<HEIGHT;ii++)
                for(int jj=0;jj<WIDTH;jj++)
                {
                    QRgb pixel = img.pixel(ii,jj);
                    int gray = qGray(pixel);
                    input1.push_back(gray/255.0);
                }

            vector<float> out=Brain->Update(input1);

            vector<pair<float,int> > li;
            for(int i=0;i<TRAIN_NUM;i++)
                li.push_back(make_pair(out[i],i));
            sort(li.begin(),li.end());



            for(int q=1;q<=10;q++)
            if(ui->ImgDirList->item(i)->text()==ui->ImgDirList->item(li[TRAIN_NUM-q].second)->text())
                correct++;


        }
    }
    ui->shibielv->setText(QString::number(correct)+"/"+QString::number(total)+"\n"+QString::number(double(correct)/double(total)*100.0));

}

void Widget::clearClick()
{
    ui->PreviewImg->setText("图像预览");
    ui->PreImgComPCA->setText("PCA特征");
    ui->PreImgComLBP->setText("LBP特征");
    ui->PreImgGrey->setText("灰度图");

    ui->ReImg1->setText("识别出的人1");
    ui->ReImg2->setText("识别出的人2");
    ui->ReImg3->setText("识别出的人3");
    ui->ReImg4->setText("识别出的人4");

    ui->ReName1->setText("名字1");
    ui->ReName2->setText("名字2");
    ui->ReName3->setText("名字3");
    ui->ReName4->setText("名字4");
}
