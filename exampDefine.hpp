#ifndef EXAMPDEFINE_HPP
#define EXAMPDEFINE_HPP

#include <QString>
#include <QTime>

#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

/*!
 * \brief Описание общих данных, констант и сообщений для Примера 1
 */
namespace exampleDefine {

    static std::atomic <qint32> totalIteration (0) ;    ///< Общее количество итераций
    static std::atomic <qint32> threadCount (0) ;       ///< Количество запущенных потоков
    static std::atomic <quint32> logNumLine (0) ;       ///< Количество строк в логе
    static std::atomic <bool> isStop (false) ;          ///< Признак завершения задания при нажатии на кнопку Стоп
    static std::mutex muRefresh ;                       ///< mutex для ожидания записи в модель отображения лога
    static std::condition_variable cvRefresh ;          ///< Условная переменная для синхронизации доступа к модели отображения лога. Возможность ложного срабатывания не учитывалась.

    const quint32 example11Timer {1000} ;               ///< Период срабатывания таймера в Пример 1
    const quint32 waitTimer {10000} ;                   ///< Период ожидания завершения всех потоков при нажатой кнопке Стоп
    const quint32 logLineMaxCount {100} ;               ///< Количество строк отображаемых в логе

    const QString btnStartText { "Старт" } ;    // Текст выводиный на кнопку Старт/Стоп
    const QString btnStopText { "Стоп" } ;      //
    const QString btnClearText { "Очистить" } ; // Текст выводиный на кнопку Очистить

    const QString example1HeaderTime { "Время" } ; // Заголовки таблицы ведения лога для "Пример 1"
    const QString example1HeaderThreadId { "Id потока" } ;
    const QString example1HeaderNumeric { "Число" } ;
    const QString example1HeaderReasonExit { "Причина завершения" } ;

    const QString reasonExitFailure {"Превышено количество итераций"} ; // Сообщения о причинах завершения потока
    const QString reasonExitEqual {"Получено совпадающее число"} ;
    const QString reasonExitStop {"Задание прекращено пользователем"} ;

    const QString messageTitle {"Сообщение"} ;  //  Сообщения о завершении всех потоков
    const QString messageText {"Все потоки завершились"} ;

            ///  Описание данных отображаемых в "Пример 1"
    struct example1LogData {
        quint32 numLine ;                       // Номер строки в таблице
        QTime time {QTime::currentTime()} ;     // Время генерации числа
        std::thread::id threadId {std::this_thread::get_id()} ; // ID потока
        quint32 numeric = {0} ;                 // Сгенерированное число
        QString reasonExit {""} ;               // Причина завершения потока
    } ;
    typedef std::shared_ptr <example1LogData> ptrExample1LogData ; // typedef для указателя на описание данных для вывода в лог
}
/*!
 * \brief Описание общих данных, констант и сообщений для Примера 2
 */
namespace example2Define {



            /// Структура передаваемых данных
    struct TTransferData {
        quint32 packageId ;     // идентификатор передаваемого пакета
        quint32 value ;         // передаваемое значение
    } ;
}

#endif // EXAMPDEFINE_HPP
