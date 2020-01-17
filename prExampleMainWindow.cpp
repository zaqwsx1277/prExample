#include "prExampleMainWindow.hpp"
#include "ui_prExampleMainWindow.h"

#include <cstdlib>
#include <ctime>
#include <thread>
#include <future>
#include <functional>
#include <sstream>

#include <iostream>

#include <QRegExpValidator>
#include <QRegExp>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

#include "exampDefine.hpp"

//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::prExampleMainWindow  Конструктор класса для Примера 1
 * \param parent
 */
prExampleMainWindow::prExampleMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::prExampleMainWindow)
{
    ui -> setupUi(this);

    initFirm () ;
    fPtrExample1Timer.reset( new QTimer ());            // Инициализируем таймер для проверки завершения всех потоков
    makeConnect () ;

    fState = prExampleMainWindow::stStop ;
    fGenRandom.seed(time (0)) ;                         // Инициализируем генератор ГСЧ

    emit signalClear () ;

    ui -> spTabExamples -> removeTab(pgExample2);       // Закрываем видимость не готовых тестовых примеров
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::~prExampleMainWindow Деструктор класса
 */
prExampleMainWindow::~prExampleMainWindow()
{
    delete ui;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::initFirm Инициализация формы
 */
void prExampleMainWindow::initFirm ()
{
    ui -> spTabExamples -> setCurrentIndex(pgExample1) ;

    ui -> spGbExample1 -> setStyleSheet("QGroupBox {border: 1px solid gray; border-radius: 3px; margin: 10px; padding: 4px; }"
                                        "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top left; padding: 4px; left: 20px; }");

    ui -> spRandomNumeric -> setValidator( new QRegExpValidator( QRegExp( "[0-9]{2}")));   // задаём все необходимые регулярные выражения
    ui -> spThreadCount -> setValidator( new QRegExpValidator( QRegExp( "[0-9]{2}")));
    ui -> spTotalIterations -> setValidator( new QRegExpValidator( QRegExp( "[0-9]{3}")));
    ui -> spGenerationFrequency -> setValidator( new QRegExpValidator( QRegExp( "[1-9]{3}")));

    ui -> btnStart -> setText(exampleDefine::btnStartText) ;
    ui -> btnStart -> setEnabled(false);
    ui -> btnClear -> setText(exampleDefine::btnClearText) ;

    QFile description (":/description/descriptions/testExample1.txt");        // Выводим описание примера
    description.open(QIODevice::ReadOnly);
    ui -> plainTextEdit -> setPlainText(description.readAll());
    description.close();

    fPrtExampleModel = std::make_shared <TExample1LogModel>  () ;
    ui -> spExample1Log -> setModel(fPrtExampleModel.get()) ;                             // Инициализируем таблицу для ведения лога
    for (quint8 i = 0; i < fPrtExampleModel -> columnCount (); i++) {
      ui-> spExample1Log -> horizontalHeader() -> setSectionResizeMode(i, QHeaderView::Stretch) ;
    }
    connect(ui -> spExample1Log-> model(), &QAbstractItemModel::rowsInserted, ui -> spExample1Log, &QTableView::scrollToBottom) ;    // Устанавливаем текущей последнюю строку
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::makeConnect  Формирует все нужные конекты
 */
void prExampleMainWindow::makeConnect ()
{
    connect (ui -> spRandomNumeric, &QLineEdit::textChanged, this, &prExampleMainWindow::on_example1ChechState); // Задаём проверку корректного заполнения всех данных
    connect (ui -> spThreadCount, &QLineEdit::textChanged, this, &prExampleMainWindow::on_example1ChechState);
    connect (ui -> spTotalIterations, &QLineEdit::textChanged, this, &prExampleMainWindow::on_example1ChechState);
    connect (ui -> spGenerationFrequency, &QLineEdit::textChanged, this, &prExampleMainWindow::on_example1ChechState);

    connect(this, &prExampleMainWindow::signalClear, this, &prExampleMainWindow::slotExample1Clear) ;                // Задаём очистку всех данных
    connect(this, &prExampleMainWindow::signalClear, fPrtExampleModel.get(), &TExample1LogModel::slotClearModel) ;

    connect(fPtrExample1Timer.get(), &QTimer::timeout, this, &prExampleMainWindow::slotExample1Timeout) ;            // Задаем срабатывние таймера для проверки завершенности всех потоков
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::example1clear    Слот для очистки формы
 */
void prExampleMainWindow::slotExample1Clear ()
{
    ui -> spRandomNumeric -> clear () ;                                                     // задаём начальные значения всех используемых полей
    ui -> spThreadCount -> setText (QString::number(std::thread::hardware_concurrency())) ; // Задаём начальное значение кол-ва потоков
    ui -> spTotalIterations -> clear () ;
    ui -> spGenerationFrequency -> clear () ;

    ui -> btnStart -> setEnabled(false) ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::getState Возвращает состояние выполнения проекта
 * \return Состояние проекта
 */
prExampleMainWindow::prState prExampleMainWindow::getState ()
{
    return fState ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::setState Устанавливает состояние проекта
 * \param inState   Устанавливаемое состояние
 */
void prExampleMainWindow::setState (prExampleMainWindow::prState inState)
{
    fState = inState ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::example1ChechState Проверяет возможность запуска примера на закладке "Пример 1"
 * \return Если все поля заполнены, то возвращается true, в противном случае false
 */
bool prExampleMainWindow::example1ChechState ()
{
    bool retValue = false ;
    if (!ui -> spRandomNumeric -> text().isEmpty() &&
        !ui -> spThreadCount -> text().isEmpty() &&
        !ui -> spTotalIterations -> text().isEmpty() &&
        !ui -> spGenerationFrequency -> text().isEmpty()) retValue = true ;

    return retValue ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::setExample1FormState Установка видимости элементов формы в зависимости от состояния
 * \param inState
 */
void prExampleMainWindow::setFormState (prExampleMainWindow::prState inState)
{
    switch (inState) {
      case prState::stStartExample1 :
        ui -> spRandomNumeric -> setEnabled(false) ;
        ui -> spThreadCount -> setEnabled(false) ;
        ui -> spTotalIterations -> setEnabled(false) ;
        ui -> spGenerationFrequency -> setEnabled(false) ;

        ui -> toolButton -> setEnabled(false) ;
        ui -> btnClear -> setEnabled(false);
        ui -> btnGenerationFrequency -> setEnabled(false);
        ui -> btnTotalIterations -> setEnabled(false);

      break ;

      case prState::stStop :
        ui -> spRandomNumeric -> setEnabled(true) ;
        ui -> spThreadCount -> setEnabled(true) ;
        ui -> spTotalIterations -> setEnabled(true) ;
        ui -> spGenerationFrequency -> setEnabled(true) ;

        ui -> toolButton -> setEnabled(true) ;
        ui -> btnClear -> setEnabled(true);
        ui -> btnGenerationFrequency -> setEnabled(true);
        ui -> btnTotalIterations -> setEnabled(true);
      break ;

      default :
      break ;
    }
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_spRandomNumeric_textChanged   Слот проверяющий генерируемое число.
 * \param arg1  Вводимая строка
 */
void prExampleMainWindow::on_spRandomNumeric_textChanged(const QString &arg1)
{
    if (arg1.toInt() > 10) ui -> spRandomNumeric -> setText("10") ;
    if (arg1.toInt() == 0) ui -> spRandomNumeric -> clear () ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_toolButton_clicked    Слот обрабатывающий нажатие кнопки автоматической генерации искомого числа
 */
void prExampleMainWindow::on_toolButton_clicked()
{
    std::uniform_int_distribution<quint32> range (1,10) ;
    ui -> spRandomNumeric -> setText (QString::number (range (fGenRandom))) ;
    on_example1ChechState() ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_btnTotalIterations_clicked Слот генерирующий общее количество итераций
 */
void prExampleMainWindow::on_btnTotalIterations_clicked()
{
    std::uniform_int_distribution<quint32> range (1,50) ;
    ui -> spTotalIterations -> setText (QString::number (range (fGenRandom))) ;
    on_example1ChechState() ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_btnGenerationFrequency_clicked    Слот генерирующий квант времени, за который должно быть сгенерированно случайное число в потоке
 */
void prExampleMainWindow::on_btnGenerationFrequency_clicked()
{
    std::uniform_int_distribution<quint32> range (1,9) ;
    ui -> spGenerationFrequency -> setText (QString::number (range (fGenRandom))) ;
    on_example1ChechState() ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_btnStart_clicked  Слот обрабатывающий нажатие на кнопку Старт/Стоп
 */
void prExampleMainWindow::on_btnStart_clicked()
{
    switch (fState) {
      case prExampleMainWindow::stStop: {
        ui -> btnStart -> setText(exampleDefine::btnStopText) ;       // Устанавливаем видимость и доступность элементов формы
        setState(prExampleMainWindow::stStartExample1) ;

        fPtrExample1Timer -> start(exampleDefine::example11Timer);    // Запускаем таймер отслеживающий завершение потоков

        qint32 numThread = ui -> spThreadCount -> text().toInt() ;    // Запускаем потоки
        exampleDefine::totalIteration.store(ui -> spTotalIterations -> text().toInt()) ;
        exampleDefine::threadCount.store(numThread) ;
        quint32 randomNumeric = ui -> spRandomNumeric -> text().toUInt() ;
        quint32 generationFrequency = ui -> spGenerationFrequency -> text().toUInt() ;

        for (qint32 i = 0; i < numThread; ++i) {
            std::thread thread (std::bind (&prExampleMainWindow::threadFunc, this, randomNumeric, generationFrequency)) ;
            thread.detach();
        }
      }
      break;

      case prExampleMainWindow::stStartExample1: {
        ui -> btnStart -> setText(exampleDefine::btnStartText) ;    // Останавливаем работу примера
        setState(prExampleMainWindow::stStop) ;
        fPtrExample1Timer -> stop();                                // Останавливаем основной таймер, т.к. меняем алгоритм ожидания завершения.

        if (exampleDefine::threadCount > 0) {                       // Проверяем завершилась ли работа штатно или нажата кнопка Стоп
            exampleDefine::isStop.store (true) ;
            std::unique_ptr <QTimer> threadWait (new QTimer);       // Таймер для гарантированного завершения задания. Данный таймер нужен если есть зависшие потоки
            connect(threadWait.get(), &QTimer::timeout, this, &prExampleMainWindow::slotWaitTimeout) ;
            threadWait -> start (exampleDefine::waitTimer) ;
            while (exampleDefine::threadCount > 0) {                // Ожидаем завершения всех очередей которое гарантированно когда-нибудь завершится.
                std::this_thread::sleep_for (std::chrono::seconds (1)) ;
                refreshLog () ;
            }

            threadWait -> stop () ;
            disconnect (threadWait.get(), &QTimer::timeout, this, &prExampleMainWindow::slotWaitTimeout) ;
            exampleDefine::isStop.store (false) ;
            QMessageBox::information(this, exampleDefine::messageTitle, exampleDefine::messageText);
        }
        exampleDefine::threadCount.store(0) ;
        exampleDefine::totalIteration.store(0) ;
        exampleDefine::logNumLine.store(0) ;
      }
      break;

    default:
        break;
    }

    setFormState (fState) ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_example1ChechState   Слот проверяющий заполненность всех исходных данных и возможность запуска тестового Примера 1
 */
void prExampleMainWindow::on_example1ChechState()
{
    if (example1ChechState ()) ui -> btnStart -> setEnabled(true);
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::on_btnClear_pressed  Слот обрабатывающий нажатие на кнопку очистки данных
 */
void prExampleMainWindow::on_btnClear_pressed()
{
    signalClear () ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::threadFunc   метод запускаемай в потоке для Пример 1
 * \param inNumber  Искомое число
 * \param inTime    Квант времени за который должно быть сгенерированно число
 */
void prExampleMainWindow::threadFunc (quint8 inNumeric, quint8 inTime)
{
    exampleDefine::example1LogData logData ;
    logData.threadId = std::this_thread::get_id() ;

    try {
        while (true) {                                                  // Основной цикл в котором подбирается число
            std::uniform_int_distribution<quint32> rangeTime (1,inTime) ;
            quint32 waitTime = rangeTime (fGenRandom) ; ;               // Выполняем задержку генерации числа
            std::this_thread::sleep_for (std::chrono::seconds (waitTime)) ;

            logData.time = QTime::currentTime() ;
            if (exampleDefine::isStop) {                                // Прекращаем выполнение задания при нажатии на кнопку Стоп
                logData.numeric = 0 ;
                logData.reasonExit = exampleDefine::reasonExitStop  ;
                throw 1 ;
            }

            if (exampleDefine::totalIteration.fetch_sub (1) > 0) {
                std::uniform_int_distribution<quint32> rangeNumeric (1,10) ; // Генерируем число и если оно совпадает с требуемым, то завершаем цикл
                logData.numeric = rangeNumeric (fGenRandom) ;
                if (logData.numeric == inNumeric) {
                    logData.reasonExit = exampleDefine::reasonExitEqual  ;
                    throw 1 ;
                }
                fPrtExampleModel -> push_back (logData);
            }
              else {     // Завершаем цикл по достижению максимального количества итераций
                logData.numeric = 0 ;
                logData.reasonExit = exampleDefine::reasonExitFailure ;
                throw 1 ;
              }
        }
    }
      catch (...) {
        fPrtExampleModel -> push_back (logData);
      }
    exampleDefine::threadCount.fetch_sub(1) ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::slotExample1Timeout  Слот обновляющий данные в модели и проверяющий завершение всех потоков
 */
void prExampleMainWindow::slotExample1Timeout ()
{
    if (exampleDefine::threadCount > 0) refreshLog () ;
      else {
        emit on_btnStart_clicked() ;                                        // Все потоки завершились
        QMessageBox::information(this, exampleDefine::messageTitle, exampleDefine::messageText);
      }
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::slotWaitTimeout  Слот завершающий задание по завершению времени ожидания
 */
void prExampleMainWindow::slotWaitTimeout ()
{
    exampleDefine::threadCount.store(0); ;
}
//---------------------------------------------------------------------------
/*!
 * \brief prExampleMainWindow::refreshLog   Обновление отображения данных
 */
void prExampleMainWindow::refreshLog ()
{
    qDebug () << "Обновление лога (блокировка): " ;

    {
        exampleDefine::isBloked = true ;
        std::lock_guard <std::mutex> lockRefresh (exampleDefine::muRefresh) ;
        fPrtExampleModel -> refreshView() ;         // Обновляем отображение данных
        QApplication::processEvents() ;
        exampleDefine::isBloked = false ;
    }
}
//---------------------------------------------------------------------------
