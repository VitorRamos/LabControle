#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "funcoes.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    pid(0,0,0),
    ui(new Ui::MainWindow)
{
    //quanser= new Quanser("10.13.99.69", 20081);
    quanser= new Quanser("127.0.0.1", 20074);

    fuc= "WAIT";
    tempo= 0;
    ui->setupUi(this);
    ui->comboBoxSinal->addItem("Degrau");
    ui->comboBoxSinal->addItem("Senoidal");
    ui->comboBoxSinal->addItem("Onda quadrada");
    ui->comboBoxSinal->addItem("Dente de serra");
    ui->comboBoxSinal->addItem("Aleatorio");

    ui->doubleSpinBox_kp->setDecimals(3);
    ui->doubleSpinBox_ki->setDecimals(3);
    ui->doubleSpinBox_kd->setDecimals(3);

    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->xAxis->setLabel("tempo(ms)");
    ui->customPlot->yAxis->setLabel("tensão(V)");
    ui->customPlot->yAxis->setRange(-7, 7);
    //ui->customPlot->setParent(ui->frame);


    ui->plotS1->addGraph();
    ui->plotS1->addGraph();    
    ui->plotS1->graph(0)->setPen(QPen(Qt::blue));
    ui->plotS1->graph(1)->setPen(QPen(Qt::red));
    ui->plotS1->xAxis->setLabel("tempo(ms)");
    ui->plotS1->yAxis->setLabel("altura(cm)");
    ui->plotS1->yAxis->setRange(-1, 31);
    ui->plotS1->legend->setVisible(true);
    ui->plotS1->legend->setFont(QFont("Helvetica", 9));
    ui->plotS1->legend->setRowSpacing(-5);
    ui->plotS1->graph(0)->setName("Sensor 1");
    ui->plotS1->graph(1)->setName("Set Point");
    ui->plotS1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);


    ui->plotS2->addGraph();
    ui->plotS2->addGraph();
    //ui->plotS2->setBackground(QBrush(Qt::green, Qt::SolidPattern));
    ui->plotS2->graph(0)->setPen(QPen(Qt::blue));
    ui->plotS2->graph(1)->setPen(QPen(Qt::red));
    ui->plotS2->xAxis->setLabel("tempo(ms)");
    ui->plotS2->yAxis->setLabel("altura(cm)");
    ui->plotS2->yAxis->setRange(-1, 31);
    ui->plotS2->legend->setVisible(true);
    ui->plotS2->legend->setFont(QFont("Helvetica", 9));
    ui->plotS2->legend->setRowSpacing(-3);
    ui->plotS2->graph(0)->setName("Sensor 2");
    ui->plotS2->graph(1)->setName("Set Point");
    ui->plotS2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

//    ui->plotS3->addGraph();
//    ui->plotS3->addGraph();
//    ui->plotS3->graph(0)->setPen(QPen(Qt::blue));
//    ui->plotS3->graph(1)->setPen(QPen(Qt::red));
//    ui->plotS3->xAxis->setLabel("x");
//    ui->plotS3->yAxis->setLabel("y");
//    ui->plotS3->yAxis->setRange(-1, 31);
//    ui->plotS3->legend->setVisible(true);
//    ui->plotS3->legend->setFont(QFont("Helvetica", 9));
//    ui->plotS3->legend->setRowSpacing(-3);
//    ui->plotS3->graph(0)->setName("Canal 4");
//    ui->plotS3->graph(1)->setName("Canal 6");
//    ui->plotS3->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

//    ui->plotS4->addGraph();
//    ui->plotS4->addGraph();
//    ui->plotS4->graph(0)->setPen(QPen(Qt::blue));
//    ui->plotS4->graph(1)->setPen(QPen(Qt::red));
//    ui->plotS4->xAxis->setLabel("x");
//    ui->plotS4->yAxis->setLabel("y");
//    ui->plotS4->yAxis->setRange(-1, 31);
//    ui->plotS4->legend->setVisible(true);
//    ui->plotS4->legend->setFont(QFont("Helvetica", 9));
//    ui->plotS4->legend->setRowSpacing(-3);
//    ui->plotS4->graph(0)->setName("Canal 5");
//    ui->plotS4->graph(1)->setName("Canal 7");
//    ui->plotS4->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    controle= new std::thread(&MainWindow::Controle, this);
    recebe= new std::thread(&MainWindow::Recebe, this);
    for(int i=0; i<8; i++)
        canais[i]= false;

    startTimer(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioButtonMalhaAberta_clicked()
{
    if(ui->radioButtonMalhaAberta->isChecked())
    {
        ui->labelTensaoNivel->setText("Tensão");
        ui->spinBoxCanal->setEnabled(true);
        ui->comboBoxSinal->setEnabled(true);
        ui->SpinBoxTensaoNivel->setMaximum(4);
        on_comboBoxSinal_activated(QString());

        ui->comboBoxTipodeControle->setDisabled(true);
        ui->doubleSpinBox_kp->setDisabled(true);
        ui->doubleSpinBox_ki->setDisabled(true);
        ui->doubleSpinBox_kd->setDisabled(true);
        ui->radioButtonGanho->setDisabled(true);
        ui->radioButtonTempo->setDisabled(true);
    }
}

void MainWindow::on_radioButtonMalhaFechada_clicked()
{
    if(ui->radioButtonMalhaFechada->isChecked())
    {
        ui->labelTensaoNivel->setText("Nivel");
        ui->spinBoxCanal->setEnabled(true);
        ui->comboBoxSinal->setEnabled(true);
        ui->SpinBoxTensaoNivel->setMaximum(30);
        on_comboBoxSinal_activated(QString());


        ui->comboBoxTipodeControle->setEnabled(true);
        ui->doubleSpinBox_kp->setEnabled(true);
        ui->doubleSpinBox_ki->setEnabled(true);
        ui->doubleSpinBox_kd->setEnabled(true);
        ui->radioButtonGanho->setEnabled(true);
        ui->radioButtonTempo->setEnabled(true);
    }
}

void MainWindow::on_comboBoxSinal_activated(const QString &arg1)
{
    QString sinal = ui->comboBoxSinal->currentText();

    if(sinal == "Degrau")
    {
        ui->SpinBoxTensaoNivel->setEnabled(true);
        ui->SpinBoxPeriodo->setEnabled(false);
    }
    else if(sinal == "Senoidal")
    {
        ui->SpinBoxTensaoNivel->setEnabled(true);
        ui->SpinBoxPeriodo->setEnabled(true);
    }
    else if(sinal == "Onda quadrada")
    {
        ui->SpinBoxTensaoNivel->setEnabled(true);
        ui->SpinBoxPeriodo->setEnabled(true);
    }
    else if(sinal == "Dente de serra")
    {
        ui->SpinBoxTensaoNivel->setEnabled(true);
        ui->SpinBoxPeriodo->setEnabled(true);
    }
    else if(sinal == "Aleatorio")
    {
        ui->SpinBoxTensaoNivel->setEnabled(false);
        ui->SpinBoxPeriodo->setEnabled(false);
    }
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    ui->customPlot->replot();
    ui->customPlot->graph(0)->removeDataBefore(tempo-12);
    ui->customPlot->xAxis->setRange(tempo + 0.25, 10, Qt::AlignRight);

    ui->plotS1->replot();
    ui->plotS1->graph(0)->removeDataBefore(tempo-12);
    ui->plotS1->xAxis->setRange(tempo + 0.25, 10, Qt::AlignRight);
    //ui->plotS1->replot();
    //ui->plotS1->graph(1)->removeDataBefore(tempo-12);
    //ui->plotS1->xAxis->setRange(tempo + 0.25, 10, Qt::AlignRight);

    ui->plotS2->replot();
    ui->plotS2->graph(0)->removeDataBefore(tempo-12);
    ui->plotS2->xAxis->setRange(tempo + 0.25, 10, Qt::AlignRight);

//    ui->plotS3->replot();
//    ui->plotS3->graph(0)->removeDataBefore(tempo-12);
//    ui->plotS3->xAxis->setRange(tempo + 0.25, 10, Qt::AlignRight);

//    ui->plotS4->replot();
//    ui->plotS4->graph(0)->removeDataBefore(tempo-12);
//    ui->plotS4->xAxis->setRange(tempo + 0.25, 10, Qt::AlignRight);

    if(canais[0])
        ui->plotS1->graph(0)->setVisible(1);
    else
        ui->plotS1->graph(0)->setVisible(0);

    if(canais[1])
        ui->plotS2->graph(0)->setVisible(1);
    else
        ui->plotS2->graph(0)->setVisible(0);

    if(canais[0])
        ui->plotS1->graph(1)->setVisible(1);
    else
        ui->plotS1->graph(1)->setVisible(0);

    if(canais[1])
        ui->plotS2->graph(1)->setVisible(1);
    else
        ui->plotS2->graph(1)->setVisible(0);

//    if(canais[4])
//        ui->plotS3->graph(0)->setVisible(1);
//    else
//        ui->plotS3->graph(0)->setVisible(0);

//    if(canais[5])
//        ui->plotS4->graph(0)->setVisible(1);
//    else
//        ui->plotS4->graph(0)->setVisible(0);

//    if(canais[6])
//        ui->plotS3->graph(1)->setVisible(1);
//    else
//        ui->plotS3->graph(1)->setVisible(0);

//    if(canais[7])
//        ui->plotS4->graph(1)->setVisible(1);
//    else
//        ui->plotS4->graph(1)->setVisible(0);


}

#include <chrono>
auto now = std::chrono::high_resolution_clock::now();

void MainWindow::Controle()
{
    double valCalculado;
    while(1)
    {
        int canal= ui->spinBoxCanal->value();

        double sensores[8];
        for(int i=0; i<8; i++)
            sensores[i]= quanser->readAD(i);

        double val=0, inAlt, erro, outAlt;

        if(fuc == "Degrau")
        {
            val= funcDegrau(A, tempo);
        }
        else if(fuc == "Senoidal")
        {
            val= funcSenoidal(A, T, tempo);
        }
        else if(fuc == "Onda quadrada")
        {
            val= funcQuadrada(A, T, tempo);
        }
        else if(fuc == "Dente de serra")
        {
            val= funcSerra(A, T, tempo);
        }
        else if(fuc == "Aleatorio") // intervalo
        {
            if(ui->radioButtonMalhaAberta->isChecked())
                val= funcAleatoria1(tempo);
            else
                val= funcAleatoria2(tempo);
        }
        else
        {
            val= 0;
        }

        ui->plotS1->graph(0)->addData(tempo, funcSensor(sensores[0]));
        ui->plotS1->graph(1)->addData(tempo, ui->SpinBoxTensaoNivel->value());
        ui->plotS2->graph(0)->addData(tempo, funcSensor(sensores[1]));
        ui->plotS2->graph(1)->addData(tempo, 30);

//        ui->plotS3->graph(0)->addData(tempo, sensores[4]);
//        ui->plotS3->graph(1)->addData(tempo, sensores[6]);
//        ui->plotS4->graph(0)->addData(tempo, sensores[5]);
//        ui->plotS4->graph(1)->addData(tempo, sensores[7]);

        if(ui->radioButtonMalhaAberta->isChecked())
        {
            outAlt = funcSensor(sensores[0]);
            //Trava #1
            valCalculado = val;
            if(val>3.9)
                val = 4;
            if(val<-3.9)
                val = -4;
            //Trava #2 e #3
            if(outAlt <= 1.5 && val < 0){
                val = 0;
            }else if(outAlt >= 30 && val > 0){
                val = 2.75; //tensao de equilibrio
            }
            quanser->writeDA(canal, val);
        }
        else if(ui->radioButtonMalhaFechada->isChecked())
        {

            // Calculo malha fechada e controle
            inAlt = val;
            //loop para todos as portas?
            outAlt = funcSensor(sensores[0]);
            erro = inAlt - outAlt;
            //val = funcAlturaTensao(inAlt)+erro;
            val = pid.Controle(erro, 0.1);
            valCalculado = val;

            //Trava #1
            if(val > 3.9)
                val = 4;
            if(val<-3.9)
                val = -4;
            //Trava #2 e #3
            if(outAlt <= 1.5 && val < 0){
                val = 0;
            }else if(outAlt >= 30 && val > 0){
                val = 2.75; //tensao de equilibrio
            }

            quanser->writeDA(canal, val);
        }
        ui->customPlot->graph(0)->addData(tempo, val);
        ui->customPlot->graph(1)->addData(tempo, valCalculado);

        //qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-now).count() << "ms\n";
        double t= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-now).count()/1000.0;
        qDebug() << valCalculado << endl;
        tempo+=t;
        now = std::chrono::high_resolution_clock::now();
        //tempo+=0.1;

        usleep(1*10E4);
    }
}

void MainWindow::Recebe()
{
    while(0)
    {
       usleep(10E4);
    }
}

void MainWindow::on_checkBox_clicked()
{
    canais[0] = ui->checkBox->isChecked();
}

void MainWindow::on_checkBox_2_clicked()
{
    canais[1] = ui->checkBox_2->isChecked();
}

void MainWindow::on_checkBox_3_clicked()
{
    canais[2] = ui->checkBox_3->isChecked();
}
void MainWindow::on_checkBox_4_clicked()
{
    canais[3] = ui->checkBox_4->isChecked();
}

void MainWindow::on_checkBox_5_clicked()
{
    canais[4] = ui->checkBox_5->isChecked();
}

void MainWindow::on_checkBox_6_clicked()
{
    canais[5] = ui->checkBox_6->isChecked();
}

void MainWindow::on_checkBox_7_clicked()
{
    canais[6] = ui->checkBox_7->isChecked();
}

void MainWindow::on_checkBox_8_clicked()
{
    canais[7] = ui->checkBox_8->isChecked();
}

void MainWindow::on_pushButtonEnviar_clicked()
{
    fuc= ui->comboBoxSinal->currentText().toStdString();
    A = ui->SpinBoxTensaoNivel->value();
    T = ui->SpinBoxPeriodo->value();

    if(ui->radioButtonGanho->isChecked()){
        pid.setConstantes(ui->doubleSpinBox_kp->value(), ui->doubleSpinBox_ki->value(), ui->doubleSpinBox_kd->value());
    }else if(ui->radioButtonTempo->isChecked()){
        pid.setConstantesT(ui->doubleSpinBox_kp->value(), ui->doubleSpinBox_ki->value(), ui->doubleSpinBox_kd->value());
    }


}

void MainWindow::on_pushButtonCancel_clicked()
{
    fuc= "";
    A = 0;
    T = 0;
}

void MainWindow::on_radioButtonGanho_clicked(bool checked)
{
    if(checked==true){
        ui->label_kd->setText("Ganho (Kd)");
        ui->label_ki->setText("Ganho (Ki)");
    }
}

void MainWindow::on_radioButtonTempo_clicked(bool checked)
{
    if(checked==true){
        ui->label_kd->setText("Tempo (Td)");
        ui->label_ki->setText("Tempo (Ti)");
    }
}

void MainWindow::on_comboBoxTipodeControle_activated(const QString &arg1)
{
    QString tipo = ui->comboBoxTipodeControle->currentText();

    if(tipo == "P"){

        ui->doubleSpinBox_ki->setEnabled(false);
        ui->doubleSpinBox_kd->setEnabled(false);

    }else if(tipo == "PI"){

        ui->doubleSpinBox_ki->setEnabled(true);
        ui->doubleSpinBox_kd->setEnabled(false);

    }else if(tipo == "PD"){

        ui->doubleSpinBox_ki->setEnabled(false);
        ui->doubleSpinBox_kd->setEnabled(true);

    }else if(tipo == "PID"){
        ui->doubleSpinBox_ki->setEnabled(true);
        ui->doubleSpinBox_kd->setEnabled(true);

    }else if(tipo == "PI-D"){

        ui->doubleSpinBox_ki->setEnabled(true);
        ui->doubleSpinBox_kd->setEnabled(true);

    }
}
