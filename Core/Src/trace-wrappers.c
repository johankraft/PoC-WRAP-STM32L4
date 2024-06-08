#include "main.h"
#include "cmsis_os.h"
#include "trcRecorder.h"

// The wrapper functions are hooked in using the linked command -Wl,--wrap=function in the project properties

// To simplify tracing multiple functions, the wrap commands are placed in a text file, wrapped-functions.txt
// This is added in the linker arguments like this: @../wrapped-functions.txt
// Add that in C/C++ Build -> Settings -> Tool Settings -> MCU GCC Linker -> Miscellaneous -> Other flags.

TraceStringHandle_t chn_hal_uart;

// Perhaps have support for dynamic filtering, that can be modified in runtime to avoid recompile?
volatile char event_filter[128] = {0};

// The ID/index of each function in the filter
#define HAL_UART_Transmit_DMA_filter 0

void init_cti_trace(void)
{
	chn_hal_uart = xTraceRegisterString("HAL_UART");
	event_filter[HAL_UART_Transmit_DMA_filter] = 1; // Include this function
}

/* For each traced function listed in wrapped-functions.txt */

// To avoid warnings
extern HAL_StatusTypeDef __real_HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);

// The wrapper function traces all calls of this function (add func
HAL_StatusTypeDef __wrap_HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef status;
	status = __real_HAL_UART_Transmit_DMA(huart, pData, Size);

	if (event_filter[HAL_UART_Transmit_DMA_filter] == 1)
	{
		xTracePrintF(chn_hal_uart, "HAL_UART_Transmit_DMA(%d) returned %d", Size, status);
	}

	return status;
}
