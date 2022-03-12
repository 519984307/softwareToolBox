/***************************************************************
 * @Copyright(C)    wangchongwei (��Ȩ��������ɾ��)
 * @FileName:       MWaveview.h
 * @Author:         wangchongwei
 * @Version:        1.0.0
 * @Date:           2021.7.11
 *
 * @Description: ������ʾ�ļ̳���
    1. �������ݲ���
        - 1.׷�ӵ㷽ʽ
        - 2.���벨������
    2. ֧��16��ͨ��
    3. �����ֿ�����С�Ŵ�
    4. ����Ҽ�����������ͣ��ʼ
    5. ����ƶ���ʾ����
    6. ��������ס�����϶�����
    7. ������ʾ�ر�ĳ��ͨ��
    8. ���ĳ��ͨ������
    9. ����X��Y�᷶Χ
    10. �������ű���

 * ����΢�ź�: wang_chongwei
 * ���˹��ں�: ������(��ӭ��ע)
 * CSDN: https://blog.csdn.net/qq_38190041
 * ������վ: https://wang_chong_wei.gitee.io/blog/
 *
 ***************************************************************/

#include "mwaveview.h"


static QColor chColor[16] = {
    QColor (255, 192, 203),
    QColor (135, 206, 250),
    QColor (152, 251, 152),
    QColor (255, 228, 16),
    QColor (0, 225, 255),
    QColor (255, 20, 255),
    QColor (0, 23, 255),
    QColor (127, 255, 0),
    QColor (255, 0, 255),
    QColor (147, 112, 219),
    QColor (0, 255, 255),
    QColor (0, 0, 128),
    QColor (128, 0, 128),
    QColor (46, 139, 87),
    QColor (0, 255, 127),
    QColor (0, 0, 255),
};


// ���캯��
MWaveView::MWaveView(QWidget *parent) : QChartView(parent)
{
    //����
//    this->wave_layout = new QBoxLayout(QBoxLayout::LeftToRight,parent);

    // set Axis------------------
    this->m_wave.axisX = new QValueAxis;
    this->m_wave.axisY = new QValueAxis;
    this->m_wave.axisX->setTickCount(9);
    this->m_wave.axisY->setTickCount(9);
    this->m_wave.axisX->setLabelFormat("%d");
    this->m_wave.axisY->setLabelFormat("%d");

    this->m_wave.axisX->setTickType(QValueAxis::TickType::TicksFixed);

    this->m_wave.axisX->setRange(0, 1000);
    this->m_wave.axisY->setRange(-1000, 1000);

    // set chart-----------------
    this->m_wave.chart = new QChart;
    this->m_wave.chart->legend()->setVisible(false);       //�����ı���ʾ
    this->m_wave.chart->setTheme(QChart::ChartThemeLight);
//    this->m_wave.chart->setTitle("motor ctl");
    this->setChart(this->m_wave.chart);

    this->m_wave.chart->addAxis(this->m_wave.axisX, Qt::AlignBottom);
    this->m_wave.chart->addAxis(this->m_wave.axisY, Qt::AlignLeft);

    this->setRenderHint(QPainter::Antialiasing, true);      //������

    // ���ͨ������ʼ��ͨ������-------------
    for (int i = 0; i < SET_GLOBLE_CHANNEL; i++){
        addChannel((WAVE_CH)i);
    }

    // ��ʼ�����ֲ���-----------------
    m_wave.rangeX = 500;
    m_wave.rangeY = 500;
    this->m_wave.multipleX = 1.2;
    this->m_wave.multipleY = 1.2;
    this->m_event.rightButtonPressed = false;
    this->m_event.pauseWave = false;

    // menu--------------------
    this->m_event.menu = new QMenu(this);

    this->m_event.startAction = new QAction("start",this);
    this->m_event.pauseAction = new QAction("pause",this);
    this->m_event.startAction->setVisible(false);

    this->m_event.menu->addAction(this->m_event.startAction);
    this->m_event.menu->addAction(this->m_event.pauseAction);

    // ʵ���Զ���תaction
    connect(m_event.startAction, &QAction::triggered, [=] {
        m_event.startAction->setVisible(false);
        m_event.pauseAction->setVisible(true);});
    connect(m_event.pauseAction, &QAction::triggered, [=] {
        m_event.pauseAction->setVisible(false);
        m_event.startAction->setVisible(true);});

    // �ź����
    connect(m_event.pauseAction, &QAction::triggered, this, &MWaveView::slots_pauseGraph);
    connect(m_event.startAction, &QAction::triggered, this, &MWaveView::slots_startGraph);
}

MWaveView::~MWaveView()
{

}


void MWaveView::addChannel(WAVE_CH ch)
{
    SeriesType *series = new SeriesType();

    QPen pen;
    pen.setWidth(2);             // �����ߵĿ��
    pen.setColor(chColor[ch]);   // ������ɫ
    series->setPen(pen);
    series->setUseOpenGL(true);  // ����OPENGL
    // ��ӽ�map
    this->m_wave.map_series.insert(ch,series);
    // ��chart��add series
    this->m_wave.chart->addSeries(this->m_wave.map_series[ch]);
    // ���߶�Ӧ��
    this->m_wave.map_series[ch]->attachAxis(m_wave.axisX);
    this->m_wave.map_series[ch]->attachAxis(m_wave.axisY);
}
/* ���ô�ͨ��ͼ��
 *
*/
void MWaveView::openChannel(WAVE_CH ch)
{
    if (this->m_wave.map_series.contains(ch))
    {
        this->m_wave.map_series[ch]->setVisible(true);
    }
}
/* ���ùر�ͨ��ͼ��
 * setPointsVisible
*/
void MWaveView::closeChannel(WAVE_CH ch)
{
    if (this->m_wave.map_series.contains(ch))
    {
        this->m_wave.map_series[ch]->setVisible(false);
    }
}

//ѡ���Ƿ��ͨ��
void MWaveView::choseOpenChannel(WAVE_CH ch,bool isOpen){
    (isOpen == true)?(openChannel(ch)):(closeChannel(ch));
}

void MWaveView::clearChannel(WAVE_CH ch)
{
    if (this->m_wave.map_series.contains(ch))
    {
        this->m_wave.map_series[ch]->clear();
    }
}
// ���÷�Χ
void MWaveView::setRangeX(int rangeX)
{
    this->m_wave.rangeX = rangeX;
}
// ���÷�Χ
void MWaveView::setRangeY(int rangeY)
{
    this->m_wave.rangeY = rangeY;
}

void MWaveView::updateRange()
{
    double maxX,minX;
//    double maxY,minY;

    maxX = m_wave.last_point_x > m_wave.rangeX? m_wave.last_point_x:m_wave.rangeX;
    minX = maxX - m_wave.rangeX >0 ? maxX - m_wave.rangeX: 0;

//    maxY = m_event.moveY + m_wave.rangeY*0.5;
//    minY = m_event.moveY - m_wave.rangeY*0.5;

    this->m_wave.axisX->setRange(minX, maxX);
//    this->m_wave.axisY->setRange(minY, maxY);
}
// ���÷Ŵ���
void MWaveView::setZoomX(double multiple)
{
    if (multiple > 0){
        this->m_wave.multipleX = multiple;
    }
}
// ���÷Ŵ���
void MWaveView::setZoomY(double multiple)
{
    if (multiple > 0){
        this->m_wave.multipleY = multiple;
    }
}

// ��С
void MWaveView::ZoomOutX(void)
{
    double zoom_out = 1.0/this->m_wave.multipleX;

    this->m_wave.rangeX *= zoom_out;

}
// �Ŵ�
void MWaveView::ZoomX(void)
{
    this->m_wave.rangeX *= this->m_wave.multipleX;

}
// ��С
void MWaveView::ZoomOutY(void)
{
    double zoom_out = 1.0/this->m_wave.multipleY;

    this->m_wave.rangeY *= zoom_out;
}
// �Ŵ�
void MWaveView::ZoomY(void)
{
    this->m_wave.rangeY *= this->m_wave.multipleY;
}

void MWaveView::startGraph(void)
{
    this->m_event.pauseWave = false;
}
void MWaveView::pauseGraph(void)
{
    this->m_event.pauseWave = true;
}

void MWaveView::slots_startGraph(void)
{
    startGraph();
}
void MWaveView::slots_pauseGraph(void)
{
    pauseGraph();
}

/* ׷�Ӷ�Ӧͨ���ĵ������
 *
*/
void MWaveView::addSeriesData(WAVE_CH ch, const QPointF& point)
{

    if (this->m_event.pauseWave == true) return;

    if (this->m_wave.map_series.contains(ch))
    {
        this->m_wave.map_series[ch]->append(point);
    }
}

void MWaveView::addSeriesData(WAVE_CH ch, const QList<QPointF>& point_list)
{
    static double x;

    if (this->m_event.pauseWave == true) return;

    if (this->m_wave.map_series.contains(ch))
    {
        this->m_wave.map_series[ch]->replace(point_list);

        if (point_list.count()){
            x = point_list[point_list.count()-1].x();
            m_wave.last_point_x = x > m_wave.last_point_x? x: m_wave.last_point_x;
        }
        else{
            m_wave.last_point_x = 0;
        }

        updateRange();
    }
}

// ��갴���¼�
void MWaveView::mousePressEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        this->m_event.leftButtonPressed = true;
        this->m_event.PressedPos = pEvent->pos();
        this->setCursor(Qt::OpenHandCursor);
    }
}
// ����ɿ��¼�
void MWaveView::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        this->m_event.leftButtonPressed = false;
        this->setCursor(Qt::ArrowCursor);
    }
}


// ����ƶ��¼�
void MWaveView::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (m_event.leftButtonPressed)
    {
        QPoint oDeltaPos = pEvent->pos() - m_event.PressedPos;

        m_wave.chart->scroll(-oDeltaPos.x(), oDeltaPos.y());

        QPointF pos = m_wave.chart->mapToValue(pEvent->pos()) - m_wave.chart->mapToValue(m_event.PressedPos);

        m_event.moveX += -pos.x();
        m_event.moveY += -pos.y();

        m_event.PressedPos = pEvent->pos();
    }


    QPointF f = m_wave.chart->mapToValue(pEvent->pos());  //��ȡ��ǰ�����λ��
    QString str = '(' + QString::number(f.x()) + ',' + QString::number(f.y()) + ')';
    QToolTip::showText(pEvent->globalPos(), str);

    //QToolTip::hideText();
}


void MWaveView::wheelEvent(QWheelEvent *pEvent)
{
    double multipleX, multipleY;

    // 1. ��ȡ��ͼ������Ϣ
    QRectF oPlotAreaRect = m_wave.chart->plotArea();
    QPointF oCenterPoint = oPlotAreaRect.center();


    // �����������÷�Χ
    if (pEvent->delta()<0){
        multipleX = this->m_wave.multipleX;
        multipleY = this->m_wave.multipleY;

        ZoomX();
        ZoomY();
    }
    else
    {
        multipleX = 1/this->m_wave.multipleX;
        multipleY = 1/this->m_wave.multipleY;

        ZoomOutX();
        ZoomOutY();
    }


    // 2. ˮƽ����
    oPlotAreaRect.setWidth(oPlotAreaRect.width() * multipleX);
    // 3. ��ֱ����
    oPlotAreaRect.setHeight(oPlotAreaRect.height() * multipleY);
    // 4 �����ӵ�
    QPointF oNewCenterPoint((2*oCenterPoint - pEvent->pos()) - (oCenterPoint - pEvent->pos()) / ((multipleX+multipleY)*0.5));
    // 5. �����ӵ�
    oPlotAreaRect.moveCenter(oNewCenterPoint);
    // ����Ҫ����Y��ƫ����
    QPointF movepos =  m_wave.chart->mapToValue(oNewCenterPoint) - m_wave.chart->mapToValue(oCenterPoint);
    m_event.moveY += movepos.ry();
    // 6. �ύ���ŵ���
    this->m_wave.chart->zoomIn(oPlotAreaRect);

}


void MWaveView::contextMenuEvent(QContextMenuEvent *event)
{

    m_event.menu->move(cursor().pos());
    m_event.menu->show();
}
