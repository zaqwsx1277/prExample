#include <iostream>
#include <utility>
#include <set>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

static const int numPos {3} ;             // позиция для поиска n-ного отсчета

typedef std::pair <int, int> tdInData ;   // typedef для одного отсчета
//---------------------------------------------------------
struct stCmpY   // Cортировкa исходного контейнера по отсчётам
{
    bool operator () (const tdInData& inData1, const tdInData& inData2) const {
        bool retVal = false ;
        if (inData1.second < inData2.second) retVal= true ;                                                   // условие для сортировки по Y
          else if ((inData1.second == inData2.second) && (inData1.first < inData2.first)) retVal = true ;   // условие для сортировки по Х при равенстве Y
        return retVal ;
    }
} ;
//---------------------------------------------------------
struct stCmpKey // Cортировкa исходного контейнера по X
{
    bool operator () (const tdInData& inData1, const tdInData& inData2) const { return (inData1.second < inData2.second) ; }
};
//---------------------------------------------------------
//struct sortX    // Сортировка результирующего контейнера по X
//{
//    bool operator () (const tdInData& inData1, const tdInData& inData2) const { return (inData1.first < inData2.) ; }
//};
//}
//---------------------------------------------------------
int main()
{
    assert (numPos > 2) ;               // Тупо остановим выполнение, если искомая позиция n-ного отсчета меньше 3
                                        // Контейнер содержащий исходный набор данных
    std::multiset <tdInData, stCmpY> setInData {{1, 10}, {2, 11}, {3, 11}, {4, 11}, {5, 11}, {6, 10}, {7, 11}, {9, 11}, {10, 11}, {11, 10}} ;

    std::set <tdInData, stCmpKey> setKeys (setInData.begin(), setInData.end()) ;   // Контейнер содержащий список ключей (уникальных значений Y)
    std::vector <tdInData> result ;     // Контейнер содержащий результат

    for (tdInData itemKey: setKeys) {   // Заносим в результирующий контейнер значение первого и последнего элемента идентичного отсчёта
        auto itemPair = setInData.equal_range(itemKey) ;
        result.push_back (*itemPair.first) ;
        result.push_back (*itemPair.second) ;
    }

    for (auto item : setInData) {       // Добавляем в контейнер n-ные отсчеты
        if (item.first % numPos == 0) result.push_back(item);
    }

    std::sort (result.begin(), result.end(), [] (const tdInData& inData1, const tdInData& inData2) { return (inData1.first < inData2.first) ; }) ;
    for (auto item : result)
        auto xxx = item ;

    cout << "Hello World!" << endl;
    return 0;
}
