#include "../GUI/Main/MainWindow.hpp"
#include "../Helper/CPU.hpp"

QCoro::Task<> GUIMainWindow::CPUMeasureStart()
{
    m_CPU_usage = CPUStats::GetCurrentValue();
    m_keep_measuring_CPU = true; // Caller should set this to false when they don't need CPU measure anymore

    // ReSharper disable once CppDFAEndlessLoop
    // ReSharper disable once CppDFAConstantConditions
    while (m_keep_measuring_CPU) {
        co_await QCoro::sleepFor(std::chrono::milliseconds(CPU_MEASURE_INTERVAL_MS));
        m_CPU_usage = CPUStats::GetCurrentValue();
    }
}
