#ifndef ALPHUNTUE_H
#define ALPHUNTUE_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class alphuntue;
}

class alphuntue : public QMainWindow
{
    Q_OBJECT

public:
    explicit alphuntue(QWidget *parent = 0);
    ~alphuntue();

private slots:
	void on_GSL_addCountry_clicked();

	void on_GSL_nextSpeaker_clicked();

	void on_GSL_time_min_valueChanged(int arg1);

	void on_GSL_time_s_valueChanged(int arg1);

	void GSL_updateTime();

	void on_unmod_second_valueChanged(int arg1);

	void on_unmod_start_clicked();

	void unmod_updateTime();

	void unmod_updateTotalTime(int hour, int minute, int second);

	void on_unmod_hour_valueChanged(int arg1);

	void on_unmod_minute_valueChanged(int arg1);

	void updateElapsedTime();

	void on_unmod_stop_clicked();

	void on_actionCountries_present_triggered();

	void updatePresent(QListWidgetItem *item);

	void updateLists();

	void on_GSL_stop_clicked();

	void GSL_stop();

	void GSL_pause(int time);

	void GSL_resume();

	void on_GSL_pause_clicked();

	void on_GSL_resume_clicked();

	void on_GSL_yield_clicked();

	void on_mod_ind_min_valueChanged(int arg1);

	void mod_updateIndTotalTime(int minute, int second);

	void mod_updatetotalTotalTime(int hour, int minute, int second);

	void on_mod_ind_s_valueChanged(int arg1);

	void on_mod_addCountry_clicked();

	void on_mod_start_clicked();

	void mod_ind_timerUpdate();

	void mod_total_timerUpdate();

	void mod_stopped();

	void mod_speakingOver();

	void on_mod_clear_clicked();

	void on_mod_remove_clicked();

	void on_mod_total_min_valueChanged(int arg1);

	void on_mod_total_h_valueChanged(int arg1);

	void on_mod_total_s_valueChanged(int arg1);

	void on_mod_nextSpeaker_clicked();

	void on_mod_stop_clicked();

	void on_mod_pause_clicked();

	void on_mod_resume_clicked();

private:
    Ui::alphuntue *ui;
};

#endif // ALPHUNTUE_H
