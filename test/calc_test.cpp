#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ICalculator.h"
#include "IHistory.h"
#include "SimpleCalculator.h"
#include "InMemoryHistory.h"


using namespace ::testing;
using namespace calc;

// Обычные тесты
// Проверка работы сложения
TEST(CalculatorArithmeticTest, AddReturnsCorrect) {
    // Arrange
    InMemoryHistory history;
    SimpleCalculator calculator(history);

    // Act
    int result1 = calculator.Add(2, 3);
    int result2 = calculator.Add(0, 0);
    int result3 = calculator.Add(0, -2);
    int result4 = calculator.Add(-1, -2);

    // Assert
    EXPECT_EQ(result1, 5);
    EXPECT_EQ(result2, 0);
    EXPECT_EQ(result3, -2);
    EXPECT_EQ(result4, -3);
}

//
// Мок-реализации интерфейсов
//

// Мок для IHistory
class MockHistory : public IHistory {
public:
    MOCK_METHOD(void, AddEntry, (const std::string&), (override));
    MOCK_METHOD(std::vector<std::string>, GetLastOperations, (size_t), (const, override));
};

// Мок для ICalculator
class MockCalculator : public ICalculator {
public:
    MOCK_METHOD(int, Add, (int, int), (override));
    MOCK_METHOD(int, Subtract, (int, int), (override));
    MOCK_METHOD(int, Multiply, (int, int), (override));
    MOCK_METHOD(int, Divide, (int, int), (override));
    MOCK_METHOD(void, SetHistory, (IHistory&), (override));
};

//
// --- Тесты взаимодействия ---
//

// Проверяет, что при вызове Add у SimpleCalculator,
// происходит запись результата в историю
TEST(SimpleCalculatorInteractionWithHistoryTest, Add_CallsHistoryWithCorrectOperation) {
    MockHistory mockHistory;
    SimpleCalculator calc(mockHistory);

    // Ожидаем, что метод истории будет вызван с конкретной строкой
    EXPECT_CALL(mockHistory, AddEntry("2 + 2 = 4")).Times(1);

    calc.Add(2, 2);
}

// Проверяет, что при вызове Subtract результат логируется корректно
TEST(SimpleCalculatorInteractionWithHistoryTest, Subtract_LogsCorrectEntryToHistory) {
    MockHistory mockHistory;
    SimpleCalculator calc(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("5 - 3 = 2")).Times(1);

    calc.Subtract(5, 3);
}

// Проверяет, что Multiply вызывает AddEntry только один раз
TEST(SimpleCalculatorInteractionWithHistoryTest, Multiply_CallsHistoryOnce) {
    MockHistory mockHistory;
    SimpleCalculator calc(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("3 * 3 = 9")).Times(1);

    calc.Multiply(3, 3);
}

// Проверяет, что Divide передаёт правильную строку в историю
TEST(SimpleCalculatorInteractionWithHistoryTest, Divide_LogsCorrectDivision) {
    MockHistory mockHistory;
    SimpleCalculator calc(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("8 / 2 = 4")).Times(1);

    calc.Divide(8, 2);
}

// Проверяет, что SetHistory изменяет используемый объект истории (необходимо исправить работу, чтобы она действительно менялась)
TEST(SimpleCalculatorInteractionWithHistoryTest, SetHistory_OverridesPreviousHistory) {
    MockHistory history1;
    MockHistory history2;

    // История 2 должна быть вызвана, а история 1 — нет
    EXPECT_CALL(history1, AddEntry).Times(0);
    EXPECT_CALL(history2, AddEntry("1 + 1 = 2")).Times(1);

    SimpleCalculator calc(history1);
    calc.SetHistory(history2);

    calc.Add(1, 1);
}

// Проверяет, что вызов GetLastOperations у истории работает, когда калькулятор — мок
TEST(HistoryInteractionWithCalculatorTest, GetLastOperations_CalledWithCorrectCount) {
    InMemoryHistory history;
    MockCalculator calc;

    // История заполняется, потом вызываем метод
    history.AddEntry("1 + 1 = 2");
    history.AddEntry("2 * 2 = 4");

    auto entries = history.GetLastOperations(1);

    ASSERT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0], "2 * 2 = 4");
}


// В C++ int от -2147483648 до 2147483647 (INT_MIN и INT_MAX).
// Проверка, что сложение не переполняется при INT_MAX - 1
TEST(SimpleCalculatorBoundaryTest, Add_JustBeforeOverflow_DoesNotCrash) {
    InMemoryHistory history;
    SimpleCalculator calc(history);

    int result = calc.Add(INT_MAX - 1, 1);

    EXPECT_EQ(result, INT_MAX);
}
// Этот тест показывает, что поведение не определено (необходимо добавить исключения)
TEST(SimpleCalculatorBoundaryTest, Add_Overflow_UndefinedBehavior) {
    InMemoryHistory history;
    SimpleCalculator calc(history);

    // WARNING: результат может быть отрицательным из-за переполнения
    int result = calc.Add(INT_MAX, 1);

    // EXPECT_NE(result, INT_MAX);  // можно просто посмотреть вручную
    std::cout << "Result: " << result << std::endl;
}

// Проверка вычитания на нижней границе диапазона
TEST(SimpleCalculatorBoundaryTest, Subtract_MinBoundary) {
    InMemoryHistory history;
    SimpleCalculator calc(history);

    int result = calc.Subtract(INT_MIN + 1, 1);

    EXPECT_EQ(result, INT_MIN);
}

// Проверка, что INT_MAX * 1 не вызывает проблем
TEST(SimpleCalculatorBoundaryTest, Multiply_MaxBoundary_Safe) {
    InMemoryHistory history;
    SimpleCalculator calc(history);

    int result = calc.Multiply(INT_MAX, 1);

    EXPECT_EQ(result, INT_MAX);
}

// Деление INT_MIN на -1 вызывает переполнение (необходимо добавить исключение)
TEST(SimpleCalculatorBoundaryTest, Divide_MinByMinusOne_UB) {
    InMemoryHistory history;
    SimpleCalculator calc(history);

    int result = calc.Divide(INT_MIN, -1);

    std::cout << "Result: " << result << std::endl;  // Может быть UB
}

// Проверка деления на границе, когда результат — 1
TEST(SimpleCalculatorBoundaryTest, Divide_MaxByMax_Equals1) {
    InMemoryHistory history;
    SimpleCalculator calc(history);

    int result = calc.Divide(INT_MAX, INT_MAX);

    EXPECT_EQ(result, 1);
}

// --- Арифметический тестовый сьют ---
class SimpleCalculatorArithmeticTest : public ::testing::Test {
protected:
    InMemoryHistory history;
    SimpleCalculator calc{ history };
};

// Сложение
TEST_F(SimpleCalculatorArithmeticTest, Add_PositiveNumbers_ReturnsCorrectResult) {
    EXPECT_EQ(calc.Add(10, 5), 15);
}

// Вычитание
TEST_F(SimpleCalculatorArithmeticTest, Subtract_PositiveNumbers_ReturnsCorrectResult) {
    EXPECT_EQ(calc.Subtract(10, 3), 7);
}

// Умножение
TEST_F(SimpleCalculatorArithmeticTest, Multiply_TwoNumbers_ReturnsCorrectResult) {
    EXPECT_EQ(calc.Multiply(4, 5), 20);
}

// Деление без остатка
TEST_F(SimpleCalculatorArithmeticTest, Divide_ExactDivision_ReturnsQuotient) {
    EXPECT_EQ(calc.Divide(10, 2), 5);
}

// Деление с остатком (дробное деление)
TEST_F(SimpleCalculatorArithmeticTest, Divide_NonExactDivision_TruncatesTowardZero) {
    EXPECT_EQ(calc.Divide(7, 2), 3);  // int: 7 / 2 = 3.5 → 3
}

// Деление на 0 поведение не определено (необходимо добавить выброс исключения)
TEST_F(SimpleCalculatorArithmeticTest, Divide_ByZero) {
    EXPECT_ANY_THROW(calc.Divide(5, 0));  // если не обрабатывается — может быть UB
}
