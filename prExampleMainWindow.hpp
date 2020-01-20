#ifndef PREXAMPLEMAINWINDOW_HPP
#define PREXAMPLEMAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QApplication>
#include <QCloseEvent>

#include <memory>
#include <random>
#include <atomic>

#include "example1LogModel.hpp"

/*!
 * \brief Класс формы демонстрирующий работы тестовых примеров
 */
namespace Ui {
class prExampleMainWindow;
}

class prExampleMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit prExampleMainWindow(QWidget *parent = 0);
    ~prExampleMainWindow();

private slots:
    void on_toolButton_clicked();
    void on_btnTotalIterations_clicked();
    void on_btnGenerationFrequency_clicked();
    void on_btnStart_clicked();
    void on_example1ChechState() ;
    void on_btnClear_pressed();
    void slotExample1Clear () ;         // Очистка данных для примера example1
    void slotExample1Timeout () ;       // Слот проверяющий завершение всех потоков
    void slotWaitTimeout () ;           // Слот завершающий задание по завершению времени ожидания

private:

    enum pageNum {pgExample1= 0, pgExample2 = 1} ;  ///< список и последовательность закладок
    enum prState {stUnknown, stStop, stStartExample1, stStartExample2} ; ///< Состояния проекта

    Ui::prExampleMainWindow *ui;

    std::shared_ptr <TExample1LogModel> fPrtExampleModel {nullptr} ;    // Указатель на модель для вывода лога работы Пример 1
    std::unique_ptr <QTimer> fPtrExample1Timer {nullptr} ;  // Указатель на таймер проверяющий завершение потоков
    std::mt19937 fGenRandom ;       // Генератор случайных чисел

    void initFirm () ;              // инициализация формы
    void makeConnect () ;           // Формирует все нужные конекты

    prState fState {stUnknown} ;    // Флаг состояния ПО

    prState getState () ;           // Возвращает состояние выполнения проекта
    void setState (prState) ;       // Устанавливает состояние проекта
    void setFormState (prExampleMainWindow::prState) ; // Установка видимости и доступности элементов формы в зависимости от состояния

    bool example1ChechState () ;    // Проверяет возможность запуска примера example1
    void threadFunc (quint8, quint8) ; // метод запускаемый в потоке
    void refreshLog () ;            // Обновление отображения данных

    void closeEvent (QCloseEvent *) ; // Обработка закрытия окна

signals:
    void signalClear () ;           // Сигнал очистки всех данных
};

#endif // PREXAMPLEMAINWINDOW_HPP
