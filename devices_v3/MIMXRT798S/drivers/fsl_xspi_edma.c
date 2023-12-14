/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_xspi_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xspi_edma"
#endif

/*<! Structure definition for xspi_edma_private_handle_t. The structure is private. */
typedef struct _xspi_edma_private_handle
{
    XSPI_Type *base;
    xspi_edma_handle_t *handle;
} xspi_edma_private_handle_t;

/* XSPI EDMA transfer handle, _xspi_edma_tansfer_states. */
enum
{
    kXSPI_Idle, /* XSPI Bus idle. */
    kXSPI_Busy  /* XSPI Bus busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to xspi bases for each instance. */
static XSPI_Type *const s_xspiBases[] = XSPI_BASE_PTRS;

/*<! Private handle only used for internally. */
static xspi_edma_private_handle_t s_edmaPrivateHandle[ARRAY_SIZE(s_xspiBases)];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief XSPI EDMA transfer finished callback function.
 *
 * This function is called when XSPI EDMA transfer finished. It disables the XSPI
 * TX/RX EDMA request and sends status to XSPI callback.
 *
 * @param handle The EDMA handle.
 * @param param Callback function parameter.
 */
static void XSPI_TransferEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint8_t XSPI_CalculatePower(uint8_t value)
{
    uint8_t power = 0;
    while (value >> 1 != 0U)
    {
        power++;
        value = value >> 1;
    }

    return power;
}
static void XSPI_TransferEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    xspi_edma_private_handle_t *xspiPrivateHandle = (xspi_edma_private_handle_t *)param;

    /* Avoid warning for unused parameters. */
    handle = handle;
    tcds   = tcds;

    if (transferDone)
    {
        /* Wait for bus idle. */
        while (!XSPI_GetBusIdleStatus(xspiPrivateHandle->base))
        {
        }
        /* Disable transfer. */
        XSPI_TransferAbortEDMA(xspiPrivateHandle->base, xspiPrivateHandle->handle);

        if (xspiPrivateHandle->handle->completionCallback != NULL)
        {
            xspiPrivateHandle->handle->completionCallback(xspiPrivateHandle->base, xspiPrivateHandle->handle,
                                                             kStatus_Success, xspiPrivateHandle->handle->userData);
        }
    }
}

/*!
 * brief Initializes the XSPI handle for transfer which is used in transactional functions and set the callback.
 *
 * param base XSPI peripheral base address
 * param handle Pointer to xspi_edma_handle_t structure
 * param callback XSPI callback, NULL means no callback.
 * param userData User callback function data.
 * param txDmaHandle User requested DMA handle for TX DMA transfer.
 * param rxDmaHandle User requested DMA handle for RX DMA transfer.
 */
void XSPI_TransferCreateHandleEDMA(XSPI_Type *base,
                                      xspi_edma_handle_t *handle,
                                      xspi_edma_callback_t callback,
                                      void *userData,
                                      edma_handle_t *txDmaHandle,
                                      edma_handle_t *rxDmaHandle)
{
    assert(handle);

    uint32_t instance = XSPI_GetInstance(base);

    s_edmaPrivateHandle[instance].base   = base;
    s_edmaPrivateHandle[instance].handle = handle;

    (void)memset(handle, 0, sizeof(*handle));

    handle->state       = kXSPI_Idle;
    handle->txDmaHandle = txDmaHandle;
    handle->rxDmaHandle = rxDmaHandle;
    handle->nsize       = kXSPI_EDMAnSize4Bytes;

    handle->completionCallback = callback;
    handle->userData           = userData;
}

/*!
 * brief Update XSPI EDMA transfer source data transfer size(SSIZE) and destination data transfer size(DSIZE).
 *
 * param base XSPI peripheral base address
 * param handle Pointer to xspi_edma_handle_t structure
 * param nsize XSPI DMA transfer data transfer size(SSIZE/DSIZE), by default the size is
 * kXSPI_EDMAnSize1Bytes(one byte).
 * see xspi_edma_transfer_nsize_t               .
 */
void XSPI_TransferUpdateSizeEDMA(XSPI_Type *base,
                                    xspi_edma_handle_t *handle,
                                    xspi_edma_transfer_nsize_t nsize)
{
    handle->nsize = nsize;
}

/*!
 * brief Transfers XSPI data using an eDMA non-blocking method.
 *
 * This function writes/receives data to/from the XSPI transmit/receive FIFO. This function is non-blocking.
 * param base XSPI peripheral base address.
 * param handle Pointer to xspi_edma_handle_t structure
 * param xfer XSPI transfer structure.
 * retval kStatus_XSPI_Busy     XSPI is busy transfer.
 * retval kStatus_InvalidArgument  The watermark configuration is invalid, the watermark should be power of
                                    2 to do successfully EDMA transfer.
 * retval kStatus_Success          XSPI successfully start edma transfer.
 */
status_t XSPI_TransferEDMA(XSPI_Type *base, xspi_edma_handle_t *handle, xspi_transfer_t *xfer)
{
    uint32_t configValue = 0;
    status_t result      = kStatus_Success;
    edma_transfer_config_t xferConfig;
    uint32_t instance = XSPI_GetInstance(base);
    uint8_t power     = 0;

    assert(handle);
    assert(xfer);

    /* Check if the XSPI bus is idle - if not return busy status. */
    if (handle->state != (uint32_t)kXSPI_Idle)
    {
        result = kStatus_XSPI_Busy;
    }
    else
    {
        handle->transferSize = xfer->dataSize;
        handle->state        = kXSPI_Busy;

        if (xfer->targetGroup == kXSPI_TargetGroup1)
        {
                /*check the target groupe 1 is empty*/
                while (0U != ((base->SUB_REG_MDAM_ARRAY[0].TGSFARS_SUB & XSPI_TGSFARS_SUB_VLD_MASK) >> XSPI_TGSFARS_SUB_VLD_SHIFT))
                {
                }

                /* Set target groupe 1 Serial Flash Address register*/
                base->SUB_REG_MDAM_ARRAY[0].SFP_TG_SUB_SFAR = xfer->deviceAddress;

                /* Configure target groupe 1 data size. */
                if ((xfer->cmdType == kXSPI_Read) || (xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
                {
                        configValue = XSPI_SFP_TG_SUB_IPCR_IDATSZ(xfer->dataSize);
                }

                /*config target groupe 1 data size sequence ID.*/
                configValue |= XSPI_SFP_TG_SUB_IPCR_SEQID(xfer->seqIndex);
                base->SUB_REG_MDAM_ARRAY[0].SFP_TG_SUB_IPCR = configValue;
        }
        else
        {
                 /*check the target groupe 0 is empty*/
                while (0U != ((base->TGSFARS & XSPI_TGSFARS_VLD_MASK) >> XSPI_TGSFARS_VLD_SHIFT))
                {
                }

                /* Set target groupe 0 Serial Flash Address register*/
                base->SFP_TG_SFAR = xfer->deviceAddress;

                /* Configure target groupe 0 data size. */
                if ((xfer->cmdType == kXSPI_Read) || (xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
                {
                        configValue = XSPI_SFP_TG_IPCR_IDATSZ(xfer->dataSize);
                }

                /*config target groupe 0 data size sequence ID.*/
                configValue |= XSPI_SFP_TG_IPCR_SEQID(xfer->seqIndex);
                base->SFP_TG_IPCR = configValue;
        }

        /* Just Clear RX  TX FIFO PT*/
        base->MCR |= XSPI_MCR_CLR_RXF_MASK;
        base->MCR |= XSPI_MCR_CLR_TXF_MASK;

    }

    if ((xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
    {
        base->TBCT = 256 - (xfer->dataSize/4 - 1);

        handle->nbytes = xfer->dataSize;

        power = XSPI_CalculatePower(4U * handle->count);

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, xfer->data, (uint32_t)handle->nsize,
                             (void *)(uint32_t *)XSPI_GetTxFifoAddress(base), (uint32_t)handle->nsize,
                             (uint32_t)handle->nbytes, xfer->dataSize, kEDMA_MemoryToPeripheral);

        /* Submit transfer. */
        (void)EDMA_SubmitTransfer(handle->txDmaHandle, &xferConfig);
        EDMA_SetModulo(handle->txDmaHandle->base, handle->txDmaHandle->channel, kEDMA_ModuloDisable,
                       (edma_modulo_t)power);
        EDMA_SetCallback(handle->txDmaHandle, XSPI_TransferEDMACallback,
                         &s_edmaPrivateHandle[XSPI_GetInstance(base)]);
        EDMA_StartTransfer(handle->txDmaHandle);

        /* Enable XSPI TX EDMA. */
        XSPI_EnableTxDMA(base, true);
    }
    else if (xfer->cmdType == kXSPI_Read)
    {
        handle->count = (uint8_t)(base->RBCT) + 1U;

        if (xfer->dataSize < 4U * (uint32_t)handle->count)
        {
            handle->nbytes = (uint8_t)xfer->dataSize;
        }
        else
        {
            /* Check the handle->count is power of 2 */
            if (((handle->count) & (handle->count - 1U)) != 0U)
            {
                return kStatus_InvalidArgument;
            }
            /* Store the initially configured eDMA minor byte transfer count into the XSPI handle */
            handle->nbytes = (4U * handle->count);
        }

        power = XSPI_CalculatePower(4U * handle->count);

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, (void *)(uint32_t *)XSPI_GetRxFifoAddress(base), (uint32_t)handle->nsize,
                             xfer->data, (uint32_t)handle->nsize, xfer->dataSize, xfer->dataSize,
                             kEDMA_MemoryToMemory);

        /* Submit transfer. */
        (void)EDMA_SubmitTransfer(handle->rxDmaHandle, &xferConfig);
        EDMA_SetModulo(handle->txDmaHandle->base, handle->txDmaHandle->channel, (edma_modulo_t)power,
                       kEDMA_ModuloDisable);
        EDMA_SetCallback(handle->rxDmaHandle, XSPI_TransferEDMACallback, &s_edmaPrivateHandle[instance]);
        EDMA_StartTransfer(handle->rxDmaHandle);

        /* Enable XSPI RX EDMA. */
        XSPI_EnableRxDMA(base, true);

    }
    else
    {
        /* Wait for bus idle. */
        while (!XSPI_GetBusIdleStatus(base))
        {
        }
        result = XSPI_CheckAndClearError(base, base->ERRSTAT);

        handle->state = kXSPI_Idle;

        if (handle->completionCallback != NULL)
        {
            handle->completionCallback(base, handle, result, handle->userData);
        }
    }

    return result;
}

/*!
 * brief Aborts the transfer data using eDMA.
 *
 * This function aborts the transfer data using eDMA.
 *
 * param base XSPI peripheral base address.
 * param handle Pointer to xspi_edma_handle_t structure
 */
void XSPI_TransferAbortEDMA(XSPI_Type *base, xspi_edma_handle_t *handle)
{
    assert(handle);

    if ((base->SR & XSPI_SR_TXWA_MASK) != 0x00U)
    {
        XSPI_EnableTxDMA(base, false);
        EDMA_AbortTransfer(handle->txDmaHandle);
    }

    if ((base->SR & XSPI_SR_RXDMA_MASK) != 0x00U)
    {
        XSPI_EnableRxDMA(base, false);
        EDMA_AbortTransfer(handle->rxDmaHandle);
    }

    handle->state = kXSPI_Idle;
}

status_t XSPI_TransferGetTransferCountEDMA(XSPI_Type *base, xspi_edma_handle_t *handle, size_t *count)
{
    assert(handle);
    assert(count);

    status_t result = kStatus_Success;

    if (handle->state != (uint32_t)kXSPI_Busy)
    {
        result = kStatus_NoTransferInProgress;
    }
    else
    {
        if ((base->SR & XSPI_SR_RXDMA_MASK) != 0x00U)
        {
            *count = (handle->transferSize -
                      (uint32_t)handle->nbytes *
                          EDMA_GetRemainingMajorLoopCount(handle->rxDmaHandle->base, handle->rxDmaHandle->channel));
        }
        else if ((base->SR & XSPI_SR_TXWA_MASK) != 0x00U)
        {
            *count = (handle->transferSize -
                      (uint32_t)handle->nbytes *
                          EDMA_GetRemainingMajorLoopCount(handle->txDmaHandle->base, handle->txDmaHandle->channel));
        }
        else
        {
            ; /* Intentional empty for MISRA C-2012 rule 15.7. */
        }
    }

    return result;
}
