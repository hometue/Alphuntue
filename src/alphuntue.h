#ifndef ALPHUNTUE_H
#define ALPHUNTUE_H

#include <QMainWindow>

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

	void updateTotalTime(int hour, int minute, int second);

	void on_unmod_hour_valueChanged(int arg1);

	void on_unmod_minute_valueChanged(int arg1);

	void updateElapsedTime();

	void on_unmod_stop_clicked();

private:
    Ui::alphuntue *ui;
};

#endif // ALPHUNTUE_H
