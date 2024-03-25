#include "hal_st/stm32fxxx/DmaStm.hpp"
#include "infra/util/ByteRange.hpp"

#if !defined(STM32F0) && !defined(STM32F1) && !defined(STM32F3)

namespace hal
{
    namespace
    {
#if defined(DMA_STREAM_BASED)
        const std::array dmaISR{
            std::array{ &DMA1->LISR, &DMA1->LISR, &DMA1->LISR, &DMA1->LISR, &DMA1->HISR, &DMA1->HISR, &DMA1->HISR, &DMA1->HISR },
            std::array{ &DMA2->LISR, &DMA2->LISR, &DMA2->LISR, &DMA2->LISR, &DMA2->HISR, &DMA2->HISR, &DMA2->HISR, &DMA2->HISR },
        };

        const std::array dmaIFCR{
            std::array{ &DMA1->LIFCR, &DMA1->LIFCR, &DMA1->LIFCR, &DMA1->LIFCR, &DMA1->HIFCR, &DMA1->HIFCR, &DMA1->HIFCR, &DMA1->HIFCR },
            std::array{ &DMA2->LIFCR, &DMA2->LIFCR, &DMA2->LIFCR, &DMA2->LIFCR, &DMA2->HIFCR, &DMA2->HIFCR, &DMA2->HIFCR, &DMA2->HIFCR },
        };

        const std::array DmaChannel{
            std::array{ DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3, DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7 },
            std::array{ DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3, DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7 },
        };

        const std::array dmaIrq{
            std::array{ DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn, DMA1_Stream3_IRQn, DMA1_Stream4_IRQn, DMA1_Stream5_IRQn, DMA1_Stream6_IRQn, DMA1_Stream7_IRQn },
            std::array{ DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn, DMA2_Stream3_IRQn, DMA2_Stream4_IRQn, DMA2_Stream5_IRQn, DMA2_Stream6_IRQn, DMA2_Stream7_IRQn },
        };

        const std::array dmaChannel
        {
            DMA_CHANNEL_0,
                DMA_CHANNEL_1,
                DMA_CHANNEL_2,
                DMA_CHANNEL_3,
                DMA_CHANNEL_4,
                DMA_CHANNEL_5,
                DMA_CHANNEL_6,
                DMA_CHANNEL_7,
#if defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
                DMA_CHANNEL_8,
                DMA_CHANNEL_9,
                DMA_CHANNEL_10,
                DMA_CHANNEL_11,
                DMA_CHANNEL_12,
                DMA_CHANNEL_13,
                DMA_CHANNEL_14,
                DMA_CHANNEL_15,
#endif
        };

        const std::array streamToTCIF{ DMA_FLAG_TCIF0_4, DMA_FLAG_TCIF1_5, DMA_FLAG_TCIF2_6, DMA_FLAG_TCIF3_7, DMA_FLAG_TCIF0_4, DMA_FLAG_TCIF1_5, DMA_FLAG_TCIF2_6, DMA_FLAG_TCIF3_7 };
        const std::array streamToHTIF{ DMA_FLAG_HTIF0_4, DMA_FLAG_HTIF1_5, DMA_FLAG_HTIF2_6, DMA_FLAG_HTIF3_7, DMA_FLAG_HTIF0_4, DMA_FLAG_HTIF1_5, DMA_FLAG_HTIF2_6, DMA_FLAG_HTIF3_7 };
        const std::array streamToTEIF{ DMA_FLAG_TEIF0_4, DMA_FLAG_TEIF1_5, DMA_FLAG_TEIF2_6, DMA_FLAG_TEIF3_7, DMA_FLAG_TEIF0_4, DMA_FLAG_TEIF1_5, DMA_FLAG_TEIF2_6, DMA_FLAG_TEIF3_7 };
#endif

#if defined(DMA_CHANNEL_BASED)
        // clang-format off
        const std::array dmaISR
        {
            std::array
            {
#if defined(STM32H5)
                &GPDMA1->MISR,
                &GPDMA1->MISR,
                &GPDMA1->MISR,
                &GPDMA1->MISR,
                &GPDMA1->MISR,
                &GPDMA1->MISR,
                &GPDMA1->MISR,
                &GPDMA1->MISR,
#else
                &DMA1->ISR,
                &DMA1->ISR,
                &DMA1->ISR,
                &DMA1->ISR,
                &DMA1->ISR,
                &DMA1->ISR,
#if defined(DMA1_Channel7)
                &DMA1->ISR,
#endif
#if defined(DMA1_Channel8)
                &DMA1->ISR,
#endif
#endif
            },
#if defined(STM32H5)
            std::array
            {
                &GPDMA2->MISR,
                &GPDMA2->MISR,
                &GPDMA2->MISR,
                &GPDMA2->MISR,
                &GPDMA2->MISR,
                &GPDMA2->MISR,
                &GPDMA2->MISR,
                &GPDMA2->MISR,
            },
#else
#if defined(DMA2_Channel1)
            std::array
            {
                &DMA2->ISR,
                &DMA2->ISR,
                &DMA2->ISR,
                &DMA2->ISR,
                &DMA2->ISR,
                &DMA2->ISR,
#if defined(DMA2_Channel7)
                &DMA2->ISR,
#endif
#if defined(DMA2_Channel8)
                &DMA2->ISR,
#endif
            },
#endif
#endif
        };

        const std::array dmaIFCR
#if defined(STM32H5)
        {
            std::array { &GPDMA1_Channel0->CFCR, &GPDMA1_Channel1->CFCR, &GPDMA1_Channel2->CFCR, &GPDMA1_Channel3->CFCR, &GPDMA1_Channel4->CFCR, &GPDMA1_Channel5->CFCR, &GPDMA1_Channel6->CFCR, &GPDMA1_Channel7->CFCR, },
            std::array { &GPDMA2_Channel0->CFCR, &GPDMA2_Channel1->CFCR, &GPDMA2_Channel2->CFCR, &GPDMA2_Channel3->CFCR, &GPDMA2_Channel4->CFCR, &GPDMA2_Channel5->CFCR, &GPDMA2_Channel6->CFCR, &GPDMA2_Channel7->CFCR, },
        };
#else
        {
            std::array
            {
                &DMA1->IFCR,
                &DMA1->IFCR,
                &DMA1->IFCR,
                &DMA1->IFCR,
                &DMA1->IFCR,
                &DMA1->IFCR,
#if defined(DMA1_Channel7)
                &DMA1->IFCR,
#endif
#if defined(DMA1_Channel8)
                &DMA1->IFCR,
#endif
            },
#if defined(DMA2_Channel1)
            std::array
            {
                &DMA2->IFCR,
                &DMA2->IFCR,
                &DMA2->IFCR,
                &DMA2->IFCR,
                &DMA2->IFCR,
                &DMA2->IFCR,
#if defined(DMA2_Channel7)
                &DMA2->IFCR,
#endif
#if defined(DMA2_Channel8)
                &DMA2->IFCR,
#endif
            },
        };
#endif
#endif

        const std::array DmaChannel
        {
            std::array
            {
                GPDMA1_Channel0,
                GPDMA1_Channel1,
                GPDMA1_Channel2,
                GPDMA1_Channel3,
                GPDMA1_Channel4,
                GPDMA1_Channel5,
                GPDMA1_Channel6,
#if defined(GPDMA1_Channel7)
                GPDMA1_Channel7,
#endif
#if defined(DMA1_Channel8)
                DMA1_Channel8,
#endif
            },
#if defined(GPDMA2_Channel1)
            std::array
            {
                GPDMA2_Channel0,
                GPDMA2_Channel1,
                GPDMA2_Channel2,
                GPDMA2_Channel3,
                GPDMA2_Channel4,
                GPDMA2_Channel5,
                GPDMA2_Channel6,
#if defined(GPDMA2_Channel7)
                GPDMA2_Channel7,
#endif
#if defined(GPDMA2_Channel8)
                DMA2_Channel8,
#endif
            },
#endif
        };
        const std::array dmaIrq
        {
            std::array
            {
                GPDMA1_Channel0_IRQn,
                GPDMA1_Channel1_IRQn,
                GPDMA1_Channel2_IRQn,
                GPDMA1_Channel3_IRQn,
                GPDMA1_Channel4_IRQn,
                GPDMA1_Channel5_IRQn,
                GPDMA1_Channel6_IRQn,
#if defined(GPDMA1_Channel7)
                GPDMA1_Channel7_IRQn,
#endif
#if defined(DMA1_Channel8)
                DMA1_Channel8_IRQn,
#endif
            },
#if defined(GPDMA2_Channel1)
            std::array
            {
                GPDMA2_Channel0_IRQn,
                GPDMA2_Channel1_IRQn,
                GPDMA2_Channel2_IRQn,
                GPDMA2_Channel3_IRQn,
                GPDMA2_Channel4_IRQn,
                GPDMA2_Channel5_IRQn,
                GPDMA2_Channel6_IRQn,
#if defined(GPDMA2_Channel7)
                GPDMA2_Channel7_IRQn,
#endif
#if defined(DMA2_Channel8)
                DMA2_Channel8_IRQn,
#endif
            },
#endif
        };

        const std::array streamToTCIF
        {
            DMA_CFCR_TCF,
            DMA_CFCR_TCF,
            DMA_CFCR_TCF,
            DMA_CFCR_TCF,
            DMA_CFCR_TCF,
            DMA_CFCR_TCF,
            DMA_CFCR_TCF,
            DMA_CFCR_TCF
#if defined(DMA_FLAG_TC7)
            DMA_FLAG_TC7,
#endif
#if defined(DMA_FLAG_TC8)
            DMA_FLAG_TC8,
#endif
        };
        const std::array streamToHTIF
        {
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
            DMA_CFCR_HTF,
#if defined(DMA_FLAG_HT7)
            DMA_FLAG_HT7,
#endif
#if defined(DMA_FLAG_HT8)
            DMA_FLAG_HT8,
#endif
        };
        const std::array streamToTEIF
        {
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
            DMA_CFCR_DTEF,
#if defined(DMA_FLAG_TE7)
            DMA_FLAG_TE7,
#endif
#if defined(DMA_FLAG_TE8)
            DMA_FLAG_TE8,
#endif
        };
#endif

        // clang-format on

        static_assert(dmaISR[0].size() == dmaIFCR[0].size());
        static_assert(dmaISR[0].size() == DmaChannel[0].size());
        static_assert(dmaISR[0].size() == dmaIrq[0].size());
        static_assert(dmaISR[0].size() == streamToTCIF.size());
        static_assert(dmaISR[0].size() == streamToHTIF.size());
        static_assert(dmaISR[0].size() == streamToTEIF.size());

        uint32_t dummy;
    }

    DmaStm::DmaStm()
    {
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_GPDMA2_CLK_ENABLE();
#if defined(STM32WB) || defined(STM32G4)
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
#endif
    }

    DmaStm::~DmaStm()
    {
        __HAL_RCC_GPDMA1_CLK_DISABLE();
        __HAL_RCC_GPDMA2_CLK_DISABLE();
#if defined(STM32WB) || defined(STM32G4)
        __HAL_RCC_DMAMUX1_CLK_DISABLE();
#endif
    }

    void DmaStm::ReserveStream(uint8_t dmaIndex, uint8_t streamIndex)
    {
        assert((streamAllocation[dmaIndex] & (1 << streamIndex)) == 0);
        streamAllocation[dmaIndex] |= 1 << streamIndex;
    }

    void DmaStm::ReleaseStream(uint8_t dmaIndex, uint8_t streamIndex)
    {
        streamAllocation[dmaIndex] &= ~(1 << streamIndex);
    }

    DmaStm::Stream::Stream(DmaStm& dma, DmaChannelId channelId)
        : dma(dma)
        , dmaIndex(channelId.dma)

#if defined(DMA_STREAM_BASED)
        , streamIndex(channelId.stream)
#else
        , streamIndex(channelId.channel)
#endif
    {
        dma.ReserveStream(dmaIndex, streamIndex);

        DMA_HandleTypeDef DmaChannelHandle = {};

        DmaChannelHandle.Instance = DmaChannel[dmaIndex][streamIndex];

        DmaChannelHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
        DmaChannelHandle.Init.SrcInc = DMA_SINC_FIXED;
        DmaChannelHandle.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        DmaChannelHandle.Init.SrcBurstLength = 1;
        DmaChannelHandle.Init.DestInc = DMA_DINC_FIXED;
        DmaChannelHandle.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        DmaChannelHandle.Init.DestBurstLength = 1;
        DmaChannelHandle.Init.Mode = DMA_NORMAL;
        DmaChannelHandle.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;

#if defined(DMA_STREAM_BASED)
        DmaChannelHandle.Init.Channel = dmaChannel[channelId.channel];
        DmaChannelHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        DmaChannelHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        DmaChannelHandle.Init.MemBurst = DMA_MBURST_SINGLE;
        DmaChannelHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;
#else
        DmaChannelHandle.Init.Request = channelId.mux;
#endif

        HAL_DMA_Init(&DmaChannelHandle);
    }

    DmaStm::Stream::~Stream()
    {
        if (streamIndex != 0xff)
            dma.ReleaseStream(dmaIndex, streamIndex);
    }

    DmaStm::Stream::Stream(Stream&& other) noexcept
        : dma(other.dma)
        , dmaIndex(other.dmaIndex)
        , streamIndex(other.streamIndex)
    {
        other.streamIndex = 0xff;
    }

    DmaStm::Stream& DmaStm::Stream::operator=(Stream&& other)
    {
        dmaIndex = other.dmaIndex;
        streamIndex = other.streamIndex;

        other.streamIndex = 0xff;

        return *this;
    }

    uint8_t DmaStm::Stream::DataSize() const
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        return 1 << ((streamRegister->CR & DMA_SxCR_MSIZE) >> POSITION_VAL(DMA_SxCR_MSIZE));
#else
        return 1 ;
#endif
    }

    void DmaStm::Stream::SetDataSize(uint8_t dataSizeInBytes)
    {
        SetPeripheralDataSize(dataSizeInBytes);
        SetMemoryDataSize(dataSizeInBytes);
    }

    void DmaStm::Stream::SetPeripheralDataSize(uint8_t dataSizeInBytes)
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR = (streamRegister->CR & ~DMA_SxCR_PSIZE) | ((dataSizeInBytes & 1) == 0 ? DMA_SxCR_PSIZE_0 : 0) | (dataSizeInBytes > 2 ? DMA_SxCR_PSIZE_1 : 0);
#else
#endif
    }

    void DmaStm::Stream::SetMemoryDataSize(uint8_t dataSizeInBytes)
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR = (streamRegister->CR & ~DMA_SxCR_MSIZE) | ((dataSizeInBytes & 1) == 0 ? DMA_SxCR_MSIZE_0 : 0) | (dataSizeInBytes > 2 ? DMA_SxCR_MSIZE_1 : 0);
#else
#endif
    }

    bool DmaStm::Stream::StopTransfer()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
        bool finished = Finished();
        DisableHalfTransferCompleteInterrupt();
        DisableTransferCompleteInterrupt();
        Disable();
        *dmaIFCR[dmaIndex][streamIndex] |= streamToTCIF[streamIndex] | streamToHTIF[streamIndex];
        return !finished;
    }

    void DmaStm::Stream::Disable()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR &= ~DMA_SxCR_EN;
#else
        streamRegister->CCR &= ~DMA_CCR_EN;
#endif
    }

    void DmaStm::Stream::DisableHalfTransferCompleteInterrupt()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR &= ~DMA_IT_HT;
#else
        streamRegister->CCR &= ~DMA_IT_HT;
#endif
    }

    void DmaStm::Stream::DisableMemoryIncrement()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR &= ~DMA_MINC_ENABLE;
#else
#endif
    }

    void DmaStm::Stream::DisableTransferCompleteInterrupt()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR &= ~DMA_IT_TC;
#else
        streamRegister->CCR &= ~DMA_IT_TC;
#endif
    }

    void DmaStm::Stream::DisableCircularMode()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR &= ~DMA_SxCR_CIRC;
#else
#endif
    }

    void DmaStm::Stream::Enable()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR |= DMA_SxCR_EN;
#else
        streamRegister->CCR |= DMA_CCR_EN;
#endif
    }

    void DmaStm::Stream::EnableHalfTransferCompleteInterrupt()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR |= DMA_IT_HT;
#else
        streamRegister->CCR |= DMA_IT_HT;
#endif
    }

    void DmaStm::Stream::EnableMemoryIncrement()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR |= DMA_MINC_ENABLE;
#else
#endif
    }

    void DmaStm::Stream::EnableTransferCompleteInterrupt()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR |= DMA_IT_TC;
#else
        streamRegister->CCR |= DMA_IT_TC;
#endif
    }

    void DmaStm::Stream::EnableCircularMode()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR |= DMA_SxCR_CIRC;
#else
#endif
    }

    bool DmaStm::Stream::Finished() const
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        return (streamRegister->CR & DMA_SxCR_EN) == 0;
#else
        return (streamRegister->CCR & DMA_CCR_EN) == 0;

#endif
    }

    void DmaStm::Stream::SetMemoryAddress(const void* memoryAddress)
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->M0AR = reinterpret_cast<uint32_t>(memoryAddress);
#else
#endif
    }

    void DmaStm::Stream::SetMemoryToPeripheralMode()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR |= DMA_MEMORY_TO_PERIPH;
#else
        streamRegister->CCR |= DMA_MEMORY_TO_PERIPH;
#endif
    }

    void DmaStm::Stream::SetPeripheralAddress(volatile void* peripheralAddress)
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->PAR = reinterpret_cast<uint32_t>(peripheralAddress);
#else
#endif
    }

    void DmaStm::Stream::SetPeripheralToMemoryMode()
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->CR &= ~DMA_MEMORY_TO_PERIPH;
#else
        streamRegister->CCR &= ~DMA_MEMORY_TO_PERIPH;
#endif
    }

    void DmaStm::Stream::SetTransferSize(uint16_t size)
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        streamRegister->NDTR = size / DataSize();
#else
#endif
    }

    bool DmaStm::Stream::IsHalfComplete() const
    {
        return *dmaISR[dmaIndex][streamIndex] & streamToHTIF[streamIndex];
    }

    bool DmaStm::Stream::IsFullComplete() const
    {
        return *dmaISR[dmaIndex][streamIndex] & streamToTCIF[streamIndex];
    }

    void DmaStm::Stream::ClearHalfComplete() const
    {
        *dmaIFCR[dmaIndex][streamIndex] |= streamToHTIF[streamIndex];
    }

    void DmaStm::Stream::ClearFullComplete() const
    {
        *dmaIFCR[dmaIndex][streamIndex] |= streamToTCIF[streamIndex];
    }

    size_t DmaStm::Stream::BytesToTransfer() const
    {
        auto streamRegister = DmaChannel[dmaIndex][streamIndex];
#if defined(STM32F7) || defined(STM32F4)
        return streamRegister->NDTR * DataSize();
#else
        return 1;
#endif
    }

    void DmaStm::TransceiveStream::StartTransmit(infra::ConstByteRange data)
    {
        SetMemoryToPeripheralMode();
        EnableMemoryIncrement();
        SetTransferSize(data.size());
        SetMemoryAddress(data.begin());
        Enable();
    }

    void DmaStm::TransceiveStream::StartTransmitDummy(uint16_t size)
    {
        SetMemoryToPeripheralMode();
        DisableMemoryIncrement();
        SetTransferSize(size);
        SetMemoryAddress(&dummy);
        Enable();
    }

    void DmaStm::TransceiveStream::StartReceive(infra::ByteRange data)
    {
        this->data = data;

        SetPeripheralToMemoryMode();
        EnableMemoryIncrement();
        SetTransferSize(data.size());
        SetMemoryAddress(data.begin());
        Enable();
    }

    void DmaStm::TransceiveStream::StartReceiveDummy(uint16_t size)
    {
        SetPeripheralToMemoryMode();
        DisableMemoryIncrement();
        SetTransferSize(size);
        SetMemoryAddress(&dummy);
        Enable();
    }

    size_t DmaStm::TransceiveStream::ReceivedSize() const
    {
        return (data.size() - BytesToTransfer()) / DataSize();
    }

    DmaStm::StreamInterruptHandler::StreamInterruptHandler(Stream& stream, const infra::Function<void()>& transferFullComplete)
        : stream{ stream }
        , dispatchedInterruptHandler{ dmaIrq[stream.dmaIndex][stream.streamIndex], [this]
            {
                OnInterrupt();
            } }
        , transferFullComplete{ transferFullComplete }
    {
        stream.DisableCircularMode();
        stream.EnableTransferCompleteInterrupt();
    }

    void DmaStm::StreamInterruptHandler::OnInterrupt()
    {
        if (stream.IsFullComplete())
        {
            stream.ClearFullComplete();

            stream.Disable();

            __DMB();

            dispatchedInterruptHandler.ClearPending();
            transferFullComplete();
        }
    }

    DmaStm::CircularStreamInterruptHandler::CircularStreamInterruptHandler(Stream& stream, const infra::Function<void()>& transferHalfComplete, const infra::Function<void()>& transferFullComplete)
        : stream{ stream }
        , immediateInterruptHandler{ dmaIrq[stream.dmaIndex][stream.streamIndex], [this]
            {
                OnInterrupt();
            } }
        , transferHalfComplete{ transferHalfComplete }
        , transferFullComplete{ transferFullComplete }
    {
        stream.EnableCircularMode();
        stream.EnableHalfTransferCompleteInterrupt();
        stream.EnableTransferCompleteInterrupt();
    }

    bool DmaStm::CircularStreamInterruptHandler::IsInterruptPending() const
    {
        return stream.IsHalfComplete() || stream.IsFullComplete();
    }

    void DmaStm::CircularStreamInterruptHandler::OnInterrupt()
    {
        if (stream.IsHalfComplete())
        {
            stream.ClearHalfComplete();

            immediateInterruptHandler.ClearPending();

            transferHalfComplete();
        }

        if (stream.IsFullComplete())
        {
            stream.ClearFullComplete();

            immediateInterruptHandler.ClearPending();

            transferFullComplete();
        }
    }

    DmaStm::PeripheralTransceiveStream::PeripheralTransceiveStream(TransceiveStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize)
        : stream{ stream }
    {
        stream.SetPeripheralAddress(peripheralAddress);
        stream.SetPeripheralDataSize(peripheralTransferSize);
    }

    void DmaStm::PeripheralTransceiveStream::SetPeripheralTransferSize(uint8_t peripheralTransferSize)
    {
        stream.SetPeripheralDataSize(peripheralTransferSize);
    }

    bool DmaStm::PeripheralTransceiveStream::StopTransfer()
    {
        return stream.StopTransfer();
    }

    void DmaStm::PeripheralTransceiveStream::StartTransmitDummy(uint16_t size, uint8_t dataSize)
    {
        stream.SetMemoryDataSize(dataSize);
        stream.StartTransmitDummy(size);
    }

    void DmaStm::PeripheralTransceiveStream::StartReceiveDummy(uint16_t size, uint8_t dataSize)
    {
        stream.SetMemoryDataSize(dataSize);
        stream.StartReceiveDummy(size);
    }

    size_t DmaStm::PeripheralTransceiveStream::ReceivedSize() const
    {
        return stream.ReceivedSize();
    }

    ///////////////////////////////

    TransceiverDmaChannelBase::TransceiverDmaChannelBase(DmaStm::TransceiveStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize)
        : peripheralStream{ stream, peripheralAddress, peripheralTransferSize }
    {
    }

    bool TransceiverDmaChannelBase::StopTransfer()
    {
        return peripheralStream.StopTransfer();
    }

    void TransceiverDmaChannelBase::SetPeripheralTransferSize(uint8_t peripheralTransferSize)
    {
        peripheralStream.SetPeripheralTransferSize(peripheralTransferSize);
    }

    void TransceiverDmaChannelBase::StartTransmitDummy(uint16_t size, uint8_t dataSize)
    {
        peripheralStream.StartTransmitDummy(size, dataSize);
    }

    size_t TransceiverDmaChannelBase::ReceivedSize() const
    {
        return peripheralStream.ReceivedSize();
    }

    void TransceiverDmaChannelBase::StartReceiveDummy(uint16_t size, uint8_t dataSize)
    {
        peripheralStream.StartReceiveDummy(size, dataSize);
    }

    TransceiverDmaChannel::TransceiverDmaChannel(DmaStm::TransceiveStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize, const infra::Function<void()>& transferFullComplete)
        : TransceiverDmaChannelBase{ stream, peripheralAddress, peripheralTransferSize }
        , streamInterruptHandler{ stream, transferFullComplete }
    {}

    TransmitDmaChannel::TransmitDmaChannel(DmaStm::TransmitStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize, const infra::Function<void()>& transferFullComplete)
        : TransceiverDmaChannel{ stream, peripheralAddress, peripheralTransferSize, transferFullComplete }
    {}

    ReceiveDmaChannel::ReceiveDmaChannel(DmaStm::ReceiveStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize, const infra::Function<void()>& transferFullComplete)
        : TransceiverDmaChannel{ stream, peripheralAddress, peripheralTransferSize, transferFullComplete }
    {}

    CircularTransceiverDmaChannel::CircularTransceiverDmaChannel(DmaStm::TransceiveStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize, const infra::Function<void()>& transferHalfComplete, const infra::Function<void()>& transferFullComplete)
        : TransceiverDmaChannelBase{ stream, peripheralAddress, peripheralTransferSize }
        , circularStreamInterruptHandler{ stream, transferHalfComplete, transferFullComplete }
    {}

    bool CircularTransceiverDmaChannel::IsInterruptPending() const
    {
        return circularStreamInterruptHandler.IsInterruptPending();
    }

    CircularTransmitDmaChannel::CircularTransmitDmaChannel(DmaStm::TransmitStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize, const infra::Function<void()>& transferHalfComplete, const infra::Function<void()>& transferFullComplete)
        : CircularTransceiverDmaChannel{ stream, peripheralAddress, peripheralTransferSize, transferHalfComplete, transferFullComplete }
    {}

    CircularReceiveDmaChannel::CircularReceiveDmaChannel(DmaStm::ReceiveStream& stream, volatile void* peripheralAddress, uint8_t peripheralTransferSize, const infra::Function<void()>& transferHalfComplete, const infra::Function<void()>& transferFullComplete)
        : CircularTransceiverDmaChannel{ stream, peripheralAddress, peripheralTransferSize, transferHalfComplete, transferFullComplete }
    {}
}

#endif
