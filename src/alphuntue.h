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
	void on_addCountry_clicked();

	void on_nextSpeaker_clicked();

	void on_time_min_valueChanged(int arg1);

	void on_time_s_valueChanged(int arg1);

	void updateTime();

private:
    Ui::alphuntue *ui;
};

#endif // ALPHUNTUE_H
