#include "example1LogModel.hpp"

#include <QDebug>

#include <sstream>

#include "exampDefine.hpp"

//---------------------------------------------------
TExample1LogModel::TExample1LogModel(QObject *parent): QAbstractTableModel (parent)
{

}
//---------------------------------------------------
int TExample1LogModel::rowCount(const QModelIndex &parent) const
{
    return this -> size() ;
}
//---------------------------------------------------
int TExample1LogModel::columnCount(const QModelIndex &parent) const
{
    return columnNumber::cnCount ;
}
//---------------------------------------------------
QVariant TExample1LogModel::data(const QModelIndex &index, int role) const
{
    QVariant retVal = QVariant () ;
    if (index.isValid()) {

        switch (role) {
          case Qt::DisplayRole : {
            exampleDefine::example1LogData logData = this -> at(index.row()) ;  // Получаем запись в контейнере
            switch (index.column()) {                                           // Получаем значение для нужной колонки
              case cnNumLine :
                retVal = logData.numLine ;
              break ;

              case cnTime :
                retVal = logData.time.toString("hh:mm:ss") ;
              break ;

              case cnThreadId : {
                std::stringstream threadId ;
                threadId << logData.threadId ;
                retVal = QString::fromStdString(threadId.str ()) ;
              }
              break ;

              case cnNumeric :
                if (logData.numeric == 0) retVal = "" ;
                  else retVal = logData.numeric ;
              break ;

              case cnReasonExit :
                retVal = logData.reasonExit ;
              break ;

              default :
              break ;
            }
          }
          break ;

          default :
          break ;
        }
    }

    return retVal ;
}
//---------------------------------------------------
QVariant TExample1LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant retVal = QVariant () ;
    if (role != Qt::DisplayRole) return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section) {
          case cnNumLine :
            retVal = "" ;
          break ;

          case cnTime :
            retVal = exampleDefine::example1HeaderTime ;
          break ;

          case cnThreadId :
            retVal = exampleDefine::example1HeaderThreadId ;
          break ;

          case cnNumeric :
            retVal = exampleDefine::example1HeaderNumeric ;
          break ;

          case cnReasonExit :
            retVal = exampleDefine::example1HeaderReasonExit ;
          break ;

          default :
          break ;
    }

    return retVal ;
}
//---------------------------------------------------
/*!
 * \brief TExample1LogModel::slotClearModel Слот очищающий модель
 */
void TExample1LogModel::slotClearModel ()
{
    beginRemoveRows (QModelIndex(), 0, this ->size() - 1);
    std::vector <exampleDefine::example1LogData>::clear() ;
    endRemoveRows();
}
//---------------------------------------------------
/*!
 * \brief TExample1LogModel::push_back  Добавляем в модель запись о сгенерированном числе.
 * \param inItem    Добавляемая запись
 *
 * Обновлять данные в этот момент нельзя, т.к. при очистке контейнера софт может обновлять отображение данных и т.о. будет падать.
 */
void TExample1LogModel::push_back (exampleDefine::example1LogData inItem)
{
    std::thread::id threadId = std::this_thread::get_id() ;                 // трассировка работы потоков на добавление
    std::hash <std::thread::id> threadHash ;
    qDebug () << "Добавление в лог (блокировка): " << threadHash (threadId) ;
    {
        std::unique_lock <std::mutex> blockPushBack (exampleDefine::muRefresh) ;// блокируем допуск остальным потокaм на работу с контейнером
        exampleDefine::cvRefresh.wait(blockPushBack);

        inItem.numLine = exampleDefine::logNumLine.fetch_add (1) + 1 ;
        if ((inItem.numLine  % exampleDefine::logLineMaxCount) == 0) slotClearModel () ;

        std::vector <exampleDefine::example1LogData>::push_back(inItem);
    }
    qDebug () << "Добавление в лог (освобождение): " << threadHash (threadId) ; // разблокируем один из ожидающих потоков
    exampleDefine::cvRefresh.notify_one();
}
//---------------------------------------------------
/*!
 * \brief TExample1LogModel::refreshView Обновление отображения данных
 */
void TExample1LogModel::refreshView ()
{
    beginResetModel();
    endResetModel();
    qDebug () << "Обновление лога (освобождение): " ;
    exampleDefine::cvRefresh.notify_one () ;      // разблокируем один из ожидающих потоков
}
//---------------------------------------------------
