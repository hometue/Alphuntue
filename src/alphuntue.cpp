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
unsigned int GSL_speakingTime=60, GSL_elapsed=0, unmod_totaltime=0, unmod_elapsed=0, mod_totaltime=600;
unsigned int mod_speakingtime_elapsed=0, mod_totaltime_elapsed=0, mod_speakingtime=60;
int GSL_remainingTimer, mod_ind_remainingTimer, mod_total_remainingTimer;
QTimer *GSLtimer=new QTimer(), *unmodtimer=new QTimer(), *mod_indtimer=new QTimer(), *mod_totaltimer=new QTimer();
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
	ui->mod_topic_label->clear();
	ui->mod_countryimg_label->clear();
	ui->mod_countryName_label->clear();
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
		ui->mod_modcountry->addItem(allcountries.at(i).toLocal8Bit().constData());
		i++;
	}
	//disable all things that need to be disabled
	ui->unmod_stop->setEnabled(false);
	ui->GSL_resume->setEnabled(false);
	ui->mod_stop->setEnabled(false);
	ui->mod_pause->setEnabled(false);
	ui->mod_nextSpeaker->setEnabled(false);
	ui->mod_resume->setEnabled(false);
	//initialize timers
	connect(GSLtimer, SIGNAL(timeout()), this, SLOT(GSL_updateTime()));
	connect(unmodtimer, SIGNAL(timeout()), this, SLOT(unmod_updateTime()));
	connect(mod_indtimer, SIGNAL(timeout()), this, SLOT(mod_ind_timerUpdate()));
	connect(mod_totaltimer, SIGNAL(timeout()), this, SLOT(mod_total_timerUpdate()));
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
	if(GSLtimer->interval()!=1000){
		GSLtimer->setInterval(1000);
	}
	if(GSL_elapsed<GSL_speakingTime){
		GSL_elapsed++;
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
	else{
		GSL_stop();
	}
}

void alphuntue::on_GSL_nextSpeaker_clicked()
{
	if(GSL_speakingTime==0){
		QMessageBox::critical(this, "Error", "Speaking time cannot be set to 0");
		return;
	}
	else{
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
			delete countryWidget;
			ui->GSL_time_min->setEnabled(false);
			ui->GSL_time_s->setEnabled(false);
			GSLtimer->start(1000);
		}
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
		unmod_updateTotalTime(ui->unmod_hour->value(), ui->unmod_minute->value(), ui->unmod_second->value());
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
	QPushButton *button=new QPushButton("Ok", this);
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
	connect(listWidget, &QListWidget::itemChanged, this, &alphuntue::updatePresent);
	connect(button, &QPushButton::clicked, &selectPresent, &QDialog::accept);
	layout->addWidget(text);
	layout->addWidget(listWidget);
	layout->addWidget(button);
	selectPresent.setLayout(layout);
	selectPresent.exec();
	updateLists();
}

void alphuntue::updatePresent(QListWidgetItem *item){
	Qt::CheckState checked=item->checkState();
	bool check;
	if(checked==0){
		check=false;
	}
	else{
		check=true;
	}
	QString countrystr=item->text();
	int i=0;
	bool done=false;
	while(i<allcountries.size() && done==false){
		if(allcountries[i]==countrystr){
			present[i]=check;
			done=true;
		}
		i++;
	}
}

void alphuntue::updateLists(){
	int i=0;
	ui->GSL_GSLcountry->clear();
	ui->mod_modcountry->clear();
	while(i<allcountries.size()){
		if(present[i]==true){
			ui->GSL_GSLcountry->addItem(allcountries.at(i).toLocal8Bit().constData());
			ui->mod_modcountry->addItem(allcountries.at(i).toLocal8Bit().constData());
		}
		i++;
	}
}

void alphuntue::on_GSL_stop_clicked()
{
	GSL_stop();
}

void alphuntue::GSL_stop(){
	GSLtimer->stop();
	ui->GSL_elapsedTime->setText("00:00");
	ui->GSL_timeBar->setValue(0);
	ui->GSL_speakingCountry->clear();
	ui->GSL_countryImage->clear();
	ui->GSL_time_min->setEnabled(true);
	ui->GSL_time_s->setEnabled(true);
	ui->GSL_resume->setEnabled(false);
	ui->GSL_pause->setEnabled(true);
}

void alphuntue::on_GSL_pause_clicked()
{
	if(GSLtimer->remainingTime()==-1){
		return;
	}
	GSL_pause(GSLtimer->remainingTime());
	ui->GSL_resume->setEnabled(true);
	ui->GSL_pause->setEnabled(false);
}

void alphuntue::GSL_pause(int time){
	GSLtimer->stop();
	GSL_remainingTimer=time;
}

void alphuntue::GSL_resume(){
	GSLtimer->start(GSL_remainingTimer);
}

void alphuntue::on_GSL_resume_clicked()
{
	GSL_resume();
	ui->GSL_resume->setEnabled(false);
	ui->GSL_pause->setEnabled(true);
}

void alphuntue::on_GSL_yield_clicked()
{
	if(GSLtimer->remainingTime()==-1){
		return;
	}
	GSL_pause(GSLtimer->remainingTime());
	//add code to change current speaking country
	GSL_resume();
}

void alphuntue::on_mod_ind_min_valueChanged(int arg1)
{
	int second=ui->mod_ind_s->value();
	int minute=arg1;
	mod_speakingtime=minute*60+second;
	mod_updateIndTotalTime(minute, second);
}

void alphuntue::mod_updateIndTotalTime(int minute, int second){
	QString total="";
	if(minute<10){
		total='0';
	}
	total.append(QString::number(minute));
	total.append(':');
	if(second<10){
		total.append('0');
	}
	total.append(QString::number(second));
	ui->mod_ind_totalTime_label->setText(total);
}

void alphuntue::on_mod_ind_s_valueChanged(int arg1)
{
	int second=arg1;
	int minute=ui->mod_ind_min->value();
	mod_speakingtime=minute*60+second;
	mod_updateIndTotalTime(minute, second);
}

void alphuntue::on_mod_addCountry_clicked()
{
	if(ui->mod_modcountry->currentIndex()!=-1){
		QString countryname=ui->mod_modcountry->currentText();
		new QListWidgetItem(countryname.toStdString().c_str(), ui->mod_countrylist);
	}
}

void alphuntue::on_mod_start_clicked()
{
	QString title=ui->mod_topic->text();
	if(title==0){
		QMessageBox msgBox;
		msgBox.setText("Empty topic!");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}
	if(mod_speakingtime==0){
		QMessageBox msgBox;
		msgBox.setText("Speaking time cannot be 0!");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}
	if(mod_totaltime==0){
		QMessageBox msgBox;
		msgBox.setText("Duration cannot be 0!");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}
	ui->mod_topic_label->setText(title);
	ui->mod_ind_progressBar->setMaximum(mod_speakingtime);
	ui->mod_total_progressBar->setMaximum(mod_totaltime);
	ui->mod_topic->setEnabled(false);
	ui->mod_start->setEnabled(false);
	ui->mod_stop->setEnabled(true);
	ui->mod_pause->setEnabled(true);
	ui->mod_nextSpeaker->setEnabled(true);
	ui->mod_ind_min->setEnabled(false);
	ui->mod_ind_s->setEnabled(false);
	ui->mod_total_h->setEnabled(false);
	ui->mod_total_min->setEnabled(false);
	ui->mod_total_s->setEnabled(false);
	mod_totaltimer->start(1000);
}


void alphuntue::mod_ind_timerUpdate(){
	if(mod_indtimer->interval()!=1000){
		mod_indtimer->setInterval(1000);
	}
	if(mod_speakingtime_elapsed<mod_speakingtime){
		mod_speakingtime_elapsed++;
		unsigned int second=mod_speakingtime_elapsed%60;
		int minute=mod_speakingtime_elapsed/60;
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
		ui->mod_ind_elapsed_label->setText(label);
		ui->mod_ind_progressBar->setValue(mod_speakingtime_elapsed);
	}
	else{
		mod_speakingOver();
	}
}

void alphuntue::mod_speakingOver(){
	mod_indtimer->stop();
	ui->mod_countryName_label->clear();
	ui->mod_countryimg_label->clear();
	ui->mod_ind_elapsed_label->setText("00:00");
	ui->mod_ind_progressBar->setValue(0);
	mod_speakingtime_elapsed=0;
}

void alphuntue::mod_total_timerUpdate(){
	if(mod_totaltimer->interval()!=1000){
		mod_totaltimer->setInterval(1000);
	}
	if(mod_totaltime_elapsed<mod_totaltime){
		mod_totaltime_elapsed++;
		unsigned int second=mod_totaltime_elapsed%60;
		int minute=((mod_totaltime_elapsed%3600)-second)/60;
		int hour=mod_totaltime_elapsed/3600;
		QString label="";
		if(hour<10){
			label=('0');
		}
		label.append(QString::number(hour));
		label.append(':');
		if(minute<10){
			label.append('0');
		}
		label.append(QString::number(minute));
		label.append(':');
		if(second<10){
			label.append('0');
		}
		label.append(QString::number(second));
		ui->mod_total_elapsed_label->setText(label);
		ui->mod_total_progressBar->setValue(mod_totaltime_elapsed);
	}
	else{
		mod_stopped();
		QMessageBox msgBox;
		msgBox.setText("Time for the moderated caucus has elapsed");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}

void alphuntue::mod_stopped(){
	mod_indtimer->stop();
	mod_totaltimer->stop();
	ui->mod_topic_label->clear();
	ui->mod_countryimg_label->clear();
	ui->mod_countryName_label->clear();
	ui->mod_ind_elapsed_label->setText("00:00");
	ui->mod_total_elapsed_label->setText("00:00:00");
	mod_totaltime_elapsed=0;
	mod_speakingtime_elapsed=0;
	ui->mod_ind_progressBar->setValue(0);
	ui->mod_total_progressBar->setValue(0);
}

void alphuntue::on_mod_clear_clicked()
{
	ui->mod_countrylist->clear();
}

void alphuntue::on_mod_remove_clicked()
{
	int row=ui->mod_countrylist->currentRow();
	if(row==-1){
		return;
	}
	QListWidgetItem * remove=ui->mod_countrylist->takeItem(row);
	delete remove;
}

void alphuntue::mod_updatetotalTotalTime(int hour, int minute, int second){
	QString label="";
	if(hour<10){
		label=('0');
	}
	label.append(QString::number(hour));
	label.append(':');
	if(minute<10){
		label.append('0');
	}
	label.append(QString::number(minute));
	label.append(':');
	if(second<10){
		label.append('0');
	}
	label.append(QString::number(second));
	ui->mod_total_totalTime_label->setText(label);
}

void alphuntue::on_mod_total_min_valueChanged(int arg1)
{
	int hour=ui->mod_total_h->value();
	int minute=arg1;
	int second=ui->mod_total_s->value();
	mod_totaltime=hour*3600+minute*60+second;
	mod_updatetotalTotalTime(hour, minute, second);
}

void alphuntue::on_mod_total_h_valueChanged(int arg1)
{
	int hour=arg1;
	int minute=ui->mod_total_min->value();
	int second=ui->mod_total_s->value();
	mod_totaltime=hour*3600+minute*60+second;
	mod_updatetotalTotalTime(hour, minute, second);
}

void alphuntue::on_mod_total_s_valueChanged(int arg1)
{
	int hour=ui->mod_total_h->value();
	int minute=ui->mod_total_min->value();
	int second=arg1;
	mod_totaltime=hour*3600+minute*60+second;
	mod_updatetotalTotalTime(hour, minute, second);
}

void alphuntue::on_mod_nextSpeaker_clicked()
{
	QListWidgetItem *countryWidget=ui->mod_countrylist->takeItem(0);
	if(countryWidget!=0){
		QString countryName=countryWidget->text();
		ui->mod_ind_progressBar->setValue(0);
		mod_speakingtime_elapsed=0;
		ui->mod_ind_elapsed_label->setText("00:00");
		QString imgloc=QCoreApplication::applicationDirPath();
		imgloc.append("/img/");
		imgloc+=countryName;
		imgloc.append(".png");
		if(QFileInfo(imgloc).exists()){
			QPixmap image(imgloc);
			image=image.scaled(ui->mod_countryimg_label->width(),ui->mod_countryimg_label->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
			ui->mod_countryimg_label->setPixmap(image);
		}
		ui->mod_countryName_label->setText(countryName);
		delete countryWidget;
		mod_indtimer->start(1000);
	}
}

void alphuntue::on_mod_stop_clicked()
{
	mod_stopped();
}

void alphuntue::on_mod_pause_clicked()
{
	if(mod_totaltimer->remainingTime()==-1){
		return;
	}
	mod_ind_remainingTimer=-1;
	if(mod_indtimer->remainingTime()!=-1){
		mod_ind_remainingTimer=mod_indtimer->remainingTime();
		mod_indtimer->stop();
	}
	mod_total_remainingTimer=mod_totaltimer->remainingTime();
	mod_totaltimer->stop();
	ui->mod_resume->setEnabled(true);
	ui->mod_pause->setEnabled(false);
}

void alphuntue::on_mod_resume_clicked()
{
	mod_totaltimer->start(mod_total_remainingTimer);
	if(mod_ind_remainingTimer!=-1){
		mod_indtimer->start(mod_ind_remainingTimer);
	}
	ui->mod_resume->setEnabled(false);
	ui->mod_pause->setEnabled(true);
}
