#ifndef TITANIUM_SAMPLE_IO_IO_H
#define TITANIUM_SAMPLE_IO_IO_H

namespace Ti::Io
{
    class Io
    {
    public:
        Io();
        ~Io();

        void poolEvents();

        double getTime() const noexcept
        {
            return m_time;
        }

        double getDeltatime() const noexcept
        {
            return m_deltatime;
        }

    private:
        double m_time = 0;
        double m_deltatime = 0.0;
    };
} // Ti::Io

#endif // TITANIUM_SAMPLE_IO_IO_H
