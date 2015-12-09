#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCoreApplication>
#include <QDir>
#include <QPixmap>
#include <QFileInfo>
#include <QTimer>
#include <QtPlugin>

#include "alphuntue.h"
#include "ui_alphuntue.h"

QString countries;
unsigned int speakingTime=60, elapsed=0;
QTimer *GSLtimer=new QTimer();

alphuntue::alphuntue(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::alphuntue)
{
	ui->setupUi(this);
	ui->speakingCountry->clear();
	ui->countryImage->clear();
	QString fileloc=QCoreApplication::applicationDirPath();
	fileloc+="/country.txt";
	QFile file(fileloc);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, tr("Error"), tr("country.txt not found in the same directory as application."));
		return;
	}
	QTextStream in(&file);
	QString countrytemp=in.readLine();
	while(countrytemp!=0){
		ui->GSLcountry->addItem(countrytemp);
		countrytemp=in.readLine();
	}
	connect(GSLtimer, SIGNAL(timeout()), this, SLOT(updateTime()));
}

alphuntue::~alphuntue()
{
	delete ui;
}

void alphuntue::on_addCountry_clicked()
{
	int current=ui->GSLcountry->currentIndex();
	if(current!=-1){
		QString countryname=ui->GSLcountry->currentText();
		new QListWidgetItem(tr(countryname.toStdString().c_str()), ui->GSL);
	}
}

void alphuntue::updateTime(){
	if(elapsed<speakingTime){
		elapsed++;
	}
	else{
		GSLtimer->stop();
	}
	int second=elapsed%60;
	int minute=elapsed/60;
	QString label="";
	if(minute<10){
		label=('0');
	}
	label.append(QString::number(minute));
	label.append(':');
	if(second<10){
		label.append('0');
	}
	label.append(QString::number(second));
	ui->elapsedTime->setText(label);
	ui->timeBar->setValue(elapsed);
}

void alphuntue::on_nextSpeaker_clicked()
{
	QListWidgetItem *countryWidget=ui->GSL->takeItem(0);
	if(countryWidget!=0){
		QString countryName=countryWidget->text();
		ui->timeBar->setMaximum(speakingTime);
		ui->timeBar->setValue(0);
		elapsed=0;
		ui->timeBar->setValue(0);
		GSLtimer->start(1000);
		QString imgloc=QCoreApplication::applicationDirPath();
		imgloc.append("/img/");
		imgloc+=countryName;
		imgloc.append(".png");
		QFileInfo checkFile(imgloc);
		if(checkFile.isFile()){
			QPixmap image(imgloc);
			int lwidth=ui->countryImage->width();
			int lheight=ui->countryImage->height();
			image=image.scaled(lwidth,lheight, Qt::KeepAspectRatio, Qt::FastTransformation);
			ui->countryImage->setPixmap(image);
		}
		ui->speakingCountry->setText(countryName);
	}
}

void alphuntue::on_time_min_valueChanged(int arg1)
{
	QString label="";
	int second=ui->time_s->value();
	if(arg1<10){
		label='0';
	}
	label.append(QString::number(arg1));
	label.append(':');
	if(second<10){
		label.append('0');
	}
	label.append(QString::number(second));
	ui->totalTime->setText(label);
	speakingTime=second+arg1*60;
}

void alphuntue::on_time_s_valueChanged(int arg1)
{
	int minute=ui->time_min->value();
	QString label="";
	if(minute<10){
		label='0';
	}
	label.append(QString::number(minute));
	label.append(':');
	if(arg1<10){
		label.append('0');
	}
	label.append(QString::number(arg1));
	ui->totalTime->setText(label);
	speakingTime=minute*60+arg1;
}
