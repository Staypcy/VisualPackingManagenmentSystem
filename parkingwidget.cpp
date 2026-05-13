#include "parkingwidget.h"
#include<QGraphicsProxyWidget>
#include<QPainter>


parkingWidget::parkingWidget(QWidget *parent)
    :QGraphicsView(parent)
{
    m_scene=new QGraphicsScene(this);
    m_scene->setSceneRect(0,0,800,500);


    m_parkset.resize(row);
    for(int i=0;i<row;i++){
        m_parkset[i].resize(col);
    }
    qDebug()<<m_parkset.size()<<" "<<m_parkset[0].size();

    setScene(m_scene);
    setFixedSize(800,500);

    drawPark();
    drawline();
}

void parkingWidget::drawPark()
{
    m_scene->clear();
    m_parkset.clear();
    m_parkset.resize(row);
    for (int i = 0; i < row; ++i) {
        m_parkset[i].resize(col);
    }
    drawline();

    int width = this->width()-120;
    int height = this->height();

    //画停车位
    int gap = 10;
    int startX = 20, startY = 20;

    int W = (width - startX - gap*(col-1)) / col;
    int H = (height - startY - gap*(row-1)) / row;

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int x = startX + j * (W);
            int y = startY + i * (H + gap);

            ParkSpace *park = new ParkSpace(nullptr);
            park->setFixedSize(W, H/2);
            park->pos=QPoint(x,y);
            park->setStyleSheet("background-color: lightgreen; border: 1px solid black;");

            QGraphicsProxyWidget *proxy = m_scene->addWidget(park);
            proxy->setPos(x, y);

            m_parkset[i][j]=(park);

        }
    }

}

void parkingWidget::drawline()
{
    int width=this->width();
    int height=this->height();

    int num=height/20;
    int sizeH=20;
    int sizeW=20;

    Car car(nullptr);
    QSize size=(car.m_size);

    QPen pen(Qt::black);
    //auto temp=parkwidget->m_wait_carSet;
    m_scene->addLine((width-size.width()),40,width-size.width(),height,pen);
    for(int i=0;i<num;i++){
        m_scene->addLine((width-size.width()),40*(i+1),width,40*(i+1),pen);
    }
}

bool parkingWidget::parkIsFull()
{
    int rowSet=m_parkset.size();
    if(rowSet==0)return false;
    int colSet=m_parkset[0].size();
    for(int i=0;i<rowSet;i++){
        for(int j=0;j<colSet;j++){
            if(!m_parkset[i][j]->haveCar){
                isfull=false;
                return false;
            }
        }
    }
    isfull=true;
    return true;
}

ParkSpace::ParkSpace(QWidget *parent)
    :QWidget(parent),
    haveCar(false),pos(0,0)
{
    setStyleSheet("background-color: lightgreen; border: 1px solid black;");
}
