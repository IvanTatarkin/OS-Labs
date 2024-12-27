#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cmath>
#include <cstdlib>
#include <cstring>


TEST(TestLibrary1, TestSinIntegralRectangle) {
    void* handle = dlopen("../LW4/liblib1.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr) << "Ошибка загрузки библиотеки: " << dlerror();

    auto SinIntegral_rectangle = (float (*)(float, float, float))dlsym(handle, "SinIntegral_rectangle");
    ASSERT_NE(SinIntegral_rectangle, nullptr) << "Не удалось найти функцию SinIntegral_rectangle";

    float A = 0.0f, B = M_PI, e = 0.01f;
    float result = SinIntegral_rectangle(A, B, e);
    float expected = 2.0f; 
    EXPECT_NEAR(result, expected, 0.01f);

    dlclose(handle);
}


TEST(TestLibrary1, TestBubbleSort) {
    void* handle = dlopen("../LW4/liblib1.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr) << "Ошибка загрузки библиотеки: " << dlerror();

    auto BubbleSort = (void (*)(int*, int))dlsym(handle, "BubbleSort");
    ASSERT_NE(BubbleSort, nullptr) << "Не удалось найти функцию BubbleSort";

    int array[] = {5, 3, 8, 4, 2};
    int sorted_array[] = {2, 3, 4, 5, 8};
    int size = 5;

    BubbleSort(array, size);
    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(array[i], sorted_array[i]);
    }

    dlclose(handle);
}


TEST(TestLibrary2, TestSinIntegralTrapezoid) {
    void* handle = dlopen("../LW4/liblib2.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr) << "Ошибка загрузки библиотеки: " << dlerror();

    auto SinIntegral_trapezoid = (float (*)(float, float, float))dlsym(handle, "SinIntegral_trapezoid");
    ASSERT_NE(SinIntegral_trapezoid, nullptr) << "Не удалось найти функцию SinIntegral_trapezoid";

    float A = 0.0f, B = M_PI, e = 0.01f;
    float result = SinIntegral_trapezoid(A, B, e);
    float expected = 2.0f; 
    EXPECT_NEAR(result, expected, 0.01f);

    dlclose(handle);
}


TEST(TestLibrary2, TestQuickSort) {
    void* handle = dlopen("../LW4/liblib2.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr) << "Ошибка загрузки библиотеки: " << dlerror();

    auto QuickSort = (void (*)(int*, int))dlsym(handle, "QuickSort");
    ASSERT_NE(QuickSort, nullptr) << "Не удалось найти функцию QuickSort";

    int array[] = {10, 1, 7, 3, 9};
    int sorted_array[] = {1, 3, 7, 9, 10};
    int size = 5;

    QuickSort(array, size);
    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(array[i], sorted_array[i]);
    }

    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
