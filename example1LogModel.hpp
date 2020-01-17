#ifndef EXAMPLE1LOGMODEL_HPP
#define EXAMPLE1LOGMODEL_HPP

#include <QAbstractTableModel>
#include <QString>

#include <vector>
#include <string>
#include <mutex>

#include "exampDefine.hpp"

/*!
 * \brief The TExample1LogModel class Модель представления данных для отображения лога работы Примера 1
 */
class TExample1LogModel : public QAbstractTableModel, public std::vector <exampleDefine::example1LogData>
{
public:
    TExample1LogModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent  = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void push_back (exampleDefine::example1LogData inItem) ;  // Добавление данных в конец списка
    void refreshView () ;           // Обновление отображения данных

private:
    enum columnNumber {cnNumLine = 0, cnTime, cnThreadId, cnNumeric, cnReasonExit, cnCount} ;  // Перечень колонок в таблице

public slots:
    void slotClearModel () ;         // Удаляем все данные из модели
};

#endif // EXAMPLE1LOGMODEL_HPP
