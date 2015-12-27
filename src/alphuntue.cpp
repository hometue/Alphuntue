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
#include <vector>

#include "alphuntue.h"
#include "ui_alphuntue.h"

QStringList allcountries;
unsigned int GSL_speakingTime=60, GSL_elapsed=0, unmod_totaltime=0, unmod_elapsed=0;
QTimer *GSLtimer=new QTimer(), *unmodtimer=new QTimer();
std::vector<bool> present;

alphuntue::alphuntue(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::alphuntue)
{
	//Qt stuff
	ui->setupUi(this);
	//Clear all labels
	ui->GSL_speakingCountry->clear();
	ui->GSL_countryImage->clear();
	ui->unmod_topic->clear();
	//load country.txt
	QString fileloc=QCoreApplication::applicationDirPath();
	fileloc+="/country.txt";
	QFile file(fileloc);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, "Error", "country.txt not found in the same directory as application.");
		return;
	}
	QTextStream in(&file);
	QString countrytemp=in.readLine();
	while(countrytemp!=0){
		allcountries.append(countrytemp);
		countrytemp=in.readLine();
	}
	present.resize(allcountries.size());
	fill(present.begin(), present.end(), true);
	int i=0;
	//Initialization of other stuff
	//TODO: Shift them based on the present.
	while(i<allcountries.size()){
		ui->GSL_GSLcountry->addItem(allcountries.at(i).toLocal8Bit().constData());
		i++;
	}
	//disable all things that need to be disabled
	ui->unmod_stop->setEnabled(false);
	//initialize timers
	connect(GSLtimer, SIGNAL(timeout()), this, SLOT(GSL_updateTime()));
	connect(unmodtimer, SIGNAL(timeout()), this, SLOT(unmod_updateTime()));
}

alphuntue::~alphuntue()
{
	delete ui;
}

void alphuntue::on_GSL_addCountry_clicked()
{
	if(ui->GSL_GSLcountry->currentIndex()!=-1){
		QString countryname=ui->GSL_GSLcountry->currentText();
		new QListWidgetItem(countryname.toStdString().c_str(), ui->GSL);
	}
}

void alphuntue::GSL_updateTime(){
	if(GSL_elapsed<GSL_speakingTime){
		GSL_elapsed++;
	}
	else{
		GSLtimer->stop();
	}
	unsigned int second=GSL_elapsed%60;
	int minute=GSL_elapsed/60;
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
	ui->GSL_elapsedTime->setText(label);
	ui->GSL_timeBar->setValue(GSL_elapsed);
}

void alphuntue::on_GSL_nextSpeaker_clicked()
{
	QListWidgetItem *countryWidget=ui->GSL->takeItem(0);
	if(countryWidget!=0){
		QString countryName=countryWidget->text();
		ui->GSL_timeBar->setMaximum(GSL_speakingTime);
		ui->GSL_timeBar->setValue(0);
		GSL_elapsed=0;
		ui->GSL_elapsedTime->setText("00:00");
		QString imgloc=QCoreApplication::applicationDirPath();
		imgloc.append("/img/");
		imgloc+=countryName;
		imgloc.append(".png");
		if(QFileInfo(imgloc).exists()){
			QPixmap image(imgloc);
			image=image.scaled(ui->GSL_countryImage->width(),ui->GSL_countryImage->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
			ui->GSL_countryImage->setPixmap(image);
		}
		ui->GSL_speakingCountry->setText(countryName);
		GSLtimer->start(1000);
	}
}

void alphuntue::on_GSL_time_min_valueChanged(int arg1)
{
	QString label="";
	int second=ui->GSL_time_s->value();
	if(arg1<10){
		label='0';
	}
	label.append(QString::number(arg1));
	label.append(':');
	if(second<10){
		label.append('0');
	}
	label.append(QString::number(second));
	ui->GSL_totalTime->setText(label);
	GSL_speakingTime=second+arg1*60;
}

void alphuntue::on_GSL_time_s_valueChanged(int arg1)
{
	int minute=ui->GSL_time_min->value();
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
	ui->GSL_totalTime->setText(label);
	GSL_speakingTime=minute*60+arg1;
}

void alphuntue::on_unmod_second_valueChanged(int arg1)
{
	int hour=ui->unmod_hour->value();
	int minute=ui->unmod_minute->value();
	unmod_updateTotalTime(hour, minute, arg1);
	unmod_totaltime=hour*3600+minute*60+arg1;
}

void alphuntue::unmod_updateTotalTime(int hour, int minute, int second){
		QString total="";
		if(hour<10){
			total="0";
		}
		total.append(QString::number(hour));
		total.append(':');
		if(minute<10){
			total.append('0');
		}
		total.append(QString::number(minute));
		total.append(':');
		if(second<10){
			total.append('0');
		}
		total.append(QString::number(second));
		ui->unmod_totalTime->setText(total);
}

void alphuntue::updateElapsedTime(){
	int second=unmod_elapsed%60;
	int minute=((unmod_elapsed%3600)-second)/60;
	int hour=unmod_elapsed/3600;
	QString total="";
	if(hour<10){
		total="0";
	}
	total.append(QString::number(hour));
	total.append(':');
	if(minute<10){
		total.append('0');
	}
	total.append(QString::number(minute));
	total.append(':');
	if(second<10){
		total.append('0');
	}
	total.append(QString::number(second));
	ui->unmod_elapsedTime->setText(total);
}

void alphuntue::on_unmod_start_clicked()
{
	QString topic=ui->unmod_topicInput->text();
	if(topic==0 || unmod_totaltime==0){
		QMessageBox msgBox;
		msgBox.setText("Empty topic or time!");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	else{
		ui->unmod_topic->setText(topic);
		unmod_elapsed=0;
		ui->unmod_hour->setEnabled(false);
		ui->unmod_minute->setEnabled(false);
		ui->unmod_second->setEnabled(false);
		ui->unmod_topicInput->setEnabled(false);
		ui->unmod_start->setEnabled(false);
		ui->unmod_stop->setEnabled(true);
		ui->unmod_elapsedTime->setText("00:00:00");
		ui->unmod_progressBar->setMaximum(unmod_totaltime);
		int second=ui->unmod_second->value();
		unmod_updateTotalTime(ui->unmod_hour->value(), ui->unmod_minute->value(), second=ui->unmod_second->value());
		unmodtimer->start(1000);
	}
}

void alphuntue::unmod_updateTime(){
	if(unmod_elapsed<unmod_totaltime){
		unmod_elapsed++;
		updateElapsedTime();
		ui->unmod_progressBar->setValue(unmod_elapsed);
	}
	else{
		unmodtimer->stop();
		QMessageBox msgBox;
		msgBox.setText("Time for the unmoderated caucus has elapsed");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		ui->unmod_hour->setEnabled(true);
		ui->unmod_minute->setEnabled(true);
		ui->unmod_second->setEnabled(true);
		ui->unmod_topicInput->setEnabled(true);
		ui->unmod_start->setEnabled(true);
		ui->unmod_stop->setEnabled(false);
		ui->unmod_elapsedTime->setText("00:00:00");
		ui->unmod_progressBar->setValue(0);
		ui->unmod_topic->clear();
	}
}

void alphuntue::on_unmod_hour_valueChanged(int arg1)
{
	int minute=ui->unmod_minute->value();
	int second=ui->unmod_second->value();
	unmod_updateTotalTime(arg1, minute, second);
	unmod_totaltime=arg1*3600+minute*60+second;
}

void alphuntue::on_unmod_minute_valueChanged(int arg1)
{
	int hour=ui->unmod_hour->value();
	int second=ui->unmod_second->value();
	unmod_updateTotalTime(hour, arg1, second);
	unmod_totaltime=hour*3600+arg1*60+second;
}

void alphuntue::on_unmod_stop_clicked()
{
	unmodtimer->stop();
	ui->unmod_hour->setEnabled(true);
	ui->unmod_minute->setEnabled(true);
	ui->unmod_second->setEnabled(true);
	ui->unmod_topicInput->setEnabled(true);
	ui->unmod_start->setEnabled(true);
	ui->unmod_stop->setEnabled(false);
	ui->unmod_elapsedTime->setText("00:00:00");
	ui->unmod_progressBar->setValue(0);
	ui->unmod_topic->clear();
}

void alphuntue::on_actionCountries_present_triggered()
{
	QDialog selectPresent;
	selectPresent.setModal(true);
	QLabel *text=new QLabel("Countries present");
	QListWidget *listWidget = new QListWidget(this);
	QVBoxLayout *layout = new QVBoxLayout();
	int i=0;
	QListWidgetItem* item;
	while(i<allcountries.size()){
		item=new QListWidgetItem(allcountries.at(i).toLocal8Bit(),listWidget);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		if(present[i]==true){
			item->setCheckState(Qt::Checked);
		}
		else{
			item->setCheckState(Qt::Unchecked);
		}
		i++;
	}
	layout->addWidget(text);
	layout->addWidget(listWidget);
	selectPresent.setLayout(layout);
	selectPresent.exec();
}
