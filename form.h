#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "ros2manager.h"
#include <QMenu>
#include <QStandardItemModel>
#include <QWheelEvent>
namespace Ui {
class Form;
}
class Form : public QWidget
{
    Q_OBJECT
protected:
    void wheelEvent(QWheelEvent *event);
public:
    explicit Form(workspace ws, QWidget *parent = nullptr);
    void show_Details(workspace ws,std::string choose_type,int pkg,int nl=-1);
    ~Form();
public slots:
    void slotMenuPopup(const QPoint &pos);
private slots:
    void on_treeView_clicked(const QModelIndex &index);
private:
    workspace form_ws;
    Ui::Form *ui;
    QMenu *menu;
    QStandardItemModel *model;
    std::string choose_type;
    int zoomLevel = 0;
    QFont baseFont;

};

#endif // FORM_H
