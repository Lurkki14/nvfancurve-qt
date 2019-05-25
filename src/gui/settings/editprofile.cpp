/*This file is part of TuxClocker.

Copyright (c) 2019 Jussi Kuokkanen

TuxClocker is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TuxClocker is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TuxClocker.  If not, see <https://www.gnu.org/licenses/>.*/

#include "editprofile.h"
#include "ui_editprofile.h"
#include "src/gui/mainwindow.h"

editProfile::editProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editProfile)
{
    ui->setupUi(this);

    // Get the main widget backgorund palette and use the colors for the plots
    QPalette palette;
    palette.setCurrentColorGroup(QPalette::Active);
    QColor color = palette.color(QPalette::Background);
    QColor textColor = palette.color(QPalette::Text);
    QColor graphColor = palette.color(QPalette::Highlight);
    QPen graphPen;
    graphPen.setWidth(2);
    graphPen.setColor(graphColor);
    QPen tickPen;
    tickPen.setWidthF(0.5);
    tickPen.setColor(textColor);

    // Define the filler line vectors and graphs so they don't need to be recreated every update
    leftLineX.append(x_lower);
    leftLineX.append(0);
    leftLineY.append(0);
    leftLineY.append(0);

    rightLineX.append(0);
    rightLineX.append(x_upper);
    rightLineY.append(0);
    rightLineY.append(0);

    ui->curvePlot->addGraph();
    ui->curvePlot->addGraph();

    ui->curvePlot->addGraph();
    ui->curvePlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->curvePlot->graph(0)->setLineStyle(QCPGraph::lsLine);

    ui->curvePlot->setBackground(color);
    ui->curvePlot->xAxis->setLabelColor(textColor);
    ui->curvePlot->yAxis->setLabelColor(textColor);
    ui->curvePlot->xAxis->setTickLabelColor(textColor);
    ui->curvePlot->yAxis->setTickLabelColor(textColor);
    ui->curvePlot->graph(0)->setPen(graphPen);
    ui->curvePlot->graph(1)->setPen(graphPen);
    ui->curvePlot->graph(2)->setPen(graphPen);
    ui->curvePlot->xAxis->setTickPen(tickPen);
    ui->curvePlot->yAxis->setTickPen(tickPen);
    ui->curvePlot->xAxis->setSubTickPen(tickPen);
    ui->curvePlot->yAxis->setSubTickPen(tickPen);
    ui->curvePlot->xAxis->setBasePen(tickPen);
    ui->curvePlot->yAxis->setBasePen(tickPen);

    ui->curvePlot->xAxis->setLabel("Temperature (°C)");
    ui->curvePlot->yAxis->setLabel("Fan speed (%)");

    ui->curvePlot->xAxis->setRange(x_lower, (x_upper+5));
    ui->curvePlot->yAxis->setRange(y_lower, (y_upper+5));

    connect(ui->curvePlot, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(clickedGraph(QMouseEvent*)));
    connect(ui->curvePlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), SLOT(clickedPoint(QCPAbstractPlottable*,int,QMouseEvent*)));
    connect(ui->curvePlot, SIGNAL(mousePress(QMouseEvent*)), SLOT(detectPress(QMouseEvent*)));
    connect(ui->curvePlot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(detectMove(QMouseEvent*)));
    connect(ui->curvePlot, SIGNAL(mouseRelease(QMouseEvent*)), SLOT(detectRelease(QMouseEvent*)));
    connect(ui->curvePlot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(getClosestCoords(QMouseEvent*)));
    connect(ui->curvePlot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(drawPointHoveredText(QMouseEvent*)));

    // Load the existing points to the graph
    MainWindow mw;
    for (int i=0; i<mw.xCurvePoints.length(); i++) {
        qv_x.append(mw.xCurvePoints[i]);
        qv_y.append(mw.yCurvePoints[i]);
    }

    ui->curvePlot->graph(0)->setData(qv_x, qv_y);
    drawFillerLines();

    QCPItemText *text = new QCPItemText(ui->curvePlot);
    coordText = text;
    coordText->setColor(textColor);
}

editProfile::~editProfile()
{
    delete ui;
}

void editProfile::addPoint(double x, double y)
{
    y = round(y);
    x = round(x);
    if (qv_x.length() != 0) {
        checkForDuplicatePoint(x, y);
    }
    if ((x_lower<=x) && (x<=x_upper) && (y_lower<=y) && (y<=y_upper) && !duplicatePoint) {
        qv_x.append(x);
        qv_y.append(y);
        index_y = qv_y.size()-1;
        index_x = qv_x.size()-1;
    }
}

void editProfile::rePlot()
{
    ui->curvePlot->replot();
    ui->curvePlot->update();
}
void editProfile::clickedGraph(QMouseEvent *event)
{
    QPoint point = event->pos();
    addPoint(ui->curvePlot->xAxis->pixelToCoord(point.x()), ui->curvePlot->yAxis->pixelToCoord(point.y()));
    ui->curvePlot->graph(0)->setData(qv_x, qv_y);
    drawFillerLines();
}

void editProfile::clickedPoint(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    checkForNearbyPoints(event);
    ycoord = round(ycoord);
    xcoord = round(xcoord);
    if (isNearbyPoint && qv_x.length() != 0) {
            for (int i=0; i<qv_y.length(); i++ ) {
                qv_y[i] = round(qv_y[i]);
                qv_x[i] = round(qv_x[i]);
                if ((qv_x[i] == xcoord) && (qv_y[i] == ycoord)) {
                    qv_x.remove(i);
                    qv_y.remove(i);
                    break;
                }
            }
      ui->curvePlot->graph(0)->setData(qv_x, qv_y);
      rePlot();
      drawFillerLines();
    }

}

bool editProfile::checkForNearbyPoints(QMouseEvent *event)
{
    if (qv_x.length() != 0) {
        QPoint point = event->pos();
        double pointerxcoord = ui->curvePlot->xAxis->pixelToCoord(point.x());
        double pointerycoord = ui->curvePlot->yAxis->pixelToCoord(point.y());
        for (int i=0; i<qv_x.length(); i++) {
            if ((abs(pointerxcoord - qv_x[i]) < selectionPrecision) && abs(pointerycoord - qv_y[i]) < selectionPrecision) {
                isNearbyPoint = true;
                break;
            } else {
                isNearbyPoint = false;
            }
        }
    return isNearbyPoint;
    }
}
void editProfile::drawPointHoveredText(QMouseEvent *event)
{
    checkForNearbyPoints(event);
    if (isNearbyPoint && !draggingPoint) {
        if (xcoord < x_upper*0.1) {
            coordText->position->setCoords(x_upper*0.1, xcoord + 4);
        } else if (xcoord > x_upper*0.9) {
            coordText->position->setCoords(x_upper*0.9, xcoord + 4);
        } else {
            coordText->position->setCoords(xcoord, ycoord + 4);
        }

        coordText->setText(QString::number(xcoord) + ", " + QString::number(ycoord));
        rePlot();
    }
    if (!isNearbyPoint && !draggingPoint) {
        coordText->setText("");
        rePlot();
    }
}
int editProfile::getDataIndex(QCPAbstractPlottable *plottable, int dataIndex)
{
    dataIndex = pointIndex;
    return pointIndex;
}

bool editProfile::checkForDuplicatePoint(int x, int y)
// Return true if there is a duplicate point
{
    for (int i=0; i<qv_x.length(); i++) {
        qv_x[i] = round(qv_x[i]);
        qv_y[i] = round(qv_y[i]);
        if ((x == qv_x[i]) && (y == qv_y[i])) {
            duplicatePoint = true;
        }
    }
    return duplicatePoint;
}
void editProfile::getClosestCoords(QMouseEvent *event)
{
    // Get the coordinates of the point that the mouse is over
    if (qv_y.size() > 0) {
        QPoint cursor = event->pos();
        double pointerycoord = ui->curvePlot->yAxis->pixelToCoord(cursor.y());
        double pointerxcoord = ui->curvePlot->xAxis->pixelToCoord(cursor.x());
        for (int i=0; i<qv_y.size(); i++) {
            if (sqrt(abs(pointerxcoord - qv_x[i]) * (abs(pointerxcoord - qv_x[i])) + abs(pointerycoord - qv_y[i]) * abs(pointerycoord - qv_y[i])) < selectionPrecision) {
                xcoord = qv_x[i];
                ycoord = qv_y[i];
                break;
            }
        }
    }
    getPointIndices();
}

bool editProfile::detectMove(QMouseEvent *event)
{
    mouseMoving = true;
    if (mouseMoving && mousePressed) {
        initializeDragging(event);
    }
    return mouseMoving;
}

bool editProfile::detectPress(QMouseEvent *event)
{
    mousePressed = true;
    if (mouseMoving && mousePressed) {
        initializeDragging(event);
    }
    return mousePressed;
}

bool editProfile::initializeDragging(QMouseEvent *event)
{
    // Decides whether to start dragging the point
    if (!pressTimer->isActive() && !mouseDragging) {
        pressTimer->start(20);
        pressTimer->setSingleShot(true);
    }
    mouseDragging = true;
    checkForNearbyPoints(event);
    if ((isNearbyPoint || draggingPoint) && pressTimer->remainingTime() <= 0) {
        dragPoint(index_x, index_y, event);
        draggingPoint = true;
    }
    return mouseDragging;
}
void editProfile::getPointIndices()
{
    if (qv_y.size() > 0) {
        for (int i=0; i<qv_y.size(); i++) {
            if ((qv_x[i] == xcoord) && (qv_y[i] == ycoord)) {
                index_x = i;
                index_y = i;
                break;
            }
        }
    }
}

void editProfile::dragPoint(int index_x, int index_y, QMouseEvent* event)
{
    // Moves the selected point by index
    // Sleep here so we don't take up so much CPU time
    QThread::msleep(10);
    QPoint point = event->pos();
    qv_y[index_y] = round(ui->curvePlot->yAxis->pixelToCoord(point.y()));
    qv_x[index_x] = round(ui->curvePlot->xAxis->pixelToCoord(point.x()));

    if (qv_x[index_x] > x_upper) {
        qv_x[index_x] = x_upper;
    }
    if (qv_x[index_x] < x_lower) {
        qv_x[index_x] = x_lower;
    }
    if (qv_y[index_y] > y_upper) {
        qv_y[index_y] = y_upper;
    }
    if (qv_y[index_y] < y_lower) {
        qv_y[index_y] = y_lower;
    }
    // Display the coordinates
    if (ui->curvePlot->xAxis->pixelToCoord(point.x()) < x_upper*0.1) {
        coordText->position->setCoords(x_upper*0.1, qv_y[index_y] + 4);
    } else if (ui->curvePlot->xAxis->pixelToCoord(point.x()) > x_upper*0.9) {
        coordText->position->setCoords(x_upper*0.9, qv_y[index_y] + 4);
    } else {
        coordText->position->setCoords(qv_x[index_x], qv_y[index_y] + 4);
    }
    QString xString = QString::number(qv_x[index_x]);
    QString yString = QString::number(qv_y[index_y]);
    coordText->setText(xString + ", " + yString);

    ui->curvePlot->graph(0)->setData(qv_x, qv_y);
    drawFillerLines();
}
void editProfile::drawFillerLines()
{
    // Draw the filler lines separately so they don't interfere with the main data. graph(1) = leftward line, graph(2) = rightward line

    leftLineX[1] = ui->curvePlot->graph(0)->dataSortKey(0);

    leftLineY[0] = ui->curvePlot->graph(0)->dataMainValue(0);
    leftLineY[1] = ui->curvePlot->graph(0)->dataMainValue(0);

    ui->curvePlot->graph(1)->setData(leftLineX, leftLineY);

    rightLineX[0] = ui->curvePlot->graph(0)->dataSortKey(qv_x.length() -1);

    rightLineY[0] = ui->curvePlot->graph(0)->dataMainValue(qv_x.length() -1);
    rightLineY[1] = ui->curvePlot->graph(0)->dataMainValue(qv_x.length() -1);

    ui->curvePlot->graph(2)->setData(rightLineX, rightLineY);
    rePlot();
}
void editProfile::detectRelease(QMouseEvent *event)
{
    mousePressed = false;
    mouseMoving = false;
    mouseDragging = false;
    draggingPoint = false;
    coordText->setText("");
    rePlot();
}

void editProfile::on_saveButton_clicked()
{
    QSettings settings("tuxclocker");
    settings.beginGroup("General");
    QString currentProfile = settings.value("currentProfile").toString();
    QString latestUUID = settings.value("latestUUID").toString();
    settings.endGroup();
    settings.beginGroup(currentProfile);
    settings.beginGroup(latestUUID);
    QString xString;
    QString yString;
    settings.beginWriteArray("curvepoints");
    for (int i=0; i<qv_x.length(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("xpoints", ui->curvePlot->graph(0)->dataSortKey(i));
        settings.setValue("ypoints", ui->curvePlot->graph(0)->dataMainValue(i));
    }
    settings.endArray();
    close();
}

void editProfile::on_clearButton_clicked()
{
    qv_x.clear();
    qv_y.clear();
    ui->curvePlot->graph(0)->setData(qv_x, qv_y);
    drawFillerLines();
}

void editProfile::on_cancelButton_pressed()
{
    close();
}
