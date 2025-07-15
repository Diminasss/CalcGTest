# Unit-тесты для библиотеки `calc`

Этот проект содержит unit-тесты для C++ библиотеки калькулятора (`calc`), которая поддерживает базовые арифметические операции и ведение истории вычислений. Тесты находятся в папке test.

## 📚 О библиотеке

Библиотека предоставляет два основных интерфейса:

* **`ICalculator`** — выполняет арифметические операции:

    * `Add(int a, int b)`
    * `Subtract(int a, int b)`
    * `Multiply(int a, int b)`
    * `Divide(int a, int b)`
    * `SetHistory(IHistory& history)`

* **`IHistory`** — ведёт историю операций:

    * `AddEntry(const std::string& operation)`
    * `GetLastOperations(size_t count)`

Реализации:

* `SimpleCalculator` — конкретная реализация `ICalculator`
* `InMemoryHistory` — конкретная реализация `IHistory`, хранит записи в памяти

## ✅ Что тестируется?

Тесты написаны с использованием **Google Test** и **Google Mock**. Основные проверки:

### 1. Арифметическая функциональность

* Сложение, вычитание, умножение, деление для положительных и отрицательных чисел
* Крайние случаи с `INT_MAX` и `INT_MIN`
* Усечение при дробном делении
* Демонстрация неопределённого поведения (overflow и деление на ноль)

### 2. Взаимодействие интерфейсов

* Проверка, что `SimpleCalculator` вызывает `AddEntry(...)` у `IHistory`
* Проверка корректной работы `SetHistory()` (требует изменения реализации)
* Поверка `GetLastOperations()` у `InMemoryHistory`

### 3. Граничные и неопределённые сценарии

* **Переполнение при сложении**: `Add(INT_MAX, 1)` — undefined
* **Деление на ноль**: `Divide(x, 0)` — UB
* **Переполнение при делении**: `Divide(INT_MIN, -1)` — UB

Тесты на эти сценарии включены, но отмечены как **ожидаемые сбои** до тех пор, пока поведение не будет определено.

## 🛠️ Как запускать тесты

1. Установить GoogleTest и GoogleMock.
2. Перейти в папку build
3. Запустить файл run.bat

## 🔍 Известные неудачные тесты и причины

Ниже перечислены тесты, которые сейчас **падают**, вместе с описанием причин и способов фикса.

1. **`SetHistory_OverridesPreviousHistory`**

    * **Проблема**: `SimpleCalculator` хранит `IHistory& m_history` и не может переназначить ссылку.
    * **Решение**: Перейти на хранение указателя `IHistory* m_history;` и обновить конструктор, метод `SetHistory()` и логику вызова `AddEntry()`.

2. **`Divide_ByZero`**

    * **Проблема**: деление на ноль вызывает SEH-исключение — неопределённое поведение.
    * **Решение**: В методе `Divide()` добавить проверку:

      ```cpp
      if (b == 0) throw std::invalid_argument("Division by zero");
      ```
    * Тест изменить на:

      ```cpp
      EXPECT_THROW(calc.Divide(5, 0), std::invalid_argument);
      ```

3. **`Divide_MinByMinusOne_UB`**

    * **Проблема**: `INT_MIN / -1` выходит за пределы `int` — undefined.
    * **Решение**: Добавить проверку переполнения:

      ```cpp
      if (a == INT_MIN && b == -1) throw std::overflow_error("Overflow on division");
      ```
    * Тест изменить на:

      ```cpp
      EXPECT_THROW(calc.Divide(INT_MIN, -1), std::overflow_error);
      ```

Тесты на эти случаи пока **включены** и ожидают обновления библиотеки. После внесения правок все тесты должны успешно проходить.

## Результат выполнения тестов из консоли при запуске из run.bat
```bash
==== Сборка проекта ====
Версия MSBuild 17.14.14+a129329f1 для .NET Framework

  CalcLib.vcxproj -> C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\Debug\CalcLib.lib
  gmock.vcxproj -> C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\lib\Debug\gmock.lib
  gtest.vcxproj -> C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\lib\Debug\gtest.lib
  gtest_main.vcxproj -> C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\lib\Debug\gtest_main.lib
  calculate_test.vcxproj -> C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\Debug\calculate_test.exe
  gmock_main.vcxproj -> C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\lib\Debug\gmock_main.lib
==== Запуск тестов ====
Running main() from C:\Users\johnworker\Desktop\Проекты C++\CalcGTest\build\_deps\googletest-src\googletest\src\gtest_main.cc
[==========] Running 19 tests from 5 test suites.
[----------] Global test environment set-up.
[----------] 1 test from CalculatorArithmeticTest
[ RUN      ] CalculatorArithmeticTest.AddReturnsCorrect
[       OK ] CalculatorArithmeticTest.AddReturnsCorrect (0 ms)
[----------] 1 test from CalculatorArithmeticTest (0 ms total)

[----------] 5 tests from SimpleCalculatorInteractionWithHistoryTest
[ RUN      ] SimpleCalculatorInteractionWithHistoryTest.Add_CallsHistoryWithCorrectOperation
[       OK ] SimpleCalculatorInteractionWithHistoryTest.Add_CallsHistoryWithCorrectOperation (0 ms)
[ RUN      ] SimpleCalculatorInteractionWithHistoryTest.Subtract_LogsCorrectEntryToHistory
[       OK ] SimpleCalculatorInteractionWithHistoryTest.Subtract_LogsCorrectEntryToHistory (0 ms)
[ RUN      ] SimpleCalculatorInteractionWithHistoryTest.Multiply_CallsHistoryOnce
[       OK ] SimpleCalculatorInteractionWithHistoryTest.Multiply_CallsHistoryOnce (0 ms)
[ RUN      ] SimpleCalculatorInteractionWithHistoryTest.Divide_LogsCorrectDivision
[       OK ] SimpleCalculatorInteractionWithHistoryTest.Divide_LogsCorrectDivision (0 ms)
[ RUN      ] SimpleCalculatorInteractionWithHistoryTest.SetHistory_OverridesPreviousHistory
C:\Users\johnworker\Desktop\������� C++\CalcGTest\test\calc_test.cpp(106): error: Mock function called more times than expected - returning directly.
    Function call: AddEntry(@000000000014EF28 "1 + 1 = 2")
         Expected: to be never called
           Actual: called once - over-saturated and active

C:\Users\johnworker\Desktop\������� C++\CalcGTest\test\calc_test.cpp(107): error: Actual function call count doesn't match EXPECT_CALL(history2, AddEntry("1 + 1 = 2"))...
         Expected: to be called once
           Actual: never called - unsatisfied and active

[  FAILED  ] SimpleCalculatorInteractionWithHistoryTest.SetHistory_OverridesPreviousHistory (0 ms)
[----------] 5 tests from SimpleCalculatorInteractionWithHistoryTest (5 ms total)

[----------] 1 test from HistoryInteractionWithCalculatorTest
[ RUN      ] HistoryInteractionWithCalculatorTest.GetLastOperations_CalledWithCorrectCount
[       OK ] HistoryInteractionWithCalculatorTest.GetLastOperations_CalledWithCorrectCount (0 ms)
[----------] 1 test from HistoryInteractionWithCalculatorTest (0 ms total)

[----------] 6 tests from SimpleCalculatorBoundaryTest
[ RUN      ] SimpleCalculatorBoundaryTest.Add_JustBeforeOverflow_DoesNotCrash
[       OK ] SimpleCalculatorBoundaryTest.Add_JustBeforeOverflow_DoesNotCrash (0 ms)
[ RUN      ] SimpleCalculatorBoundaryTest.Add_Overflow_UndefinedBehavior
Result: -2147483648
[       OK ] SimpleCalculatorBoundaryTest.Add_Overflow_UndefinedBehavior (1 ms)
[ RUN      ] SimpleCalculatorBoundaryTest.Subtract_MinBoundary
[       OK ] SimpleCalculatorBoundaryTest.Subtract_MinBoundary (0 ms)
[ RUN      ] SimpleCalculatorBoundaryTest.Multiply_MaxBoundary_Safe
[       OK ] SimpleCalculatorBoundaryTest.Multiply_MaxBoundary_Safe (0 ms)
[ RUN      ] SimpleCalculatorBoundaryTest.Divide_MinByMinusOne_UB
unknown file: error: SEH exception with code 0xc0000095 thrown in the test body.
Stack trace:


[  FAILED  ] SimpleCalculatorBoundaryTest.Divide_MinByMinusOne_UB (0 ms)
[ RUN      ] SimpleCalculatorBoundaryTest.Divide_MaxByMax_Equals1
[       OK ] SimpleCalculatorBoundaryTest.Divide_MaxByMax_Equals1 (0 ms)
[----------] 6 tests from SimpleCalculatorBoundaryTest (5 ms total)

[----------] 6 tests from SimpleCalculatorArithmeticTest
[ RUN      ] SimpleCalculatorArithmeticTest.Add_PositiveNumbers_ReturnsCorrectResult
[       OK ] SimpleCalculatorArithmeticTest.Add_PositiveNumbers_ReturnsCorrectResult (0 ms)
[ RUN      ] SimpleCalculatorArithmeticTest.Subtract_PositiveNumbers_ReturnsCorrectResult
[       OK ] SimpleCalculatorArithmeticTest.Subtract_PositiveNumbers_ReturnsCorrectResult (0 ms)
[ RUN      ] SimpleCalculatorArithmeticTest.Multiply_TwoNumbers_ReturnsCorrectResult
[       OK ] SimpleCalculatorArithmeticTest.Multiply_TwoNumbers_ReturnsCorrectResult (0 ms)
[ RUN      ] SimpleCalculatorArithmeticTest.Divide_ExactDivision_ReturnsQuotient
[       OK ] SimpleCalculatorArithmeticTest.Divide_ExactDivision_ReturnsQuotient (0 ms)
[ RUN      ] SimpleCalculatorArithmeticTest.Divide_NonExactDivision_TruncatesTowardZero
[       OK ] SimpleCalculatorArithmeticTest.Divide_NonExactDivision_TruncatesTowardZero (0 ms)
[ RUN      ] SimpleCalculatorArithmeticTest.Divide_ByZero
unknown file: error: SEH exception with code 0xc0000094 thrown in the test body.
Stack trace:


[  FAILED  ] SimpleCalculatorArithmeticTest.Divide_ByZero (0 ms)
[----------] 6 tests from SimpleCalculatorArithmeticTest (5 ms total)

[----------] Global test environment tear-down
[==========] 19 tests from 5 test suites ran. (23 ms total)
[  PASSED  ] 16 tests.
[  FAILED  ] 3 tests, listed below:
[  FAILED  ] SimpleCalculatorInteractionWithHistoryTest.SetHistory_OverridesPreviousHistory
[  FAILED  ] SimpleCalculatorBoundaryTest.Divide_MinByMinusOne_UB
[  FAILED  ] SimpleCalculatorArithmeticTest.Divide_ByZero

 3 FAILED TESTS

==== Тесты завершены ====
Нажмите ENTER, чтобы запустить снова, или CTRL+C для выхода...
```

## Домашка

Перед вами мини-библиотека калькулятор. Она умеет:
1. Складывать
2. Вычитать
3. Умножать
4. Делить
5. Записывать результаты в историю.

## Что сделать:

Ваша задача написать на нее unit тесты используя gtest. Идеи для тестов:
1. Моки на интерфейсы калькулятора и хранилки истории. Проверка взаимодействия между ними.
Пример: пусть есть объект калькулятора и мока хранилки истории. Просим калькулятор сложить 2 и 2, а потом видим, что в
моке был вызван метод для сохранения параметров. И наоборот, когда калькулятор -- это мока.
2. Проверки на граничные значения
3. Деление на ноль? Умножение?
4. Правильно ли сохраняются логи в историю? 
5. Дробное деление

P.S. Часть поведения не определена в коде (например, как много можно хранить записей истории и тд). 
Если вы найдете такое, то будет круто, если вы оформите это в виде теста, который сейчас не будет проходить

P.S.S. При желании код библиотеки можно менять как вам вздумается

Задание со звездочкой -- это собраться с codecoverage утилитой и прикрепить результат (можно ввиде отчета, можно скрин из IDE) 
